const std = @import("std");

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const zlap_dep = b.dependency("zlap", .{
        .target = target,
        .optimize = optimize,
    });
    const vaxis_dep = b.dependency("vaxis", .{
        .target = target,
        .optimize = optimize,
    });

    const exe_mod = b.createModule(.{
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    exe_mod.addIncludePath(b.path("./include"));
    exe_mod.addIncludePath(b.path("./include/miniaudio"));
    exe_mod.addImport("zlap", zlap_dep.module("zlap"));
    exe_mod.addImport("vaxis", vaxis_dep.module("vaxis"));

    const exe = b.addExecutable(.{
        .name = "musik",
        .root_module = exe_mod,
    });
    exe.addCSourceFile(.{
        .file = b.path("./src/musik.c"),
        .flags = &.{"-O3"},
    });
    const root_mod_target = exe.rootModuleTarget();
    if (root_mod_target.os.tag == .linux or root_mod_target.os.tag == .macos) {
        exe.linkSystemLibrary("pthread");
    }

    b.installArtifact(exe);

    const run_cmd = b.addRunArtifact(exe);
    run_cmd.step.dependOn(b.getInstallStep());

    if (b.args) |args| {
        run_cmd.addArgs(args);
    }

    const run_step = b.step("run", "Run the app");
    run_step.dependOn(&run_cmd.step);

    const exe_unit_tests = b.addTest(.{
        .root_module = exe_mod,
    });

    const run_exe_unit_tests = b.addRunArtifact(exe_unit_tests);

    const test_step = b.step("test", "Run unit tests");
    test_step.dependOn(&run_exe_unit_tests.step);
}
