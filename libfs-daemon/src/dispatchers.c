#define __USE_XOPEN_EXTENDED
#include "dispatchers.h"

#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "globals.h"
#include "libfs_locations.h"
#include "log.h"
#include "response.h"
#include "types.h"
#include "utils.h"

void send_response(pid_t sender, const libfs_response_t* response) {
    char path_buf[256];
    libfs_get_process_pipe_path(path_buf, 256, sender);

    LOG_INFO("opening pipe to sender: %s", path_buf);

    fd_type fd = open(path_buf, O_WRONLY);

    while (fd < 0 && errno == ENOENT) {
        LOG_WARN("%s", "sender did not create pipe yet, retrying in 1 second");
        fd = open(path_buf, O_WRONLY);
    }

    if (fd < 0) {
        LOG_ERRNO("could not open pipe to sender");
        shutdown_daemon(1);
    }

    LOG_INFO("status: %d, data_size: %ld", response->status, response->data_size);

    isize result = write(fd, &response->status, sizeof(response->status));

    if (result < 0) {
        LOG_ERRNO("could not write response status");
        shutdown_daemon(1);
    } else if (result != sizeof(response->status)) {
        LOG_ERROR("could not write response status, expected %ld bytes, got %ld bytes",
                  sizeof(response->status), result);
        shutdown_daemon(1);
    }

    result = write(fd, &response->data_size, sizeof(response->data_size));

    if (result < 0) {
        LOG_ERRNO("could not write response data size");
        shutdown_daemon(1);
    } else if (result != sizeof(response->data_size)) {
        LOG_ERROR("could not write response data size, expected %ld bytes, got %ld bytes",
                  sizeof(response->data_size), result);
        shutdown_daemon(1);
    }

    result = write(fd, response->data, response->data_size);

    if (result < 0) {
        LOG_ERRNO("could not write response data");
        shutdown_daemon(1);
    } else if ((usize)result != response->data_size) {
        LOG_ERROR("could not write response data, expected %ld bytes, got %ld bytes",
                  response->data_size, result);
        shutdown_daemon(1);
    }
}

void chmode_handler(pid_t sender, const REQUEST_TYPE(chmode) * request) {
    LOG_INFO("mode: %o, name: %s", request->mode, request->name);

    char path_buf[256];
    libfs_get_files_path(path_buf, 256);

    strncat(path_buf, request->name, 256 - strlen(path_buf));

    LOG_INFO("path: %s", path_buf);

    u32 prev_umask = umask(0);
    int result = chmod(path_buf, request->mode);
    umask(prev_umask);

    if (result < 0) {
        LOG_ERRNO("could not change file mode");
        libfs_response_t response = {
            .status = errno,
            .data_size = 0,
            .data = NULL,
        };

        send_response(sender, &response);
        return;
    }

    u8* data = malloc(sizeof(int));
    memcpy(data, (void*)&result, sizeof(int));

    libfs_response_t response = {
        .status = 0,
        .data_size = sizeof(int),
        .data = data,
    };

    send_response(sender, &response);
}

void close_handler(pid_t sender, const REQUEST_TYPE(close) * request) {
    LOG_INFO("fd: %d", request->fd);

    int result = close(request->fd);

    if (result < 0) {
        LOG_ERRNO("could not close file descriptor");
        libfs_response_t response = {
            .status = errno,
            .data_size = 0,
            .data = NULL,
        };

        send_response(sender, &response);
        return;
    }

    u8* data = malloc(sizeof(int));
    memcpy(data, (void*)&result, sizeof(int));

    libfs_response_t response = {
        .status = 0,
        .data_size = sizeof(int),
        .data = data,
    };

    send_response(sender, &response);
}

void create_handler(pid_t sender, const REQUEST_TYPE(create) * request) {
    LOG_INFO("mode: %o, name: %s", request->mode, request->name);

    char path_buf[256];
    libfs_get_files_path(path_buf, 256);

    strncat(path_buf, request->name, 256 - strlen(path_buf));

    LOG_INFO("path: %s", path_buf);

    if (access(path_buf, F_OK) == 0) {
        LOG_ERROR("file already exists: %s", path_buf);
        libfs_response_t response = {
            .status = EEXIST,
            .data_size = 0,
            .data = NULL,
        };

        send_response(sender, &response);
        return;
    }

    u32 prev_umask = umask(0);

    fd_type fd = creat(path_buf, request->mode);

    umask(prev_umask);

    if (fd < 0) {
        LOG_ERRNO("could not create file");
        libfs_response_t response = {
            .status = errno,
            .data_size = 0,
            .data = NULL,
        };

        send_response(sender, &response);
        return;
    }

    u8* data = malloc(sizeof(fd_type));
    memcpy(data, (void*)&fd, sizeof(fd_type));

    libfs_response_t response = {
        .status = 0,
        .data_size = sizeof(fd_type),
        .data = data,
    };

    send_response(sender, &response);
}

void link_handler(pid_t sender, const REQUEST_TYPE(link) * request) {
    LOG_INFO("source: %s, destination: %s", request->source, request->destination);

    char path_buf[256];
    char source_buf[256];
    char destination_buf[256];
    libfs_get_files_path(path_buf, 256);

    memcpy(source_buf, path_buf, 256);
    memcpy(destination_buf, path_buf, 256);

    strncat(source_buf, request->source, 256 - strlen(source_buf));
    strncat(destination_buf, request->destination, 256 - strlen(destination_buf));

    LOG_INFO("path: %s", path_buf);

    int result = link(source_buf, destination_buf);

    if (result < 0) {
        LOG_ERRNO("could not create a hard link to a file");
        libfs_response_t response = {
            .status = errno,
            .data_size = 0,
            .data = NULL,
        };

        send_response(sender, &response);
        return;
    }

    u8* data = malloc(sizeof(int));
    memcpy(data, (void*)&result, sizeof(int));

    libfs_response_t response = {
        .status = 0,
        .data_size = sizeof(int),
        .data = data,
    };

    send_response(sender, &response);
}

void open_handler(pid_t sender, const REQUEST_TYPE(open) * request) {
    LOG_INFO("flags: %u, name: %s", request->flags, request->name);

    char path_buf[256];
    libfs_get_files_path(path_buf, 256);

    strncat(path_buf, request->name, 256 - strlen(path_buf));

    LOG_INFO("path: %s", path_buf);

    fd_type fd = open(path_buf, (int)request->flags);

    if (fd < 0) {
        LOG_ERRNO("could not open file");
        libfs_response_t response = {
            .status = errno,
            .data_size = 0,
            .data = NULL,
        };

        send_response(sender, &response);
        return;
    }

    u8* data = malloc(sizeof(fd_type));
    memcpy(data, (void*)&fd, sizeof(fd_type));

    libfs_response_t response = {
        .status = 0,
        .data_size = sizeof(fd_type),
        .data = data,
    };

    send_response(sender, &response);
}

void read_handler(pid_t sender, const REQUEST_TYPE(read) * request) {
    LOG_INFO("fd: %d, size: %ld", request->fd, request->size);

    u8* data = malloc(sizeof(request->size));
    ssize_t data_size = read(request->fd, data, request->size);

    if (data_size < 0) {
        LOG_ERRNO("could not read from file descriptor");
        libfs_response_t response = {
            .status = errno,
            .data_size = 0,
            .data = NULL,
        };

        send_response(sender, &response);
        return;
    }

    libfs_response_t response = {
        .status = 0,
        .data_size = (size_t)data_size,
        .data = data,
    };

    send_response(sender, &response);
}

void rename_handler(pid_t sender, const REQUEST_TYPE(rename) * request) {
    LOG_INFO("old_name: %s, new_name: %s", request->old_name, request->new_name);

    char path_buf[256];
    char old_name_buf[256];
    char new_name_buf[256];
    libfs_get_files_path(path_buf, 256);

    memcpy(old_name_buf, path_buf, 256);
    memcpy(new_name_buf, path_buf, 256);

    strncat(old_name_buf, request->old_name, 256 - strlen(old_name_buf));
    strncat(new_name_buf, request->new_name, 256 - strlen(new_name_buf));

    int result = rename(old_name_buf, new_name_buf);

    if (result < 0) {
        LOG_ERRNO("could not rename file");
        libfs_response_t response = {
            .status = errno,
            .data_size = 0,
            .data = NULL,
        };

        send_response(sender, &response);
        return;
    }

    u8* data = malloc(sizeof(int));
    memcpy(data, (void*)&result, sizeof(int));

    libfs_response_t response = {
        .status = 0,
        .data_size = sizeof(int),
        .data = data,
    };

    send_response(sender, &response);
}

void seek_handler(pid_t sender, const REQUEST_TYPE(seek) * request) {
    LOG_INFO("fd: %d, offset: %ld", request->fd, request->offset);

    ssize_t result = lseek(request->fd, request->offset, SEEK_CUR);

    if (result < 0) {
        LOG_ERRNO("could not reposition file offset of the open file descriptor");
        libfs_response_t response = {
            .status = errno,
            .data_size = 0,
            .data = NULL,
        };

        send_response(sender, &response);
        return;
    }

    u8* data = malloc(sizeof(ssize_t));
    memcpy(data, (void*)&result, sizeof(ssize_t));

    libfs_response_t response = {
        .status = 0,
        .data_size = sizeof(ssize_t),
        .data = data,
    };

    send_response(sender, &response);
}

void stat_handler(pid_t sender, const REQUEST_TYPE(stat) * request) {
    (void)sender;  // unused
    LOG_INFO("path_name: %s", request->pathname);

    char path_buf[256];
    libfs_get_files_path(path_buf, 256);

    strncat(path_buf, request->pathname, 256 - strlen(path_buf));

    LOG_INFO("path: %s", path_buf);

    // struct stat stat;
    // int result = fstat(&path_buf, &stat);

    // TODO: finish me!
}

void symlink_handler(pid_t sender, const REQUEST_TYPE(symlink) * request) {
    (void)sender;  // unused
    LOG_INFO("source: %s, destination: %s", request->source, request->destination);

    char path_buf[256];
    char source_buf[256];
    char destination_buf[256];
    libfs_get_files_path(path_buf, 256);

    memcpy(source_buf, path_buf, 256);
    memcpy(destination_buf, path_buf, 256);

    strncat(source_buf, request->source, 256 - strlen(source_buf));
    strncat(destination_buf, request->destination, 256 - strlen(destination_buf));

    LOG_INFO("path: %s", path_buf);

    // FIXME: change me to symlink!!!
    int result = link(source_buf, destination_buf);

    if (result < 0) {
        LOG_ERRNO("could not create a soft link to a file");
        libfs_response_t response = {
            .status = errno,
            .data_size = 0,
            .data = NULL,
        };

        send_response(sender, &response);
        return;
    }

    u8* data = malloc(sizeof(int));
    memcpy(data, (void*)&result, sizeof(int));

    libfs_response_t response = {
        .status = 0,
        .data_size = sizeof(int),
        .data = data,
    };

    send_response(sender, &response);
}

void unlink_handler(pid_t sender, const REQUEST_TYPE(unlink) * request) {
    LOG_INFO("name: %s", request->name);

    char path_buf[256];
    libfs_get_files_path(path_buf, 256);

    strncat(path_buf, request->name, 256 - strlen(path_buf));

    LOG_INFO("path: %s", path_buf);

    int result = unlink(path_buf);

    if (result < 0) {
        LOG_ERRNO("could not unlink file");
        libfs_response_t response = {
            .status = errno,
            .data_size = 0,
            .data = NULL,
        };

        send_response(sender, &response);
        return;
    }

    u8* data = malloc(sizeof(int));
    memcpy(data, (void*)&result, sizeof(int));

    libfs_response_t response = {
        .status = 0,
        .data_size = sizeof(int),
        .data = data,
    };

    send_response(sender, &response);
}

void write_handler(pid_t sender, const REQUEST_TYPE(write) * request) {
    LOG_INFO("fd: %d, size: %ld", request->fd, request->size);

    ssize_t result = write(request->fd, request->data, request->size);

    if (result < 0) {
        LOG_ERRNO("could not write to a file");
        libfs_response_t response = {
            .status = errno,
            .data_size = 0,
            .data = NULL,
        };

        send_response(sender, &response);
        return;
    }

    u8* data = malloc(sizeof(ssize_t));
    memcpy(data, (void*)&result, sizeof(ssize_t));

    libfs_response_t response = {
        .status = 0,
        .data_size = sizeof(ssize_t),
        .data = data,
    };

    send_response(sender, &response);
}

DISPATCHER_FN_DECL(chmode, chmode_handler)
DISPATCHER_FN_DECL(close, close_handler)
DISPATCHER_FN_DECL(create, create_handler)
DISPATCHER_FN_DECL(link, link_handler)
DISPATCHER_FN_DECL(open, open_handler)
DISPATCHER_FN_DECL(read, read_handler)
DISPATCHER_FN_DECL(rename, rename_handler)
DISPATCHER_FN_DECL(seek, seek_handler)
DISPATCHER_FN_DECL(stat, stat_handler)
DISPATCHER_FN_DECL(symlink, symlink_handler)
DISPATCHER_FN_DECL(unlink, unlink_handler)
DISPATCHER_FN_DECL(write, write_handler)

dispatcher_fn dispatcher_table[] = {
    [LIBFS_REQUEST_CHMODE] = dispatcher_chmode, [LIBFS_REQUEST_CLOSE] = dispatcher_close,
    [LIBFS_REQUEST_CREATE] = dispatcher_create, [LIBFS_REQUEST_LINK] = dispatcher_link,
    [LIBFS_REQUEST_OPEN] = dispatcher_open,     [LIBFS_REQUEST_READ] = dispatcher_read,
    [LIBFS_REQUEST_RENAME] = dispatcher_rename, [LIBFS_REQUEST_SEEK] = dispatcher_seek,
    [LIBFS_REQUEST_STAT] = dispatcher_stat,     [LIBFS_REQUEST_SYMLINK] = dispatcher_symlink,
    [LIBFS_REQUEST_UNLINK] = dispatcher_unlink, [LIBFS_REQUEST_WRITE] = dispatcher_write,
};

void dispatch(libfs_request_t request) {
    dispatcher_fn fn = dispatcher_table[request.kind];
    LOG_INFO("dispatching request kind: %d", request.kind);
    if (fn) {
        fn(request);
    } else {
        LOG_ERROR("no dispatcher for request kind: %d", request.kind);
    }
}
