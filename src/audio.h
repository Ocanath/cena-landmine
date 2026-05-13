#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Decode the bundled WAV asset into memory. Call once at startup.
// Returns 0 on success.
int audio_init(void);

// Start playback of the notification sound. Non-blocking; the engine thread
// handles output. Safe to call from any thread. Restarts if already playing.
void audio_play(void);

// Stop playback immediately.
void audio_stop(void);

// Release audio engine resources. Call on shutdown.
void audio_cleanup(void);

#ifdef __cplusplus
}
#endif
