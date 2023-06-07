#!/bin/bash

FILE="concurrent.txt"

FDW=$(libfs-create "$FILE")
libfs-close "$FDW"


FDW_1=$(libfs-open "$FILE")
FDW_2=$(libfs-open "$FILE")

libfs-write "$FDW_1" "$(<./files/random-file)" &
printf "PID of first write: %s\n" "$!"
libfs-write "$FDW_2" "test" &
printf "PID of second write: %s\n" "$!"

wait

libfs-close "$FDW_1"
libfs-close "$FDW_2"

FDR=$(libfs-open "$FILE")
libfs-read "$FDR" 1024
libfs-close "$FDR"

libfs-unlink "$FILE"
