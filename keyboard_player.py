"""
Background keyboard-triggered audio player.
Monitors spacebar globally and plays audio at max volume.
"""

import os
import sys
import winsound
from pynput import keyboard
from ctypes import cast, POINTER
from comtypes import CLSCTX_ALL
from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
import winreg


# Audio file path (works both in dev and when bundled by PyInstaller)
def get_audio_path():
    """Get the path to the audio file, handling PyInstaller bundling."""
    if getattr(sys, 'frozen', False):
        # Running as compiled executable
        base_path = sys._MEIPASS
    else:
        # Running as script
        base_path = os.path.dirname(os.path.abspath(__file__))

    return os.path.join(base_path, 'sounds', 'notification.wav')


AUDIO_FILE = get_audio_path()


class AudioPlayer:
    """Manages audio playback and system volume control."""

    def __init__(self):
        # Only print when running as script (not as bundled exe)
        if not getattr(sys, 'frozen', False):
            print("Initializing AudioPlayer...")

        self.volume_interface = self._get_volume_interface()

        if not getattr(sys, 'frozen', False):
            if self.volume_interface:
                print("✓ Volume control initialized successfully")
            else:
                print("✗ Volume control failed - make sure running as administrator")

    def _get_volume_interface(self):
        """Get the Windows audio volume control interface."""
        try:
            devices = AudioUtilities.GetSpeakers()
            # Newer pycaw API uses .EndpointVolume property directly
            return devices.EndpointVolume
        except Exception as e:
            if not getattr(sys, 'frozen', False):
                print(f"  Error: {type(e).__name__}: {e}")
            return None

    def set_max_volume(self):
        """Set system volume to maximum (100%)."""
        if self.volume_interface:
            try:
                if not getattr(sys, 'frozen', False):
                    current = self.volume_interface.GetMasterVolumeLevelScalar()
                    print(f"  Current volume: {current:.0%}")

                self.volume_interface.SetMasterVolumeLevelScalar(1.0, None)

                if not getattr(sys, 'frozen', False):
                    new = self.volume_interface.GetMasterVolumeLevelScalar()
                    print(f"  New volume: {new:.0%}")
                    if new >= 0.99:
                        print("  ✓ Volume set to max")
            except Exception as e:
                if not getattr(sys, 'frozen', False):
                    print(f"  Error setting volume: {e}")

    def play(self):
        """Play the audio file at max volume."""
        if not getattr(sys, 'frozen', False):
            print("\n[SPACEBAR PRESSED]")

        # Stop any currently playing audio first (restart from beginning)
        winsound.PlaySound(None, winsound.SND_PURGE)

        # Set volume to max
        if not getattr(sys, 'frozen', False):
            print("Setting volume to max...")
        self.set_max_volume()

        # Play audio asynchronously
        if not getattr(sys, 'frozen', False):
            print("Playing audio...")
        try:
            winsound.PlaySound(AUDIO_FILE, winsound.SND_FILENAME | winsound.SND_ASYNC)
            if not getattr(sys, 'frozen', False):
                print("✓ Audio started\n")
        except Exception as e:
            if not getattr(sys, 'frozen', False):
                print(f"✗ Audio error: {e}\n")


class StartupManager:
    """Manages Windows startup registration."""

    REG_PATH = r"Software\Microsoft\Windows\CurrentVersion\Run"
    APP_NAME = "KeyboardAudioPlayer"

    @classmethod
    def register_startup(cls):
        """Register the application to run at Windows startup."""
        try:
            # Get the executable path
            if getattr(sys, 'frozen', False):
                exe_path = sys.executable
            else:
                # When running as script, don't register
                return False

            # Open registry key
            key = winreg.OpenKey(
                winreg.HKEY_CURRENT_USER,
                cls.REG_PATH,
                0,
                winreg.KEY_SET_VALUE
            )

            # Set the value
            winreg.SetValueEx(key, cls.APP_NAME, 0, winreg.REG_SZ, exe_path)
            winreg.CloseKey(key)
            return True
        except Exception:
            return False

    @classmethod
    def is_registered(cls):
        """Check if the application is already registered for startup."""
        try:
            key = winreg.OpenKey(
                winreg.HKEY_CURRENT_USER,
                cls.REG_PATH,
                0,
                winreg.KEY_READ
            )

            try:
                winreg.QueryValueEx(key, cls.APP_NAME)
                winreg.CloseKey(key)
                return True
            except FileNotFoundError:
                winreg.CloseKey(key)
                return False
        except Exception:
            return False


class KeyboardMonitor:
    """Monitors keyboard for spacebar presses."""

    def __init__(self, audio_player):
        self.audio_player = audio_player
        self.listener = None

    def on_press(self, key):
        """Handle key press events."""
        try:
            # Check if spacebar was pressed
            if key == keyboard.Key.space:
                self.audio_player.play()
        except AttributeError:
            pass  # Ignore non-special keys

    def start(self):
        """Start monitoring keyboard."""
        self.listener = keyboard.Listener(on_press=self.on_press)
        self.listener.start()

    def stop(self):
        """Stop monitoring keyboard."""
        if self.listener:
            self.listener.stop()


def main():
    """Main entry point."""
    # Only print when running as script
    if not getattr(sys, 'frozen', False):
        print("\n" + "="*50)
        print("KEYBOARD AUDIO PLAYER")
        print("="*50)

    # Register for startup if not already registered
    if not StartupManager.is_registered():
        StartupManager.register_startup()

    # Initialize audio player
    audio_player = AudioPlayer()

    # Start keyboard monitoring
    if not getattr(sys, 'frozen', False):
        print("\nStarting keyboard monitor...")

    monitor = KeyboardMonitor(audio_player)
    monitor.start()

    if not getattr(sys, 'frozen', False):
        print("✓ Ready! Press SPACEBAR to play audio")
        print("  (Press Ctrl+C to exit)\n")

    # Keep the application running
    try:
        monitor.listener.join()
    except KeyboardInterrupt:
        if not getattr(sys, 'frozen', False):
            print("\nExiting...")
        monitor.stop()


if __name__ == "__main__":
    main()
