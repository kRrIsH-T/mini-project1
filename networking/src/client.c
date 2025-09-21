#include "../include/sham.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <server_ip>\n", argv[0]);
        exit(1);
    }

    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SHAM_PORT);
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    SHAMPacket pkt;
    pkt.seq = 1;
    pkt.ack = 0;
    strcpy(pkt.data, "Hello from client");
    pkt.len = strlen(pkt.data);

    sendto(sock, &pkt, sizeof(pkt), 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("Sent packet to %s\n", argv[1]);

    SHAMPacket resp;
    struct sockaddr_in src_addr;
    socklen_t src_len = sizeof(src_addr);

    if (recvfrom(sock, &resp, sizeof(resp), 0, (struct sockaddr *)&src_addr, &src_len) > 0) {
        printf("Received ACK: seq=%d, ack=%d\n", resp.seq, resp.ack);
    }

    close(sock);
    return 0;
}
