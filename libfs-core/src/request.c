#include "request.h"

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "types.h"

libfs_request_t libfs_request_new(libfs_request_kind_t kind, usize data_size, u8* data) {
    libfs_request_t request = {
        .kind = kind,
        .sender = getpid(),
        .data_size = data_size,
    };

    if (data_size > 0) {
        request.data = malloc(data_size);
        memcpy(request.data, data, data_size);
    }

    return request;
}

void libfs_request_free(libfs_request_t request) {
    if (request.data_size > 0) {
        free(request.data);
    }
}

usize libfs_request_size(const libfs_request_t* request) {
    return sizeof(libfs_request_kind_t) + sizeof(pid_t) + sizeof(usize) + request->data_size;
}

u8* libfs_request_serialize(const libfs_request_t* request) {
    usize size = sizeof(libfs_request_kind_t) + sizeof(pid_t) + sizeof(usize) + request->data_size;
    u8* data = malloc(size);

    memcpy(data, &request->kind, sizeof(libfs_request_kind_t));
    memcpy(data + sizeof(libfs_request_kind_t), &request->sender, sizeof(pid_t));
    memcpy(data + sizeof(libfs_request_kind_t) + sizeof(pid_t), &request->data_size,
           sizeof(usize));
    if (request->data_size > 0) {
        memcpy(data + sizeof(libfs_request_kind_t) + sizeof(pid_t) + sizeof(usize),
               request->data, request->data_size);
    }

    return data;
}

libfs_request_t libfs_request_deserialize(const u8* data) {
    libfs_request_t request;
    memcpy(&request.kind, data, sizeof(libfs_request_kind_t));
    memcpy(&request.sender, data + sizeof(libfs_request_kind_t), sizeof(pid_t));
    memcpy(&request.data_size, data + sizeof(libfs_request_kind_t) + sizeof(pid_t),
           sizeof(usize));
    if (request.data_size > 0) {
        request.data = malloc(request.data_size);
        memcpy(request.data,
               data + sizeof(libfs_request_kind_t) + +sizeof(pid_t) + sizeof(usize),
               request.data_size);
    }
    return request;
}

libfs_request_t libfs_request_create_pack(const libfs_request_create_t* request_create) {
    usize name_len = strlen(request_create->name) + 1;

    libfs_request_t request;
    request.kind = LIBFS_REQUEST_CREATE;
    request.sender = getpid();
    request.data_size = sizeof(u32) + name_len;
    request.data = malloc(request.data_size);

    memcpy(request.data, &request_create->mode, sizeof(u32));
    memcpy(request.data + sizeof(u32), request_create->name, name_len);

    return request;
}
libfs_request_create_t libfs_request_create_unpack(const libfs_request_t* request) {
    libfs_request_create_t request_create;
    memcpy(&request_create.mode, request->data, sizeof(u32));
    request_create.name = (char*)request->data + sizeof(u32);  // No deep copy, deserialized
                                                               // request is a "view" of the
                                                               // serialized data
    return request_create;
}

libfs_request_t libfs_request_chmode_pack(const libfs_request_chmode_t* request_chmode) {
    usize name_len = strlen(request_chmode->name) + 1;

    libfs_request_t request;
    request.kind = LIBFS_REQUEST_CHMODE;
    request.sender = getpid();
    request.data_size = sizeof(u32) + name_len;
    request.data = malloc(request.data_size);

    memcpy(request.data, &request_chmode->mode, sizeof(u32));
    memcpy(request.data + sizeof(u32), request_chmode->name, name_len);

    return request;
}
libfs_request_chmode_t libfs_request_chmode_unpack(const libfs_request_t* request) {
    libfs_request_chmode_t request_chmode;
    memcpy(&request_chmode.mode, request->data, sizeof(u32));
    request_chmode.name = (char*)request->data + sizeof(u32);  // No deep copy, deserialized
                                                               // request is a "view" of the
                                                               // serialized data
    return request_chmode;
}

libfs_request_t libfs_request_rename_pack(const libfs_request_rename_t* request_rename) {
    usize old_name_len = strlen(request_rename->old_name) + 1;
    usize new_name_len = strlen(request_rename->new_name) + 1;

    libfs_request_t request;
    request.kind = LIBFS_REQUEST_RENAME;
    request.sender = getpid();
    request.data_size = old_name_len + new_name_len;
    request.data = malloc(request.data_size);

    memcpy(request.data, request_rename->old_name, old_name_len);
    memcpy(request.data + old_name_len, request_rename->new_name, new_name_len);

    return request;
}
libfs_request_rename_t libfs_request_rename_unpack(const libfs_request_t* request) {
    libfs_request_rename_t request_rename;
    request_rename.old_name = (char*)request->data;  // No deep copy, deserialized request is a
                                                     // "view" of the serialized data
    request_rename.new_name = (char*)request->data + strlen(request_rename.old_name) + 1;
    return request_rename;
}

libfs_request_t libfs_request_unlink_pack(const libfs_request_unlink_t* request_unlink) {
    usize name_len = strlen(request_unlink->name) + 1;

    libfs_request_t request;
    request.kind = LIBFS_REQUEST_UNLINK;
    request.sender = getpid();
    request.data_size = name_len;
    request.data = malloc(request.data_size);

    memcpy(request.data, request_unlink->name, name_len);

    return request;
}
libfs_request_unlink_t libfs_request_unlink_unpack(const libfs_request_t* request) {
    libfs_request_unlink_t request_unlink;
    request_unlink.name = (char*)request->data;  // No deep copy, deserialized request is a
                                                 // "view" of the serialized data
    return request_unlink;
}

libfs_request_t libfs_request_open_pack(const libfs_request_open_t* request_open) {
    usize name_len = strlen(request_open->name) + 1;

    libfs_request_t request;
    request.kind = LIBFS_REQUEST_OPEN;
    request.sender = getpid();
    request.data_size = sizeof(u32) + name_len;
    request.data = malloc(request.data_size);

    memcpy(request.data, &request_open->flags, sizeof(u32));
    memcpy(request.data + sizeof(u32), request_open->name, name_len);

    return request;
}
libfs_request_open_t libfs_request_open_unpack(const libfs_request_t* request) {
    libfs_request_open_t request_open;
    memcpy(&request_open.flags, request->data, sizeof(u32));
    request_open.name = (char*)request->data + sizeof(u32);  // No deep copy, deserialized
                                                             // request is a "view" of the
                                                             // serialized data
    return request_open;
}

libfs_request_t libfs_request_read_pack(const libfs_request_read_t* request_read) {
    libfs_request_t request;
    request.kind = LIBFS_REQUEST_READ;
    request.sender = getpid();
    request.data_size = sizeof(fd_type) + sizeof(u32);
    request.data = malloc(request.data_size);

    memcpy(request.data, &request_read->fd, sizeof(fd_type));
    memcpy(request.data + sizeof(fd_type), &request_read->size, sizeof(u32));

    return request;
}
libfs_request_read_t libfs_request_read_unpack(const libfs_request_t* request) {
    libfs_request_read_t request_read;
    memcpy(&request_read.fd, request->data, sizeof(fd_type));
    memcpy(&request_read.size, request->data + sizeof(fd_type), sizeof(usize));
    return request_read;
}

libfs_request_t libfs_request_write_pack(const libfs_request_write_t* request_write) {
    libfs_request_t request;
    request.kind = LIBFS_REQUEST_WRITE;
    request.sender = getpid();
    request.data_size = sizeof(fd_type) + sizeof(usize) + request_write->size;
    request.data = malloc(request.data_size);

    memcpy(request.data, &request_write->fd, sizeof(fd_type));
    memcpy(request.data + sizeof(fd_type), &request_write->size, sizeof(usize));
    memcpy(request.data + sizeof(fd_type) + sizeof(usize), request_write->data,
           request_write->size);

    return request;
}
libfs_request_write_t libfs_request_write_unpack(const libfs_request_t* request) {
    libfs_request_write_t request_write;
    memcpy(&request_write.fd, request->data, sizeof(fd_type));
    memcpy(&request_write.size, request->data + sizeof(fd_type), sizeof(usize));
    request_write.data = request->data + sizeof(fd_type) + sizeof(usize);
    return request_write;
}

libfs_request_t libfs_request_seek_pack(const libfs_request_seek_t* request_seek) {
    libfs_request_t request;
    request.kind = LIBFS_REQUEST_SEEK;
    request.sender = getpid();
    request.data_size = sizeof(fd_type) + sizeof(isize);
    request.data = malloc(request.data_size);

    memcpy(request.data, &request_seek->fd, sizeof(fd_type));
    memcpy(request.data + sizeof(fd_type), &request_seek->offset, sizeof(isize));

    return request;
}
libfs_request_seek_t libfs_request_seek_unpack(const libfs_request_t* request) {
    libfs_request_seek_t request_seek;
    memcpy(&request_seek.fd, request->data, sizeof(fd_type));
    memcpy(&request_seek.offset, request->data + sizeof(fd_type), sizeof(isize));
    return request_seek;
}

libfs_request_t libfs_request_close_pack(const libfs_request_close_t* request_close) {
    libfs_request_t request;
    request.kind = LIBFS_REQUEST_CLOSE;
    request.sender = getpid();
    request.data_size = sizeof(fd_type);
    request.data = malloc(request.data_size);

    memcpy(request.data, &request_close->fd, sizeof(fd_type));

    return request;
}
libfs_request_close_t libfs_request_close_unpack(const libfs_request_t* request) {
    libfs_request_close_t request_close;
    memcpy(&request_close.fd, request->data, sizeof(fd_type));
    return request_close;
}
