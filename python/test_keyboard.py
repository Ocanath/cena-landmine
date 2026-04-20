"""
Simple test script to verify keyboard_player.py works before building.
Run this in the console to test the functionality.
Press spacebar to play audio, Ctrl+C to exit.
"""

import keyboard_player

if __name__ == "__main__":
    print("Starting Keyboard Audio Player Test...")
    print("Press SPACEBAR to play audio (will set volume to max)")
    print("Press Ctrl+C to exit")
    print("-" * 50)

    try:
        keyboard_player.main()
    except KeyboardInterrupt:
        print("\nTest stopped by user.")
