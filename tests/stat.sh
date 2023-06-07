#!/bin/bash

FILE="stat.txt"

FD=$(libfs-create "$FILE")

STATS=$(libfs-stat "$FD")

echo $STATS | sed 's/[0-9]//g'

libfs-close "$FD"

# Clean up

libfs-unlink "$FILE"
