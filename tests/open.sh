#!/bin/bash

FILE="open.txt"

libfs_read() {
    FDR=$(libfs-open "$FILE" -r)
    CONTENT=$(libfs-read "$FDR" 1024)
    libfs-close "$FDR"

    echo $CONTENT
}

# Prepare files

FDW=$(libfs-create "$FILE" rw-rw-rw-)
BYTES_WRITTEN=$(libfs-write "$FDW" "CONTENT")
libfs-close "$FDW"

libfs_read

# Change file mode

libfs-chmode "$FILE" r--r--r--

# Permission will be denied

echo

libfs-open "$FILE" -w

# Read permission still works

libfs_read

# Once again change file mode

libfs-chmode "$FILE" -w--w--w-

echo

# Now read does not work

libfs-open "$FILE" -r

# Clean up

libfs-unlink "$FILE"
