#!/bin/bash

FDW=$(libfs-create test_link.txt)
BYTES_WRITTEN=$(libfs-write $FDW "Before link")
libfs-close $FDW

libfs-link test_link.txt test_link2.txt

FDW=$(libfs-open test_link.txt -w)
BYTES_WRITTEN=$(libfs-write $FDW "After link ")
libfs-close $FDW

FDR=$(libfs-open test_link2.txt -r)
CONTENT=$(libfs-read $FDR 1024)
libfs-close $FDR

echo $CONTENT
