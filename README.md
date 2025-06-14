# musik
simple (?) tui music player

## Prerequisits
This program is written in FASM assembly, so one should have a FASM assembler.

## how to build?
First clone this repo by the following command
```console
git clone --recursive https://github.com/e0328eric/musik.git
```

Since this project uses [nob.h](https://github.com/tsoding/nob.h.git), one
should build `nob.c` file

### on windows
```console
cl nob.c
```

### on POSIX
```console
cc nob.c -o nob
```

Finally run `nob`(or `nob.exe`) to compile this project.
