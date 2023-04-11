.PHONY: clean libfs-core libfs-daemon libfs clients

all: libfs-core libfs-daemon libfs clients

clean:
	make -C libfs-core clean
	make -C libfs-daemon clean
	make -C libfs clean
	make -C clients clean

libfs-core:
	make -C libfs-core all

libfs-daemon: libfs-core
	make -C libfs-daemon all

libfs: libfs-core
	make -C libfs all

clients: libfs
	make -C clients all
