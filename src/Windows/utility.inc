;; string struct implementation
struc string [data] {
    common
    . db data
    .len = $ - .
}

;; [in]  rdi <- size of bytes to allocate
;; [out] rax -> allocated pointer
alloc:
    invoke VirtualAlloc, 0, rdi, MEM_COMMIT, PAGE_READWRITE
    ret

;; [in]  rdi <- pointer from alloc
;; [out] rax -> whether dealloc success or not
dealloc:
    invoke VirtualFree, rdi, 0, MEM_RELEASE
    ret

;; [in]  rdi <- string pointer (ascii)
;; [out] rax -> returns number of bytes printed
printStrANul:
    push  rbp
    mov   rbp, rsp
    sub   rsp, 18h
    invoke GetStdHandle, STD_OUTPUT_HANDLE
    mov   QWORD [rbp - 8], rax
    call  strlenA
    invoke WriteConsoleA, QWORD [rbp - 8], rdi, rax, 0, 0
    leave
    ret

;; [in]  rdi <- string pointer (utf-16)
;; [out] rax -> returns number of bytes printed
printStrWNul:
    push  rbp
    mov   rbp, rsp
    sub   rsp, 18h
    invoke GetStdHandle, STD_OUTPUT_HANDLE
    mov   QWORD [rbp - 8], rax
    call  strlenW
    invoke WriteConsoleW, QWORD [rbp - 8], rdi, rax, 0, 0
    leave
    ret

;; [in]  rdi <- string pointer
;; [in]  rsi <- string length (in ascii bytes)
;; [out] nothing
printStrA:
    push  rbp
    mov   rbp, rsp
    sub   rsp, 18h
    invoke GetStdHandle, STD_OUTPUT_HANDLE
    invoke WriteConsoleA, rax, rdi, rsi, 0, 0
    leave
    ret

;; [in]  rdi <- string pointer
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
    invoke WriteFile, rax, rdi, 1, DWORD [rbp - 10h], 0
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

;; [in]  rdi <- string pointer (const char*)
;; [out] rax -> a length of the string, or -1 if [in] is NULL
strlenA:
    push  rdi
    test  rdi, rdi
    jnz   @F
    mov   rax, -1
    ret
@@:
    push  rcx
    or    rcx, -1  ;; makes rcx = max(u64)
    xor   al, al
    repne scasb    ;; find AL byte starting at [RDI] (here, al = \0)
    neg   rcx
    sub   rcx, 2
    mov   rax, rcx
    pop   rcx
    pop   rdi
    ret

;; [in]  rdi <- string pointer (const wchar_t*)
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

;; [in]  rdi <- null terminated string
;; [in]  rsi <- null terminated string
;; [out] rax -> 1 if they are equal, 0 otherwise
strcmp:
    xor  rax, rax
    xor  rcx, rcx
    test rdi, rsi
    jz   .ret
@@:
    mov  bl, BYTE [rdi + rcx]
    mov  dl, BYTE [rsi + rcx]
    cmp  bl, dl
    sete al
    jne  strcmp.ret
    test bl, bl
    setz al
    jz   strcmp.ret
    inc  rcx
    jmp  @B
.ret:
    ret
