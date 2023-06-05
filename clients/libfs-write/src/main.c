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
        LOG_ERROR("usage: %s <file-descriptor> <content>", argv[0]);
        return 1;
    }

    ssize_t result = libfs_write(atoi(argv[1]), (u8*)argv[2], (u32)strlen(argv[2]));

    if (result < 0) {
        LOG_LIBFS_ERRNO("libfs_write failed");
        return 1;
    }

    LOG_INFO("libfs_write succeeded: %ld", result);
    printf("%ld", result);

    return 0;
}
