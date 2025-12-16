#include "../../include/sham.h"
#include "../../include/utils.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/select.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdio.h>  // Add this for perror

static uint32_t initial_seq_num = 1000;

int sham_socket(void) {
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        return -1;
    }
    return sockfd;
}

int sham_bind(int sockfd, int port) {
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return -1;
    }
    return 0;
}

static sham_packet_t create_packet(uint32_t seq, uint32_t ack, uint16_t flags, uint16_t window) {
    sham_packet_t packet;
    memset(&packet, 0, sizeof(packet));
    packet.header.seq_num = htonl(seq);
    packet.header.ack_num = htonl(ack);
    packet.header.flags = htons(flags);
    packet.header.window_size = htons(window);
    packet.data_len = 0;
    return packet;
}

static int send_packet(sham_connection_t* conn, const sham_packet_t* packet) {
    // Simulate packet loss
    if (should_drop_packet(conn->loss_rate)) {
        if (packet->data_len > 0) {
            log_event("DROP DATA SEQ=%u", ntohl(packet->header.seq_num));
        }
        return sizeof(struct sham_header) + packet->data_len; // Pretend sent
    }
    
    size_t packet_size = sizeof(struct sham_header) + packet->data_len;
    ssize_t sent = sendto(conn->socket_fd, packet, packet_size, 0,
                         (struct sockaddr*)&conn->peer_addr, conn->peer_addr_len);
    
    if (sent < 0) {
        perror("sendto");
        return -1;
    }
    
    return sent;
}

static int recv_packet(sham_connection_t* conn, sham_packet_t* packet, int timeout_ms) {
    fd_set readfds;
    struct timeval timeout;
    struct timeval *timeout_ptr;
    
    FD_ZERO(&readfds);
    FD_SET(conn->socket_fd, &readfds);
    
    if (timeout_ms < 0) {
        // Block indefinitely
        timeout_ptr = NULL;
    } else {
        timeout.tv_sec = timeout_ms / 1000;
        timeout.tv_usec = (timeout_ms % 1000) * 1000;
        timeout_ptr = &timeout;
    }
    
    int ready = select(conn->socket_fd + 1, &readfds, NULL, NULL, timeout_ptr);
    if (ready <= 0) {
        return ready; // Timeout or error
    }
    
    socklen_t addr_len = sizeof(conn->peer_addr);
    ssize_t received = recvfrom(conn->socket_fd, packet, MAX_PACKET_SIZE, 0,
                               (struct sockaddr*)&conn->peer_addr, &addr_len);
    
    if (received < 0) {
        perror("recvfrom");
        return -1;
    }
    
    packet->data_len = received - sizeof(struct sham_header);
    return received;
}

sham_connection_t* sham_accept(int sockfd) {
    sham_connection_t* conn = safe_malloc(sizeof(sham_connection_t));
    memset(conn, 0, sizeof(sham_connection_t));
    
    conn->socket_fd = sockfd;
    conn->state = STATE_LISTEN;
    conn->seq_num = initial_seq_num++;
    conn->window_size = BUFFER_SIZE;
    conn->peer_addr_len = sizeof(conn->peer_addr);
    
    // Wait for SYN
    sham_packet_t syn_packet;
    log_event("Waiting for SYN...");
    
    while (1) {
        int result = recv_packet(conn, &syn_packet, -1); // Block indefinitely
        if (result <= 0) continue;
        
        uint16_t flags = ntohs(syn_packet.header.flags);
        if (flags & SHAM_SYN) {
            uint32_t client_seq = ntohl(syn_packet.header.seq_num);
            log_event("RCV SYN SEQ=%u", client_seq);
            
            conn->ack_num = client_seq + 1;
            conn->state = STATE_SYN_RECEIVED;
            break;
        }
    }
    
    // Send SYN-ACK
    sham_packet_t synack_packet = create_packet(conn->seq_num, conn->ack_num, 
                                               SHAM_SYN | SHAM_ACK, conn->window_size);
    send_packet(conn, &synack_packet);
    log_event("SND SYN-ACK SEQ=%u ACK=%u", conn->seq_num, conn->ack_num);
    
    // Wait for ACK
    sham_packet_t ack_packet;
    while (1) {
        int result = recv_packet(conn, &ack_packet, RTO_MS);
        if (result <= 0) {
            // Retransmit SYN-ACK
            send_packet(conn, &synack_packet);
            log_event("RETX SYN-ACK SEQ=%u ACK=%u", conn->seq_num, conn->ack_num);
            continue;
        }
        
        uint16_t flags = ntohs(ack_packet.header.flags);
        if (flags & SHAM_ACK) {
            log_event("RCV ACK FOR SYN");
            conn->state = STATE_ESTABLISHED;
            conn->seq_num++;
            break;
        }
    }
    
    return conn;
}

sham_connection_t* sham_connect(int sockfd, const char* server_ip, int server_port) {
    sham_connection_t* conn = safe_malloc(sizeof(sham_connection_t));
    memset(conn, 0, sizeof(sham_connection_t));
    
    conn->socket_fd = sockfd;
    conn->state = STATE_CLOSED;
    conn->seq_num = initial_seq_num++;
    conn->window_size = BUFFER_SIZE;
    
    // Setup server address
    memset(&conn->peer_addr, 0, sizeof(conn->peer_addr));
    conn->peer_addr.sin_family = AF_INET;
    conn->peer_addr.sin_port = htons(server_port);
    inet_pton(AF_INET, server_ip, &conn->peer_addr.sin_addr);
    conn->peer_addr_len = sizeof(conn->peer_addr);
    
    // Send SYN
    sham_packet_t syn_packet = create_packet(conn->seq_num, 0, SHAM_SYN, conn->window_size);
    send_packet(conn, &syn_packet);
    log_event("SND SYN SEQ=%u", conn->seq_num);
    conn->state = STATE_SYN_SENT;
    
    // Wait for SYN-ACK
    sham_packet_t synack_packet;
    int retries = 0;
    while (retries < MAX_RETRIES) {
        int result = recv_packet(conn, &synack_packet, RTO_MS);
        if (result <= 0) {
            // Timeout, retransmit SYN
            send_packet(conn, &syn_packet);
            log_event("TIMEOUT SEQ=%u", conn->seq_num);
            log_event("RETX SYN SEQ=%u", conn->seq_num);
            retries++;
            continue;
        }
        
        uint16_t flags = ntohs(synack_packet.header.flags);
        if ((flags & (SHAM_SYN | SHAM_ACK)) == (SHAM_SYN | SHAM_ACK)) {
            uint32_t server_seq = ntohl(synack_packet.header.seq_num);
            log_event("RCV SYN-ACK SEQ=%u ACK=%u", server_seq, ntohl(synack_packet.header.ack_num));
            
            conn->ack_num = server_seq + 1;
            conn->seq_num++;
            break;
        }
        retries++;
    }
    
    if (retries >= MAX_RETRIES) {
        free(conn);
        return NULL;
    }
    
    // Send ACK
    sham_packet_t ack_packet = create_packet(conn->seq_num, conn->ack_num, SHAM_ACK, conn->window_size);
    send_packet(conn, &ack_packet);
    log_event("SND ACK=ACK FOR SYN");
    
    conn->state = STATE_ESTABLISHED;
    return conn;
}

int sham_send(sham_connection_t* conn, const char* data, size_t len) {
    if (conn->state != STATE_ESTABLISHED) {
        return -1;
    }
    
    size_t sent = 0;
    while (sent < len) {
        // Wait if window is full
        while (conn->window_count >= SLIDING_WINDOW_SIZE) {
            usleep(1000); // 1ms
            // TODO: Handle ACKs to clear window
        }
        
        size_t chunk_size = (len - sent < MAX_DATA_SIZE) ? len - sent : MAX_DATA_SIZE;
        
        sham_packet_t packet = create_packet(conn->seq_num, conn->ack_num, 0, conn->window_size);
        memcpy(packet.data, data + sent, chunk_size);
        packet.data_len = chunk_size;
        
        send_packet(conn, &packet);
        log_event("SND DATA SEQ=%u LEN=%zu", conn->seq_num, chunk_size);
        
        // Add to sliding window
        window_entry_t* entry = safe_malloc(sizeof(window_entry_t));
        entry->packet = packet;
        gettimeofday(&entry->sent_time, NULL);
        entry->retries = 0;
        entry->acked = 0;
        entry->next = conn->window_head;
        conn->window_head = entry;
        conn->window_count++;
        
        conn->seq_num += chunk_size;
        sent += chunk_size;
    }
    
    return sent;
}

int sham_recv(sham_connection_t* conn, char* buffer, size_t len) {
    if (conn->state != STATE_ESTABLISHED) {
        return -1;
    }
    
    sham_packet_t packet;
    int result = recv_packet(conn, &packet, RTO_MS);
    if (result <= 0) {
        return result;
    }
    
    if (packet.data_len > 0) {
        log_event("RCV DATA SEQ=%u LEN=%zu", ntohl(packet.header.seq_num), packet.data_len);
        
        // Send ACK
        conn->ack_num = ntohl(packet.header.seq_num) + packet.data_len;
        sham_packet_t ack_packet = create_packet(conn->seq_num, conn->ack_num, SHAM_ACK, conn->window_size);
        send_packet(conn, &ack_packet);
        log_event("SND ACK=%u WIN=%u", conn->ack_num, conn->window_size);
        
        size_t copy_len = (packet.data_len < len) ? packet.data_len : len;
        memcpy(buffer, packet.data, copy_len);
        return copy_len;
    }
    
    return 0;
}

int sham_close(sham_connection_t* conn) {
    if (conn->state == STATE_ESTABLISHED) {
        // Send FIN
        sham_packet_t fin_packet = create_packet(conn->seq_num, conn->ack_num, SHAM_FIN, conn->window_size);
        send_packet(conn, &fin_packet);
        log_event("SND FIN SEQ=%u", conn->seq_num);
        
        conn->state = STATE_FIN_WAIT_1;
        conn->seq_num++;
        
        // Wait for ACK and FIN
        sham_packet_t response;
        int ack_received = 0, fin_received = 0;
        
        while (!ack_received || !fin_received) {
            int result = recv_packet(conn, &response, RTO_MS);
            if (result <= 0) continue;
            
            uint16_t flags = ntohs(response.header.flags);
            
            if ((flags & SHAM_ACK) && !ack_received) {
                log_event("RCV ACK FOR FIN");
                ack_received = 1;
                conn->state = STATE_FIN_WAIT_2;
            }
            
            if ((flags & SHAM_FIN) && !fin_received) {
                log_event("RCV FIN SEQ=%u", ntohl(response.header.seq_num));
                fin_received = 1;
                
                // Send final ACK
                conn->ack_num = ntohl(response.header.seq_num) + 1;
                sham_packet_t final_ack = create_packet(conn->seq_num, conn->ack_num, SHAM_ACK, conn->window_size);
                send_packet(conn, &final_ack);
                log_event("SND ACK FOR FIN");
            }
        }
    }
    
    conn->state = STATE_CLOSED;
    return 0;
}

void sham_cleanup(sham_connection_t* conn) {
    if (!conn) return;
    
    // Free sliding window
    window_entry_t* current = conn->window_head;
    while (current) {
        window_entry_t* next = current->next;
        free(current);
        current = next;
    }
    
    free(conn);
}