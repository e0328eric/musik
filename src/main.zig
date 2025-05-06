const std = @import("std");
const rl = @import("raylib");
const vaxis = @import("vaxis");

const ArrayList = std.ArrayList;
const Renderer = @import("./Renderer.zig");
const Music = @import("./Music.zig");

pub fn main() !void {
    var gpa = std.heap.GeneralPurposeAllocator(.{}){};
    defer _ = gpa.deinit();
    const allocator = gpa.allocator();

    var tty = try vaxis.Tty.init();
    defer tty.deinit();

    var vx = try vaxis.init(allocator, .{});
    defer vx.deinit(allocator, tty.anyWriter());

    var loop: vaxis.Loop(Renderer.Event) = .{ .tty = &tty, .vaxis = &vx };
    try loop.init();
    try loop.start();
    defer loop.stop();

    try vx.enterAltScreen(tty.anyWriter());
    try vx.queryTerminal(tty.anyWriter(), 1 * std.time.ns_per_s);

    try vx.queryColor(tty.anyWriter(), .fg);
    try vx.queryColor(tty.anyWriter(), .bg);

    var song_time = ArrayList(u8).init(allocator);
    defer song_time.deinit();

    var music = try Music.init(allocator, "test.flac");
    defer music.deinit();

    const thread = try std.Thread.spawn(.{}, Music.play, .{&music});
    _ = thread;

    var renderer = Renderer.init(allocator, &vx, &tty, &loop);

    var pause = true;

    while (true) {
        while (loop.tryEvent()) |event| {
            switch (event) {
                .key_press => |key| {
                    if (key.matches('c', .{ .ctrl = true })) {
                        music.exit();
                        return;
                    } else if (key.matches(vaxis.Key.space, .{})) {
                        pause = !pause;
                        if (pause) music.pause() else music.@"resume"();
                    } else if (key.matches(vaxis.Key.left, .{})) {
                        const time_len, const time_played = music.timeInfo();
                        music.rewind(@min(@max(time_played - 5.0, 0.0), time_len));
                    } else if (key.matches(vaxis.Key.right, .{})) {
                        const time_len, const time_played = music.timeInfo();
                        music.rewind(@min(time_played + 5.0, time_len));
                    }
                },
                .winsize => |ws| try vx.resize(allocator, tty.anyWriter(), ws),
            }
        }

        song_time.clearRetainingCapacity();
        const writer = song_time.writer();
        {
            const time_len, const time_played = music.timeInfo();
            const time_len_info = timeToHMS(time_len);
            const time_played_info = timeToHMS(time_played);
            try writer.print(
                "{:02}:{:02}:{:02} / {:02}:{:02}:{:02}",
                .{
                    time_played_info.hour,
                    time_played_info.min,
                    time_played_info.sec,
                    time_len_info.hour,
                    time_len_info.min,
                    time_len_info.sec,
                },
            );
        }

        try renderer.tick(.{
            .song_name = music.song_name,
            .song_time = song_time.items,
        });
    }
}

fn timeToHMS(raw_time: f32) struct { hour: usize, min: usize, sec: usize } {
    const raw_time_int = @as(u64, @intFromFloat(raw_time));
    const raw_time_min = raw_time_int / 60;

    return .{
        .hour = raw_time_min / 60,
        .min = raw_time_min % 60,
        .sec = raw_time_int % 60,
    };
}
