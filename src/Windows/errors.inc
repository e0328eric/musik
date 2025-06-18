section ".text" code readable executable

invalidArgument:
    invoke MessageBoxW, 0, invalid_argument_msg, msg_title, MB_OK or MB_ICONERROR
    lea rcx, [usage]
    call printf
    jmp exitFailure

termInitFailed:
    invoke MessageBoxW, 0, term_err_str, msg_title, MB_OK or MB_ICONERROR
    jmp exitFailure

;; NOTE: must jump here using `jmp` instruction not `call` or `invoke` macro
;; [in] r12 <- musik errorkind
musikInitFailed:
    cmp  r12, MUSIK_ERROR_KIND_COUNT
    ja   exitFailure
    dec  r12
    lea  rax, [musikInitFailed.errkind]
    jmp  QWORD [rax + r12 * 8]
.openFileFailed:
    push rbp
    mov  rbp, rsp
    sub  rsp, 20h
    ;;invoke MessageBoxW, 0, open_file_failed_msg, msg_title, MB_OK or MB_ICONERROR
    leave
    jmp  exitFailure
.initDeviceFailed:
.startDeviceFailed:
.stopDeviceFailed:
.exit:
    jmp  exitFailure
.errkind:
    dq .openFileFailed
    dq .initDeviceFailed
    dq .startDeviceFailed
    dq .stopDeviceFailed
    dq exitFailure

section ".errd" data readable
msg_title            du "Musik Error", 0
invalid_argument_msg du "Invalid argument was given. See the console for more "
                     du "information.", 0
term_err_str         du "Failed to enter terminal raw mode", 0
