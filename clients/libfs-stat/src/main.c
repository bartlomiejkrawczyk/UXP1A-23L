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
        LOG_ERROR("usage: %s <path>", argv[0]);
        return 1;
    }

    libfs_stat_struct_t* stats = malloc(sizeof(libfs_stat_struct_t));
    int result = libfs_stat(argv[1], stats);

    if (result < 0) {
        LOG_LIBFS_ERRNO("libfs_stat failed");
        return 1;
    }

    LOG_INFO("libfs_stat succeeded: %d", result);
    // TODO: print all the stats
    printf("stats:\nsize %ld", stats->st_size);
    free(stats);

    return 0;
}
