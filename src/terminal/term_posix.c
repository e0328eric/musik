#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "../terminal.h"

#define SHOW_CURSOR           "\x1b[?25h"
#define HIDE_CURSOR           "\x1b[?25l"
#define ENTER_ALT_SCREEN_BUF  "\x1b[?1049h"
#define EXIT_ALT_SCREEN_BUF   "\x1b[?1049l"

struct Terminal {
};

Terminal* initTerminal(void) {
    assert(0 && "Noy yet implemented");
}

void deinitTerminal(Terminal* self) {
    assert(0 && "Noy yet implemented");
}


void getKeyCode(const Terminal* self, int* keycode, bool* is_pressed) {
    assert(0 && "Noy yet implemented");
}

bool getCursorPos(const Terminal* self, Cursor* output) {
    assert(0 && "Noy yet implemented");
}

void setCursorPos(const Terminal* self, Cursor cursor) {
    assert(0 && "Noy yet implemented");
}
