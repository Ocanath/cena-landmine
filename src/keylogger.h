#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    KEY_UNKNOWN = 0,
    KEY_SPACE   = 32
} keylogger_key_t;

typedef void (*keylogger_callback_t)(keylogger_key_t key);

// Register a callback for a specific key. Must be called before keylogger_start().
void keylogger_register(keylogger_key_t key, keylogger_callback_t cb);

// Spawn internal OS thread and return immediately. Returns 0 on success.
int keylogger_start(void);

// Signal the internal thread to stop and block until it exits.
void keylogger_stop(void);

#ifdef __cplusplus
}
#endif
