#!/bin/bash

FILE="read.txt"

libfs_write() {
    FDW=$(libfs-open "$FILE" -w)
    BYTES_WRITTEN=$(libfs-write "$FDW" "$1")
    libfs-close "$FDW"
}

libfs_read() {
    FDR=$(libfs-open "$FILE" -r)
    CONTENT=$(libfs-read "$FDR" 1024)
    libfs-close "$FDR"

    echo $CONTENT
}

# Prepare files

FDW=$(libfs-create "$FILE" rw-rw-rw-)
BYTES_WRITTEN=$(libfs-write "$FDW" "1")
libfs-close "$FDW"

libfs_read

libfs_write "2"

libfs_read

libfs_write "3"

libfs_read

# Clean up

libfs-unlink "$FILE"
