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
        LOG_ERROR("usage: %s <path>", argv[0]);
        printf("-1");
        return 1;
    }

    int result = libfs_unlink(argv[1]);

    if (result < 0) {
        LOG_LIBFS_ERRNO("libfs_unlink failed");
        printf("-1");
        return 1;
    }

    LOG_INFO("libfs_unlink succeeded: %d", result);
    printf("%d", result);

    return 0;
}
