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
        LOG_ERROR("usage: %s <file-descriptor> <size>", argv[0]);
        return 1;
    }

    usize size = (usize)atoi(argv[2]);
    u8* buf = malloc(size);
    int result = libfs_read((fd_type)atoi(argv[1]), buf, size);

    if (result < 0) {
        LOG_LIBFS_ERRNO("libfs_read failed");
        return 1;
    }

    LOG_INFO("libfs_read succeeded: %d", result);
    printf("%s", buf);
    free(buf);

    return 0;
}
