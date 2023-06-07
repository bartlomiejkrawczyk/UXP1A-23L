#!/bin/bash

FILE="unlink.txt"

FDW=$(libfs-create "$FILE")
libfs-close $FDW

# Delete created file

libfs-unlink "$FILE"

# Deleted file cannot be open

libfs-open "$FILE"

# Cannot delete already deleted file
libfs-unlink "$FILE"
