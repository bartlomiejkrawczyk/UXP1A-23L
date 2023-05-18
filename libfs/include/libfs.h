#pragma once

#include <errno.h>
#include "libfs_errno.h"
#include "types.h"

int libfs_create(const char *path, u32 mode);
