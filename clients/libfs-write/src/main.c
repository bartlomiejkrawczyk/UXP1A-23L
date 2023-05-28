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
        printf("-1");
        return 1;
    }

    // TODO: verify content lenght works correctly
    int result = libfs_write(atoi(argv[1]), (u8*)argv[2], (u32)strlen(argv[2]) + 1);

    if (result < 0) {
        LOG_LIBFS_ERRNO("libfs_write failed");
        printf("-1");
        return 1;
    }

    LOG_INFO("libfs_write succeeded: %d", result);
    printf("%d", result);

    return 0;
}
