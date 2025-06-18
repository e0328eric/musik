format MS64 COFF
public main

include "macro/struct.inc"
include "encoding/utf8.inc" ; `du` converts utf8 into windows wchar_t type
include "winapi.inc"
include "libc.inc"

include "structs.inc"
include "errors.inc"
include "utility.inc"
include "terminal.inc"
include "miniaudio.inc"

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

section ".text" code readable executable

main:
    ;; at least 32 bytes of stack must be allocated even if the function
    ;; does not use more than four arguments
    ;; in addition, the stack will always be maintained 16-byte aligned
    ;; 
    ;; Reference:
    ;; https://learn.microsoft.com/en-us/cpp/build/stack-usage?view=msvc-170
    push rbp
    mov  rbp, rsp
    sub  rsp, 30h

    lea  rcx, [rbp - 8h]
    mov  rdx, 12
    lea  r8, [fmt_str]
    lea  r9,  [foo]
    call sprintf_s

    lea rcx, [rbp - 8h]
    call printf
    leave
    xor rax, rax
    ret

    invoke GetCommandLineW
    invoke CommandLineToArgvW, rax, argc
    mov  QWORD [argv], rax

    cmp  [argc], 2
    jl   invalidArgument

    dec  DWORD [argc]
    mov  rax, QWORD [argv]
    mov  ebx, DWORD [argc]
    imul ebx, ebx, 8
    mov  rcx, 32
    bzhi rbx, rbx, rcx
    add  rax, rbx
    mov  rbx, QWORD [rax]
    mov  QWORD [filename], rbx
    invoke printStrWNul, QWORD [filename]
    leave
    xor rax, rax
    ret

    invoke initTerminal
    invoke setCursorPos, 20, 0
    invoke printStrWNul, QWORD [filename]

    invoke setCursorPos, 30, 20
    invoke printNumber, -1234

    invoke setCursorPos, 0, 20
    invoke printNumber, 2024010745

    invoke MessageBoxW, 0, QWORD [filename], msg_title, MB_OK

    invoke deinitTerminal
    leave
    xor  rax, rax
    ret
;; every errors must jump to here
exitFailure:
    leave
    mov  rax, 1
    ret

section ".mdata" data readable
usage   db "usage: musik [-L] <wav filename>", 10, 0
fmt_str db "%s", 10, 0
foo     db "Hello?", 0

section ".mglobal" data readable writable
argc     dd ?
argv     dq ?
filename dq ?

