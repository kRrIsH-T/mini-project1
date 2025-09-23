#include <string.h>
#include <stdio.h>

#define F(x, y, z) ((x & y) | (~x & z))
#define G(x, y, z) ((x & z) | (y & ~z))
#define H(x, y, z) (x ^ y ^ z)
#define I(x, y, z) (y ^ (x | ~z))

void md5_transform(unsigned char *data, unsigned char *hash) {
    unsigned int a = 0x67452301;
    unsigned int b = 0xefcdab89;
    unsigned int c = 0x98badcfe;
    unsigned int d = 0x10325476;

    unsigned int x[16];
    for (int i = 0; i < 16; i++) {
        x[i] = data[i * 4] | (data[i * 4 + 1] << 8) |
               (data[i * 4 + 2] << 16) | (data[i * 4 + 3] << 24);
    }

    unsigned int t = a + F(b, c, d) + x[0];
    a = b;
    b += t;
    c = d;
    d = c;

    for (int i = 0; i < 4; i++) {
        hash[i] = (a >> (i * 8)) & 0xFF;
    }
    for (int i = 0; i < 4; i++) {
        hash[4 + i] = (b >> (i * 8)) & 0xFF;
    }
    for (int i = 0; i < 4; i++) {
        hash[8 + i] = (c >> (i * 8)) & 0xFF;
    }
    for (int i = 0; i < 4; i++) {
        hash[12 + i] = (d >> (i * 8)) & 0xFF;
    }
}

void compute_md5(const char *data, unsigned char *hash) {
    unsigned char padded[64];
    int len = strlen(data);

    memcpy(padded, data, len);
    memset(padded + len, 0, 64 - len);

    md5_transform(padded, hash);
}
