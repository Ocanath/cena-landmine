"""
Diagnostic script to check if all dependencies are installed correctly.
"""

import sys

print("="*60)
print("DEPENDENCY CHECK")
print("="*60)
print()

# Check Python version
print(f"Python version: {sys.version}")
print()

# Check each dependency
dependencies = [
    ('pynput', 'Keyboard monitoring'),
    ('pycaw', 'Volume control'),
    ('comtypes', 'COM interface (required by pycaw)'),
]

all_ok = True

for module_name, description in dependencies:
    try:
        __import__(module_name)
        print(f"✓ {module_name:15s} - {description}")
    except ImportError as e:
        print(f"✗ {module_name:15s} - MISSING ({description})")
        print(f"  Error: {e}")
        all_ok = False

print()
print("="*60)

if all_ok:
    print("✓ All dependencies installed correctly!")
    print()
    print("Testing pycaw audio interface...")
    print()

    try:
        from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
        from ctypes import cast, POINTER
        from comtypes import CLSCTX_ALL

        print("  Getting audio device...")
        devices = AudioUtilities.GetSpeakers()
        print(f"  ✓ Found device: {devices}")

        print("  Activating interface...")
        interface = devices.Activate(IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
        volume = cast(interface, POINTER(IAudioEndpointVolume))
        print("  ✓ Volume interface activated")

        current = volume.GetMasterVolumeLevelScalar()
        print(f"  ✓ Current volume: {current:.0%}")

        print()
        print("="*60)
        print("✓ Volume control working! You're ready to run.")

    except AttributeError as e:
        print(f"  ✗ AttributeError: {e}")
        print()
        print("  This usually means the pycaw API changed.")
        print("  Try running this script as ADMINISTRATOR.")

    except Exception as e:
        print(f"  ✗ Error: {type(e).__name__}: {e}")
        print()
        print("  This might be a permissions issue.")
        print("  Try running this script as ADMINISTRATOR.")
else:
    print("✗ Some dependencies are missing!")
    print()
    print("To install dependencies, run:")
    print("  pip install -r requirements.txt")

print()
input("Press Enter to exit...")
