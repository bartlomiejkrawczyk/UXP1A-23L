#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libfs.h"
#include "libfs_errno.h"
#include "log.h"
#include "request.h"
#include "types.h"

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        LOG_ERROR("usage: %s <file-descriptor> [offset]", argv[0]);
        return 1;
    }

    long offset = 0;

    if (argc == 3) {
        offset = atol(argv[2]);
    }

    int result = libfs_seek((fd_type)atoi(argv[1]), offset);

    if (result < 0) {
        LOG_LIBFS_ERRNO("libfs_seek failed");
        return 1;
    }

    LOG_INFO("libfs_seek succeeded: %d", result);

    return 0;
}
