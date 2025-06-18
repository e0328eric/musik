section ".text" code readable executable

;; [in]  rcx <- string pointer (utf-16 string)
;; [in]  rdx <- string length (in UTF-16 bytes)
;; [out] rax -> returns number of bytes printed
printStrW:
    push rcx
    push rdx
    invoke GetStdHandle, STD_OUTPUT_HANDLE
    pop  rsi
    pop  rdi
    invoke WriteConsoleW, rax, rdi, rsi, 0, 0
    ret

;; [in]  rcx <- string pointer (utf-16 string)
;; [out] rax -> returns number of bytes printed
printStrWNul:
    invoke strlenW, rcx
    invoke printStrW, rcx, rax
    ret

;; [in]  rcx <- character to print
;; [out] nothing
printChar:
    push rbp
    mov  rbp, rsp
    sub  rsp, 20h
    mov  BYTE [rbp - 8], cl
    invoke GetStdHandle, STD_OUTPUT_HANDLE
    mov  rdi, rbp
    sub  rdi, 8h
    invoke WriteConsoleA, rax, rdi, 1, 0, 0
    leave
    ret

;; [in]  rcx <- *unsigned* integer to print
;; [out] nothing
printNumber:
    push rcx
    push r8
    push r12
    test rcx, rcx
    jns  @F
    push rcx
    mov  cl, 45 ;; '-'
    call printChar
    pop  rcx
    neg  rcx
@@:
    xor  r8,  r8
    mov  r12, 14757395258967641293
.rev_digit:
    mov  rax, r8
    imul rax, rax, 10
    xchg rax, rcx
    mov  r8,  rax
    mul  r12
    shr  rdx, 3
    imul rax, rdx, 10
    sub  r8,  rax
    add  r8,  rcx
    mov  rcx, rdx
    cmp  rcx, 0
    jg   .rev_digit
.print:
    mov  rax, r8
    mul  r12
    shr  rdx, 3
    imul rax, rdx, 10
    sub  r8,  rax
    xchg r8,  rdx
    add  rdx, 48 ;; '0'
    mov  cl, dl
    push r8
    call printChar
    pop  r8
    cmp  r8,  0
    jg   .print
    pop  r12
    pop  r8
    pop  rcx
    ret

;; [in]  rcx <- string pointer (utf-16 string)
;; [out] rax -> a length of the string, or -1 if [in] is NULL
strlenW:
    push  rcx
    test  rcx, rcx
    jnz   @F
    mov   rax, -1
    ret
@@:
    mov   rdi, rcx
    or    rcx, -1  ;; makes rcx = max(u64)
    xor   ax, ax
    repne scasw    ;; find AX byte starting at [RDI] (here, ax = \0)
    neg   rcx
    sub   rcx, 2
    mov   rax, rcx
    pop   rcx
    ret

