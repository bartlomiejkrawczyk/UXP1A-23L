#include "libfs_errno.h"
#include "example_errno.h"

int libfs_errno_setting_function(void) {
    libfs_set_errno(LIBFS_ERRNO_PPIPE_OPEN);
    return -1;
}
