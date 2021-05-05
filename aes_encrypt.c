#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "aes.h"

int main(int argc, char **argv) {
    FILE *in_file;
    FILE *out_file;
    size_t file_size;
    size_t n_read;
    size_t n_write;
    char out_path[1024];
    uint8_t w[11][4][4];
    uint8_t data[4][4];
    uint8_t out[4][4];
    char key[16] = {0};

    // check arg length
    if (argc != 3) {
        fprintf(stderr, "./aes_encrypt [PATH] [KEY]\n");
        return 1;
    }

    if (!(in_file = fopen(argv[1], "r"))) {
        fprintf(stderr, "Error opening file %s\n", argv[1]);
        return 1;
    }

    sprintf(out_path, "%s.AESCIPHER", argv[1]);

    if (!(out_file = fopen(out_path, "w"))) {
        fclose(in_file);
        fprintf(stderr, "Error opening file %s\n", argv[1]);
        return 1;
    }

    // populate key schedule
    strncpy(key, argv[2], 16);
    key_expansion((uint8_t (*)[4])key, w);

    fseek(in_file, 0L, SEEK_END);
    file_size = ftell(in_file);
    rewind(in_file);

    // write length to head
    fwrite(&file_size, 1, sizeof(size_t), out_file);

    do {
        // zero out the block buffer
        bzero(data, 16);
        // read from file into block
        n_read = fread(data, 1, 16, in_file);
        if (n_read == 0) {
            break;
        }
        // run block through AES
        cipher(data, out, w);
        // write out block to file
        n_write = fwrite(out, 1, 16, out_file);
    } while (n_read == 16);

    fclose(in_file);
    fclose(out_file);

    printf("Finished\n");

    return 0;
}
