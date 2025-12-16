#ifndef SHAM_H
#define SHAM_H

#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>
#include <sys/time.h> 

// S.H.A.M. Header Structure
struct sham_header {
    uint32_t seq_num;      // Sequence Number
    uint32_t ack_num;      // Acknowledgment Number
    uint16_t flags;        // Control flags (SYN, ACK, FIN)
    uint16_t window_size;  // Flow control window size
};

// Flag definitions
#define SHAM_SYN 0x1  // Synchronize
#define SHAM_ACK 0x2  // Acknowledge
#define SHAM_FIN 0x4  // Finish

// Protocol constants
#define MAX_DATA_SIZE 1024
#define MAX_PACKET_SIZE (sizeof(struct sham_header) + MAX_DATA_SIZE)
#define SLIDING_WINDOW_SIZE 10
#define RTO_MS 500
#define MAX_RETRIES 5
#define BUFFER_SIZE 8192

// Packet structure
typedef struct {
    struct sham_header header;
    char data[MAX_DATA_SIZE];
    size_t data_len;
} sham_packet_t;

// Connection state
typedef enum {
    STATE_CLOSED,
    STATE_LISTEN,
    STATE_SYN_SENT,
    STATE_SYN_RECEIVED,
    STATE_ESTABLISHED,
    STATE_FIN_WAIT_1,
    STATE_FIN_WAIT_2,
    STATE_CLOSE_WAIT,
    STATE_LAST_ACK
} connection_state_t;

// Sliding window entry
typedef struct window_entry {
    sham_packet_t packet;
    struct timeval sent_time;
    int retries;
    int acked;
    struct window_entry* next;
} window_entry_t;

// Connection structure
typedef struct {
    int socket_fd;
    struct sockaddr_in peer_addr;
    socklen_t peer_addr_len;
    
    connection_state_t state;
    uint32_t seq_num;
    uint32_t ack_num;
    uint16_t window_size;
    
    // Sliding window
    window_entry_t* window_head;
    int window_count;
    
    // Receive buffer
    char recv_buffer[BUFFER_SIZE];
    int buffer_used;
    
    // Packet loss simulation
    float loss_rate;
    
    // Chat mode
    int chat_mode;
} sham_connection_t;

// Function declarations
int sham_socket(void);
int sham_bind(int sockfd, int port);
sham_connection_t* sham_accept(int sockfd);
sham_connection_t* sham_connect(int sockfd, const char* server_ip, int server_port);
int sham_send(sham_connection_t* conn, const char* data, size_t len);
int sham_recv(sham_connection_t* conn, char* buffer, size_t len);
int sham_close(sham_connection_t* conn);
void sham_cleanup(sham_connection_t* conn);

// Utility functions
int should_drop_packet(float loss_rate);
void init_logging(const char* filename);
void log_event(const char* format, ...);
void cleanup_logging(void);

#endif