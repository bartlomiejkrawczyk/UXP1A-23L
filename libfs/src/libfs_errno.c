#include "libfs_errno.h"

#include <stdio.h>
#include <string.h>
#include <threads.h>

thread_local int libfs_errno;

void libfs_set_errno(int err) {
    libfs_errno = err;
}

const char* libfs_strerror(int err) {
    switch (err) {
        case LIBFS_ERRNO_OK:
            return "No error";
        case LIBFS_ERRNO_MPIPE_OPEN:
            return "Could not open message pipe";
        case LIBFS_ERRNO_MPIPE_WRITE:
            return "Could not write to message pipe";
        case LIBFS_ERRNO_MPIPE_READ:
            return "Could not read from message pipe";
        case LIBFS_ERRNO_PPIPE_OPEN:
            return "Could not open process response pipe";
        case LIBFS_ERRNO_PPIPE_WRITE:
            return "Could not write to process response pipe";
        case LIBFS_ERRNO_PPIPE_READ:
            return "Could not read from process response pipe";
        default:
            return strerror(err);
    }
}

void libfs_perror(const char* msg) {
    fprintf(stderr, "%s: %s\n", msg, libfs_strerror(libfs_errno));
}
