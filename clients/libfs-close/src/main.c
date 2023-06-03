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
    if (argc != 2) {
        LOG_ERROR("usage: %s <file_descriptor>", argv[0]);
        printf("-1");
        return 1;
    }

    int result = libfs_close((fd_type)atoi(argv[1]));

    if (result < 0) {
        LOG_LIBFS_ERRNO("libfs_close failed");
        printf("-1");
        return 1;
    }

    LOG_INFO("libfs_close succeeded: %d", result);
    printf("%d", result);

    return 0;
}
