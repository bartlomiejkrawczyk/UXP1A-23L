#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
// #include <syslog.h>
#include <unistd.h>

// If we decide to log to a file, uncomment this.
// #define TERMCOLOR_DISABLE

#include "dispatchers.h"
#include "globals.h"
#include "libfs_locations.h"
#include "log.h"
#include "types.h"
#include "utils.h"

#define DAEMON_LOG_PATH "libfs_daemon.log"

libfs_request_t serialize_deserialize(libfs_request_t request) {
    usize buffer_size = libfs_request_size(&request);
    u8* buffer = libfs_request_serialize(&request);
    // serialize of the request copies relevant data into the buffer.
    // We can free the request now.
    libfs_request_free(request);

    dump_buffer(buffer, buffer_size, 8);

    libfs_request_t unpacked_request = libfs_request_deserialize(buffer);
    // deserialize of the request copies relevant data into the buffer.
    // We can free the buffer now.
    free(buffer);

    return unpacked_request;
}

void ensure_pipe_readable(void) {
    if (access(g_libfs_pipe_path, R_OK) == -1) {
        LOG_ERRNO("could not access pipe");
        shutdown_daemon(1);
    }

    if (g_libfs_pipe_fd != 0) {
        if (close(g_libfs_pipe_fd) != 0) {
            LOG_ERRNO("could not close pipe");
            shutdown_daemon(1);
        }
        g_libfs_pipe_fd = 0;
    }

    g_libfs_pipe_fd = open(g_libfs_pipe_path, O_RDONLY);
    if (g_libfs_pipe_fd == -1) {
        LOG_ERRNO("could not open pipe");
        shutdown_daemon(1);
    }

    LOG_INFO("opened pipe fd: %d", g_libfs_pipe_fd);
}

libfs_request_t read_request_from_pipe(fd_type fd) {
    libfs_request_kind_t kind;
    isize result = read(fd, &kind, sizeof(kind));
    if (result == 0) {
        LOG_WARN("%s", "pipe closed, trying to recover...");
        ensure_pipe_readable();
        result = read(fd, &kind, sizeof(kind));
        if (result <= 0) {
            LOG_ERRNO("failed to recover pipe");
            shutdown_daemon(1);
        } else if (result != sizeof(kind)) {
            LOG_ERROR("could not read request kind, expected %ld bytes, got %ld bytes",
                      sizeof(kind), result);
            shutdown_daemon(1);
        }
    } else if (result == -1) {
        LOG_ERRNO("could not read request kind");
        shutdown_daemon(1);
    }

    LOG_INFO("received request kind: %d", kind);

    pid_t pid;
    result = read(fd, &pid, sizeof(pid));
    if (result <= 0) {
        LOG_ERRNO("could not read request pid");
        shutdown_daemon(1);
    } else if (result != sizeof(pid)) {
        LOG_ERROR("could not read request pid, expected %ld bytes, got %ld bytes", sizeof(pid),
                  result);
        shutdown_daemon(1);
    }

    LOG_INFO("request coming from pid: %d", pid);

    usize buffer_size;
    result = read(fd, &buffer_size, sizeof(buffer_size));
    if (result <= 0) {
        LOG_ERRNO("could not read request buffer size");
        shutdown_daemon(1);
    } else if (result != sizeof(buffer_size)) {
        LOG_ERROR("could not read request buffer size, expected %ld bytes, got %ld bytes",
                  sizeof(buffer_size), result);
        shutdown_daemon(1);
    }

    usize total_size = buffer_size + sizeof(buffer_size) + sizeof(pid) + sizeof(kind);
    u8* buffer = malloc(total_size);

    LOG_INFO("total request buffer size: %ld", total_size);

    memcpy(buffer, &kind, sizeof(kind));
    memcpy(buffer + sizeof(kind), &pid, sizeof(pid));
    memcpy(buffer + sizeof(kind) + sizeof(pid), &buffer_size, sizeof(buffer_size));

    result = read(fd, buffer + sizeof(kind) + sizeof(pid) + sizeof(buffer_size), buffer_size);

    LOG_INFO("read %ld bytes", result);

    if (result <= 0) {
        LOG_ERRNO("could not read request buffer");
        shutdown_daemon(1);
    } else if ((usize)result != buffer_size) {
        LOG_ERROR("could not read request buffer, expected %ld bytes, got %ld bytes",
                  buffer_size, result);
        shutdown_daemon(1);
    }

    libfs_request_t req = libfs_request_deserialize(buffer);

    free(buffer);

    return req;
}

void terminate_handler(int signal) {
    (void)signal;  // unused
    shutdown_daemon(0);
}

static void start_daemon() {
    pid_t pid;

    LOG_INFO("%s", "pid");
    /* Fork off the parent process */
    pid = fork();
    LOG_INFO("%s %d", "forked", pid);

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);
    LOG_INFO("%s", "parent or child");

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);
    LOG_INFO("%s", "child");

    /* On success: The child process becomes session leader */
    if (setsid() < 0)
        exit(EXIT_FAILURE);
    LOG_INFO("%s", "ignore signals");

    /* Catch, ignore and handle signals */
    // TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();
    LOG_INFO("%s", "second fork");

    /* An error occurred */
    if (pid < 0)
        exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0)
        exit(EXIT_SUCCESS);
    LOG_INFO("%s", "child");

    /* Set new file permissions */
    umask(0);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/");
    LOG_INFO("%s", "change root");

    char logs_buffer[256];
    libfs_get_logs_path(logs_buffer, 256);

    char out_buffer[256];
    strcpy(out_buffer, logs_buffer);
    strcat(out_buffer, "out.log");

    char err_buffer[256];
    strcpy(err_buffer, logs_buffer);
    strcat(err_buffer, "err.log");
    LOG_INFO("%s", logs_buffer);
    LOG_INFO("%s", out_buffer);
    LOG_INFO("%s", err_buffer);

    /* Close all open file descriptors */
    int x;
    for (x = (int)sysconf(_SC_OPEN_MAX); x >= 0; x--) {
        close(x);
    }

    stdin = fopen("/dev/null", "r");   // fd=0
    stdout = fopen(out_buffer, "w+");  // fd=1
    stderr = fopen(err_buffer, "w+");  // fd=2

    // openlog("firstdaemon", LOG_PID, LOG_DAEMON);
}

int main(int argc, char* argv[]) {
    (void)argc;  // unused
    (void)argv;  // unused

    LOG_INFO("%s", "starting daemon");
    LOG_INFO("pid: %d", getpid());

    start_daemon();

    LOG_INFO("%s", "starting daemon");
    LOG_INFO("pid: %d", getpid());

    signal(SIGINT, terminate_handler);
    signal(SIGTERM, terminate_handler);

    LOG_INFO("%s", "installed signal handlers");

    libfs_get_main_pipe_path(g_libfs_pipe_path, 256);

    LOG_INFO("main pipe path: %s", g_libfs_pipe_path);

    if (libfs_ensure_directories() < 0) {
        LOG_ERROR("%s", "could not ensure directories");
        exit(1);
    }

    int result = access(g_libfs_pipe_path, F_OK);

    if (result == 0) {
        LOG_INFO("%s", "pipe already exists, removing it");
        result = unlink(g_libfs_pipe_path);

        if (result == -1) {
            LOG_ERRNO("could not remove pipe");
            exit(1);
        }
    }

    result = mkfifo(g_libfs_pipe_path, 0666);

    if (result == -1) {
        LOG_ERRNO("could not create pipe");
        exit(1);
    }

    LOG_INFO("created pipe: %s", g_libfs_pipe_path);

    g_libfs_pipe_fd = open(g_libfs_pipe_path, O_RDONLY);

    if (g_libfs_pipe_fd == -1) {
        LOG_ERRNO("could not open pipe");
        exit(1);
    }

    LOG_INFO("opened pipe: %s", g_libfs_pipe_path);

    LOG_INFO("%s", "starting dispatcher loop");

    while (true) {
        libfs_request_t request = read_request_from_pipe(g_libfs_pipe_fd);
        dispatch(request);
        libfs_request_free(request);
    }

    return 0;
}
