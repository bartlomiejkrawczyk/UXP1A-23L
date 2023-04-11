#include "libfs_errno.h"
#include "example_errno.h"

int main(void) {
    int r = libfs_errno_setting_function();

    if (r != 0) {
        libfs_perror("libfs_errno_setting_function");
        return 1;
    }

    return 0;
}
