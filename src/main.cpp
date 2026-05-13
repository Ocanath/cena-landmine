#include <cstdio>
#include "keylogger.h"
#include "audio.h"
#include "volume_maximizer.h"

static void on_space(keylogger_key_t) {
    volume_maximize();
    audio_play();
}

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

    // printf("Listening... press Enter to stop\n");
    while(1)
	{
		getchar();
	}

    keylogger_stop();
    audio_cleanup();
    return 0;
}
