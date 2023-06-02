set -euox

fdw=$(../clients/libfs-create/libfs-create hello.txt)

../clients/libfs-write/libfs-write $fdw "Hello"
../clients/libfs-close/libfs-close $fdw

fdr=$(../clients/libfs-open/libfs-open -r hello.txt)
content=$(../clients/libfs-read/libfs-read $fdr)
../clients/libfs-close/libfs-close $fdr

echo $content