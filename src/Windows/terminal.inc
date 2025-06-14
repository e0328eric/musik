section ".text" code readable executable

;; [in]  nothing
;; [out] nothing
initTerminal:
    invoke GetStdHandle, STD_INPUT_HANDLE
    cmp  rax, INVALID_HANDLE_VALUE
    je   termErr
    mov  QWORD [term.win_stdin], rax
    invoke GetStdHandle, STD_OUTPUT_HANDLE
    cmp  rax, INVALID_HANDLE_VALUE
    je   termErr
    mov  QWORD [term.win_stdout], rax

    invoke GetConsoleMode, QWORD [term.win_stdin], term.stdin_orig_mode
    invoke GetConsoleMode, QWORD [term.win_stdout], term.stdout_orig_mode
    mov  edi, DWORD [term.stdout_orig_mode]

    ;; Optionally disable Ctrl+C handling
    mov  eax, ENABLE_ECHO_INPUT or ENABLE_LINE_INPUT or ENABLE_PROCESSED_INPUT
    not  eax
    and  edi, eax
    mov  eax, ENABLE_PROCESSED_OUTPUT or ENABLE_VIRTUAL_TERMINAL_PROCESSING
    or   edi, eax
    invoke SetConsoleMode, QWORD [term.win_stdout], edi

    mov  edi, ENABLE_WINDOW_INPUT or ENABLE_MOUSE_INPUT or ENABLE_PROCESSED_INPUT
    invoke SetConsoleMode, QWORD [term.win_stdin], edi

    invoke WriteConsoleA, QWORD [term.win_stdout], HIDE_CURSOR, HIDE_CURSOR.len, 0
    invoke WriteConsoleA, QWORD [term.win_stdout], ENTER_ALT_SCREEN_BUF, ENTER_ALT_SCREEN_BUF.len, 0
    invoke WriteConsoleA, QWORD [term.win_stdout], CLEAR_ENTIRE_SCREEN, CLEAR_ENTIRE_SCREEN.len, 0
    ret

;; [in]  nothing
;; [out] nothing
deinitTerminal:
    invoke WriteConsoleA, QWORD [term.win_stdout], EXIT_ALT_SCREEN_BUF, EXIT_ALT_SCREEN_BUF.len, 0
    invoke WriteConsoleA, QWORD [term.win_stdout], SHOW_CURSOR, SHOW_CURSOR.len, 0
    invoke SetConsoleMode, QWORD [term.win_stdin], DWORD [term.stdin_orig_mode]
    invoke SetConsoleMode, QWORD [term.win_stdout], DWORD [term.stdout_orig_mode]
    ret

;; [in]  cx <- x coordinate
;; [in]  dx <- y coordinate
;; [out] nothing
setCursorPos:
    push rbp
    mov  rbp, rsp
    sub  rsp, 18h

    mov  WORD [rbp - 8], cx
    mov  WORD [rbp - 10], dx
    mov  eax, DWORD [rbp - 10]
    invoke SetConsoleCursorPosition, QWORD [term.win_stdout], eax
    leave
    ret

termErr:
    invoke MessageBoxW, 0, term_err_str, msg_title, MB_OK or MB_ICONERROR
    jmp exit_failure

section ".termd" data readable
SHOW_CURSOR           string 1bh, "[?25h"
HIDE_CURSOR           string 1bh, "[?25l"
ENTER_ALT_SCREEN_BUF  string 1bh, "[?1049h"
EXIT_ALT_SCREEN_BUF   string 1bh, "[?1049l"
CLEAR_ENTIRE_SCREEN   string 1bh, "[2J"

section ".termb" data readable writable
term Terminal


