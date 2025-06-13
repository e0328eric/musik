;; string struct implementation
struc string [data] {
    common
    . db data
    .len = $ - .
}

;; Terminal Struct
struct Terminal
    win_stdin        dq ?
    win_stdout       dq ?
    stdin_orig_mode  dd ?
    stdout_orig_mode dd ?
ends

