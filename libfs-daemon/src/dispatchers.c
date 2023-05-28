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
    (void)sender;   // unused
    (void)request;  // unused
    // TODO: implement me
}

void close_handler(pid_t sender, const REQUEST_TYPE(close) * request) {
    (void)sender;   // unused
    (void)request;  // unused
    // TODO: implement me
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
    (void)sender;   // unused
    (void)request;  // unused
    // TODO: implement me
}

void open_handler(pid_t sender, const REQUEST_TYPE(open) * request) {
    (void)sender;  // unused
    LOG_INFO("flags: %u, name: %s", request->flags, request->name);
}

void read_handler(pid_t sender, const REQUEST_TYPE(read) * request) {
    (void)sender;   // unused
    (void)request;  // unused
    // TODO: implement me
}

void rename_handler(pid_t sender, const REQUEST_TYPE(rename) * request) {
    (void)sender;   // unused
    (void)request;  // unused
    // TODO: implement me
}

void seek_handler(pid_t sender, const REQUEST_TYPE(seek) * request) {
    (void)sender;   // unused
    (void)request;  // unused
    // TODO: implement me
}

void stat_handler(pid_t sender, const REQUEST_TYPE(stat) * request) {
    (void)sender;   // unused
    (void)request;  // unused
    // TODO: implement me
}

void symlink_handler(pid_t sender, const REQUEST_TYPE(symlink) * request) {
    (void)sender;   // unused
    (void)request;  // unused
    // TODO: implement me
}

void unlink_handler(pid_t sender, const REQUEST_TYPE(unlink) * request) {
    (void)sender;   // unused
    (void)request;  // unused
    // TODO: implement me
}

void write_handler(pid_t sender, const REQUEST_TYPE(write) * request) {
    (void)sender;   // unused
    (void)request;  // unused
    // TODO: implement me
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
