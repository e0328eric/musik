/*
 * Copyright (C) 2021-2023 Sungbae Jeong
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Dynamically sized string library for pure C
 * version : 2.0.0
 *
 * v1.0.1 : add mkStringFmt, appendFmtStr and appendFmtStrBack
 *
 * v2.0.0 : window supports and add `dyns` prefix
 *
 * Usage
 * This library uses the stb header only library style. So, defining
 * DYN_STRING_IMPL macro on the one of the c source code to generate the
 * whole implementations of this library.
 */

#ifndef DYN_STRING_H_
#define DYN_STRING_H_

#include <ctype.h>
#include <stdbool.h>

#if defined(__APPLE__) || defined(__linux__)
#include <sys/types.h>
#elif defined(_WIN32)
typedef long long ssize_t;
#endif

#ifndef DYNSTRDEF
#define DYNSTRDEF
#endif  // DYNSTRDEF

typedef struct DynString DynString;

DYNSTRDEF DynString* dyns_make_string(const char* str);
DYNSTRDEF DynString* dyns_make_n_string(const char* str, size_t strLen);
DYNSTRDEF DynString* dyns_make_string_fmt(const char* format, ...);

DYNSTRDEF void dyns_free_string(DynString* pString);

DYNSTRDEF void dyns_append_char(DynString* pString, char chr);
DYNSTRDEF void dyns_append_cstr(DynString* pString, const char* str);
DYNSTRDEF void dyns_append_n_cstr(DynString* pString, const char* str, size_t strLen);
DYNSTRDEF void dyns_append_fmt_str(DynString* pString, const char* format, ...);
DYNSTRDEF void dyns_append_char_back(DynString* pString, char chr);
DYNSTRDEF void dyns_append_cstr_back(DynString* pString, const char* str);
DYNSTRDEF void dyns_append_n_cstr_back(DynString* pString, const char* str, size_t strLen);
DYNSTRDEF void dyns_append_fmt_str_back(DynString* pString, const char* format, ...);

DYNSTRDEF void dyns_concat_string(DynString* dst, const DynString* src);
DYNSTRDEF void dyns_concat_string_back(DynString* dst, const DynString* src);
DYNSTRDEF void dyns_concat_string_free(DynString* dst, DynString* src);
DYNSTRDEF void dyns_concat_string_back_free(DynString* dst, DynString* src);

DYNSTRDEF int dyns_cmp_string(const DynString* pString1, const DynString* pString2);
DYNSTRDEF int dyns_cmp_string_str(const DynString* pString, const char* str);
DYNSTRDEF int dyns_cmp_cstr_string(const char* str, const DynString* pString);

DYNSTRDEF ssize_t dyns_find_char(const DynString* pString, char chr);
DYNSTRDEF ssize_t dyns_find_char_nth(const DynString* pString, char chr, size_t nth);
DYNSTRDEF ssize_t dyns_find_char_reverse(const DynString* pString, char chr);
DYNSTRDEF ssize_t dyns_find_char_reverse_nth(const DynString* pString, char chr, size_t nth);

DYNSTRDEF bool dyns_pop_char(DynString* pString, char* output);

DYNSTRDEF void dyns_clear_string_after_pos(DynString* pString, size_t pos);
DYNSTRDEF void dyns_clear_string_before_pos(DynString* pString, size_t pos);
DYNSTRDEF void dyns_erase(DynString* pString);

DYNSTRDEF const char* dyns_get_cstr(const DynString* const pString);
DYNSTRDEF size_t dyns_get_len(const DynString* const pString);
DYNSTRDEF size_t dyns_get_capacity(const DynString* const pString);

#define DYNS_FMT "%s"
#define DYNS_ARG(str) (dyns_get_cstr(str))

#ifdef DYN_STRING_IMPLEMENTATION

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct DynString {
    size_t capacity;
    size_t len;
    char* inner;
};

DynString* dyns_make_string(const char* str) {
    DynString* string = (DynString*)malloc(sizeof(DynString));

    if (!str) {
        string->capacity = 0;
        string->len = 0;
        string->inner = NULL;
    } else {
        size_t strLen = strlen(str);

        string->capacity = (strLen + 1) << 1;
        string->len = strLen;
        string->inner = (char*)malloc(string->capacity);
        memcpy(string->inner, str, strLen);
        string->inner[strLen] = '\0';
    }

    return string;
}

DynString* dyns_make_n_string(const char* str, size_t strLen) {
    DynString* string = (DynString*)malloc(sizeof(DynString));

    if (!str || strLen == 0) {
        string->capacity = 0;
        string->len = 0;
        string->inner = NULL;
    } else {
        string->capacity = (strLen + 1) << 1;
        string->len = strLen;
        string->inner = (char*)malloc(string->capacity);
        memcpy(string->inner, str, strLen);
        string->inner[strLen] = '\0';
    }

    return string;
}

DynString* dyns_make_string_fmt(const char* format, ...) {
    size_t len;

    va_list args;
    {
        va_start(args, format);
        int tmp = vsnprintf(NULL, 0, format, args);
        va_end(args);

        if (tmp < 0)
            return NULL;
        len = (size_t)tmp;
    }

    DynString* string = (DynString*)malloc(sizeof(DynString));
    string->capacity = (len + 1) << 1;
    string->len = len;
    string->inner = (char*)malloc(string->capacity);

    va_start(args, format);
    vsnprintf(string->inner, len + 1, format, args);
    va_end(args);

    return string;
}

void dyns_free_string(DynString* pString) {
    if (!pString)
        return;

    free(pString->inner);
    free(pString);
}

void dyns_append_char(DynString* pString, char chr) {
    if (pString->len + 1 >= pString->capacity) {
        pString->capacity = (pString->capacity + 1) << 1;
        pString->inner = (char*)realloc(pString->inner, pString->capacity);
    }
    pString->inner[pString->len++] = chr;
    pString->inner[pString->len] = '\0';
}

void dyns_append_cstr(DynString* pString, const char* str) {
    if (!str) {
        return;
    }

    size_t strLen = strlen(str);
    if (pString->len + strLen + 1 >= pString->capacity) {
        pString->capacity = (pString->capacity + strLen + 1) << 1;
        pString->inner = (char*)realloc(pString->inner, pString->capacity);
    }
    memcpy(pString->inner + pString->len, str, strLen);
    pString->inner[pString->len + strLen] = '\0';
    pString->len += strLen;
}

void dyns_append_n_cstr(DynString* pString, const char* str, size_t strLen) {
    if (!str || strLen == 0) {
        return;
    }

    if (pString->len + strLen + 1 >= pString->capacity) {
        pString->capacity = (pString->capacity + strLen + 1) << 1;
        pString->inner = (char*)realloc(pString->inner, pString->capacity);
    }
    memcpy(pString->inner + pString->len, str, strLen);
    pString->inner[pString->len + strLen] = '\0';
    pString->len += strLen;
}

void dyns_append_fmt_str(DynString* pString, const char* format, ...) {
    size_t len;

    va_list args;
    {
        va_start(args, format);
        int tmp = vsnprintf(NULL, 0, format, args);
        va_end(args);

        if (tmp < 0)
            return;
        len = (size_t)tmp;
    }

    if (pString->len + len + 1 >= pString->capacity) {
        pString->capacity = (pString->capacity + len + 1) << 1;
        pString->inner = (char*)realloc(pString->inner, pString->capacity);
    }

    va_start(args, format);
    vsnprintf(pString->inner + pString->len, len + 1, format, args);
    va_end(args);
    pString->len += len;
}

void dyns_append_char_back(DynString* pString, char chr) {
    if (pString->len + 1 >= pString->capacity) {
        pString->capacity = (pString->capacity + 1) << 1;
        pString->inner = (char*)realloc(pString->inner, pString->capacity);
    }

    memmove(pString->inner + 1, pString->inner, ++pString->len);
    pString->inner[0] = chr;
}

void dyns_append_cstr_back(DynString* pString, const char* str) {
    size_t strLen = strlen(str);
    if (pString->len + strLen + 1 >= pString->capacity) {
        pString->capacity = (pString->capacity + strLen + 1) << 1;
        pString->inner = (char*)realloc(pString->inner, pString->capacity);
    }
    memmove(pString->inner + strLen, pString->inner, pString->len);
    memcpy(pString->inner, str, strLen);
    pString->inner[pString->len + strLen] = '\0';
    pString->len += strLen;
}

void dyns_append_n_cstr_back(DynString* pString, const char* str, size_t strLen) {
    if (pString->len + strLen + 1 >= pString->capacity) {
        pString->capacity = (pString->capacity + strLen + 1) << 1;
        pString->inner = (char*)realloc(pString->inner, pString->capacity);
    }
    memmove(pString->inner + strLen, pString->inner, pString->len);
    memcpy(pString->inner, str, strLen);
    pString->inner[pString->len + strLen] = '\0';
    pString->len += strLen;
}

void dyns_append_fmt_str_back(DynString* pString, const char* format, ...) {
    size_t len;

    va_list args;
    {
        va_start(args, format);
        int tmp = vsnprintf(NULL, 0, format, args);
        va_end(args);

        if (tmp < 0)
            return;
        len = (size_t)tmp;
    }

    if (pString->len + len + 2 >= pString->capacity) {
        pString->capacity = (pString->capacity + len + 2) << 1;
        pString->inner = (char*)realloc(pString->inner, pString->capacity);
    }

    memmove(pString->inner + len + 1, pString->inner, pString->len + 1);
    va_start(args, format);
    vsnprintf(pString->inner, len + 1, format, args);
    va_end(args);
    memmove(pString->inner + len, pString->inner + len + 1, pString->len + 1);

    pString->len += len;
}

void dyns_concat_string(DynString* dst, const DynString* src) {
    if (!dst || !src)
        return;

    if (dst->len + src->len + 1 >= dst->capacity) {
        dst->capacity = (dst->capacity + src->len + 1) << 1;
        dst->inner = (char*)realloc(dst->inner, dst->capacity);
    }
    memcpy(dst->inner + dst->len, src->inner, src->len);
    dst->inner[dst->len + src->len] = '\0';
    dst->len += src->len;
}

void dyns_concat_string_free(DynString* dst, DynString* src) {
    dyns_concat_string(dst, src);
    dyns_free_string(src);
}

void dyns_concat_string_back(DynString* dst, const DynString* src) {
    if (!dst || !src)
        return;

    if (dst->len + src->len + 1 >= dst->capacity) {
        dst->capacity = (dst->capacity + src->len + 1) << 1;
        dst->inner = (char*)realloc(dst->inner, dst->capacity);
    }
    memmove(dst->inner + src->len, dst->inner, dst->len);
    memcpy(dst->inner, src->inner, src->len);
    dst->inner[dst->len + src->len] = '\0';
    dst->len += src->len;
}

void dyns_concat_string_back_free(DynString* dst, DynString* src) {
    dyns_concat_string_back(dst, src);
    dyns_free_string(src);
}

int dyns_cmp_string(const DynString* pString1, const DynString* pString2) {
    if (!pString1)
        return -(int)pString2->len;
    if (!pString2)
        return (int)pString1->len;
    return strcmp(pString1->inner, pString2->inner);
}

int dyns_cmp_string_str(const DynString* pString, const char* str) {
    if (!pString)
        return -(int)strlen(str);
    if (!str)
        return (int)pString->len;
    return strcmp(pString->inner, str);
}

int dyns_cmp_cstr_string(const char* str, const DynString* pString) {
    if (!str)
        return -(int)pString->len;
    if (!pString)
        return (int)strlen(str);
    return strcmp(str, pString->inner);
}

ssize_t dyns_find_char(const DynString* pString, char chr) {
    return dyns_find_char_nth(pString, chr, 1);
}

ssize_t dyns_find_char_nth(const DynString* pString, char chr, size_t nth) {
    if (!pString)
        return -1;

    char* ptr = pString->inner - 1;
    while (nth--) {
        ptr = strchr(++ptr, chr);
        if (!ptr)
            return -1;
    }

    return ptr - pString->inner;
}

ssize_t dyns_find_char_reverse(const DynString* pString, char chr) {
    return dyns_find_char_reverse_nth(pString, chr, 1);
}

ssize_t dyns_find_char_reverse_nth(const DynString* pString, char chr, size_t nth) {
    if (!pString)
        return -1;

    char* ptr = pString->inner - 1;
    while (nth--) {
        ptr = strrchr(++ptr, chr);
        if (!ptr)
            return -1;
    }

    return ptr - pString->inner;
}

bool dyns_pop_char(DynString* pString, char* output) {
    if (pString->len == 0)
        return false;

    char tmp = pString->inner[--pString->len];
    if (output) {
        *output = tmp;
    }
    return true;
}

void dyns_clear_string_after_pos(DynString* pString, size_t pos) {
    if (!pString)
        return;

    pos = pos >= pString->len ? pString->len : pos;

    memset(pString->inner + pos, 0, sizeof(char) * (pString->len - pos));
    pString->len = pos;
}

void dyns_clear_string_before_pos(DynString* pString, size_t pos) {
    if (!pString)
        return;

    pos = pos >= pString->len ? pString->len : pos;

    memmove(pString->inner, pString->inner + pos, sizeof(char) * (pString->len - pos));
    memset(pString->inner + pString->len - pos, 0, sizeof(char) * pos);
    pString->len -= pos;
}

void dyns_erase(DynString* pString) {
    dyns_clear_string_after_pos(pString, 0);
}

const char* dyns_get_cstr(const DynString* const pString) {
    if (!pString)
        return NULL;
    return pString->inner;
}
size_t dyns_get_len(const DynString* const pString) {
    if (!pString)
        return 0;
    return pString->len;
}

size_t dyns_get_capacity(const DynString* const pString) {
    if (!pString)
        return 0;
    return pString->capacity;
}

#endif  // DYN_STRING_IMPLEMENTATION

#endif  // DYN_STRING_H_
