#include <sys/types.h>


unsigned char lookup_encode(const unsigned char);
unsigned char poly_encode(const unsigned char);
unsigned char switch_encode(const unsigned char);

unsigned char lookup_decode(const unsigned char);
unsigned char poly_decode(const unsigned char);
unsigned char switch_decode(const unsigned char);

void alloc_decode_lookup_tbl(void);
ssize_t ws_encode(const unsigned char *, unsigned char *, const ssize_t, unsigned char (*)(unsigned char));
ssize_t ws_decode(const unsigned char *, unsigned char *, const ssize_t, unsigned char (*)(unsigned char));
