#!/bin/bash

FILE="seek.txt"

# Prepare files

FDRW=$(libfs-create "$FILE" rw-rw-rw-)
BYTES_WRITTEN=$(libfs-write "$FDRW" "1")
libfs-close "$FDRW"

FDRW=$(libfs-open "$FILE")

libfs-seek "$FDRW" 1

BYTES_WRITTEN=$(libfs-write "$FDRW" "2")

BYTES_WRITTEN=$(libfs-write "$FDRW" "3")

libfs-seek "$FDRW" -3

CONTENT=$(libfs-read "$FDRW" 1024)
echo "$CONTENT"

libfs-seek "$FDRW" -2

BYTES_WRITTEN=$(libfs-write "$FDRW" "0")

libfs-seek "$FDRW" 1

BYTES_WRITTEN=$(libfs-write "$FDRW" "4")

libfs-seek "$FDRW" -4

CONTENT=$(libfs-read "$FDRW" 1024)
echo "$CONTENT"

# Seek out of file range

libfs-seek "$FDRW" -10

libfs-close "$FDRW"

# Clean up

libfs-unlink "$FILE"
