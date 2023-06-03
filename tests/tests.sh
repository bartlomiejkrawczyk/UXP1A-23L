set -euox

PATH=$PATH:../binaries

fdw=$(libfs-create hello.txt)

libfs-write $fdw "Hello"
libfs-close $fdw

fdr=$(libfs-open hello.txt)
content=$(libfs-read $fdr)
libfs-close $fdr

echo $content
