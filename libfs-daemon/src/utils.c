#include "utils.h"

#include "log.h"
#include "types.h"
#include "globals.h"

#include <stdnoreturn.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

noreturn void shutdown_daemon(int code) {
    LOG_INFO("SHUTTING DOWN DAEMON WITH CODE: %d", code);
    LOG_INFO("closing pipe fd: %d", g_libfs_pipe_fd);
    close(g_libfs_pipe_fd);
    unlink("libfs_pipe");

    exit(code);
}

void dump_buffer(const u8* buffer, usize buffer_size, usize width) {
    printf("buffer_size: %ld\n", buffer_size);
    printf("buffer: [\n");
    for (usize i = 0; i < buffer_size; i++) {
        printf("\t0x%02x,", buffer[i]);
        if (i % width == width - 1) {
            printf("\n");
        }
    }
    printf("\n]\n");
}

