#include "libfs_locations.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "types.h"

#define LIBFS_STORAGE_PATH "/.local/share/libfs/"
#define LIBFS_PIPES_PATH "pipes/"
#define LIBFS_LOGS_PATH "logs/"
#define LIBFS_FILES_PATH "files/"
#define LIBFS_MAIN_PIPE "main_pipe"

isize libfs_get_storage_path(char* buffer, usize max_size) {
    char* home = getenv("HOME");
    if (home == NULL) {
        return -1;
    }
    usize home_len = strlen(home);
    usize total_len = home_len + strlen(LIBFS_STORAGE_PATH) + 1;
    if (total_len > max_size) {
        return -1;
    }
    strcpy(buffer, home);
    strcat(buffer, LIBFS_STORAGE_PATH);
    return (isize)total_len;
}

isize libfs_get_pipes_path(char* buffer, usize max_size) {
    isize storage_path_len = libfs_get_storage_path(buffer, max_size);
    if (storage_path_len < 0) {
        return -1;
    }
    usize total_len = (usize)storage_path_len + strlen(LIBFS_PIPES_PATH);
    if (total_len > max_size) {
        return -1;
    }
    strcat(buffer, LIBFS_PIPES_PATH);
    return (isize)total_len;
}

isize libfs_get_logs_path(char* buffer, usize max_size) {
    isize storage_path_len = libfs_get_storage_path(buffer, max_size);
    if (storage_path_len < 0) {
        return -1;
    }
    usize total_len = (usize)storage_path_len + strlen(LIBFS_LOGS_PATH);
    if (total_len > max_size) {
        return -1;
    }
    strcat(buffer, LIBFS_LOGS_PATH);
    return (isize)total_len;
}

isize libfs_get_main_pipe_path(char* buffer, usize max_size) {
    isize pipes_path_len = libfs_get_pipes_path(buffer, max_size);
    if (pipes_path_len < 0) {
        return -1;
    }
    usize total_len = (usize)pipes_path_len + strlen(LIBFS_MAIN_PIPE);
    if (total_len > max_size) {
        return -1;
    }
    strcat(buffer, LIBFS_MAIN_PIPE);
    return (isize)total_len;
}

isize libfs_get_files_path(char* buffer, usize max_size) {
    isize storage_path_len = libfs_get_storage_path(buffer, max_size);
    if (storage_path_len < 0) {
        return -1;
    }
    usize total_len = (usize)storage_path_len + strlen(LIBFS_FILES_PATH);
    if (total_len > max_size) {
        return -1;
    }
    strcat(buffer, LIBFS_FILES_PATH);
    return (isize)total_len;
}

isize libfs_get_process_pipe_path(char* buffer, usize max_size, pid_t process_id) {
    isize pipes_path_len = libfs_get_pipes_path(buffer, max_size);
    if (pipes_path_len < 0) {
        return -1;
    }
    usize total_len = (usize)pipes_path_len + 10;
    if (total_len > max_size) {
        return -1;
    }
    char process_id_str[sizeof(pid_t) * 3 + 1];  // Max 3 digits per byte + null terminator
    sprintf(process_id_str, "%d", process_id);
    strcat(buffer, process_id_str);
    return (isize)total_len;
}

isize libfs_get_lockfile_path(char* buffer, usize max_size) {
    isize storage_path_len = libfs_get_storage_path(buffer, max_size);
    if (storage_path_len < 0) {
        return -1;
    }
    usize total_len = (usize)storage_path_len + 10;
    if (total_len > max_size) {
        return -1;
    }
    strcat(buffer, "lockfile");
    return (isize)total_len;
}

isize libfs_get_daemon_path(char* buffer, usize max_size) {
    isize storage_path_len = libfs_get_storage_path(buffer, max_size);
    if (storage_path_len < 0) {
        return -1;
    }
    usize total_len = (usize)storage_path_len + 13;
    if (total_len > max_size) {
        return -1;
    }
    strcat(buffer, "libfs-daemon");
    return (isize)total_len;
}

isize libfs_ensure_directories(void) {
    char buffer[256];
    isize len = libfs_get_storage_path(buffer, sizeof(buffer));
    if (len < 0) {
        return -1;
    }
    if (access(buffer, F_OK) < 0) {
        if (mkdir(buffer, 0700) < 0) {
            return -1;
        }
    }

    len = libfs_get_pipes_path(buffer, sizeof(buffer));
    if (len < 0) {
        return -1;
    }
    if (access(buffer, F_OK) < 0) {
        if (mkdir(buffer, 0700) < 0) {
            return -1;
        }
    }

    len = libfs_get_logs_path(buffer, sizeof(buffer));
    if (len < 0) {
        return -1;
    }
    if (access(buffer, F_OK) < 0) {
        if (mkdir(buffer, 0700) < 0) {
            return -1;
        }
    }

    len = libfs_get_files_path(buffer, sizeof(buffer));
    if (len < 0) {
        return -1;
    }
    if (access(buffer, F_OK) < 0) {
        if (mkdir(buffer, 0700) < 0) {
            return -1;
        }
    }

    return 0;
}
