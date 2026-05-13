#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// Unmute and maximize system output volume. Best-effort — never aborts on failure.
void volume_maximize(void);

#ifdef __cplusplus
}
#endif
