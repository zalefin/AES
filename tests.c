#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "aes.h"

#define NTESTS 256

int main() {
    uint8_t w[11][4][4];
    uint8_t data[4][4];
    uint8_t cipher_block[4][4];
    uint8_t plain_block[4][4];
    int key[4];

    for (int test = 0; test < NTESTS; test++) {
        for (int i = 0; i < 4; i++) {
            *data[i] = rand();
        }

        // init random key
        for (int i = 0; i < 4; i++) {
            key[i] = rand();
        }

        key_expansion((uint8_t (*)[4])key, w);

        cipher(data, cipher_block, w);
        inverse_cipher(cipher_block, plain_block, w);

        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                if (data[i][j] != plain_block[i][j]) {
                    printf("MISMATCH\n");
                    printf("%x -> %x -> %x\n", data[i][j], cipher_block[i][j], plain_block[i][j]);
                    return 1;
                }
            }
        }
    }

    printf("PASSED %d TESTS\n", NTESTS);

    return 0;
}
