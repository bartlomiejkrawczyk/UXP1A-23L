#include "libfs.h"

#include <fcntl.h>
#include <stdlib.h>
#include <sys/file.h>
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

// lock the main pipe (with the provided fd)
// use fcntl to lock the pipe
static int libfs_pipe_lock(fd_type fd) {
    struct flock lock = {
        .l_type = F_WRLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0,
    };

    if (fcntl(fd, F_SETLKW, &lock) < 0) {
        return -1;
    }

    return 0;
}

// unlock the main pipe (with the provided fd)
// use fcntl to unlock the pipe
static int libfs_pipe_unlock(fd_type fd) {
    struct flock lock = {
        .l_type = F_UNLCK,
        .l_whence = SEEK_SET,
        .l_start = 0,
        .l_len = 0,
    };

    while (fcntl(fd, F_SETLK, &lock) < 0) {
        if (errno != EINTR) {
            return -1;
        }
    }

    return 0;
}

static void ensure_daemon(void);

static int request_daemon_response(const libfs_request_t* request, libfs_response_t* response) {
    ensure_daemon();
    char path_buf[256];

    // open the main pipe
    libfs_get_main_pipe_path(path_buf, 256);
    fd_type request_fd = open(path_buf, O_WRONLY);

    int attempts = 0;

    while (request_fd < 0 && attempts < 10) {
        sleep(1);
        request_fd = open(path_buf, O_WRONLY);
        attempts++;
    }

    if (request_fd < 0) {
        libfs_set_errno(LIBFS_ERRNO_MPIPE_OPEN);
        return -1;
    }

    usize buffer_size = libfs_request_size(request);
    u8* buffer = libfs_request_serialize(request);

    if (libfs_pipe_lock(request_fd) < 0) {
        libfs_set_errno(LIBFS_ERRNO_MPIPE_LOCK);
        return -1;
    }

    isize already_written = 0;
    while (already_written < buffer_size) {
        isize result = write(request_fd, buffer, buffer_size);

        if (result <= 0) {
            libfs_set_errno(LIBFS_ERRNO_MPIPE_WRITE);
            return -1;
        }
        already_written += result;
    }

    if (libfs_pipe_unlock(request_fd) < 0) {
        libfs_set_errno(LIBFS_ERRNO_MPIPE_UNLOCK);
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

static bool check_daemon(void) {
    char path_buf[256];
    libfs_get_lockfile_path(path_buf, 256);

    if (access(path_buf, F_OK) == -1) {
        return false;
    }

    fd_type lockfile_fd = open(path_buf, O_RDONLY);

    if (lockfile_fd < 0) {
        return false;
    }

    int flock_result = flock(lockfile_fd, LOCK_EX | LOCK_NB);

    if (flock_result < 0) {
        return true;
    }

    close(lockfile_fd);

    return false;
}

static void ensure_daemon(void) {
    if (check_daemon()) {
        return;
    }

    pid_t pid = fork();

    if (pid != 0) {
        return;
    }

    // child process
    char path_buf[256];
    libfs_get_daemon_path(path_buf, 256);

    // exec the daemon
    execl(path_buf, path_buf, NULL);

    // if that fails, exit
    exit(1);
}

int libfs_chmode(const char* name, u32 mode) {
    libfs_request_chmode_t chmode_request = {
        .name = (char*)name,
        .mode = mode,
    };

    libfs_response_t response;
    libfs_request_t packed_request = libfs_request_chmode_pack(&chmode_request);

    if (request_daemon_response(&packed_request, &response) != 0) {
        return -1;
    }

    if (response.status != 0) {
        libfs_set_errno(response.status);
        return -1;
    }

    return 0;
}

int libfs_close(fd_type fd) {
    libfs_request_close_t close_request = {
        .fd = fd,
    };

    libfs_response_t response;
    libfs_request_t packed_request = libfs_request_close_pack(&close_request);

    if (request_daemon_response(&packed_request, &response) != 0) {
        return -1;
    }

    if (response.status != 0) {
        libfs_set_errno(response.status);
        return -1;
    }

    return 0;
}

fd_type libfs_create(const char* path, u32 mode) {
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

    fd_type file_descriptor;
    memcpy(&file_descriptor, response.data, response.data_size);
    return file_descriptor;
}

int libfs_link(const char* source, const char* destination) {
    libfs_request_link_t link_request = {
        .source = (char*)source,
        .destination = (char*)destination,
    };

    libfs_response_t response;
    libfs_request_t packed_request = libfs_request_link_pack(&link_request);

    if (request_daemon_response(&packed_request, &response) != 0) {
        return -1;
    }

    if (response.status != 0) {
        libfs_set_errno(response.status);
        return -1;
    }

    return 0;
}

fd_type libfs_open(char* name, u32 flags) {
    libfs_request_open_t open_request = {
        .name = (char*)name,
        .flags = flags,
    };

    libfs_response_t response;
    libfs_request_t packed_request = libfs_request_open_pack(&open_request);

    if (request_daemon_response(&packed_request, &response) != 0) {
        return -1;
    }

    if (response.status != 0) {
        libfs_set_errno(response.status);
        return -1;
    }

    fd_type file_descriptor;
    memcpy(&file_descriptor, response.data, response.data_size);
    return file_descriptor;
}

int libfs_read(fd_type fd, u8* buf, usize size) {
    libfs_request_read_t read_request = {
        .fd = fd,
        .size = size,
    };

    libfs_response_t response;
    libfs_request_t packed_request = libfs_request_read_pack(&read_request);

    if (request_daemon_response(&packed_request, &response) != 0) {
        return -1;
    }

    if (response.status != 0) {
        libfs_set_errno(response.status);
        return -1;
    }

    memcpy(buf, response.data, response.data_size);

    return (int)response.data_size;
}

int libfs_rename(char* oldname, char* newname) {
    libfs_request_rename_t rename_request = {
        .old_name = (char*)oldname,
        .new_name = (char*)newname,
    };

    libfs_response_t response;
    libfs_request_t packed_request = libfs_request_rename_pack(&rename_request);

    if (request_daemon_response(&packed_request, &response) != 0) {
        return -1;
    }

    if (response.status != 0) {
        libfs_set_errno(response.status);
        return -1;
    }

    return 0;
}

ssize_t libfs_seek(fd_type fd, ssize_t offset) {
    libfs_request_seek_t seek_request = {
        .fd = fd,
        .offset = offset,
    };

    libfs_response_t response;
    libfs_request_t packed_request = libfs_request_seek_pack(&seek_request);

    if (request_daemon_response(&packed_request, &response) != 0) {
        return -1;
    }

    if (response.status != 0) {
        libfs_set_errno(response.status);
        return -1;
    }

    return (ssize_t)(*(ssize_t*)(response.data));
}

int libfs_stat(int fd, libfs_stat_struct_t* restrict statbuf) {
    libfs_request_stat_t stat_request = {
        .fd = fd,
    };

    libfs_response_t response;
    libfs_request_t packed_request = libfs_request_stat_pack(&stat_request);

    if (request_daemon_response(&packed_request, &response) != 0) {
        return -1;
    }

    if (response.status != 0) {
        libfs_set_errno(response.status);
        return -1;
    }

    memcpy(statbuf, response.data, response.data_size);

    return 0;
}

int libfs_symlink(const char* source, const char* destination) {
    libfs_request_symlink_t symlink_request = {
        .source = (char*)source,
        .destination = (char*)destination,
    };

    libfs_response_t response;
    libfs_request_t packed_request = libfs_request_symlink_pack(&symlink_request);

    if (request_daemon_response(&packed_request, &response) != 0) {
        return -1;
    }

    if (response.status != 0) {
        libfs_set_errno(response.status);
        return -1;
    }

    return 0;
}

int libfs_unlink(char* name) {
    libfs_request_unlink_t unlink_request = {
        .name = (char*)name,
    };

    libfs_response_t response;
    libfs_request_t packed_request = libfs_request_unlink_pack(&unlink_request);

    if (request_daemon_response(&packed_request, &response) != 0) {
        return -1;
    }

    if (response.status != 0) {
        libfs_set_errno(response.status);
        return -1;
    }

    return 0;
}

ssize_t libfs_write(fd_type fd, u8* buf, unsigned int size) {
    libfs_request_write_t write_request = {
        .fd = fd,
        .data = buf,
        .size = size,
    };

    libfs_response_t response;
    libfs_request_t packed_request = libfs_request_write_pack(&write_request);

    if (request_daemon_response(&packed_request, &response) != 0) {
        return -1;
    }

    if (response.status != 0) {
        libfs_set_errno(response.status);
        return -1;
    }

    return (ssize_t)(*(ssize_t*)(response.data));
}

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
