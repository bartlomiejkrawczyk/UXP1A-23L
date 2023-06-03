#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libfs.h"
#include "libfs_errno.h"
#include "log.h"
#include "request.h"
#include "types.h"

int parse_flags(char* flags) {
    if (strcmp(flags, "-r") == 0) {
        return O_RDONLY;
    } else if (strcmp(flags, "-w") == 0) {
        return O_WRONLY;
    } else {
        return O_RDWR;
    }
}

int main(int argc, char** argv) {
    if (argc < 2 || argc > 3) {
        LOG_ERROR("usage: %s <path> [flags]", argv[0]);
        printf("-1");
        return 1;
    }

    int flags = O_RDWR;

    if (argc == 3) {
        flags = parse_flags(argv[2]);
    }

    int result = libfs_open(argv[1], (u32)flags);

    if (result < 0) {
        LOG_LIBFS_ERRNO("libfs_open failed");
        return 1;
    }

    LOG_INFO("libfs_open succeeded: %d", result);
    printf("%d", result);

    return 0;
}
