#include "utility.h"

#ifdef _WIN32
#include <windows.h>
#else // linux and macos (POSIX systems)
#include <unistd.h>
#endif

void musikSleep(uint32_t millisecs) {
#ifdef _WIN32
    Sleep(millisecs);
#else
    usleep(millisecs * 1000);
#endif
}

