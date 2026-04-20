#!/usr/bin/env python3
"""
UDP listener for light control notifications (Windows only).
Listens on UDP port 4593 for 'lightson'/'lightsoff' commands
and plays a notification sound on rising edge transitions.
Aborts playback on falling edge transitions.
"""

import socket
import winsound
from pathlib import Path

# Configuration
UDP_IP = "0.0.0.0"
UDP_PORT = 4593
SOUND_FILE = "sounds/notification.wav"
BUFFER_SIZE = 1024

def play_sound(sound_file):
    """Play audio file asynchronously (non-blocking)."""
    winsound.PlaySound(sound_file, winsound.SND_FILENAME | winsound.SND_ASYNC)

def stop_sound():
    """Stop currently playing sound."""
    winsound.PlaySound(None, winsound.SND_PURGE)

def main():
    # Verify sound file exists
    sound_path = Path(SOUND_FILE)
    if not sound_path.exists():
        print(f"Warning: Sound file '{SOUND_FILE}' not found!")

    # Create UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

    # Enable broadcast reception
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
    # sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

    # Set timeout to allow Ctrl+C to work
    sock.settimeout(0.5)

    # Bind to empty string instead of 0.0.0.0 for better broadcast reception on Windows
    sock.bind(('', UDP_PORT))

    # Send a dummy packet to "wake up" the socket for broadcast reception (Windows quirk)
    try:
        sock.sendto(b'fuck', ('10.0.4.255', UDP_PORT+1))
    except:
        pass  # Ignore errors

    print(f"UDP listener started on 0.0.0.0:{UDP_PORT}")
    print(f"Listening for 'lightson' and 'lightsoff' commands...")
    print(f"Broadcast reception enabled. Press Ctrl+C to exit.")

    # Track previous state (0 = off, 1 = on)
    previous_state = 0

    try:
        while True:
            try:
                # Receive data
                data, addr = sock.recvfrom(BUFFER_SIZE)
            except socket.timeout:
                # Timeout allows Ctrl+C to be processed
                # Send keepalive to prevent Windows from resetting broadcast reception
                try:
                    sock.sendto(b'', ('10.0.4.255', UDP_PORT+1))
                except:
                    print("Error when sending keepalive packet")
                    pass
                continue
			
            message = data.decode('utf-8', errors='ignore').lower()
            # Determine current state
            current_state = None
            if 'lightson' in message:
                current_state = 1
                print(f"Received 'lightson' from {addr}")
            elif 'lightsoff' in message:
                current_state = 0
                print(f"Received 'lightsoff' from {addr}")
            else:
                if(len(message) != 0):
                    print(f"Received unknown message from {addr}: {message}")
                continue

            # Detect rising edge (0 -> 1 transition)
            if previous_state == 0 and current_state == 1:
                print("Rising edge detected! Playing notification sound...")
                if sound_path.exists():
                    play_sound(str(sound_path))
                else:
                    print(f"Cannot play sound: {SOUND_FILE} not found")

            # Detect falling edge (1 -> 0 transition)
            elif previous_state == 1 and current_state == 0:
                print("Falling edge detected! Stopping notification sound...")
                stop_sound()

            # Update state
            previous_state = current_state

    except KeyboardInterrupt:
        print("\nShutting down UDP listener...")
    finally:
        sock.close()

if __name__ == "__main__":
    main()
