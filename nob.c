#define NOB_IMPLEMENTATION
#define NOB_STRIP_PREFIX
#include "nob.h"

#define BUILD_FOLDER "./build/"
#define SRC_FOLDER   "./src/"

int main(int argc, char** argv)
{
    NOB_GO_REBUILD_URSELF(argc, argv);
    if (!mkdir_if_not_exists(BUILD_FOLDER)) return 1;

    Cmd cmd = {0};
#if defined(_MSC_VER)
    cmd_append(&cmd, "cl", "/W4", SRC_FOLDER"musik.c", 
            "/I"SRC_FOLDER, "/I./thirdparty", "/Fe:"BUILD_FOLDER"musik");
#else
#error "Not supported yet"
#endif
    if (!cmd_run_sync(cmd)) return 1;

    return 0;
}

