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
        LOG_ERROR("usage: %s <path> [mode]", argv[0]);
        return 1;
    }

    int mode = 0777;

    if (argc == 3) {
        usize len = strlen(argv[2]);

        mode = parse_mode(argv[2], len);

        if (mode < 0) {
            LOG_ERROR("invalid mode: %s", argv[2]);
            return 1;
        }
    }

    int result = libfs_create(argv[1], (u32)mode);

    if (result < 0) {
        LOG_LIBFS_ERRNO("libfs_create failed");
        return 1;
    }

    LOG_INFO("libfs_create succeeded: %d", result);

    return 0;
}
