#include <stdio.h>

#define MINIAUDIO_IMPLEMENTATION
#include <miniaudio.h>

#include "music_player.h"

static void dataCallback(
    ma_device* pDevice,
    void* pOutput,
    const void* pInput,
    ma_uint32 frameCount
) {
    ma_decoder* pDecoder = (ma_decoder*)pDevice->pUserData;
    if (pDecoder == NULL) {
        return;
    }

    ma_decoder_read_pcm_frames(pDecoder, pOutput, frameCount, NULL);

    (void)pInput;
}

MusikErrKind initMusik(Musik* output, const char* filename) {
    MusikErrKind result = MUSIK_OK;

    if (ma_decoder_init_file(filename, NULL, &output->decoder) != MA_SUCCESS) {
        fprintf(stderr, "Failed to open %s\n", filename);
        return MUSIK_OPEN_FILE_FAILED;
    }

    ma_device_config device_conf = ma_device_config_init(ma_device_type_playback);
    device_conf.playback.format   = output->decoder.outputFormat;
    device_conf.playback.channels = output->decoder.outputChannels;
    device_conf.sampleRate        = output->decoder.outputSampleRate;
    device_conf.dataCallback      = dataCallback;
    device_conf.pUserData         = &output->decoder;

    if (ma_device_init(NULL, &device_conf, &output->device) != MA_SUCCESS) {
        fprintf(stderr, "Failed to initialize audio device.\n");
        result = MUSIK_INIT_DEVICE_FAILED;
        goto CLEAN_DECODER;
    }

    if (ma_device_start(&output->device) != MA_SUCCESS) {
        fprintf(stderr, "Failed to start playback.\n");
        result = MUSIK_START_DEVICE_FAILED;
        goto CLEAN_DEVICE;
    }

    if (ma_device_stop(&output->device) != MA_SUCCESS) {
        fprintf(stderr, "Failed to stop playback.\n");
        return MUSIK_STOP_DEVICE_FAILED;
    }

    return result;

CLEAN_DEVICE:
    ma_device_uninit(&output->device);
CLEAN_DECODER:
    ma_decoder_uninit(&output->decoder);

    return result;
}

void deinitMusik(Musik* musik) {
    ma_device_uninit(&musik->device);
    ma_decoder_uninit(&musik->decoder);
}

MusikErrKind startMusik(Musik* musik) {
    if (ma_device_start(&musik->device) != MA_SUCCESS) {
        fprintf(stderr, "Failed to start playback.\n");
        return MUSIK_START_DEVICE_FAILED;
    }

    return MUSIK_OK;
}

MusikErrKind stopMusik(Musik* musik) {
    if (ma_device_stop(&musik->device) != MA_SUCCESS) {
        fprintf(stderr, "Failed to stop playback.\n");
        return MUSIK_STOP_DEVICE_FAILED;
    }

    return MUSIK_OK;
}

MusikErrKind getTotalLen(Musik* const musik, double* output) {
    ma_uint64 total_frame_count;
    if (ma_decoder_get_length_in_pcm_frames(&musik->decoder, &total_frame_count) != MA_SUCCESS) {
        return MUSIK_GET_TOTAL_LEN_FAILED;
    }

    *output = (double)total_frame_count / musik->decoder.outputSampleRate;
    return MUSIK_OK;
}

MusikErrKind getCurrentLen(Musik* const musik, double* output) {
    ma_uint64 current_frame_count;
    if (ma_decoder_get_cursor_in_pcm_frames(&musik->decoder, &current_frame_count) != MA_SUCCESS) {
        return MUSIK_GET_CURRENT_LEN_FAILED;
    }

    *output = (double)current_frame_count / musik->decoder.outputSampleRate;
    return MUSIK_OK;
}

MusikErrKind setCurrentLen(Musik* musik, double pos) {
    if (pos < 0.0) pos = 0.0;

    ma_uint64 total_frame_count, set_frame_count;
    if (ma_decoder_get_length_in_pcm_frames(&musik->decoder, &total_frame_count) != MA_SUCCESS) {
        return MUSIK_GET_TOTAL_LEN_FAILED;
    }

    set_frame_count = (ma_uint64)(pos * musik->decoder.outputSampleRate);
    if (set_frame_count >= total_frame_count) {
        return MUSIK_OUT_OF_BOUND;
    }

    if (ma_decoder_seek_to_pcm_frame(&musik->decoder, set_frame_count) != MA_SUCCESS) {
        return MUSIK_SEEK_MUSIC_FAILED;
    }

    return MUSIK_OK;
}

MusikErrKind moveBySeconds(Musik* musik, double secs) {
    MusikErrKind result = MUSIK_OK;
    double current_len;
    if ((result = getCurrentLen(musik, &current_len)) != MUSIK_OK) {
        return result;
    }

    return setCurrentLen(musik, current_len + secs);
}
