#!/bin/bash

FILE_WRITE="write.txt"
FILE_READ="read.txt"
FILE_READ_WRITE="read_write.txt"

libfs_write() {
    FDW=$(libfs-open "$1" -w)
    BYTES_WRITTEN=$(libfs-write "$FDW" "$2")
    libfs-close "$FDW"
}

libfs_read() {
    FDR=$(libfs-open "$1" -r)
    CONTENT=$(libfs-read "$FDR" 1024)
    libfs-close "$FDR"

    echo $CONTENT
}

# Create read write file

FDW=$(libfs-create "$FILE_READ_WRITE" rwxrwxrwx)
BYTES_WRITTEN=$(libfs-write "$FDW" "READ WRITE")
libfs-close "$FDW"

libfs_read "$FILE_READ_WRITE"

# Create write file

FDW=$(libfs-create "$FILE_WRITE" -w--w--w-)
BYTES_WRITTEN=$(libfs-write "$FDW" "WRITE")
libfs-close "$FDW"

libfs-open "$FILE_WRITE" -r

# Create read file

FDW=$(libfs-create "$FILE_READ" 444)
libfs-close "$FDW"

libfs_read "$FILE_READ"

libfs-open "$FILE_READ" -w

# Clean up

libfs-unlink "$FILE_WRITE"
libfs-unlink "$FILE_READ"
libfs-unlink "$FILE_READ_WRITE"
