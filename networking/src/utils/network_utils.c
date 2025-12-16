#include "../../include/utils.h"
#include "../../include/sham.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        perror("malloc");
        exit(1);
    }
    return ptr;
}

char* safe_strdup(const char* str) {
    if (!str) return NULL;
    char* copy = safe_malloc(strlen(str) + 1);
    strcpy(copy, str);
    return copy;
}

int should_drop_packet(float loss_rate) {
    if (loss_rate <= 0.0) return 0;
    return (rand() / (float)RAND_MAX) < loss_rate;
}

void print_usage_server(void) {
    printf("Usage: ./server <port> [--chat] [loss_rate]\n");
    printf("  port:      Port number to listen on\n");
    printf("  --chat:    Enable chat mode (optional)\n");
    printf("  loss_rate: Packet loss rate 0.0-1.0 (optional)\n");
}

void print_usage_client(void) {
    printf("File Transfer Mode:\n");
    printf("  ./client <server_ip> <server_port> <input_file> <output_file_name> [loss_rate]\n");
    printf("Chat Mode:\n");
    printf("  ./client <server_ip> <server_port> --chat [loss_rate]\n");
    printf("\n");
    printf("  server_ip:        Server IP address\n");
    printf("  server_port:      Server port number\n");
    printf("  input_file:       File to send (file transfer mode)\n");
    printf("  output_file_name: Name for received file (file transfer mode)\n");
    printf("  --chat:           Enable chat mode\n");
    printf("  loss_rate:        Packet loss rate 0.0-1.0 (optional)\n");
}