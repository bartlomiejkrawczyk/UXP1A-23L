.PHONY: clean libfs-chmode libfs-close libfs-create libfs-open libfs-read libfs-rename libfs-seek libfs-unlink libfs-write

all: libfs-chmode libfs-close libfs-create libfs-open libfs-read libfs-rename libfs-seek libfs-unlink libfs-write

clean:
	make -C libfs-chmode clean
	make -C libfs-close clean
	make -C libfs-create clean
	make -C libfs-open clean
	make -C libfs-read clean
	make -C libfs-rename clean
	make -C libfs-seek clean
	make -C libfs-unlink clean
	make -C libfs-write clean

libfs-chmode:
	make -C libfs-chmode all

libfs-close:
	make -C libfs-close all

libfs-create:
	make -C libfs-create all

libfs-open:
	make -C libfs-open all

libfs-read:
	make -C libfs-read all

libfs-rename:
	make -C libfs-rename all

libfs-seek:
	make -C libfs-seek all

libfs-unlink:
	make -C libfs-unlink all

libfs-write:
	make -C libfs-write all
