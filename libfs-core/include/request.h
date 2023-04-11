#pragma once

#include "types.h"

typedef enum libfs_request_kind {
    LIBFS_REQUEST_CREATE,
    LIBFS_REQUEST_CHMODE,
    LIBFS_REQUEST_RENAME,
    LIBFS_REQUEST_UNLINK,
    LIBFS_REQUEST_OPEN,
    LIBFS_REQUEST_READ,
    LIBFS_REQUEST_WRITE,
    LIBFS_REQUEST_SEEK,
    LIBFS_REQUEST_CLOSE,
} libfs_request_kind_t;

typedef struct libfs_request {
    libfs_request_kind_t kind;
    usize data_size;
    u8* data;
} libfs_request_t;

// Creates a new request with the given kind and data. The data is copied.
libfs_request_t libfs_request_new(libfs_request_kind_t kind, usize data_size, u8* data);
// Frees the request's data. Request can be assumed to be invalid after this call.
void libfs_request_free(libfs_request_t request);

// Serializes the request into a buffer. The buffer is allocated with malloc and must be freed
// by the caller. The size of the buffer is returned in size.
u8* libfs_request_serialize(const libfs_request_t* request, usize* size);
// Deserializes a request from a buffer. The buffer is not freed.
libfs_request_t libfs_request_deserialize(const u8* data);

// Individually defined request types
//
// Each request type has a pack and unpack function. The pack function takes a concrete request
// type and returns a generic, packed request. A deep copy of the data (eg. name) is made.
//
// The unpack function takes a generic request and deserializes it as a concrete request type.
// The data is not copied, so the returned request is only valid as long as the original request
// is valid. It can be thought of as a view into the original request when deserialized.
#define __DO_NOT_SHOW_THIS_COMMENT_AS_libfs_request_create_DOCUMENTATION__

typedef struct libfs_request_create {
    char* name;
    u32 mode;
} libfs_request_create_t;

libfs_request_t libfs_request_create_pack(const libfs_request_create_t* request_create);
libfs_request_create_t libfs_request_create_unpack(const libfs_request_t* request);

typedef struct libfs_request_chmode {
    char* name;
    u32 mode;
} libfs_request_chmode_t;

libfs_request_t libfs_request_chmode_pack(const libfs_request_chmode_t* request_chmode);
libfs_request_chmode_t libfs_request_chmode_unpack(const libfs_request_t* request);

typedef struct libfs_request_rename {
    char* old_name;
    char* new_name;
} libfs_request_rename_t;

libfs_request_t libfs_request_rename_pack(const libfs_request_rename_t* request_rename);
libfs_request_rename_t libfs_request_rename_unpack(const libfs_request_t* request);

typedef struct libfs_request_unlink {
    char* name;
} libfs_request_unlink_t;

libfs_request_t libfs_request_unlink_pack(const libfs_request_unlink_t* request_unlink);
libfs_request_unlink_t libfs_request_unlink_unpack(const libfs_request_t* request);

typedef struct libfs_request_open {
    char* name;
    u32 flags;
} libfs_request_open_t;

libfs_request_t libfs_request_open_pack(const libfs_request_open_t* request_open);
libfs_request_open_t libfs_request_open_unpack(const libfs_request_t* request);

typedef struct libfs_request_read {
    fd_type fd;
    usize size;
} libfs_request_read_t;

libfs_request_t libfs_request_read_pack(const libfs_request_read_t* request_read);
libfs_request_read_t libfs_request_read_unpack(const libfs_request_t* request);

typedef struct libfs_request_write {
    fd_type fd;
    usize size;
    u8* data;
} libfs_request_write_t;

libfs_request_t libfs_request_write_pack(const libfs_request_write_t* request_write);
libfs_request_write_t libfs_request_write_unpack(const libfs_request_t* request);

typedef struct libfs_request_seek {
    fd_type fd;
    isize offset;
} libfs_request_seek_t;

libfs_request_t libfs_request_seek_pack(const libfs_request_seek_t* request_seek);
libfs_request_seek_t libfs_request_seek_unpack(const libfs_request_t* request);

typedef struct libfs_request_close {
    fd_type fd;
} libfs_request_close_t;

libfs_request_t libfs_request_close_pack(const libfs_request_close_t* request_close);
libfs_request_close_t libfs_request_close_unpack(const libfs_request_t* request);
