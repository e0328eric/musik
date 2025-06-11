format PE64 console
entry _start

include "win64w.inc"
include "encoding/utf8.inc" ; `du` converts utf8 into windows wchar_t type

include "utility.inc"

;;------------------------------------------------------------------------------
;; Windows x64 calling convention
;;
;; Parameter type                fifth and higher   fourth  third  second  leftmost
;; floating-point                      stack        XMM3    XMM2   XMM1    XMM0
;; integer                             stack        R9      R8     RDX     RCX
;; Aggregates (8 ~ 64 bits) and __m64  stack        R9      R8     RDX     RCX
;; Other aggregates, as pointer        stack        R9      R8     RDX     RCX
;; __m128, as a pointer                stack        R9      R8     RDX     RCX
;;------------------------------------------------------------------------------

;; NOTE: In this program, every function uses UNIX x86_64 calling convention.
;; which is, use registers rdi, rsi, rdx, rcx, r8, r9

section ".text" code readable executable

_start:
    ;; at least 32 bytes of stack must be allocated even if the function
    ;; does not use more than four arguments
    sub  rsp, 28h

    lea  rdi, [test_print]
    call printAsciiStr

    mov rdi, -123412
    call printNumber

    mov dil, 10
    call printChar

    lea rdi, [sz_text]
    mov rsi, sz_text_len
    call printStr

    invoke GetCommandLineW
    invoke CommandLineToArgvW, rax, argc
    mov QWORD [argv], rax

    cmp [argc], 2
    jne failed

    mov rax, QWORD [argv]
    invoke MessageBoxW, 0, sz_text, QWORD [rax + 8], MB_OK or MB_ICONERROR

    mov rdi, 100
    call alloc
    mov rdi, rax
    call dealloc

    invoke ExitProcess, 0

failed:
    invoke MessageBoxW, 0, sz_text, sz_text, MB_OK
    invoke ExitProcess, 1

section ".data" data readable
test_print db "Hello, World!", 10, 0
sz_text du "간단한 한글", 0
sz_text_len = $ - sz_text

section ".bss" data readable writable
argc dd ?
argv dq ?

section ".idata" import data readable writable
library kernel32, "KERNEL32.DLL",\
        user32,   "USER32.DLL",\
        shell32,  "SHELL32.DLL"

include "api/kernel32.inc"
include "api/user32.inc"

import shell32,\
       CommandLineToArgvW, "CommandLineToArgvW"

