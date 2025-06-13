;; [in]  rdi <- string pointer (utf-16 string)
;; [in]  rsi <- string length (in UTF-16 bytes)
;; [out] nothing
printStrW:
    push  rbp
    mov   rbp, rsp
    sub   rsp, 18h
    invoke GetStdHandle, STD_OUTPUT_HANDLE
    invoke WriteConsoleW, rax, rdi, rsi, 0, 0
    leave
    ret

;; [in]  rdi <- string pointer (utf-16 string)
;; [out] rax -> returns number of bytes printed
printStrWNul:
    call strlenW
    mov  rsi, rax
    call printStrW
    ret

;; [in]  rdi <- character to print
;; [out] nothing
printChar:
    push rbp
    mov  rbp, rsp
    sub  rsp, 18h
    mov  BYTE [rbp - 8], dil
    invoke GetStdHandle, STD_OUTPUT_HANDLE
    mov  rdi, rbp
    sub  rdi, 8h
    invoke WriteConsoleA, rax, rdi, 1, 0, 0
    leave
    ret

;; [in]  rdi <- *unsigned* integer to print
;; [out] nothing
printNumber:
    push rdi
    push r8
    push r12
    test rdi, rdi
    jns  @F
    push rdi
    mov  dil, 45 ;; '-'
    call printChar
    pop  rdi
    neg  rdi
@@:
    xor  r8,  r8
    mov  r12, 14757395258967641293
.rev_digit:
    mov  rax, r8
    imul rax, rax, 10
    xchg rax, rdi
    mov  r8,  rax
    mul  r12
    shr  rdx, 3
    imul rax, rdx, 10
    sub  r8,  rax
    add  r8,  rdi
    mov  rdi, rdx
    cmp  rdi, 0
    jg   .rev_digit
.print:
    mov  rax, r8
    mul  r12
    shr  rdx, 3
    imul rax, rdx, 10
    sub  r8,  rax
    xchg r8,  rdx
    add  rdx, 48 ;; '0'
    mov  dil, dl
    push r8
    call printChar
    pop  r8
    cmp  r8,  0
    jg   .print
    pop  r12
    pop  r8
    pop  rdi
    ret

;; [in]  rdi <- string pointer (utf-16 string)
;; [out] rax -> a length of the string, or -1 if [in] is NULL
strlenW:
    push  rdi
    test  rdi, rdi
    jnz   @F
    mov   rax, -1
    ret
@@:
    push  rcx
    or    rcx, -1  ;; makes rcx = max(u64)
    xor   ax, ax
    repne scasw    ;; find AX byte starting at [RDI] (here, ax = \0)
    neg   rcx
    sub   rcx, 2
    mov   rax, rcx
    pop   rcx
    pop   rdi
    ret


