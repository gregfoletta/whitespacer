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

unsigned char lookup_encode(const unsigned char dibit) {
    return encode_lookup_tbl[ dibit ];
}

unsigned char poly_encode(const unsigned char dibit) {
    return 9.0 + 4.666667 * dibit - 6.0 * (dibit * dibit) + 2.333333333333 * (dibit * dibit * dibit);
}

unsigned char switch_encode(const unsigned char dibit) {
    switch (dibit) {
        case 0:
            return '\t';
        case 1:
            return '\n';
        case 2:
            return '\r';
        case 3:
            return ' ';
    }
}

unsigned char lookup_decode(const unsigned char ws) {
    return decode_lookup_tbl[ ws ];
}

unsigned char poly_decode(const unsigned char ws) {
    return -31.82920741111 + 6.42174606666 * ws + -0.3807988444 * (ws * ws) + 0.00669163333 * (ws * ws * ws);
}

unsigned char switch_decode(const unsigned char ws) {
    switch (ws) {
        case '\t':
            return 0;
        case '\n':
            return 1;
        case '\r':
            return 2;
        case ' ':
            return 3;
    }

    return LOOKUP_CANARY;
}

//Encode each 2 bits of the byte into whitespace
ssize_t ws_encode(const unsigned char *bytes_in, unsigned char *ws_out, const ssize_t bytes, unsigned char (*encoder)(unsigned char)) {
    int x, y;

    for (x = 0; x < bytes; x++) {
        for (y = 0; y < 4; y++) {
            //For each two bits in the byte, look up its value (0 to 3) in the
            //lookup table and add it to the outbound whitespace buffer, taking into
            //account that each inbound byte becomes four outbound bytes
            ws_out[(4 * x) + y] = encoder( (bytes_in[x] >> (2 * y)) & 0x03 );
        }
    }

    return x * 4;
}



//Decode each group of four bytes back into a single byte.
ssize_t ws_decode(const unsigned char *ws_in, unsigned char *bytes_out, const ssize_t bytes_read, unsigned char (*decoder)(unsigned char)) {
    int x, y;
    unsigned char dibit;

    for (x = 0; x < bytes_read; x += 4) {
        //Ensure out destination byte is zeroed out.
        *bytes_out = 0;

        for (y = 0; y < 4; y++) {
            //Pull each of the two bits of our outbound byte from
            //four of the inbound bytes.
            unsigned char whitespace = ws_in[x + y];
            dibit = decoder(whitespace);

            //Ensure our inbound whitespace byte is
            //valid for our encoding
            if (dibit == LOOKUP_CANARY) {
                return 0;
            }

            //Or in the two bits at the appropriate position in the byte
            *bytes_out |= dibit << (2 * y);
        }

        *bytes_out++;
    }
    
    return x / 4;
}


