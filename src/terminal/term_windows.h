#ifndef MUSIK_TUI_TERMINAL_WINDOWS_H_
#define MUSIK_TUI_TERMINAL_WINDOWS_H_

#include <stdbool.h>
#include <stdint.h>

typedef struct Terminal Terminal;

typedef struct {
    uint16_t x;
    uint16_t y;
} Cursor;

Terminal* initTerminal(void);
void deinitTerminal(Terminal* self);
void getKeyCode(const Terminal* self, int* keycode, bool* is_pressed, uint32_t timeout);
bool getCursorPos(const Terminal* self, Cursor* output);
void setCursorPos(const Terminal* self, Cursor cursor);

#endif // MUSIK_TUI_TERMINAL_WINDOWS_H_
