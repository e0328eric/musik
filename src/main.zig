const std = @import("std");
const vaxis = @import("vaxis");

const ArrayList = std.ArrayList;
const Musik = @import("./Musik.zig");

const Event = union(enum) {
    key_press: vaxis.Key,
    winsize: vaxis.Winsize,
};

pub fn main() !void {
    const alloc = std.heap.page_allocator;
    var zlap = try @import("zlap").Zlap(@embedFile("./commands.zlap")).init(alloc);
    defer zlap.deinit();

    if (zlap.is_help) {
        std.debug.print("{s}\n", .{zlap.help_msg});
        return;
    }

    const filename = zlap.main_args.get("FILENAME").?.value.string;
    const is_loop = zlap.main_flags.get("loop").?.value.bool;

    if (filename.len == 0) {
        std.debug.print("{s}\n", .{zlap.help_msg});
        return error.FilenameNotGiven;
    }

    var musik = try Musik.init(alloc, filename);
    defer musik.deinit();

    var tty = try vaxis.Tty.init();
    defer tty.deinit();

    var vx = try vaxis.init(alloc, .{});
    defer vx.deinit(alloc, tty.anyWriter());

    var loop: vaxis.Loop(Event) = .{
        .tty = &tty,
        .vaxis = &vx,
    };
    try loop.init();
    try loop.start();
    defer loop.stop();

    // exitAltScreen is called when vx.deinit is called
    try vx.enterAltScreen(tty.anyWriter());
    try vx.queryTerminal(tty.anyWriter(), 1 * std.time.ns_per_s);

    const music_len = try musik.getTotalLen();
    var curr_pos: f64 = 0.0;
    var is_paused = true;
    var line_buf = try ArrayList(u8).initCapacity(alloc, 200);
    defer line_buf.deinit();

    while (true) {
        if (loop.tryEvent()) |event| {
            switch (event) {
                .key_press => |key| {
                    if (key.matches('q', .{})) {
                        break;
                    } else if (key.matches(' ', .{})) {
                        try if (is_paused) musik.start() else musik.stop();
                        is_paused = !is_paused;
                    }
                },
                .winsize => |ws| try vx.resize(alloc, tty.anyWriter(), ws),
            }
        }

        curr_pos = try musik.getCurrentLen();
        if (music_len - curr_pos < 1e-7) {
            if (is_loop) try musik.setCurrentLen(0.0) else break;
        }

        const win = vx.window();
        win.clear();

        const info_bar_style: vaxis.Style = .{
            .fg = vaxis.Color.rgbFromUint(0x000000),
            .bg = vaxis.Color.rgbFromUint(0xFFFFFF),
        };
        const info_bar = win.child(.{
            .x_off = 0,
            .y_off = win.height -| 1,
            .width = win.width,
            .height = 1,
        });

        const total_time = getHumanTime(music_len);
        const curr_time = getHumanTime(curr_pos);

        line_buf.clearRetainingCapacity();
        const col_offset = win.width -| 21;
        try line_buf.writer().writeByteNTimes(' ', col_offset);
        try line_buf.writer().print(
            "{d:0>2}:{d:0>2}:{d:0>2} / {d:0>2}:{d:0>2}:{d:0>2}",
            .{
                curr_time.h,
                curr_time.m,
                curr_time.s,
                total_time.h,
                total_time.m,
                total_time.s,
            },
        );
        try line_buf.writer().writeByteNTimes(' ', 2);
        //std.debug.assert(line_buf.len == win.width);

        _ = info_bar.printSegment(.{
            .text = line_buf.items,
            .style = info_bar_style,
        }, .{ .wrap = .grapheme });

        try vx.render(tty.anyWriter());

        std.time.sleep(50 * std.time.ns_per_ms);
    }
}

fn getHumanTime(raw_time: f64) struct { h: u16, m: u8, s: u8 } {
    const raw_time_trunc: u32 = @intFromFloat(raw_time);

    const h: u16 = @intCast(raw_time_trunc / 3600);
    const m: u8 = @intCast(raw_time_trunc % 3600 / 60);
    const s: u8 = @intCast(raw_time_trunc % 60);

    return .{
        // what kind of music is so that long?
        .h = if (h > 99) 99 else h,
        .m = m,
        .s = s,
    };
}
