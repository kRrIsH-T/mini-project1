#include "../../include/sham.h"

void send_packet(int sock, struct sockaddr_in *addr, SHAMPacket *pkt) {
    calculate_checksum(pkt);
    sendto(sock, pkt, sizeof(*pkt), 0, (struct sockaddr *)addr, sizeof(*addr));
}

int recv_packet(int sock, struct sockaddr_in *addr, SHAMPacket *pkt) {
    socklen_t addr_len = sizeof(*addr);
    return recvfrom(sock, pkt, sizeof(*pkt), 0, (struct sockaddr *)addr, &addr_len);
}

void calculate_checksum(SHAMPacket *pkt) {
    unsigned int sum = 0;
    unsigned char *ptr = (unsigned char *)pkt;

    for (int i = 0; i < sizeof(*pkt); i++) {
        sum += ptr[i];
    }

    pkt->flags = (sum & 0xFF);
}
