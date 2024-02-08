#include "helpers.h"

ssize_t write_all(int fd, const void *buf, size_t count) {
    const char *p = (const char *)buf;
    while (count > 0) {
        ssize_t written = write(fd, p, count);
        if (written == -1) {
            if (errno == EINTR || errno == EAGAIN) continue; // Retry
            return -1; // Other errors
        }
        count -= written;
        p += written;
    }
    return 0;
}