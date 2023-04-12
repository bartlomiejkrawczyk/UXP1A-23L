#include "response.h"

#include <stdlib.h>
#include <string.h>

#include "types.h"

libfs_response_t libfs_response_new(int status, usize data_size, u8* data) {
    libfs_response_t response = {
        .status = status,
        .data_size = data_size,
    };

    if (data_size > 0) {
        response.data = malloc(data_size);
        memcpy(response.data, data, data_size);
    }

    return response;
}
void libfs_response_free(libfs_response_t response) {
    if (response.data_size > 0) {
        free(response.data);
    }
}

usize libfs_response_size(const libfs_response_t* response) {
    return sizeof(int) + sizeof(usize) + response->data_size;
}

u8* libfs_response_serialize(const libfs_response_t* response) {
    usize size = libfs_response_size(response);
    u8* data = malloc(size);

    memcpy(data, &response->status, sizeof(int));
    memcpy(data + sizeof(usize), &response->data_size, sizeof(usize));
    if (response->data_size > 0) {
        memcpy(data + sizeof(int) + sizeof(usize), response->data, response->data_size);
    }
    return data;
}
libfs_response_t libfs_response_deserialize(const u8* data) {
    libfs_response_t response;
    memcpy(&response.status, data, sizeof(int));
    memcpy(&response.data_size, data + sizeof(int), sizeof(usize));
    if (response.data_size > 0) {
        response.data = malloc(response.data_size);
        memcpy(response.data, data + sizeof(int) + sizeof(usize), response.data_size);
    }
    return response;
}
