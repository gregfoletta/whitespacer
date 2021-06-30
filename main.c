#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "include/encoding.h"

#define INPUT_OUTPUT_RATIO 4
#define READ_BUFFER 10485760 
#define OUTPUT_BUFFER READ_BUFFER * INPUT_OUTPUT_RATIO

#define STDIN 0
#define STDOUT 1


int is_decoder(int argc, char **argv) {
    //Even with one argument we've still got two characters (char + NUL)
    if (argc > 1 && argv[1][0] == '-' && argv[1][1] == 'd')
        return 1;

    return 0;
}

            
int main(int argc, char **argv) {
    ssize_t bytes_read = 0, total_bytes_read = 0, num_bytes_write = 0;
    unsigned char *bytes_in, *bytes_out;
    int ret;

    int decoding = is_decoder(argc, argv);

    alloc_decode_lookup_tbl();


    bytes_in = malloc(READ_BUFFER * sizeof(*bytes_in));
    bytes_out = malloc(OUTPUT_BUFFER * sizeof(*bytes_in));

    if (!bytes_in || !bytes_out) {
        fprintf(stderr, "- Could not allocate memory for input/output\n");
        ret = -1;
        goto out;
    }

    unsigned char *buffer_pos = bytes_in;

    while ( (bytes_read = read(STDIN, buffer_pos, READ_BUFFER - total_bytes_read)) ) {
        if (bytes_read < 0) {
            fprintf(stderr, "- read() error\n");
            ret = -1;
            goto out;
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
            if (!num_bytes_write) {
                fprintf(stderr, "- Encoding error, likely invalid byte");
                ret = -1;
                goto out;
            }
        }

        //Write out to stdout
        if ( write(STDOUT, bytes_out, num_bytes_write) < 0 ) {
            fprintf(stderr, "- write() error\n");
            ret = -1;
        }

        //Reset buffer position and total bytes read.
        buffer_pos = bytes_in;
        total_bytes_read = 0;
    }

    //If we got there and total_bytes_read isn't zero, it means that our input wasn't
    //a multiple of 4 bytes and our decoding failed.
    if (total_bytes_read != 0) {
        fprintf(stderr, "- Error: decoding and input not a multiple of 4 bytes\n");
        ret = -1;
        goto out;
    }

    ret = 0;

out:
    free(bytes_in);
    free(bytes_out);
    return ret;
}


