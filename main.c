#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>

#include "include/encoding.h"

#define INPUT_OUTPUT_RATIO 4
#define READ_BUFFER 10485760 
#define OUTPUT_BUFFER READ_BUFFER * INPUT_OUTPUT_RATIO

#define STDIN 0
#define STDOUT 1

struct cmdline_opts {
    int decoding;
    unsigned char (*encode_func)(unsigned char);
    unsigned char (*decode_func)(unsigned char);
};


struct encode_decode_algos {
    char *cmdline_arg;
    char *name;
    unsigned char (*encode_func)(unsigned char);
    unsigned char (*decode_func)(unsigned char);
};

struct cmdline_opts *parse_cmdline(int argc, char **argv) {
    struct cmdline_opts *opts;
    int opt;

    struct encode_decode_algos algos[] = {
        { 
            .cmdline_arg = "lookup",
            .encode_func = lookup_encode,
            .decode_func = lookup_decode

        },
        { 
            .cmdline_arg = "poly",
            .encode_func = poly_encode,
            .decode_func = poly_decode
        },
        { 
            .cmdline_arg = "switch",
            .encode_func = switch_encode,
            .decode_func = switch_decode
        }
    };
    
    int num_algos = sizeof(algos) / sizeof(struct encode_decode_algos);


    //Allocate and set default values
    opts = malloc(sizeof(*opts));
    opts->decoding = 0;
    opts->encode_func = lookup_encode;
    opts->decode_func = lookup_decode;

    //Command line options
    while ((opt = getopt(argc, argv, "da:")) != -1) {
        switch (opt) {
        case 'd':
            opts->decoding = 1;
            break;
        case 'a':
            for (int x = 0; x < num_algos; x++) {
                if (!strcmp(optarg, algos[x].cmdline_arg)) {
                    opts->encode_func = algos[x].encode_func;
                    opts->decode_func = algos[x].decode_func;
                    break;
                }
            }
        }
    }

    return opts;
}



int main(int argc, char **argv) {
    ssize_t bytes_read = 0, total_bytes_read = 0, num_bytes_write = 0;
    unsigned char *bytes_in, *bytes_out;
    struct cmdline_opts *opts;
    int ret;

    opts = parse_cmdline(argc, argv);


    alloc_decode_lookup_tbl();


    bytes_in = malloc(READ_BUFFER * sizeof(*bytes_in));
    bytes_out = malloc(OUTPUT_BUFFER * sizeof(*bytes_out));

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

        if (opts->decoding) {
            //If decoding we need a multiple of four bytes.
            if (!total_bytes_read & 4) {
                //Move our buffer along and continue reading.
                buffer_pos += bytes_read;
                continue;
            }

            num_bytes_write = ws_decode(bytes_in, bytes_out, total_bytes_read, opts->decode_func);

            if (!num_bytes_write) {
                fprintf(stderr, "- Decoding error, likely invalid byte\n");
                ret = -1;
                goto out;
            }
        } else {
            num_bytes_write = ws_encode(bytes_in, bytes_out, total_bytes_read, opts->encode_func);
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
    free(opts);
    free(bytes_in);
    free(bytes_out);
    return ret;
}


