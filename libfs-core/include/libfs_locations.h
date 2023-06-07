#pragma once
#include <sys/types.h>

#include "types.h"

// Get the path to the directory containing the storage.
// The path is guaranteed to end with a slash.
// The string is written to the buffer and the number of bytes written is returned.
// If the buffer is too small, -1 is returned.
isize libfs_get_storage_path(char* buffer, usize max_size);

// Get the path to the directory containing the pipes.
// The path is guaranteed to end with a slash.
// The string is written to the buffer and the number of bytes written is returned.
// If the buffer is too small, -1 is returned.
isize libfs_get_pipes_path(char* buffer, usize max_size);

// Get the path to the directory containing the logs.
// The path is guaranteed to end with a slash.
// The string is written to the buffer and the number of bytes written is returned.
// If the buffer is too small, -1 is returned.
isize libfs_get_logs_path(char* buffer, usize max_size);

// Get the path to the main pipe.
// The string is written to the buffer and the number of bytes written is returned.
// If the buffer is too small, -1 is returned.
isize libfs_get_main_pipe_path(char* buffer, usize max_size);

// Get the path to the directory containing the files.
// The path is guaranteed to end with a slash.
// The string is written to the buffer and the number of bytes written is returned.
// If the buffer is too small, -1 is returned.
isize libfs_get_files_path(char* buffer, usize max_size);

// Get the path to the pipe for the given process.
// The path is guaranteed to end with a slash.
// The string is written to the buffer and the number of bytes written is returned.
// If the buffer is too small, -1 is returned.
isize libfs_get_process_pipe_path(char* buffer, usize max_size, pid_t process_id);

// Get the path to the lockfile for the daemon.
isize libfs_get_lockfile_path(char* buffer, usize max_size);

// Get the path to the daemon executable.
isize libfs_get_daemon_path(char* buffer, usize max_size);

// Ensure that the directories for the storage, pipes and files exist.
isize libfs_ensure_directories(void);
