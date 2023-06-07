#!/bin/bash

FILE_BEFORE="before.txt"
FILE_AFTER="after.txt"

libfs_read() {
    FDR=$(libfs-open "$1" -r)
    CONTENT=$(libfs-read "$FDR" 1024)
    libfs-close "$FDR"

    echo $CONTENT
}

# Create read write file

FDW=$(libfs-create "$FILE_BEFORE" rw-rw-rw-)
BYTES_WRITTEN=$(libfs-write "$FDW" "CONTENT")
libfs-close "$FDW"

libfs_read "$FILE_BEFORE"

# Reaname file

libfs-rename "$FILE_BEFORE" "$FILE_AFTER"

libfs_read "$FILE_AFTER"

# File does not exist anymore

libfs-open "$FILE_BEFORE"

# Clean up

libfs-unlink "$FILE_AFTER"