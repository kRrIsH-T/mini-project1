#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>

// Memory utilities
void* safe_malloc(size_t size);
char* safe_strdup(const char* str);

// MD5 utilities
void calculate_md5(const char* filename, char* output);
void md5_to_string(unsigned char* md5, char* output);

// File utilities
int file_exists(const char* filename);
long get_file_size(const char* filename);
int send_file(const char* filename, void* connection);
int receive_file(const char* filename, void* connection);

// Network utilities
int setup_socket(void);
int bind_socket(int sockfd, int port);
void print_usage_server(void);
void print_usage_client(void);

#endif