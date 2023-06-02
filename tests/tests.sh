../clients/libfs-create/libfs-create hello.txt
fdw=$(../clients/libfs-open/libfs-open -w hello.txt)
../clients/libfs-write/libfs-write $fdw "Hello"
../clients/libfs-close/libfs-close $fdw

fdr=$(../clients/libfs-open/libfs-open -r hello.txt)
content=$(../clients/libfs/libfs -read $fdr)
../clients/libfs-close/libfs-close $fdr

echo $content