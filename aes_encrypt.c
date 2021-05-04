#include <stdint.h>
#include <stdio.h>

#include "aes.h"

int main(int argc, char **argv) {
    uint8_t w[11][4][4];
    uint8_t data[4][4];
    uint8_t out[4][4];
    uint8_t original[4][4];
    char *key = "wew YOU foo bar!";


    // populate data
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            data[i][j] = 4*i + j;
        }
    }

    key_expansion((uint8_t (*)[4])key, w);

    /* for (int round = 0; round < 11; round++) { */
    /*     for (int j = 0; j < 4; j++) { */
    /*         printf("%d %d %d %d\n", w[round][0][j], w[round][1][j], w[round][2][j], w[round][3][j]); */
    /*     } */
    /* } */

    cipher(data, out, w);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d -> %d\n", data[i][j], out[i][j]);
        }
    }

    printf("\n");

    inverse_cipher(out, original, w);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            printf("%d -> %d\n", out[i][j], original[i][j]);
        }
    }

    return 0;
}
