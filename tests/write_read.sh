#!/bin/bash

FDW=$(libfs-create hello.txt)

BYTES_WRITTEN=$(libfs-write $FDW "Hello, World!")
libfs-close $FDW

FDR=$(libfs-open hello.txt -r)
CONTENT=$(libfs-read $FDR 1024)
libfs-close $FDR

echo $CONTENT
