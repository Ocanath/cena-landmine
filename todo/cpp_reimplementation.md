# Cena Landmine

This project is an executable that should run as a background process and listen for 
keystrokes. If the space key is pressed, it should maximize the system volume and play 
an audio asset file which should be bundled into the executable. The software should also listen
on a UDP port for a dedicated string on port 6768 "AND HIS NAME IS...", and trigger the audio playback 
if that string is received. The executable should be standalone, with no external assets.

A first pass implementation of this software has been implemented in python under `python/`. 
The task: C++ re-implementation of the sw_cen executable with some improvements.


## Project Requirements

1. Persistence: the executable must be persistent. This is accomplished by the deployment mechanism (duckyscript), so the software doesn't have to consider it.
2. Cross-platform: the software should build for windows, linux, and macos clients. The duckyscript deployment script for windows is sufficient for now - we will consider badusb/duckyscript deployment for linux and macos clients later.
3. Volume maximization: the software should attempt to maximize system volume before playing. If system volume is muted, it should unmute. The python script accomplishes this for windows well - use this as a model. 
4. Standalone executable: no external file resources. All assets must ship with the executable blob, including audio files
5. Audio playback: the software should playback the software using an established audio playback library. `miniaudio` is preferred for simplicity
6. Dependency management/build toolchain: the project should submodule all dependencies and link them via cmake. Do not use FetchContent. 
	- prefer miniaudio dependency for playback
	- prefer tinycsocket for socket programming. 
7. Triggers:
	- udp listening: port 6768, listen for `"AND HIS NAME IS..."`, trigger the audio playback subroutine on match. Should bind to 0.0.0.0. Should continue on error - udp listening is not essential, so do not abort program if the bind fails.
	- keystroke: the space key should trigger the audio playback subroutine

### Note on Key Logging

Rather than using an external dependency for this, a hand-rolled solution is preferred. This should exist in its own translation unit with macro logic for platform detection and implementation, and link statically to the rest of the project so it's easy to link to examples and standalone tests.

### Note on Audio Asset

The file asset to play is `notification.wav`, which can be found under `python/sounds/notification.wav`. The preferred method to bundle is to use xxd.
The notification asset has already been produced as a header, directly from the tool use:

```bash
xxd -i python/sounds/notification.wav > cena.h
```

miniaudio should be able to accept this array as an alternative to a filebuffer for playback.







## Implementation Plan

Implement the required functionality in the following stages. Each step requires review, compilation and testing before completion. At each stage, if testing passes, the code must be committed before continuing to the next step. 

1. **Keylogger:**
Implement a thin cross platform solution to background monitoring of keystrokes. Keystroke logger will be implemented as its own translation unit. The main entry point is created at this stage as well. The keystroke logger will be tested by registering on the space key, and committing some action when the key is pressed (i.e. `printf`).

1. **Media playback:**
Link `miniaudio` and set up the keystroke trigger via the bundled .wav header blob

1. **Volume controls** 
Implement system dependent volume maximization in a separate translation unit, e.g. `volume_maximizer.cpp` and `volume_maximizer.h`. Wire it as a step preceeding playback, as part of the playback subroutine which runs on trigger

1. **Socket Trigger**
Link tinycsocket and add socket creation, binding, and the udp socket command trigger.


