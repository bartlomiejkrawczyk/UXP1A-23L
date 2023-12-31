#!/bin/bash

FILE="chmode.txt"

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

FDW=$(libfs-create "$FILE" rwxrwxrwx)
BYTES_WRITTEN=$(libfs-write "$FDW" "Before chmode")
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

libfs-chmode "$FILE" 755

echo

# Now write works again

libfs_write "After chmode "

libfs_read

# Clean up

libfs-unlink "$FILE"
