#include <stdio.h>

#include "request.h"
#include "response.h"
#include "types.h"

int main(int argc, char* argv[]) {
    char* name = "test.txt";

    if (argc > 1) {
        name = argv[1];
    }

    libfs_request_create_t request = {
        .mode = 0777, // octal
        .name = name,
    };

    libfs_request_t packed_request = libfs_request_create_pack(&request);

    usize buffer_size;
    u8* buffer = libfs_request_serialize(&packed_request, &buffer_size);

    printf("buffer_size: %ld\n", buffer_size);
    printf("buffer: [\n");
    for (usize i = 0; i < buffer_size; i++) {
        printf("\t0x%02x,", buffer[i]);
        if (i % 8 == 7) {
            printf("\n");
        }
    }
    printf("\n]\n");

    libfs_request_t unpacked_request = libfs_request_deserialize(buffer);

    libfs_request_create_t unpacked_request_create =
        libfs_request_create_unpack(&unpacked_request);

    printf("mode: %o, name: %s\n", unpacked_request_create.mode, unpacked_request_create.name);

    return 0;
}
