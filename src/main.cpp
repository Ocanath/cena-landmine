#include <cstdio>
#include "keylogger.h"

static void on_space(keylogger_key_t) {
    printf("[keylogger test] SPACE pressed\n");
    fflush(stdout);
}

int main() {
    keylogger_register(KEY_SPACE, on_space);

    if (keylogger_start() != 0) {
        fprintf(stderr, "keylogger_start() failed\n");
        return 1;
    }

    printf("Listening... press Enter to stop\n");
    getchar();

    keylogger_stop();
    return 0;
}
