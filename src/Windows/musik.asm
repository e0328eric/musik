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

    invoke GetCommandLineW
    invoke CommandLineToArgvW, rax, argc
    mov  QWORD [argv], rax

    cmp  [argc], 2
    jl   invalid_argument

    dec  DWORD [argc]
    mov  rax, QWORD [argv]
    mov  ebx, DWORD [argc]
    imul ebx, ebx, 8
    mov  rcx, 32
    bzhi rbx, rbx, rcx
    add  rax, rbx
    mov  rbx, QWORD [rax]
    mov  QWORD [filename], rbx

    invoke MessageBoxW, 0, QWORD [filename], msg_title, MB_OK
    invoke PlaySoundW, 
    invoke ExitProcess, 0

invalid_argument:
    invoke MessageBoxW, 0, invalid_argument_msg, msg_title, MB_OK or MB_ICONERROR
    lea rdi, [_usage]
    call printAsciiStr
    jmp exit_failure

exit_failure:
    invoke ExitProcess, 1

section ".data" data readable
msg_title            du "Musik Error", 0
invalid_argument_msg du "Invalid argument was given. See the console for more "
                     du "information.", 0

_usage db 0dh, 0ah
       db "usage: musik [-L] <wav filename>", 0dh, 0ah, 0

section ".bss" data readable writable
argc     dd ?
argv     dq ?
filename dq ?

section ".idata" import data readable writable
library kernel32, "KERNEL32.DLL",\
        user32,   "USER32.DLL",\
        shell32,  "SHELL32.DLL",\
        winmm,    "WINMM.DLL"

include "api/kernel32.inc"
include "api/user32.inc"

import shell32,\
       CommandLineToArgvW, "CommandLineToArgvW"

