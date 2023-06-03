#pragma once

#include <sys/types.h>
#include <time.h>

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
    LIBFS_REQUEST_STAT,
    LIBFS_REQUEST_LINK,
    LIBFS_REQUEST_SYMLINK,
} libfs_request_kind_t;

typedef struct libfs_request {
    libfs_request_kind_t kind;
    pid_t sender;
    usize data_size;
    u8* data;
} libfs_request_t;

// Returns the size of the request.
usize libfs_request_size(const libfs_request_t* request);

// Creates a new request with the given kind and data. The data is copied.
libfs_request_t libfs_request_new(libfs_request_kind_t kind, usize data_size, u8* data);
// Frees the request's data. Request can be assumed to be invalid after this call.
void libfs_request_free(libfs_request_t request);

// Serializes the request into a buffer. The buffer is allocated with malloc and must be freed
// by the caller. The size of the buffer is returned in size.
u8* libfs_request_serialize(const libfs_request_t* request);
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

typedef struct libfs_stat_struct {
    ino_t st_ino;           /* System Inode number */
    mode_t st_mode;         /* File type and mode */
    nlink_t st_nlink;       /* Number of hard links */
    off_t st_size;          /* Total size, in bytes */
    __blksize_t st_blksize; /* Block size for filesystem I/O */
    blkcnt_t st_blocks;     /* Number of 512B blocks allocated */

    struct timespec st_atim; /* Time of last access */
    struct timespec st_mtim; /* Time of last modification */
    struct timespec st_ctim; /* Time of last status change */
} libfs_stat_struct_t;

typedef struct libfs_request_stat {
    char* pathname;
    libfs_stat_struct_t* statbuf;
} libfs_request_stat_t;

libfs_request_t libfs_request_stat_pack(const libfs_request_stat_t* request_stat);
libfs_request_stat_t libfs_request_stat_unpack(const libfs_request_t* request);

typedef struct libfs_request_link {
    char* source;
    char* destination;
} libfs_request_link_t;

libfs_request_t libfs_request_link_pack(const libfs_request_link_t* request_link);
libfs_request_link_t libfs_request_link_unpack(const libfs_request_t* request);

typedef struct libfs_request_symlink {
    char* source;
    char* destination;
} libfs_request_symlink_t;

libfs_request_t libfs_request_symlink_pack(const libfs_request_symlink_t* request_symlink);
libfs_request_symlink_t libfs_request_symlink_unpack(const libfs_request_t* request);
