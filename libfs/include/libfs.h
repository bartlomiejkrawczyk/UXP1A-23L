#pragma once

#include "types.h"

int libfs_chmode(const char *name, long mode);

int libfs_close(fd_type fd);

fd_type libfs_create(const char *path, u32 mode);

int libfs_link(const char *source, const char *destination);

fd_type libfs_open(char *name, int flags);

int libfs_read(fd_type fd, char *buf, unsigned int size);

int libfs_rename(char *oldname, char *newname);

int libfs_seek(fd_type fd, long int offset);

int libfs_stat(const char *restrict pathname, struct libfs_stat_struct_t *restrict statbuf);

int libfs_symlink(const char *source, const char *destination);

int libfs_unlink(char *name);

int libfs_write(fd_type fd, char *buf, unsigned int size);
