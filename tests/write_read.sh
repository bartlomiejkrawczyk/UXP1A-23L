set -euox

FDW=$(libfs-create hello.txt)

libfs-write $FDW "Hello, World!"
libfs-close $FDW

FDR=$(libfs-open hello.txt 1024)
CONTENT=$(libfs-read $FDR)
libfs-close $FDR

echo $CONTENT
