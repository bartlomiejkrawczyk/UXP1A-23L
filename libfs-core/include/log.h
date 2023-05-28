#pragma once

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "termcolor.h"

#define LOG_INFO(pattern, ...) \
    printf(TERMCOLOR(BRIGHT_GREEN, "[INFO] ") pattern "\n", ##__VA_ARGS__);

#define LOG_WARN(pattern, ...) printf(TERMCOLOR(YELLOW, "[WARN] ") pattern "\n", ##__VA_ARGS__);

#define LOG_ERROR(pattern, ...) \
    printf(TERMCOLOR(BRIGHT_RED, "[ERROR] ") pattern "\n", ##__VA_ARGS__);

#define LOG_ERRNO(message) \
    printf(TERMCOLOR(BRIGHT_RED, "[ERROR] ") message ": %s (%s:%d)\n", strerror(errno), __FILE__, __LINE__);
