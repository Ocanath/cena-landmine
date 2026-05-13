#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    char* data;
    size_t   size;
} buffer_t;

typedef struct {
    void (*on_enable)(buffer_t* reply);
    void (*on_disable)(buffer_t* reply);
    void (*on_trigger)(buffer_t* reply);
    void (*on_stop)(buffer_t* reply);
} udp_callbacks_t;

// Bind to 0.0.0.0:7878 and spawn a listener thread. Returns 0 on success.
// Continues silently on bind failure — UDP is non-essential.
int  udp_listener_start(udp_callbacks_t cbs);
void udp_listener_stop(void);

#ifdef __cplusplus
}
#endif
