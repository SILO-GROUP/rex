#pragma once
#include <unistd.h> // for close()

// Simplified RAII wrapper for file descriptors (C-style compatible)
class FdGuard {
    int fd_;
public:
    // Constructor
    explicit FdGuard(int fd = -1) : fd_(fd) {}

    // Disable copy semantics
    FdGuard(const FdGuard&) = delete;
    FdGuard& operator=(const FdGuard&) = delete;

    // Move constructor
    FdGuard(FdGuard&& other) /* no noexcept */ {
        fd_ = other.fd_;
        other.fd_ = -1;
    }

    // Move assignment
    FdGuard& operator=(FdGuard&& other) /* no noexcept */ {
        if (this != &other) {
            reset(); // Close current if valid
            fd_ = other.fd_;
            other.fd_ = -1;
        }
        return *this;
    }

    // Destructor
    ~FdGuard() {
        close_if_valid();
    }

    // Reset with new fd (or close if -1)
    void reset(int new_fd = -1) {
        close_if_valid();
        fd_ = new_fd;
    }

    // Get the raw fd
    int get() const {
        return fd_;
    }

private:
    void close_if_valid() {
        if (fd_ != -1) {
            ::close(fd_);
            fd_ = -1;
        }
    }
};
