const std = @import("std");
const vaxis = @import("vaxis");

const Allocator = std.mem.Allocator;

pub const Event = union(enum) {
    key_press: vaxis.Key,
    winsize: vaxis.Winsize,
};

allocator: Allocator,
vx: *vaxis.Vaxis,
tty: *vaxis.Tty,
loop: *vaxis.Loop(Event),

const Self = @This();

pub fn init(
    allocator: Allocator,
    vx: *vaxis.Vaxis,
    tty: *vaxis.Tty,
    loop: *vaxis.Loop(Event),
) Self {
    return .{
        .allocator = allocator,
        .vx = vx,
        .tty = tty,
        .loop = loop,
    };
}

pub fn tick(self: *Self, render_data: RenderData) !void {
    try self.render(render_data);
    try self.vx.render(self.tty.anyWriter());
    std.time.sleep(16 * std.time.ns_per_ms);
}

pub const RenderData = struct {
    song_name: [:0]const u8,
    song_time: []const u8,
};

pub fn render(self: *Self, render_data: RenderData) !void {
    const win = self.vx.window();
    win.clear();

    const bottom_bar_style: vaxis.Style = .{
        .fg = .rgbFromUint(0x000000),
        .bg = .rgbFromUint(0xFFFFFF),
    };
    const bottom_bar = vaxis.widgets.alignment.bottomLeft(
        win,
        win.width,
        1,
    );
    _ = bottom_bar.fill(.{ .char = .{}, .style = bottom_bar_style });

    const song_name_padding_segment: vaxis.Segment = .{
        .text = " ",
        .style = bottom_bar_style,
    };
    const song_name_segment: vaxis.Segment = .{
        .text = render_data.song_name[0..],
        .style = bottom_bar_style,
    };
    const song_name = vaxis.widgets.alignment.bottomLeft(
        win,
        @min(@as(u16, @intCast(render_data.song_name.len)), win.width >> 1) + 1,
        1,
    );
    _ = song_name.print(&.{
        song_name_padding_segment,
        song_name_segment,
    }, .{ .wrap = .grapheme });

    const song_time_segment: vaxis.Segment = .{
        .text = render_data.song_time,
        .style = bottom_bar_style,
    };
    const song_time = vaxis.widgets.alignment.bottomRight(
        win,
        @as(u16, @intCast(render_data.song_time.len)) + 1,
        1,
    );
    _ = song_time.printSegment(song_time_segment, .{ .wrap = .grapheme });
}
