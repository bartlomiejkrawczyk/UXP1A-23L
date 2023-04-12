#include "libfs_errno.h"

#include <stdio.h>
#include <threads.h>

thread_local int libfs_errno;

void libfs_set_errno(int errno) {
    libfs_errno = errno;
}

const char* libfs_strerror(int errno) {
    switch (errno) {
        case LIBFS_ERRNO_OK:
            return "No error";
        case LIBFS_ERRNO_UNKNOWN:
            return "Unknown error";
        default:
            return "Unknown error";
    }
}

void libfs_perror(const char* msg) {
    fprintf(stderr, "%s: %s\n", msg, libfs_strerror(libfs_errno));
}
