#include <stdio.h>

int main(void) {
#ifdef _WIN32
    system("fasm ./src/Windows/musik.asm musik.exe");
#endif
}
