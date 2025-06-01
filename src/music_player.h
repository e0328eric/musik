#ifndef MUSIK_MUSIC_PLAYER_H_
#define MUSIK_MUSIC_PLAYER_H_

#include <stdbool.h>
#include <miniaudio.h>

typedef enum {
    MUSIK_OK = 0,
    MUSIK_OPEN_FILE_FAILED,
    MUSIK_INIT_DEVICE_FAILED,
    MUSIK_START_DEVICE_FAILED,
    MUSIK_STOP_DEVICE_FAILED,
    MUSIK_GET_TOTAL_LEN_FAILED,
    MUSIK_GET_CURRENT_LEN_FAILED,
    MUSIK_ERR_KIND_COUNT,
} MusikErrKind;

typedef struct {
    ma_decoder decoder;
    ma_device device;
} Musik;

MusikErrKind initMusik(Musik* output, const char* filename);
void uninitMusik(Musik* musik);

MusikErrKind startMusik(Musik* musik);
MusikErrKind stopMusik(Musik* musik);

MusikErrKind getTotalLen(double* output, const Musik* musik);
MusikErrKind getCurrentLen(double* output, const Musik* musik);

#endif // MUSIK_MUSIC_PLAYER_H_
