#include <stdio.h>
#include <assert.h>

#define CLPARSE_IMPLEMENTATION
#define NOT_ALLOW_EMPTY_ARGUMENT
#include <clparse.h>

#include "music_player.h"
#include "terminal.h"
#include "render.h"

int main(int argc, char** argv) {
    int result = 0;
    clparseStart("musik", "simple tui music player");
    
    // TODO: Allow using unicode input
    const ArrayList* filenames = clparseMainArg("FILENAME", "wav filename", NO_SUBCMD);
    const bool* is_loop = clparseBool("loop", 'L', false, "loop music", NO_SUBCMD);

    if (!clparseParse(argc, argv)) {
        fprintf(stderr, "ERROR: failed to parse commandline. %s\n", clparseGetErr());
        result = 1;
        goto CLEAN_CLPARSE;
    }

    if (clparseIsHelp()) {
        clparsePrintHelp();
        goto CLEAN_CLPARSE;
    }

    assert(filenames->kind == ARRAY_LIST_STRING);
    if (filenames->len != 1) {
        fprintf(stderr, "For now, only one song can be played\n");
        result = 1;
        goto CLEAN_CLPARSE;
    }

    const char* filename = *(const char**)filenames->items;
    Musik musik;
    if (initMusik(&musik, filename) != MUSIK_OK) {
        result = 1;
        goto CLEAN_CLPARSE;
    }

    Terminal* term = initTerminal();
    if (!term) {
        fprintf(stderr, "ERROR: cannot set terminal into raw mode\n");
        result = 1;
        goto DEINIT_MUSIK;
    }
    setCursorPos(term, (Cursor){ .x = 10, .y = 10});

    double music_len, curr_pos = 0.0;
    if (getTotalLen(&musik, &music_len) != MUSIK_OK) goto DEINIT_TERM;

    int code;
    bool is_pressed, is_paused = true;
    for (;;) {
        getKeyCode(term, &code, &is_pressed);

        // code must be capital letter
        if (is_pressed && code == 'Q') break;
        else if (is_pressed && code == ' ') {
            if (is_paused) startMusik(&musik); else stopMusik(&musik);
            is_paused = !is_paused;
        }
        else if (is_pressed && code == VK_LEFT) moveBySeconds(&musik, -2.0);
        else if (is_pressed && code == VK_RIGHT) moveBySeconds(&musik, 2.0);

        if (getCurrentLen(&musik, &curr_pos) != MUSIK_OK) goto DEINIT_TERM;
        if (music_len - curr_pos < 1e-7) {
            if (*is_loop) {
                setCurrentLen(&musik, 0.0);
            } else {
                return;
            }
        }

        if (!drawMusik(term, music_len, curr_pos)) {
            fprintf(stderr, "ERROR: rendering failed\n");
            goto DEINIT_TERM;
        }

        musikSleep(50);
    }

DEINIT_TERM:
    deinitTerminal(term);

DEINIT_MUSIK:
    deinitMusik(&musik);

CLEAN_CLPARSE:
    clparseClose();

    return result;
}

