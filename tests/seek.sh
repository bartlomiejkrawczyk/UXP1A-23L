FDW=$(libfs-create test.txt)
BYTES_WRITTEN=$(libfs-write $FDW "Before seek")
libfs-close $FDW

FDR=$(libfs-open test.txt 1)
libfs-seek $FDR 7
CONTENT=$(libfs-read $FDR 1024)
libfs-close $FDR

echo $CONTENT
