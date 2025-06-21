const std = @import("std");
const c = @import("c");

pub const Terminal = opaque {
    const Self = @This();
    pub fn init() *Self {
        return @ptrCast(c.initTerminal());
    }

    pub fn deinit(self: *Self) void {
        c.deinitTerminal(@ptrCast(self));
    }
};
