#include "../include/sham.h"

int main(int argc, char *argv[]) {
    int sock;
    struct sockaddr_in addr, client_addr;
    socklen_t addr_len;
    SHAMPacket pkt;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        exit(1);
    }

    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SHAM_PORT);

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        exit(1);
    }

    printf("Server listening on port %d\n", SHAM_PORT);

    while (1) {
        addr_len = sizeof(client_addr);
        int n = recvfrom(sock, &pkt, sizeof(pkt), 0, (struct sockaddr *)&client_addr, &addr_len);

        if (n > 0) {
            printf("Received from %s: %s\n", inet_ntoa(client_addr.sin_addr), pkt.data);

            SHAMPacket ack;
            ack.seq = pkt.ack;
            ack.ack = pkt.seq;
            ack.len = 0;

            sendto(sock, &ack, sizeof(ack), 0, (struct sockaddr *)&client_addr, addr_len);
        }
    }

    close(sock);
    return 0;
}
