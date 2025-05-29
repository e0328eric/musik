const std = @import("std");
const musik = @cImport({
    @cInclude("musik.h");
});
const vaxis = @import("vaxis");

const ArrayList = std.ArrayList;
const Key = vaxis.Key;
const Cell = vaxis.Cell;

pub fn main() !void {
    const allocator = std.heap.c_allocator;

    const args = try std.process.argsAlloc(allocator);
    if (args.len < 2) {
        std.debug.print("Usage: {s} <wavfile>\n", .{args[0]});
        return error.InvalidArgs;
    }
    const wavfile = args[1];

    ////////////////////////////////////////////////////////////////////////////
    ////                        Vaxis Initialization                        ////
    ////////////////////////////////////////////////////////////////////////////

    var tty = try vaxis.Tty.init();
    defer tty.deinit();

    var vx = try vaxis.init(allocator, .{});
    defer vx.deinit(allocator, tty.anyWriter());

    var loop: vaxis.Loop(Event) = .{
        .tty = &tty,
        .vaxis = &vx,
    };
    try loop.init();
    try loop.start();
    defer loop.stop();

    // enter alt screen
    try vx.enterAltScreen(tty.anyWriter());
    defer vx.exitAltScreen(tty.anyWriter()) catch @panic("failed to exit altscreen");

    // Sends queries to terminal to detect certain features. This should always
    // be called after entering the alt screen, if you are using the alt screen
    try vx.queryTerminal(tty.anyWriter(), 1 * std.time.ns_per_s);

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

    var is_pause = false;
    mainLoop: while (true) {
        while (loop.tryEvent()) |event| {
            switch (event) {
                .key_press => |key| {
                    if (key.matches(Key.space, .{})) {
                        if (is_pause) {
                            _ = musik.startMusik(&device);
                            is_pause = false;
                        } else {
                            _ = musik.stopMusik(&device);
                            is_pause = true;
                        }
                    } else if (key.matches('q', .{})) {
                        return;
                    }
                },
                .winsize => |ws| try vx.resize(allocator, tty.anyWriter(), ws),
                else => {},
            }
        }

        _ = musik.getCurrentLen(&curr_pos, &device);
        if (music_len - curr_pos < 1e-7) break :mainLoop;

        const win = vx.window();
        win.clear();

        buf.clearRetainingCapacity();
        try buf.writer().print("{} / {}", .{ curr_pos, music_len });

        const debug_win = win.child(.{
            .height = 1,
            .x_off = 20,
            .y_off = 20,
        });

        _ = debug_win.print(&.{
            .{ .text = " Space : play/stop  q : quit " },
            .{ .text = buf.items },
        }, .{ .wrap = .grapheme });

        try vx.render(tty.anyWriter());
        std.time.sleep(std.time.ns_per_ms * 50);
    }
}

const Event = union(enum) {
    key_press: vaxis.Key,
    winsize: vaxis.Winsize,
    focus_in,
};
