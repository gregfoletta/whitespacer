#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define READ_BUFFER 10485760 
#define OUTPUT_BUFFER READ_BUFFER * 4

#define STDIN 0
#define STDOUT 1

//Quaternary translation - these need to be contiguous characters.
char ws_lookup[] = {'\b', '\t', '\n', '\v'};

//Encode each 2 bits of the byte into whitespace
int ws_encode(unsigned char *bytes_in, unsigned char *ws_out, ssize_t bytes) {
    int x;

    for (x = 0; x < bytes; x++) {
        *ws_out++ = ws_lookup[ bytes_in[x] & 0x03 ];
        *ws_out++ = ws_lookup[ bytes_in[x] >> 2 & 0x03 ];
        *ws_out++ = ws_lookup[ bytes_in[x] >> 4 & 0x03 ];
        *ws_out++ = ws_lookup[ bytes_in[x] >> 6 & 0x03 ];
    }

    return x * 4;
}

//Decode each group of four bytes back into a single byte.
int ws_decode(unsigned char *bytes_in, unsigned char *bytes_out, ssize_t bytes_read) {
    int x;

    for (x = 0; x < bytes_read; x += 4) {
        *bytes_out++ = 
            bytes_in[x] - ws_lookup[0] |
            (bytes_in[x + 1] - ws_lookup[0]) << 2 |
            (bytes_in[x + 2] - ws_lookup[0]) << 4 |
            (bytes_in[x + 3] - ws_lookup[0]) << 6;
    }
    
    return x / 4;
}


int is_decoder(int argc, char **argv) {
    //Even with one argument we've still got two characters (char + NUL)
    if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'd')
        return 1;

    return 0;
}

            

int main(int argc, char **argv) {
    ssize_t bytes_read = 0, total_bytes_read = 0, num_bytes_write = 0;
    unsigned char *bytes_in, *bytes_out;

    int decoding = is_decoder(argc, argv);

    bytes_in = malloc(READ_BUFFER * sizeof(*bytes_in));
    bytes_out = malloc(4 * READ_BUFFER * sizeof(*bytes_in));

    unsigned char *buffer_pos = bytes_in;

    while ( (bytes_read = read(STDIN, buffer_pos, READ_BUFFER - total_bytes_read)) ) {
        if (bytes_read < 0) {
            fprintf(stderr, "- read() error\n");
            return -1;
        }

        total_bytes_read += bytes_read;

        if (decoding) {
            //If decoding we need a multiple of four bytes.
            if (!total_bytes_read & 4) {
                fprintf(stderr, "%ld\n", total_bytes_read);
                //Move our buffer along
                buffer_pos += bytes_read;
                continue;
            }

            num_bytes_write = ws_decode(bytes_in, bytes_out, total_bytes_read);
        } else {
            num_bytes_write = ws_encode(bytes_in, bytes_out, total_bytes_read);
        }

        //Write out to stdout
        if ( write(STDOUT, bytes_out, num_bytes_write) < 0 ) {
            fprintf(stderr, "- write() error\n");
            return -1;
        }

        //Reset buffer position and total bytes read.
        buffer_pos = bytes_in;
        total_bytes_read = 0;
    }

    //If we got there and total_bytes_read isn't zero, it means that our input wasn't
    //a multiple of 4 bytes and our decoding failed.
    if (total_bytes_read != 0) {
        fprintf(stderr, "- Error: decoding and input not a multiple of 4 bytes\n");
        return -1;
    }

    return 0;
}


