#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define BUILD_FOLDER "./build/"
#define SRC_FOLDER   "./src/"
#define LIB_FOLDER   "./thirdparty/"

int main(int argc, char** argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    Cmd cmd = {0};

    const char* program_name = shift(argv, argc);
    const char* command_name = "build";
    if (argc > 0) command_name = shift(argv, argc);

    if (strcmp(command_name, "build") == 0) {
        if (!mkdir_if_not_exists(BUILD_FOLDER)) return 1;
        
#ifdef _WIN32
        cmd_append(&cmd, "fasm", SRC_FOLDER"Windows/musik.asm", BUILD_FOLDER"musik.obj");
        if (!cmd_run_sync(cmd)) return 1;

        cmd.count = 0;
        cmd_append(&cmd, "cl", "/std:c11", "/c", LIB_FOLDER"miniaudio.c", "/Fo" BUILD_FOLDER"miniaudio.obj");
        if (!cmd_run_sync(cmd)) return 1;

        cmd.count = 0;
        cmd_append(&cmd, "link", BUILD_FOLDER"musik.obj", BUILD_FOLDER"miniaudio.obj",
                "kernel32.lib", "user32.lib", "shell32.lib",
                "/SUBSYSTEM:CONSOLE", "/DEFAULTLIB:libcmt", "/ENTRY:mainCRTStartup", "/OUT:"BUILD_FOLDER"musik.exe");
        if (!cmd_run_sync(cmd)) return 1;
#endif
        return 0;
    }

    if (strcmp(command_name, "clear") == 0) {
        return 0;
    }

    nob_log(ERROR, "Unknown command %s", command_name);
    return 1;
}
