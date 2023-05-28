#pragma once

#include "request.h"

typedef void (*dispatcher_fn)(libfs_request_t);

#define REQUEST_TYPE(name) libfs_request_##name##_t

#define DISPATCHER_FN_SIG(name) void dispatcher_##name(libfs_request_t request)

#define DISPATCHER_FN_DECL(name, handler)                                              \
    DISPATCHER_FN_SIG(name) {                                                          \
        REQUEST_TYPE(name) unpacked_request = libfs_request_##name##_unpack(&request); \
        LOG_INFO("%s", "dispatcher " #name);                                           \
        handler(request.sender, &unpacked_request);                                    \
    }

void dispatch(libfs_request_t request);
