#!/bin/bash

FILE="link.txt"
HARD_LINK="hard_link.txt"

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

# Prepare files

FDW=$(libfs-create "$FILE" rwxrwxrwx)
BYTES_WRITTEN=$(libfs-write "$FDW" "FILE")
libfs-close "$FDW"

libfs-link "$FILE" "$HARD_LINK"

libfs_read "$HARD_LINK"

libfs_write "$HARD_LINK" "LINK"

libfs_read "$FILE"

libfs-unlink "$FILE"

libfs_read "$HARD_LINK"

libfs-unlink "$HARD_LINK"
