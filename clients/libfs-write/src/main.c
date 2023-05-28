#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "libfs.h"
#include "libfs_errno.h"
#include "log.h"
#include "request.h"
#include "types.h"

static int parse_octal(const char* mode) {
    int result = 0;

    for (usize i = 0; i < 3; i++) {
        if (mode[i] < '0' || mode[i] > '7') {
            return -1;
        }

        result = result * 8 + (mode[i] - '0');
    }

    return result;
}

static int parse_rwx(const char* mode) {
    int result = 0;

    for (usize i = 0; i < 9; i++) {
        if (i % 3 == 0) {
            if (mode[i] != 'r' && mode[i] != '-') {
                return -1;
            }

            result |= mode[i] == 'r';
            result <<= 1;

        } else if (i % 3 == 1) {
            if (mode[i] != 'w' && mode[i] != '-') {
                return -1;
            }

            result |= mode[i] == 'w';
            result <<= 1;
        } else {
            if (mode[i] != 'x' && mode[i] != '-') {
                return -1;
            }

            result |= mode[i] == 'x';
            result <<= 1;
        }
    }

    result >>= 1;  // compensate for the last shift

    return result;
}

int parse_mode(const char* mode, usize len) {
    if (len == 3) {
        return parse_octal(mode);
    } else if (len == 9) {
        return parse_rwx(mode);
    } else {
        return -1;
    }
}

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
