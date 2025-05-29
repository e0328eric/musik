const std = @import("std");
const musik = @cImport({
    @cInclude("musik.h");
});
const vaxis = @import("vaxis");

const Cell = vaxis.Cell;
const TextInput = vaxis.widgets.TextInput;
const border = vaxis.widgets.border;

pub fn main() !void {
    const allocator = std.heap.c_allocator;

    const args = try std.process.argsAlloc(allocator);
    if (args.len < 2) {
        std.debug.print("Usage: {s} <wavfile>\n", .{args[0]});
        return error.InvalidArgs;
    }
    const wavfile = args[1];

    var device: musik.Musik = undefined;
    if (musik.initMusik(&device, wavfile.ptr) != musik.MUSIK_OK) {
        return error.Foo;
    }
    defer musik.uninitMusik(&device);

    var music_len: f64 = undefined;
    var curr_pos: f64 = 0.0;

    _ = musik.getTotalLen(&music_len, &device);

    // TODO: while loop is not ended
    // TODO: make a poll like system to make CPU rate down
    while (curr_pos <= music_len) {
        _ = musik.getCurrentLen(&curr_pos, &device);
    }
}
