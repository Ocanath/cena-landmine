#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    void (*on_enable)(void);
    void (*on_disable)(void);
    void (*on_trigger)(void);
    void (*on_stop)(void);
} udp_callbacks_t;

// Bind to 0.0.0.0:7878 and spawn a listener thread. Returns 0 on success.
// Continues silently on bind failure — UDP is non-essential.
int  udp_listener_start(udp_callbacks_t cbs);
void udp_listener_stop(void);

#ifdef __cplusplus
}
#endif
