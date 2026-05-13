#include "keylogger.h"
#include <cstdio>

// -------------------------------------------------------------------------
// Shared state (all platforms)
// -------------------------------------------------------------------------
static keylogger_key_t      g_reg_key = KEY_UNKNOWN;
static keylogger_callback_t g_cb      = nullptr;

void keylogger_register(keylogger_key_t key, keylogger_callback_t cb) {
    g_reg_key = key;
    g_cb      = cb;
}

static inline void dispatch(keylogger_key_t key) {
    if (g_cb && (g_reg_key == KEY_UNKNOWN || g_reg_key == key)) {
        g_cb(key);
    }
}

// =========================================================================
// Windows — WH_KEYBOARD_LL
// =========================================================================
#if defined(_WIN32)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

static HHOOK  g_hook        = NULL;
static HANDLE g_thread      = NULL;
static DWORD  g_thread_id   = 0;

static LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && wParam == WM_KEYDOWN) {
        KBDLLHOOKSTRUCT* kbs = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);
        if (kbs->vkCode == VK_SPACE) {
            dispatch(KEY_SPACE);
        }
    }
    return CallNextHookEx(g_hook, nCode, wParam, lParam);
}

static DWORD WINAPI HookThreadProc(LPVOID) {
    g_hook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);
    if (!g_hook) {
        fprintf(stderr, "[keylogger] SetWindowsHookEx failed (%lu)\n", GetLastError());
        return 1;
    }
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UnhookWindowsHookEx(g_hook);
    g_hook = NULL;
    return 0;
}

int keylogger_start(void) {
    g_thread = CreateThread(NULL, 0, HookThreadProc, NULL, 0, &g_thread_id);
    return (g_thread != NULL) ? 0 : 1;
}

void keylogger_stop(void) {
    if (g_thread) {
        PostThreadMessage(g_thread_id, WM_QUIT, 0, 0);
        WaitForSingleObject(g_thread, INFINITE);
        CloseHandle(g_thread);
        g_thread    = NULL;
        g_thread_id = 0;
    }
}

// =========================================================================
// macOS — CGEventTap
// =========================================================================
#elif defined(__APPLE__)

#include <ApplicationServices/ApplicationServices.h>
#include <pthread.h>

// kVK_Space = 0x31 (Carbon HIToolbox/Events.h)
static const CGKeyCode kVK_Space = 0x31;

static pthread_t    g_thread;
static CFRunLoopRef g_run_loop = NULL;

static CGEventRef EventTapCallback(CGEventTapProxy, CGEventType type,
                                   CGEventRef event, void*) {
    if (type == kCGEventKeyDown) {
        CGKeyCode code = (CGKeyCode)CGEventGetIntegerValueField(
            event, kCGKeyboardEventKeycode);
        if (code == kVK_Space) {
            dispatch(KEY_SPACE);
        }
    }
    return event;
}

static void* MacOSThreadProc(void*) {
    CGEventMask mask = CGEventMaskBit(kCGEventKeyDown);
    CFMachPortRef tap = CGEventTapCreate(
        kCGSessionEventTap,
        kCGHeadInsertEventTap,
        kCGEventTapOptionListenOnly,
        mask,
        EventTapCallback,
        NULL);
    if (!tap) {
        fprintf(stderr, "[keylogger] CGEventTapCreate failed — grant Accessibility permission\n");
        return NULL;
    }
    CFRunLoopSourceRef src = CFMachPortCreateRunLoopSource(kCFAllocatorDefault, tap, 0);
    g_run_loop = CFRunLoopGetCurrent();
    CFRunLoopAddSource(g_run_loop, src, kCFRunLoopCommonModes);
    CGEventTapEnable(tap, true);
    CFRunLoopRun();
    CGEventTapEnable(tap, false);
    CFMachPortInvalidate(tap);
    CFRelease(tap);
    CFRelease(src);
    g_run_loop = NULL;
    return NULL;
}

int keylogger_start(void) {
    return pthread_create(&g_thread, NULL, MacOSThreadProc, NULL);
}

void keylogger_stop(void) {
    if (g_run_loop) {
        CFRunLoopStop(g_run_loop);
    }
    pthread_join(g_thread, NULL);
}

// =========================================================================
// Linux — evdev (/dev/input/eventX)
// =========================================================================
#elif defined(__linux__)

#include <pthread.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <cstring>

// Capture the evdev scancode (57) before undefing the kernel macro,
// which otherwise shadows our keylogger_key_t::KEY_SPACE enumerator (32).
static constexpr int EVDEV_SPACE = KEY_SPACE;
#undef KEY_SPACE

static pthread_t        g_thread;
static volatile int     g_running = 0;

// Returns an open fd for the first /dev/input/eventX that reports EVDEV_SPACE (57),
// or -1 if none found.
static int find_keyboard_device(void) {
    for (int i = 0; i < 32; ++i) {
        char path[32];
        snprintf(path, sizeof(path), "/dev/input/event%d", i);
        int fd = open(path, O_RDONLY | O_NONBLOCK);
        if (fd < 0) continue;

        unsigned long evbits = 0;
        ioctl(fd, EVIOCGBIT(0, sizeof(evbits)), &evbits);
        if (!(evbits & (1 << EV_KEY))) { close(fd); continue; }

        unsigned long keybits[(KEY_MAX / (8 * sizeof(unsigned long))) + 1] = {};
        ioctl(fd, EVIOCGBIT(EV_KEY, sizeof(keybits)), keybits);

        int word  = EVDEV_SPACE / (8 * sizeof(unsigned long));
        int bit   = EVDEV_SPACE % (8 * sizeof(unsigned long));
        if (keybits[word] & (1UL << bit)) {
            char name[256] = {};
            ioctl(fd, EVIOCGNAME(sizeof(name)), name);
            printf("[keylogger] using %s (%s)\n", path, name);
            return fd;
        }
        close(fd);
    }
    return -1;
}

static void* EvdevThreadProc(void*) {
    int fd = find_keyboard_device();
    if (fd < 0) {
        fprintf(stderr, "[keylogger] no keyboard device found under /dev/input/ "
                        "(may need root or input group membership)\n");
        return NULL;
    }
    struct input_event ev;
    while (g_running) {
        struct pollfd pfd = { fd, POLLIN, 0 };
        if (poll(&pfd, 1, 100) <= 0) continue;
        if (read(fd, &ev, sizeof(ev)) != (ssize_t)sizeof(ev)) continue;
        if (ev.type == EV_KEY && ev.value == 1 && ev.code == EVDEV_SPACE) {
            dispatch(KEY_SPACE);
        }
    }
    close(fd);
    return NULL;
}

int keylogger_start(void) {
    g_running = 1;
    return pthread_create(&g_thread, NULL, EvdevThreadProc, NULL);
}

void keylogger_stop(void) {
    g_running = 0;
    pthread_join(g_thread, NULL);
}

// =========================================================================
// Unsupported platform stub
// =========================================================================
#else

int keylogger_start(void) {
    fprintf(stderr, "[keylogger] platform not supported\n");
    return 1;
}

void keylogger_stop(void) {}

#endif
