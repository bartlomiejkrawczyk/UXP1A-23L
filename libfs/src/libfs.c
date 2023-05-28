#include "libfs.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

#include "libfs_errno.h"
#include "libfs_locations.h"
#include "log.h"
#include "request.h"
#include "response.h"
#include "types.h"

static int read_response(fd_type fd, libfs_response_t* response) {
    int status;
    usize data_size;

    isize result = read(fd, &status, sizeof(status));

    if (result < 0) {
        libfs_set_errno(LIBFS_ERRNO_PPIPE_READ);
        return -1;
    } else if (result != sizeof(status)) {
        libfs_set_errno(LIBFS_ERRNO_PPIPE_READ);
        return -1;
    }

    result = read(fd, &data_size, sizeof(data_size));

    if (result < 0) {
        libfs_set_errno(LIBFS_ERRNO_PPIPE_READ);
        return -1;
    } else if (result != sizeof(data_size)) {
        libfs_set_errno(LIBFS_ERRNO_PPIPE_READ);
        return -1;
    }

    u8* data = malloc(data_size);

    result = read(fd, data, data_size);

    if (result < 0) {
        libfs_set_errno(LIBFS_ERRNO_PPIPE_READ);
        return -1;
    } else if ((usize)result != data_size) {
        libfs_set_errno(LIBFS_ERRNO_PPIPE_READ);
        return -1;
    }

    response->status = status;
    response->data_size = data_size;
    response->data = data;

    return 0;
}

static int request_daemon_response(const libfs_request_t* request, libfs_response_t* response) {
    char path_buf[256];

    // open the main pipe
    libfs_get_main_pipe_path(path_buf, 256);
    fd_type request_fd = open(path_buf, O_WRONLY);

    if (request_fd < 0) {
        libfs_set_errno(LIBFS_ERRNO_MPIPE_OPEN);
        return -1;
    }

    usize buffer_size = libfs_request_size(request);
    u8* buffer = libfs_request_serialize(request);

    isize result = write(request_fd, buffer, buffer_size);

    if (result <= 0 || (usize)result != buffer_size) {
        libfs_set_errno(LIBFS_ERRNO_MPIPE_WRITE);
        return -1;
    }

    close(request_fd);
    free(buffer);

    // create the process pipe
    libfs_get_process_pipe_path(path_buf, 256, request->sender);
    if (access(path_buf, F_OK) == -1) {
        if (mkfifo(path_buf, 0666) < 0) {
            libfs_set_errno(LIBFS_ERRNO_PPIPE_OPEN);
            return -1;
        }
    }

    // open the process pipe
    // (before writing to the main pipe, we need to be ready to receive the response)
    fd_type response_fd = open(path_buf, O_RDONLY);

    if (read_response(response_fd, response) != 0) {
        libfs_set_errno(LIBFS_ERRNO_PPIPE_READ);
        close(response_fd);
        return -1;
    }

    close(response_fd);

    unlink(path_buf);

    return 0;
}

int libfs_create(const char* path, u32 mode) {
    libfs_request_create_t create_request = {
        .name = (char*)path,
        .mode = mode,
    };

    libfs_response_t response;
    libfs_request_t packed_request = libfs_request_create_pack(&create_request);

    if (request_daemon_response(&packed_request, &response) != 0) {
        return -1;
    }

    if (response.status != 0) {
        libfs_set_errno(response.status);
        return -1;
    }

    return 0;
}
