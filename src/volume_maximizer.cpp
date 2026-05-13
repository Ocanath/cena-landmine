#include "volume_maximizer.h"
#include <cstdio>

// =========================================================================
// Windows — COM / IAudioEndpointVolume (WASAPI)
// =========================================================================
#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#define INITGUID
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

void volume_maximize(void) {
    if (FAILED(CoInitialize(NULL))) return;

    IMMDeviceEnumerator* enumerator = nullptr;
    HRESULT hr = CoCreateInstance(
        __uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL,
        __uuidof(IMMDeviceEnumerator), reinterpret_cast<void**>(&enumerator));
    if (FAILED(hr)) { CoUninitialize(); return; }

    IMMDevice* device = nullptr;
    hr = enumerator->GetDefaultAudioEndpoint(eRender, eConsole, &device);
    enumerator->Release();
    if (FAILED(hr)) { CoUninitialize(); return; }

    IAudioEndpointVolume* vol = nullptr;
    hr = device->Activate(
        __uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL,
        reinterpret_cast<void**>(&vol));
    device->Release();
    if (FAILED(hr)) { CoUninitialize(); return; }

    vol->SetMute(FALSE, NULL);
    vol->SetMasterVolumeLevelScalar(1.0f, NULL);
    vol->Release();

    CoUninitialize();
}

// =========================================================================
// macOS — osascript (no extra frameworks required)
// =========================================================================
#elif defined(__APPLE__)

#include <cstdlib>

void volume_maximize(void) {
    system("osascript -e 'set volume output volume 100'");
}

// =========================================================================
// Linux — not yet implemented
// =========================================================================
#elif defined(__linux__)

void volume_maximize(void) {
    // TODO: implement via pactl / amixer
}

// =========================================================================
// Unsupported platform stub
// =========================================================================
#else

void volume_maximize(void) {}

#endif
