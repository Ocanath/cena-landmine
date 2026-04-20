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
3. Volume maximization: the software should attempt to maximize system volume before playing. If system volume is muted, it should unmute. 
4. Standalone executable: no external file resources. All assets must ship with the executable blob, including audio files
5. Audio playback: the software should playback the software using an established audio playback library. Discuss this with the user.
6. Dependency management/build toolchain: the project should submodule all dependencies and link them via cmake.  
7. Triggers:
	- udp listening: port 6768, listen for `"AND HIS NAME IS..."`, trigger the audio playback subroutine on match.
	- keystroke: the space key should trigger the audio playback subroutine

The file asset to play is `notification.wav`, which can be found under `python/sounds/notification.wav`.




