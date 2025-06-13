const std = @import("std");
const c = @import("c");
const unicode = std.unicode;

const Allocator = std.mem.Allocator;

alloc: Allocator,
decoder: *c.ma_decoder,
device: *c.ma_device,

const Self = @This();

pub fn init(alloc: Allocator, filename: []const u8) !Self {
    const filename_wz = try unicode.utf8ToUtf16LeAllocZ(alloc, filename);
    defer alloc.free(filename_wz);

    var output: Self = undefined;
    output.alloc = alloc;
    output.decoder = try alloc.create(c.ma_decoder);
    errdefer alloc.destroy(output.decoder);
    output.device = try alloc.create(c.ma_device);
    errdefer alloc.destroy(output.device);

    if (c.ma_decoder_init_file_w(filename_wz, null, output.decoder) != c.MA_SUCCESS) {
        return error.MusikOpenFileFailed;
    }
    errdefer _ = c.ma_decoder_uninit(output.decoder);

    var device_conf = c.ma_device_config_init(c.ma_device_type_playback);
    device_conf.playback.format = output.decoder.outputFormat;
    device_conf.playback.channels = output.decoder.outputChannels;
    device_conf.sampleRate = output.decoder.outputSampleRate;
    device_conf.dataCallback = dataCallback;
    device_conf.pUserData = output.decoder;

    if (c.ma_device_init(null, &device_conf, output.device) != c.MA_SUCCESS) {
        return error.MusikInitDeviceFailed;
    }
    errdefer _ = c.ma_device_uninit(output.device);

    if (c.ma_device_start(output.device) != c.MA_SUCCESS) {
        return error.MusikStartDeviceFailed;
    }
    if (c.ma_device_stop(output.device) != c.MA_SUCCESS) {
        return error.MusikStopDeviceFailed;
    }

    return output;
}

pub fn deinit(self: Self) void {
    _ = c.ma_device_uninit(self.device);
    _ = c.ma_decoder_uninit(self.decoder);

    self.alloc.destroy(self.device);
    self.alloc.destroy(self.decoder);
}

pub fn start(self: Self) !void {
    if (c.ma_device_start(self.device) != c.MA_SUCCESS) {
        return error.MusikStartDeviceFailed;
    }
}

pub fn stop(self: Self) !void {
    if (c.ma_device_stop(self.device) != c.MA_SUCCESS) {
        return error.MusikStopDeviceFailed;
    }
}

pub fn getTotalLen(self: Self) !f64 {
    var total_frame_count: u64 = undefined;
    if (c.ma_decoder_get_length_in_pcm_frames(self.decoder, &total_frame_count) != c.MA_SUCCESS) {
        return error.MusikGetTotalLenFailed;
    }

    return @as(f64, @floatFromInt(total_frame_count)) /
        @as(f64, @floatFromInt(self.decoder.outputSampleRate));
}

pub fn getCurrentLen(self: Self) !f64 {
    var curr_frame_count: u64 = undefined;
    if (c.ma_decoder_get_cursor_in_pcm_frames(self.decoder, &curr_frame_count) != c.MA_SUCCESS) {
        return error.MusikGetCurrLenFailed;
    }

    return @as(f64, @floatFromInt(curr_frame_count)) /
        @as(f64, @floatFromInt(self.decoder.outputSampleRate));
}

pub fn setCurrentLen(self: Self, pos: f64) !void {
    const pos_s = if (pos < 0.0) 0.0 else pos;
    if (pos_s >= try self.getTotalLen()) {
        return error.MusikOutOfBound;
    }

    const set_frame_count: u32 = @as(u32, @intFromFloat(pos_s)) * self.decoder.outputSampleRate;

    if (c.ma_decoder_seek_to_pcm_frame(self.decoder, set_frame_count) != c.MA_SUCCESS) {
        return error.MusikSeekMusikFailed;
    }
}

pub fn moveBySeconds(self: Self, secs: f64) !void {
    const current_len = try self.getCurrentLen();
    try self.setCurrentLen(current_len + secs);
}

fn dataCallback(
    device: ?*c.ma_device,
    output: ?*anyopaque,
    input: ?*const anyopaque,
    frame_count: u32,
) callconv(.C) void {
    _ = input;

    const decoder: *c.ma_decoder = @ptrCast(@alignCast(device.?.pUserData));
    _ = c.ma_decoder_read_pcm_frames(decoder, output, frame_count, null);
}
