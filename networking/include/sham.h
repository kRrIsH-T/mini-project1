#ifndef SHAM_H
#define SHAM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#define SHAM_PORT 8080
#define WINDOW_SIZE 5
#define TIMEOUT 2

typedef struct {
    unsigned short seq;
    unsigned short ack;
    unsigned short len;
    unsigned char flags;
    char data[1024];
} SHAMPacket;

void send_packet(int sock, struct sockaddr_in *addr, SHAMPacket *pkt);
int recv_packet(int sock, struct sockaddr_in *addr, SHAMPacket *pkt);
void calculate_checksum(SHAMPacket *pkt);

#endif
