#include <cstdio>
#include "keylogger.h"
#include "audio.h"
#include "volume_maximizer.h"
#include "udp_listener.h"

static volatile int g_space_enabled = 1;

static void on_space(keylogger_key_t) {
    if (!g_space_enabled) return;
    volume_maximize();
    audio_play();
}

static void cb_enable(void)  { g_space_enabled = 1; }
static void cb_disable(void) { g_space_enabled = 0; }
static void cb_trigger(void) { volume_maximize(); audio_play(); }
static void cb_stop(void)    { audio_stop(); }

int main() {
    if (audio_init() != 0) {
        fprintf(stderr, "audio_init() failed\n");
        return 1;
    }

    keylogger_register(KEY_SPACE, on_space);
    if (keylogger_start() != 0) {
        fprintf(stderr, "keylogger_start() failed\n");
        audio_cleanup();
        return 1;
    }

    udp_callbacks_t cbs = { cb_enable, cb_disable, cb_trigger, cb_stop };
    udp_listener_start(cbs);

    while(1) { getchar(); }

    udp_listener_stop();
    keylogger_stop();
    audio_cleanup();
    return 0;
}
