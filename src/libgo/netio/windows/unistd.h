#ifndef LIBGO_WINDOWS_UNISTD_H
#define LIBGO_WINDOWS_UNISTD_H
#include <WinSock2.h>
#include <Windows.h>
#include <stdint.h>

typedef int64_t ssize_t;

typedef int socklen_t;

extern "C" {
    static void usleep(uint64_t microseconds)
    {
        ::Sleep((uint32_t)(microseconds / 1000));
    }

    static unsigned int sleep(unsigned int seconds)
    {
        ::Sleep(seconds * 1000);
        return seconds;
    }

    static int poll(struct pollfd *fds, unsigned long nfds, int timeout)
    {
#if(_WIN32_WINNT >= 0x0600) && INCL_WINSOCK_API_PROTOTYPES
        return WSAPoll(fds, nfds, timeout);
#endif
        return 0;
    }

}
#endif // LIBGO_WINDOWS_UNISTD_H
