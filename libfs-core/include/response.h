#pragma once

#include "types.h"

typedef struct libfs_response {
    int status;
    usize data_size;
    u8* data;
} libfs_response_t;

libfs_response_t libfs_response_new(int status, usize data_size, u8* data);
void libfs_response_free(libfs_response_t response);

const u8* libfs_response_serialize(const libfs_response_t* response, usize* size);
libfs_response_t libfs_response_deserialize(const u8* data);
