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
        LOG_ERROR("usage: %s <file-descriptor>", argv[0]);
        return 1;
    }

    libfs_stat_struct_t* stats = malloc(sizeof(libfs_stat_struct_t));
    int result = libfs_stat(atoi(argv[1]), stats);

    if (result < 0) {
        LOG_LIBFS_ERRNO("libfs_stat failed");
        return 1;
    }

    LOG_INFO("libfs_stat succeeded: %d", result);

    printf("System Inode number: \t\t\t%ld\n", stats->st_ino);
    printf("File type and mode: \t\t\t%d\n", stats->st_mode);
    printf("Number of hard links: \t\t\t%ld\n", stats->st_nlink);
    printf("Total size, in bytes: \t\t\t%ld\n", stats->st_size);
    printf("Block size for filesystem I/O: \t\t%ld\n", stats->st_blksize);
    printf("Number of 512B blocks allocated: \t%ld\n\n", stats->st_blocks);
    printf("Time of last access: \t\t\t%ld.%.9ld\n", stats->st_atim.tv_sec,
           stats->st_atim.tv_nsec);
    printf("Time of last modification: \t\t%ld.%.9ld\n", stats->st_mtim.tv_sec,
           stats->st_mtim.tv_nsec);
    printf("Time of last status change: \t\t%ld.%.9ld\n", stats->st_ctim.tv_sec,
           stats->st_ctim.tv_nsec);

    free(stats);

    return 0;
}
