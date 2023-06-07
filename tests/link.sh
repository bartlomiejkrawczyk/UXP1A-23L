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

# Create hard link between files

libfs-link "$FILE" "$HARD_LINK"

# Read and write is possible using hard link

libfs_read "$HARD_LINK"

libfs_write "$HARD_LINK" "LINK"

# Changes are made to the FILE too

libfs_read "$FILE"

# Delete one of the linked files

libfs-unlink "$FILE"

# File content is still available

libfs_read "$HARD_LINK"

# After adding file named FILE, the content of HARD_LINK stays the same

FDW=$(libfs-create "$FILE" rwxrwxrwx)
BYTES_WRITTEN=$(libfs-write "$FDW" "AFTER")
libfs-close "$FDW"

libfs_read "$HARD_LINK"

libfs-unlink "$HARD_LINK"
