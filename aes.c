#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "aes.h"


uint8_t box_lookup(uint8_t val, const uint8_t box[16][16]) {
    uint8_t x = (val >> 4) & 0x0f;
    uint8_t y = val & 0x0f;
    return box[x][y];
}

void sub_bytes(uint8_t block[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            block[i][j] = box_lookup(block[i][j], sbox);
        }
    }
}

void inv_sub_bytes(uint8_t block[4][4]) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            block[i][j] = box_lookup(block[i][j], inv_sbox);
        }
    }
}

void shift_row_left(uint8_t row[4], int N, int n) {
    uint8_t t[4];
    // get mapping
    for (int i = 0; i < 4; i++) {
        /* t[(i + N + n) % N] = row[i]; */
        t[i] = row[(i + N + n) % N];
    }
    // copy mapping back to original
    for (int i = 0; i < 4; i++) {
        row[i] = t[i];
    }
}

void shift_row_right(uint8_t row[4], int N, int n) {
    uint8_t t[4];
    // get mapping
    for (int i = 0; i < 4; i++) {
        t[(i + n) % N] = row[i];
    }
    // copy mapping back to original
    for (int i = 0; i < 4; i++) {
        row[i] = t[i];
    }
}

void shift_rows(uint8_t block[4][4]) {
    for (int i = 0; i < 4; i++) {
        shift_row_left(block[i], 4, i);
    }
}

void inv_shift_rows(uint8_t block[4][4]) {
    for (int i = 0; i < 4; i++) {
        shift_row_right(block[i], 4, i);
    }
}

uint8_t xtime(uint8_t x) {
    bool of;
    uint8_t o;
    // shift x left
    o = x << 1;
    // check if overflow
    of = o < x;
    // return o if no overflow else xor 0x1b
    return of ? o ^ 0x1b : o;
}

// finite field multiplication implementation on 1 byte
uint8_t gf_mult(uint8_t x, uint8_t y) {
    uint8_t t;
    uint8_t o;
    o = (y & 0x01) ? x : 0;
    t = x;
    for (int i = 1; i < 8; i++) {
        t = xtime(t);
        if (y & (0x01 << i)) {
            o ^= t;
        }
    }
    return o;
}

void mix_cols(uint8_t block[4][4]) {
    uint8_t col[4];

    for (int j = 0; j < 4; j++) {
        // process into col buffer
        for (int i = 0; i < 4; i++) {
            col[i] = gf_mult(mixer[i][0], block[0][j]) ^
                     gf_mult(mixer[i][1], block[1][j]) ^
                     gf_mult(mixer[i][2], block[2][j]) ^
                     gf_mult(mixer[i][3], block[3][j]);
        }

        // copy col buffer into original
        for (int i = 0; i < 4; i++) {
            block[i][j] = col[i];
        }
    }
}

void inv_mix_cols(uint8_t block[4][4]) {
    uint8_t col[4];

    for (int j = 0; j < 4; j++) {
        // process into col buffer
        for (int i = 0; i < 4; i++) {
            col[i] = gf_mult(inv_mixer[i][0], block[0][j]) ^
                     gf_mult(inv_mixer[i][1], block[1][j]) ^
                     gf_mult(inv_mixer[i][2], block[2][j]) ^
                     gf_mult(inv_mixer[i][3], block[3][j]);
        }

        // copy col buffer into original
        for (int i = 0; i < 4; i++) {
            block[i][j] = col[i];
        }
    }
}

void add_round_key(uint8_t block[4][4], uint8_t w[11][4][4], int round) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            block[i][j] = block[i][j] ^ w[round][i][j];
        }
    }
}

void key_expansion(uint8_t key[4][4], uint8_t w[11][4][4]) {
    uint8_t word[4];

    // copy key to w
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            w[0][i][j] = key[i][j];
        }
    }

    for (int round = 1; round < 11; round++) {

        // copy last col to word
        for (int i = 0; i < 4; i++) {
            word[i] = w[round - 1][i][3];
        }
        shift_row_left(word, 4, 1);
        // sub bytes and rcon
        for (int i = 0; i < 4; i++) {
            word[i] = box_lookup(word[i], sbox) ^ rc_i[round - 1];
        }

        for (int j = 0; j < 4; j++) {
            for (int i = 0; i < 4; i++) {
                /* printf("round - 1 %d\n", round - 1); */
                word[i] ^= w[round - 1][i][j];
                w[round][i][j] = word[i];
            }
        }
    }
}

void cipher(uint8_t block[4][4], uint8_t out[4][4], uint8_t w[11][4][4]) {
    uint8_t state[4][4];

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[i][j] = block[i][j];
        }
    }

    add_round_key(state, w, 0);

    for (int round = 1; round < 10; round++) {
        sub_bytes(state);
        shift_rows(state);
        mix_cols(state);
        add_round_key(state, w, round);
    }

    sub_bytes(state);
    shift_rows(state);
    add_round_key(state, w, 10);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            out[i][j] = state[i][j];
        }
    }
}

void inverse_cipher(uint8_t block[4][4], uint8_t out[4][4], uint8_t w[11][4][4]) {
    uint8_t state[4][4];

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            state[i][j] = block[i][j];
        }
    }

    add_round_key(state, w, 10);

    for (int round = 8; round > -1; round--) {
        inv_shift_rows(state);
        inv_sub_bytes(state);
        add_round_key(state, w, round + 1);
        inv_mix_cols(state);
    }

    inv_shift_rows(state);
    inv_sub_bytes(state);
    add_round_key(state, w, 0);

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            out[i][j] = state[i][j];
        }
    }
}
