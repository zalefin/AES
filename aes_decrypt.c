#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "aes.h"

int main(int argc, char **argv) {
    FILE *in_file;
    FILE *out_file;
    size_t file_size;
    size_t n_read;
    char out_path[1024];
    uint8_t w[11][4][4];
    uint8_t data[4][4];
    uint8_t out[4][4];
    char key[16];

    // check arg length
    if (argc != 3) {
        fprintf(stderr, "./aes_decrypt [PATH] [KEY]\n");
        return 1;
    }

    if (!(in_file = fopen(argv[1], "r"))) {
        fprintf(stderr, "Error opening file %s\n", argv[1]);
        return 1;
    }

    sprintf(out_path, "%s.AESPLAIN", argv[1]);

    if (!(out_file = fopen(out_path, "w"))) {
        fclose(in_file);
        fprintf(stderr, "Error opening file %s\n", argv[1]);
        return 1;
    }

    // populate key schedule
    strncpy(key, argv[2], 16);
    key_expansion((uint8_t (*)[4])key, w);

    // get file size
    fread(&file_size, 1, sizeof(size_t), in_file);
    printf("File size: %ld\n", file_size);

    do {
        // zero out the block buffer
        bzero(data, 16);
        // read from file into block
        n_read = fread(data, 1, 16, in_file);
        if (n_read == 0) {
            break;
        }
        // run block through AES
        inverse_cipher(data, out, w);
        // write out block to file
        fwrite(out, 1, (file_size > 16) ? 16 : file_size, out_file);
        file_size -= n_read;
    } while (n_read == 16);

    fclose(in_file);
    fclose(out_file);

    printf("Finished\n");

    return 0;
}
