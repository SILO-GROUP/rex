#ifndef LCPEX_HELPERS_H
#define LCPEX_HELPERS_H

// helper for sanity
enum PIPE_ENDS {
    READ_END = 0,
    WRITE_END = 1
};

// helper for sanity
// these do NOT represent fd values
enum CHILD_PIPE_NAMES {
    STDOUT_READ = 0,
    STDERR_READ = 1
};

#define BUFFER_SIZE 1024

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

#endif //LCPEX_HELPERS_H
