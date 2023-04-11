#ifndef LIBFS_ERRNO_H
#define LIBFS_ERRNO_H

// Error codes
#define LIBFS_ERRNO_OK 0
#define LIBFS_ERRNO_UNKNOWN -1

extern int libfs_errno;

void libfs_set_errno(int errno);

const char* libfs_strerror(int errno);

void libfs_perror(const char* msg);

#endif  // LIBFS_ERRNO_H
