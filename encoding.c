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
    int x, y;
    unsigned char whitespace;

    for (x = 0; x < bytes; x++) {
        for (y = 0; y < 4; y++) {
            //For each two bits in the byte, look up its value (0 to 3) in the
            //lookup table and add it to the outbound whitespace buffer, taking into
            //account that each inbound byte becomes four outbound bytes
            ws_out[(4 * x) + y] = encode_lookup_tbl[ (bytes_in[x] >> (2 * y)) & 0x03 ];
        }
    }

    return x * 4;
}

//Decode each group of four bytes back into a single byte.
ssize_t ws_decode(const unsigned char *ws_in, unsigned char *bytes_out, const ssize_t bytes_read) {
    int x, y;
    unsigned char dibit;

    for (x = 0; x < bytes_read; x += 4) {
        //Ensure out destination byte is zeroed out.
        *bytes_out = 0;

        for (y = 0; y < 4; y++) {
            //Pull each of the two bits of our outbound byte from
            //four of the inbound bytes.
            dibit = decode_lookup_tbl[ ws_in[x + y] ];

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
