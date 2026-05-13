#include "udp_listener.h"
#include <cstdio>
#include <cstring>

#define UDP_PORT 7878
#define BUF_SIZE 256

// =========================================================================
// Platform socket abstraction
// =========================================================================
#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include <winsock2.h>
   typedef SOCKET     sock_t;
#  define SOCK_INVALID INVALID_SOCKET
#  define sock_close   closesocket
   static void sock_init(void)    { WSADATA w; WSAStartup(MAKEWORD(2,2), &w); }
   static void sock_cleanup(void) { WSACleanup(); }
   typedef int socklen_t;
   static void set_recv_timeout(sock_t s) {
       DWORD ms = 500;
       setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&ms, sizeof(ms));
   }
#else
#  include <sys/socket.h>
#  include <netinet/in.h>
#  include <unistd.h>
   typedef int        sock_t;
#  define SOCK_INVALID (-1)
#  define sock_close   close
   static void sock_init(void)    {}
   static void sock_cleanup(void) {}
   static void set_recv_timeout(sock_t s) {
       struct timeval tv = { 0, 500000 }; // 500ms
       setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));
   }
#endif

// =========================================================================
// Shared state
// =========================================================================
static udp_callbacks_t  g_cbs;
static volatile int     g_running = 0;

// =========================================================================
// Listener logic (shared across platforms)
// =========================================================================
static void listener_loop(void) {
    sock_init();

    sock_t s = socket(AF_INET, SOCK_DGRAM, 0);
    if (s == SOCK_INVALID) {
        fprintf(stderr, "[udp] socket() failed\n");
        sock_cleanup();
        return;
    }

    set_recv_timeout(s);

    struct sockaddr_in addr = {};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port        = htons(UDP_PORT);

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) != 0) {
        fprintf(stderr, "[udp] bind() failed on port %d — continuing without UDP\n", UDP_PORT);
        sock_close(s);
        sock_cleanup();
        return;
    }

    char buf[BUF_SIZE];
    char reply_buf[BUF_SIZE];
    struct sockaddr_in sender;
    socklen_t sender_len = sizeof(sender);

    buffer_t reply = { reply_buf, BUF_SIZE };

    while (g_running)
	{
        int n = (int)recvfrom(s, buf, BUF_SIZE - 1, 0, (struct sockaddr*)&sender, &sender_len);
        if (n <= 0) continue; // timeout or error — check g_running and loop

        buf[n] = '\0';
        reply_buf[0] = '\0';

        if (strcmp(buf, "enable cena landmine") == 0)
		{
			if (g_cbs.on_enable)
			{
				g_cbs.on_enable(&reply);
			}
		}
		else if (strcmp(buf, "disable cena landmine") == 0)
		{
			if (g_cbs.on_disable)
			{
				g_cbs.on_disable(&reply);
			}
		}
		else if (strcmp(buf, "trigger cena") == 0)
		{
			if (g_cbs.on_trigger)
			{
				g_cbs.on_trigger(&reply);
			}
		}
		else if (strcmp(buf, "stop cena") == 0)
		{
			if (g_cbs.on_stop)
			{
				g_cbs.on_stop(&reply);
			}
		}

		if (reply_buf[0] != '\0')
		{
			sendto(s, reply_buf, (int)strlen(reply_buf), 0, (struct sockaddr*)&sender, sender_len);
		}
    }

    sock_close(s);
    sock_cleanup();
}

// =========================================================================
// Platform thread
// =========================================================================
#ifdef _WIN32

static HANDLE g_thread;

static DWORD WINAPI thread_proc(LPVOID) { listener_loop(); return 0; }

int udp_listener_start(udp_callbacks_t cbs) {
    g_cbs    = cbs;
    g_running = 1;
    g_thread  = CreateThread(NULL, 0, thread_proc, NULL, 0, NULL);
    return g_thread ? 0 : 1;
}

void udp_listener_stop(void) {
    g_running = 0;
    WaitForSingleObject(g_thread, INFINITE);
    CloseHandle(g_thread);
    g_thread = NULL;
}

#else

#include <pthread.h>

static pthread_t g_thread;

static void* thread_proc(void*) { listener_loop(); return NULL; }

int udp_listener_start(udp_callbacks_t cbs) {
    g_cbs     = cbs;
    g_running = 1;
    return pthread_create(&g_thread, NULL, thread_proc, NULL);
}

void udp_listener_stop(void) {
    g_running = 0;
    pthread_join(g_thread, NULL);
}

#endif
