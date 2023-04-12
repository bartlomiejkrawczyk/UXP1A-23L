#pragma once
#include <threads.h>

// Error codes
#define LIBFS_ERRNO_OK 0
#define LIBFS_ERRNO_UNKNOWN -1

extern thread_local int libfs_errno;

void libfs_set_errno(int errno);

const char* libfs_strerror(int errno);

void libfs_perror(const char* msg);
