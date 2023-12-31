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
    if (argc != 3) {
        LOG_ERROR("usage: %s <source> <destination>", argv[0]);
        printf("-1");
        return 1;
    }

    int result = libfs_symlink(argv[1], argv[2]);

    if (result < 0) {
        LOG_LIBFS_ERRNO("libfs_symlink failed");
        return 1;
    }

    LOG_INFO("libfs_symlink succeeded: %d", result);

    return 0;
}
