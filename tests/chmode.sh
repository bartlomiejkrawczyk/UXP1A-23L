#!/bin/bash

FDW=$(libfs-create chmode.txt)
BYTES_WRITTEN=$(libfs-write $FDW "Before chmode")
libfs-close $FDW

# libfs-chmode chmode.txt 755

# # TODO: FIX ME
# echo done
