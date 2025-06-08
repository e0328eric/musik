#ifndef MUSIK_TUI_TERMINAL_H_
#define MUSIK_TUI_TERMINAL_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct Terminal Terminal;

// top left corner should be (0,0)
typedef struct {
    uint16_t x;
    uint16_t y;
} Cursor;

typedef struct {
    uint16_t width;
    uint16_t height;
} WinSize;

Terminal* initTerminal(void);
void deinitTerminal(Terminal* term);
void getKeyCode(const Terminal* term, int* keycode, bool* is_pressed);
bool getCursorPos(const Terminal* term, Cursor* output);
void setCursorPos(const Terminal* term, Cursor cursor);
bool getWinSize(const Terminal* term, WinSize* output);

#endif // MUSIK_TUI_TERMINAL_H_
