#pragma once

#include <threads.h>

// Error codes
#define LIBFS_ERRNO_OK 2000
// Message pipe open error
#define LIBFS_ERRNO_MPIPE_OPEN 2001
// Message pipe read error
#define LIBFS_ERRNO_MPIPE_READ 2002
// Message pipe write error
#define LIBFS_ERRNO_MPIPE_WRITE 2003
// Process pipe open error
#define LIBFS_ERRNO_PPIPE_OPEN 2004
// Process pipe read error
#define LIBFS_ERRNO_PPIPE_READ 2005
// Process pipe write error
#define LIBFS_ERRNO_PPIPE_WRITE 2006
// Message pipe lock error
#define LIBFS_ERRNO_MPIPE_LOCK 2007
// Message pipe unlock error
#define LIBFS_ERRNO_MPIPE_UNLOCK 2008

extern thread_local int libfs_errno;

void libfs_set_errno(int err);

const char* libfs_strerror(int err);

void libfs_perror(const char* msg);

#define LOG_LIBFS_ERRNO(message)                                       \
    printf(TERMCOLOR(BRIGHT_RED, "[ERROR] ") message ": %s (%s:%d)\n", \
           libfs_strerror(libfs_errno), __FILE__, __LINE__);
