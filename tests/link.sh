FDW=$(libfs-create test_link.txt)
BYTES_WRITTEN=$(libfs-write $FDW "Before link")
libfs-close $FDW

libfs_link test_link.txt test_link2.txt

FDW=$(libfs-create test_link.txt)
BYTES_WRITTEN=$(libfs-write $FDW "After link")
libfs-close $FDW

FDR=$(libfs-open test_link2.txt 1)
CONTENT=$(libfs-read $FDR 1024)
libfs-close $FDR

echo $CONTENT