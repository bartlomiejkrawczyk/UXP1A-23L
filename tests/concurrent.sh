#!/bin/bash

FILE="concurrent.txt"

FDW=$(libfs-create "$FILE")
libfs-write "$FDW" "abcdefghijk lmnopqrstwxyz" > /dev/null
libfs-close "$FDW"


FDW_1=$(libfs-open "$FILE")
FDW_2=$(libfs-open "$FILE")

libfs-seek "$FDW_2" 12

libfs-write "$FDW_1" "ABCDEFGHIJK" > /dev/null  &
libfs-write "$FDW_2" "LMNOPQRSTWXYZ" > /dev/null

sleep 1

libfs-close "$FDW_1"
libfs-close "$FDW_2"

FDR=$(libfs-open "$FILE")
libfs-read "$FDR" 1024
libfs-close "$FDR"

libfs-unlink "$FILE"
