#!/bin/bash

FILE="close.txt"

FD=$(libfs-create "$FILE")

# Close open file descriptor

libfs-close "$FD"

libfs-close "$FD"

# Bad file descriptor

libfs-close -1

# Clean up

libfs-unlink "$FILE"
