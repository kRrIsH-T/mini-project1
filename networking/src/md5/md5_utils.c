#include "../../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/md5.h>

void calculate_md5(const char* filename, char* output) {
    FILE* file = fopen(filename, "rb");
    if (!file) {
        strcpy(output, "error");
        return;
    }
    
    MD5_CTX md5_ctx;
    MD5_Init(&md5_ctx);
    
    char buffer[1024];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        MD5_Update(&md5_ctx, buffer, bytes);
    }
    
    unsigned char md5[MD5_DIGEST_LENGTH];
    MD5_Final(md5, &md5_ctx);
    
    fclose(file);
    
    md5_to_string(md5, output);
}

void md5_to_string(unsigned char* md5, char* output) {
    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(output + (i * 2), "%02x", md5[i]);
    }
    output[32] = '\0';
}