#!/bin/bash

FDW=$(libfs-create test_unlink.txt)
BYTES_WRITTEN=$(libfs-write $FDW "Before link")
libfs-close $FDW

libfs-link test_unlink.txt test_unlink2.txt

FDR=$(libfs-open test_unlink2.txt -r)
CONTENT=$(libfs-read $FDR 1024)
libfs-close $FDR

echo $CONTENT

libfs-unlink test_unlink2.txt

# FDR=$(libfs-open test_unlink2.txt -r)
# libfs-close $FDR
