#include <stdio.h>

#include "render.h"

#define ANSI "\x1b["

typedef enum {
    // Text styles
    ANSI_RESET = 0,
    ANSI_BOLD = 1,
    ANSI_DIM = 2,
    ANSI_ITALIC = 3,
    ANSI_UNDERLINE = 4,
    ANSI_BLINK = 5,
    ANSI_REVERSE = 7,
    ANSI_HIDDEN = 8,
    ANSI_STRIKETHROUGH = 9,

    // Foreground colors
    ANSI_FG_BLACK = 30,
    ANSI_FG_RED = 31,
    ANSI_FG_GREEN = 32,
    ANSI_FG_YELLOW = 33,
    ANSI_FG_BLUE = 34,
    ANSI_FG_MAGENTA = 35,
    ANSI_FG_CYAN = 36,
    ANSI_FG_WHITE = 37,
    ANSI_FG_BRIGHT_BLACK = 90,
    ANSI_FG_BRIGHT_RED = 91,
    ANSI_FG_BRIGHT_GREEN = 92,
    ANSI_FG_BRIGHT_YELLOW = 93,
    ANSI_FG_BRIGHT_BLUE = 94,
    ANSI_FG_BRIGHT_MAGENTA = 95,
    ANSI_FG_BRIGHT_CYAN = 96,
    ANSI_FG_BRIGHT_WHITE = 97,

    // Background colors
    ANSI_BG_BLACK = 40,
    ANSI_BG_RED = 41,
    ANSI_BG_GREEN = 42,
    ANSI_BG_YELLOW = 43,
    ANSI_BG_BLUE = 44,
    ANSI_BG_MAGENTA = 45,
    ANSI_BG_CYAN = 46,
    ANSI_BG_WHITE = 47,
    ANSI_BG_BRIGHT_BLACK = 100,
    ANSI_BG_BRIGHT_RED = 101,
    ANSI_BG_BRIGHT_GREEN = 102,
    ANSI_BG_BRIGHT_YELLOW = 103,
    ANSI_BG_BRIGHT_BLUE = 104,
    ANSI_BG_BRIGHT_MAGENTA = 105,
    ANSI_BG_BRIGHT_CYAN = 106,
    ANSI_BG_BRIGHT_WHITE = 107,

    // Cursor controls (add with `\x1b[` and a number prefix as needed)
    ANSI_CURSOR_UP = 'A',
    ANSI_CURSOR_DOWN = 'B',
    ANSI_CURSOR_RIGHT = 'C',
    ANSI_CURSOR_LEFT = 'D',
    ANSI_CURSOR_NEXT_LINE = 'E',
    ANSI_CURSOR_PREV_LINE = 'F',
    ANSI_CURSOR_COLUMN = 'G',
    ANSI_CURSOR_POSITION = 'H',

    // Erase controls
    ANSI_ERASE_SCREEN_FROM_CURSOR = 0x4A,   // 0J
    ANSI_ERASE_SCREEN_TO_CURSOR = 0x4A + 1, // 1J
    ANSI_ERASE_SCREEN_ALL = 0x4A + 2,       // 2J
    ANSI_ERASE_LINE_FROM_CURSOR = 0x4B,     // 0K
    ANSI_ERASE_LINE_TO_CURSOR = 0x4B + 1,   // 1K
    ANSI_ERASE_LINE_ALL = 0x4B + 2,         // 2K
} AnsiOption;

typedef struct {
    uint16_t hour;
    uint8_t minute;
    uint8_t second;
} MusicTime;

static MusicTime convertMusicTime(double raw_time);

bool drawMusik(Terminal* term, double total_music_len, double curr_music_pos) {
    WinSize win;
    if (!getWinSize(term, &win)) return false;
    if (win.width < 25) {
        fprintf(stderr, "ERROR: terminal is too small\n");
        return false;
    }

    setCursorPos(term, (Cursor){ .x = 0, .y = 1});
    printf(ANSI"%dm""w: %d, h: %d", ANSI_RESET, win.width, win.height);

    // HH:MM:SS / HH:MM:SS
    char time_buf[20];
    MusicTime total_time = convertMusicTime(total_music_len);
    MusicTime curr_time = convertMusicTime(curr_music_pos);

    snprintf(time_buf, sizeof(time_buf), "%.2hu:%.2hhu:%.2hhu / %.2hu:%.2hhu:%.2hhu", 
             curr_time.hour, curr_time.minute, curr_time.second,
             total_time.hour, total_time.minute, total_time.second);

    const uint16_t start_to_write = win.width - sizeof(time_buf) - 2;
    setCursorPos(term, (Cursor){ .x = 0, .y = win.height - 1});
    printf(ANSI"%d;%dm", ANSI_FG_BLACK, ANSI_BG_WHITE);
    for (uint16_t i = 0; i < win.width; ++i) {
        if (i == start_to_write) {
            i += printf("%s", time_buf);
        } else {
            printf(" ");
        }
    }
    printf(ANSI"%dm", ANSI_RESET);

    fflush(stdout);

    return true;
}

static MusicTime convertMusicTime(double raw_time) {
    uint32_t raw_time_trunc = (uint32_t)raw_time;
    MusicTime output;

    output.hour = (uint16_t)(raw_time_trunc / 3600);
    output.minute = (uint8_t)(raw_time_trunc % 3600 / 60);
    output.second = (uint8_t)(raw_time_trunc % 60);

    // what kind of music is so that long?
    if (output.hour > 99) output.hour = 99;

    return output;
}
