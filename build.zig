const std = @import("std");

const INCLUDE_SEARCH_PATH = &.{
    "./thirdparty/",
    "./src/",
};

pub fn build(b: *std.Build) void {
    const target = b.standardTargetOptions(.{});
    const optimize = b.standardOptimizeOption(.{});

    const zlap_dep = b.dependency("zlap", .{
        .target = target,
        .optimize = optimize,
    });

    const musik_headers = b.addTranslateC(.{
        .root_source_file = b.path("./src/musik.h"),
        .target = target,
        .optimize = optimize,
    });
    inline for (INCLUDE_SEARCH_PATH) |path| {
        musik_headers.addIncludePath(b.path(path));
    }

    const musik_c = b.addModule("musik-c", .{
        .root_source_file = musik_headers.getOutput(),
        .target = musik_headers.target,
        .optimize = musik_headers.optimize,
        .link_libc = musik_headers.link_libc,
    });
    inline for (INCLUDE_SEARCH_PATH) |path| {
        musik_c.addIncludePath(b.path(path));
    }
    musik_c.addCSourceFiles(
        .{
            .files = &.{
                "./src/miniaudio.c",
                if (target.result.os.tag == .windows)
                    "./src/terminal/terminal_windows.c"
                else
                    "./src/terminal/terminal_posix.c",
            },
            .flags = &.{
                "-std=c11", "-Wall", "-Wextra", "-Wpedantic",
            },
            .language = .c,
        },
    );

    const exe_mod = b.createModule(.{
        .root_source_file = b.path("src/main.zig"),
        .target = target,
        .optimize = optimize,
        .link_libc = true,
    });
    exe_mod.addImport("zlap", zlap_dep.module("zlap"));
    exe_mod.addImport("c", musik_c);

    const exe = b.addExecutable(.{
        .name = "musik",
        .root_module = exe_mod,
    });
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
