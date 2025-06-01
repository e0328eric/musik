const std = @import("std");
const win = @cImport({
    @cDefine("WIN32_LEAN_AND_MEAN", {});
    @cInclude("windows.h");
});

const Allocator = std.mem.Allocator;
const ArrayList = std.ArrayList;

const log = std.log;
const io = std.io;
const log10Int = std.math.log10_int;

win_stdout: win.HANDLE,
orig_mode: win.DWORD,
stdout: @TypeOf(io.bufferedWriter(io.getStdOut().writer())),

const Self = @This();

// consts for ansi escape code readability

pub fn init() !Self {
    var output: Self = undefined;

    output.win_stdout = win.GetStdHandle(win.STD_OUTPUT_HANDLE);
    if (output.win_stdout == win.INVALID_HANDLE_VALUE) {
        log.err("cannot get the stdout handle", .{});
        return error.CannotGetStdHandle;
    }

    // Disable line input, echo input, and processed input
    _ = win.GetConsoleMode(output.win_stdout, &output.orig_mode);
    var rawMode: win.DWORD = output.orig_mode;
    rawMode &= @intCast(~(win.ENABLE_ECHO_INPUT | win.ENABLE_LINE_INPUT | win.ENABLE_PROCESSED_INPUT));

    // Optionally disable Ctrl+C handling:
    rawMode &= @intCast(~win.ENABLE_PROCESSED_INPUT);

    _ = win.SetConsoleMode(output.win_stdout, rawMode);

    var console_info: win.CONSOLE_SCREEN_BUFFER_INFO = undefined;
    if (win.GetConsoleScreenBufferInfo(output.win_stdout, &console_info) != win.TRUE) {
        log.err("cannot get the console screen buffer info", .{});
        return error.CannotGetConsoleScreenBufInfo;
    }

    const stdout = io.getStdOut();
    output.stdout = io.bufferedWriter(stdout.writer());

    var writer = output.stdout.writer();
    try writer.writeAll(ENTER_ALT_SCREEN_BUF);
    try writer.writeAll(HIDE_CURSOR);
    try output.stdout.flush();

    return output;
}

pub fn deinit(self: *Self) void {
    var writer = self.stdout.writer();
    writer.writeAll(SHOW_CURSOR) catch @panic("stdout write failed");
    writer.writeAll(EXIT_ALT_SCREEN_BUF) catch @panic("stdout write failed");
    _ = win.SetConsoleMode(self.win_stdout, self.orig_mode);
    self.stdout.flush() catch @panic("stdout write failed");
}

pub const Cursor = struct {
    x: u16,
    y: u16,
};

pub fn getCursorPos(self: Self) !Cursor {
    var console_info: win.CONSOLE_SCREEN_BUFFER_INFO = undefined;
    if (win.GetConsoleScreenBufferInfo(self.win_stdout, &console_info) != win.TRUE) {
        log.err("cannot get the console screen buffer info", .{});
        return error.CannotGetConsoleScreenBufInfo;
    }
    const cursor_pos = console_info.dwCursorPosition;

    return .{
        .x = @intCast(cursor_pos.X),
        .y = @intCast(cursor_pos.Y),
    };
}

pub fn setCursorPos(self: Self, x: u16, y: u16) void {
    _ = win.SetConsoleCursorPosition(self.win_stdout, .{
        .X = @intCast(x),
        .Y = @intCast(y),
    });
}

pub fn print(
    self: *Self,
    comptime fmt_str: []const u8,
    args: anytype,
) !void {
    const writer = self.stdout.writer();

    try writer.print(fmt_str, args);
    try self.stdout.flush();
}
