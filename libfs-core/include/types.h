#ifndef LIBFS_CORE_TYPES_H
#define LIBFS_CORE_TYPES_H
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef float f32;
typedef double f64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef int8_t i8;
typedef int16_t i16;
typedef int32_t i32;
typedef int64_t i64;
typedef size_t usize;   // "natural" size of a pointer
typedef __ssize_t isize;

typedef int fd_type;

#endif // LIBFS_CORE_TYPES_H

