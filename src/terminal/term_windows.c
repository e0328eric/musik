#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../terminal.h"

#define SHOW_CURSOR           "\x1b[?25h"
#define HIDE_CURSOR           "\x1b[?25l"
#define ENTER_ALT_SCREEN_BUF  "\x1b[?1049h"
#define EXIT_ALT_SCREEN_BUF   "\x1b[?1049l"
#define CLEAR_ENTIRE_SCREEN          "\x1b[2J"

struct Terminal {
    HANDLE win_stdin;
    HANDLE win_stdout;
    DWORD stdin_orig_mode;
    DWORD stdout_orig_mode;
};

Terminal* initTerminal(void) {
    Terminal* self = malloc(sizeof(Terminal));

    if ((self->win_stdin = GetStdHandle(STD_INPUT_HANDLE)) == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Cannot get the stdin handle\n");
        goto EXIT_ON_FAILURE;
    }
    if ((self->win_stdout = GetStdHandle(STD_OUTPUT_HANDLE)) == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Cannot get the stdout handle\n");
        goto EXIT_ON_FAILURE;
    }
    GetConsoleMode(self->win_stdin, &self->stdin_orig_mode);
    GetConsoleMode(self->win_stdout, &self->stdout_orig_mode);
    DWORD raw_mode = self->stdout_orig_mode;

    // Optionally disable Ctrl+C handling
    raw_mode &= ~(ENABLE_ECHO_INPUT | ENABLE_LINE_INPUT | ENABLE_PROCESSED_INPUT);
    raw_mode &= ~ENABLE_PROCESSED_INPUT;

    SetConsoleMode(self->win_stdin, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT | ENABLE_PROCESSED_INPUT);
    SetConsoleMode(self->win_stdout, raw_mode);

    // hide cursor (ANSI code does not work)
    CONSOLE_CURSOR_INFO cursor_info;
    GetConsoleCursorInfo(self->win_stdout, &cursor_info);
    cursor_info.bVisible = FALSE;
    SetConsoleCursorInfo(self->win_stdout, &cursor_info);

    fprintf(stdout, ENTER_ALT_SCREEN_BUF);
    fprintf(stdout, CLEAR_ENTIRE_SCREEN);
    fflush(stdout);

    return self;

EXIT_ON_FAILURE:
    free(self);
    return NULL;
}

void deinitTerminal(Terminal* self) {
    assert(self && "term should be non NULL");

    printf(EXIT_ALT_SCREEN_BUF);

    // show cursor (ANSI code does not work)
    CONSOLE_CURSOR_INFO cursor_info;
    GetConsoleCursorInfo(self->win_stdout, &cursor_info);
    cursor_info.bVisible = TRUE;
    SetConsoleCursorInfo(self->win_stdout, &cursor_info);

    SetConsoleMode(self->win_stdout, self->stdout_orig_mode);
    SetConsoleMode(self->win_stdin, self->stdin_orig_mode);
    free(self);
}

void getKeyCode(const Terminal* self, int* keycode, bool* is_pressed) {

    INPUT_RECORD buf;
    DWORD read_count;

    // ReadConsoleInput do block the thread. So peeking is necessary to
    // make nonblocking one
    GetNumberOfConsoleInputEvents(self->win_stdin, &read_count);
    if (read_count > 0) {
        ReadConsoleInput(self->win_stdin, &buf, 1, &read_count);
    } else {
        *keycode = -1;
        return;
    }

    *keycode = buf.Event.KeyEvent.wVirtualKeyCode;
    *is_pressed = buf.Event.KeyEvent.bKeyDown;
}

bool getCursorPos(const Terminal* self, Cursor* output) {
    CONSOLE_SCREEN_BUFFER_INFO console_info;
    if (!GetConsoleScreenBufferInfo(self->win_stdout, &console_info)) {
        fprintf(stderr, "cannot get the console screen buffer info\n");
        return false;
    }
    output->x = console_info.dwCursorPosition.X;
    output->y = console_info.dwCursorPosition.Y;
    return true;
}

void setCursorPos(const Terminal* self, Cursor cursor) {
    SetConsoleCursorPosition(self->win_stdout, *(COORD*)&cursor);
}
