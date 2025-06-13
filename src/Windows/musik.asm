format MS64 COFF
public main

include "macro/struct.inc"
include "encoding/utf8.inc" ; `du` converts utf8 into windows wchar_t type
include "winapi.inc"
include "libc.inc"

include "structs.inc"
include "utility.inc"
include "terminal.inc"

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

main:
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

    call initTerminal

    mov  di, 20
    mov  si, 0
    call setCursorPos

    mov  rdi, QWORD [filename]
    call printStrWNul
    invoke MessageBoxW, 0, QWORD [filename], msg_title, MB_OK

    call deinitTerminal
    invoke ExitProcess, 0

invalid_argument:
    invoke MessageBoxW, 0, invalid_argument_msg, msg_title, MB_OK or MB_ICONERROR
    lea rcx, [usage]
    call printf
    jmp exit_failure

exit_failure:
    invoke ExitProcess, 1

section ".mdata" data readable
msg_title            du "Musik Error", 0
invalid_argument_msg du "Invalid argument was given. See the console for more "
                     du "information.", 0
term_err_str         du "Terminal handling failed.", 0

usage db "usage: musik [-L] <wav filename>", 10, 0

section ".mglobal" data readable writable
argc     dd ?
argv     dq ?
filename dq ?

