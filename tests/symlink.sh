#!/bin/bash

FDW=$(libfs-create test_symlink.txt)
BYTES_WRITTEN=$(libfs-write $FDW "Before symlink")
libfs-close $FDW

libfs-symlink test_symlink.txt test_symlink2.txt

FDR=$(libfs-open test_symlink2.txt -r)
CONTENT=$(libfs-read $FDR 1024)
libfs-close $FDR

echo $CONTENT
