const std = @import("std");
const rl = @import("raylib");

const Allocator = std.mem.Allocator;

const MusicState = enum {
    Update,
    Pause,
    Resume,
    Exit,
};

allocator: Allocator,
song_name: [:0]const u8,
inner: rl.Music,
state: MusicState,
rwlock: std.Thread.RwLock,

const Self = @This();

pub fn init(allocator: Allocator, song_name: []const u8) !Self {
    const song_name_z = try allocator.dupeZ(u8, song_name);
    errdefer allocator.free(song_name_z);

    rl.initAudioDevice();
    errdefer rl.closeAudioDevice();

    const music = try rl.loadMusicStream(song_name_z);

    return .{
        .allocator = allocator,
        .song_name = song_name_z,
        .inner = music,
        .state = .Pause,
        .rwlock = std.Thread.RwLock{},
    };
}

pub fn deinit(self: Self) void {
    rl.unloadMusicStream(self.inner);
    rl.closeAudioDevice();
    self.allocator.free(self.song_name);
}

//pub fn changeMusic(self: *Self, song_name: []const u8) !void {
//    rl.unloadMusicStream(self.inner);
//    self.inner = try rl.loadMusicStream(song_name);
//    errdefer rl.unloadMusicStream(self.inner);
//}

pub fn pause(self: *Self) void {
    self.rwlock.lock();
    defer self.rwlock.unlock();
    self.state = .Pause;
}

pub fn @"resume"(self: *Self) void {
    self.rwlock.lock();
    defer self.rwlock.unlock();
    self.state = .Resume;
}
pub fn exit(self: *Self) void {
    self.rwlock.lock();
    defer self.rwlock.unlock();
    self.state = .Exit;
}

pub fn timeInfo(self: *Self) struct { f32, f32 } {
    self.rwlock.lockShared();
    defer self.rwlock.unlockShared();
    return .{
        rl.getMusicTimeLength(self.inner),
        rl.getMusicTimePlayed(self.inner),
    };
}

pub fn rewind(self: *Self, position: f32) void {
    self.rwlock.lock();
    defer self.rwlock.unlock();
    rl.seekMusicStream(self.inner, position);
}

pub fn play(self: *Self) void {
    while (true) {
        switch (self.state) {
            .Update => rl.updateMusicStream(self.inner),
            .Pause => rl.pauseMusicStream(self.inner),
            .Resume => {
                self.rwlock.lock();
                defer self.rwlock.unlock();
                self.state = .Update;
                rl.resumeMusicStream(self.inner);
                rl.playMusicStream(self.inner);
            },
            .Exit => break,
        }

        const time_len, const time_played = self.timeInfo();
        if (time_played >= time_len) break;
    }
}
