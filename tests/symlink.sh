#!/bin/bash

FILE="link.txt"
SYMLINK="symlink.txt"

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
BYTES_WRITTEN=$(libfs-write "$FDW" "BEFORE")
libfs-close "$FDW"

# Create symlink between files

libfs-link "$FILE" "$SYMLINK"

# Read and write is possible using symlink

libfs_read "$SYMLINK"

libfs_write "$SYMLINK" "LINK  "

# Changes are made to the FILE too

libfs_read "$FILE"

# Delete file pointed by symlink

libfs-unlink "$FILE"

# File content is not available

libfs_read "$SYMLINK"

FDW=$(libfs-create "$FILE" rwxrwxrwx)
BYTES_WRITTEN=$(libfs-write "$FDW" "AFTER")
libfs-close "$FDW"

# Now the content is available again

libfs_read "$SYMLINK"

# Clean up

libfs-unlink "$FILE"
libfs-unlink "$SYMLINK"
