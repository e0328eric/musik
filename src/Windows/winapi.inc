extrn CommandLineToArgvW
extrn ExitProcess
extrn GetCommandLineW
extrn GetConsoleMode
extrn GetStdHandle
extrn MessageBoxW
extrn SetConsoleCursorPosition
extrn SetConsoleMode
extrn WriteConsoleA
extrn WriteConsoleW

;; windows constansts
STD_INPUT_HANDLE                   = 0xFFFFFFF6
STD_OUTPUT_HANDLE                  = 0xFFFFFFF5
STD_ERROR_HANDLE                   = 0xFFFFFFF4
INVALID_HANDLE_VALUE               = -1

ENABLE_PROCESSED_INPUT             = 0x00000001
ENABLE_LINE_INPUT                  = 0x00000002
ENABLE_ECHO_INPUT                  = 0x00000004

ENABLE_PROCESSED_OUTPUT            = 0x00000001
ENABLE_VIRTUAL_TERMINAL_PROCESSING = 0x00000004
ENABLE_WINDOW_INPUT                = 0x00000008
ENABLE_MOUSE_INPUT                 = 0x00000010

MB_OK                              = 0x00000000
MB_ICONERROR                       = 0x00000010

NULL                               = 0

include "macro/proc64.inc"

;; since we are using winapi functions through extrn keyword, invoke macro
;; should be same with fastcall macro
macro invoke proc,[arg] {
    common fastcall proc, arg
}

