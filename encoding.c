#include "include/encoding.h"

#define LOOKUP_CANARY 255

//Encoding and decoding lookup tables table for each of the 2-bit values
char encode_lookup_tbl[] = { '\t', '\n', '\r', ' ' };
unsigned char decode_lookup_tbl[256];

// Build the decoding table, including a canary to help
// us detect invalid bytes. The buffer needs to be freed by
// the caller.
void alloc_decode_lookup_tbl(void) {
    int x;

    //Fill the entire array with our canary 
    for (x = 0; x < 256; x++) {
        decode_lookup_tbl[x] = LOOKUP_CANARY;
    }

    //Add the four encoding characters by using the inverse
    //of the encoding table
    for (x = 0; x < 4; x++) {
        decode_lookup_tbl[ encode_lookup_tbl[x] ] = x;
    }
}


//Encode each 2 bits of the byte into whitespace
ssize_t ws_encode(const unsigned char *bytes_in, unsigned char *ws_out, const ssize_t bytes) {
    int x, y, pos;
    unsigned char whitespace;

    for (x = 0; x < bytes; x++) {
        for (y = 0; y < 4; y++) {
            whitespace = encode_lookup_tbl[ (bytes_in[x] >> (2 * y)) & 0x03 ];
            pos = (4 * x) + y;

            ws_out[pos] = whitespace;
        }
    }

    return x * 4;
}

#include <stdio.h>

//Decode each group of four bytes back into a single byte.
ssize_t ws_decode(const unsigned char *ws_in, unsigned char *bytes_out, const ssize_t bytes_read) {
    int x, y;
    unsigned char dibit;

    for (x = 0; x < bytes_read; x += 4) {
        *bytes_out = 0;

        for (y = 0; y < 4; y++) {
            dibit = decode_lookup_tbl[ ws_in[x + y] ];

            if (dibit == LOOKUP_CANARY) {
                return 0;
            }

            *bytes_out |= dibit << (2 * y);
        }

        *bytes_out++;
    }
    
    return x / 4;
}
