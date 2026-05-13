#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include "audio.h"
#include "cena.h"  // python_sounds_notification_wav, python_sounds_notification_wav_len

#include <cstdio>

static ma_engine  g_engine;
static ma_decoder g_decoder;  // must outlive g_sound — sound reads from it on-the-fly
static ma_sound   g_sound;
static int        g_ready = 0;

int audio_init(void) {
    if (ma_engine_init(NULL, &g_engine) != MA_SUCCESS) {
        fprintf(stderr, "[audio] ma_engine_init failed\n");
        return 1;
    }

    if (ma_decoder_init_memory(python_sounds_notification_wav,
                               python_sounds_notification_wav_len,
                               NULL, &g_decoder) != MA_SUCCESS) {
        fprintf(stderr, "[audio] ma_decoder_init_memory failed\n");
        ma_engine_uninit(&g_engine);
        return 1;
    }

    ma_result result = ma_sound_init_from_data_source(
        &g_engine, &g_decoder,
        MA_SOUND_FLAG_NO_SPATIALIZATION,
        NULL, &g_sound);

    if (result != MA_SUCCESS) {
        fprintf(stderr, "[audio] ma_sound_init_from_data_source failed\n");
        ma_decoder_uninit(&g_decoder);
        ma_engine_uninit(&g_engine);
        return 1;
    }

    g_ready = 1;
    return 0;
}

void audio_play(void) {
    if (!g_ready) return;
    ma_sound_seek_to_pcm_frame(&g_sound, 0);
    ma_sound_start(&g_sound);
}

void audio_stop(void) {
    if (!g_ready) return;
    ma_sound_stop(&g_sound);
}

void audio_cleanup(void) {
    if (g_ready) {
        ma_sound_uninit(&g_sound);
        ma_decoder_uninit(&g_decoder);
        g_ready = 0;
    }
    ma_engine_uninit(&g_engine);
}
