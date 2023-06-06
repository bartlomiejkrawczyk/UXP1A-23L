FDW=$(libfs-create chmode.txt)
BYTES_WRITTEN=$(libfs-write $FDW "Before chmode")
libfs-close $FDW

libfs_chmode chmode.txt 755

MODE=$(libfs_stat chmode.txt | awk -F' ' '{print $2}')
EXPECTED_MODE=755

if [[ "$MODE" -eq "$EXPECTED_MODE" ]]; then
  echo 1
fi