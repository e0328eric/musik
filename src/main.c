#include <stdio.h>

#include "terminal.h"

int main(int argc, char** argv) {
    Terminal* term = initTerminal();
    setCursorPos(term, (Cursor){ .x = 10, .y = 10});

    int code;
    bool is_pressed;
    for (;;) {
        getKeyCode(term, &code, &is_pressed, 0);
        if (is_pressed && code == 'q') break; else printf("%d", code);
    }

    deinitTerminal(term);
}

/*

pub fn main2() !void {
    const allocator = std.heap.c_allocator;

    const args = try std.process.argsAlloc(allocator);
    if (args.len < 2) {
        std.debug.print("Usage: {s} <wavfile>\n", .{args[0]});
        return error.InvalidArgs;
    }
    const wavfile = args[1];

    ////////////////////////////////////////////////////////////////////////////
    ////                      Initialize Musik Stream                       ////
    ////////////////////////////////////////////////////////////////////////////

    var device: musik.Musik = undefined;
    if (musik.initMusik(&device, wavfile.ptr) != musik.MUSIK_OK) {
        return error.Foo;
    }
    defer musik.uninitMusik(&device);

    ////////////////////////////////////////////////////////////////////////////
    ////                          Main Event Loop                           ////
    ////////////////////////////////////////////////////////////////////////////

    var music_len: f64 = undefined;
    var curr_pos: f64 = 0.0;

    _ = musik.getTotalLen(&music_len, &device);

    var buf = try ArrayList(u8).initCapacity(allocator, 50);
    defer buf.deinit();

    while (true) {
        _ = musik.getCurrentLen(&curr_pos, &device);
        if (music_len - curr_pos < 1e-7) return;

        std.time.sleep(std.time.ns_per_ms * 50);
    }
}

*/
