const std = @import("std");

const ArrayList = std.ArrayList;
const Musik = @import("./Musik.zig");
const Terminal = @import("./terminal/terminal.zig").Terminal;

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

    const term = Terminal.init();
    defer term.deinit();

    const music_len = try musik.getTotalLen();
    var curr_pos: f64 = 0.0;
    //var is_paused = true;
    var line_buf = try ArrayList(u8).initCapacity(alloc, 200);
    defer line_buf.deinit();

    while (true) {
        curr_pos = try musik.getCurrentLen();
        if (music_len - curr_pos < 1e-7) {
            if (is_loop) try musik.setCurrentLen(0.0) else break;
        }

        const total_time = getHumanTime(music_len);
        const curr_time = getHumanTime(curr_pos);

        line_buf.clearRetainingCapacity();
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
