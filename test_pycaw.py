"""Quick test to figure out the correct pycaw API."""

print("Testing pycaw imports and API...")
print()

try:
    print("Method 1: from pycaw.pycaw import ...")
    from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
    print("  ✓ Import successful")

    devices = AudioUtilities.GetSpeakers()
    print(f"  Device: {devices}")
    print(f"  Type: {type(devices)}")
    print(f"  Has Activate: {hasattr(devices, 'Activate')}")

    if hasattr(devices, 'Activate'):
        print("  ✓ Activate method exists!")
    else:
        print("  Available methods:")
        methods = [m for m in dir(devices) if not m.startswith('_')]
        for m in methods[:15]:
            print(f"    - {m}")

except Exception as e:
    print(f"  ✗ Error: {e}")

print()
print("Method 2: Direct comtypes approach...")
try:
    from comtypes import CoCreateInstance, CLSCTX_ALL
    import comtypes

    # Initialize COM
    comtypes.CoInitialize()

    # Import after CoInitialize
    from pycaw.pycaw import AudioUtilities, IAudioEndpointVolume
    from ctypes import cast, POINTER

    devices = AudioUtilities.GetSpeakers()
    interface = devices.Activate(IAudioEndpointVolume._iid_, CLSCTX_ALL, None)
    volume = cast(interface, POINTER(IAudioEndpointVolume))

    current = volume.GetMasterVolumeLevelScalar()
    print(f"  ✓ SUCCESS! Current volume: {current:.0%}")

except Exception as e:
    print(f"  ✗ Error: {type(e).__name__}: {e}")
    import traceback
    traceback.print_exc()

input("\nPress Enter to exit...")
