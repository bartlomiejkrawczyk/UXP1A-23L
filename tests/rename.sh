FDW=$(libfs-create before.txt)

BYTES_WRITTEN=$(libfs-write $FDW "Before rename")
libfs-close $FDW

libfs-rename before.txt after.txt

FDR=$(libfs-open after.txt 1)
CONTENT=$(libfs-read $FDR 1024)
libfs-close $FDR

echo $CONTENT