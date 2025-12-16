#include "../include/sham.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage_server();
        return 1;
    }
    
    int port = atoi(argv[1]);
    int chat_mode = 0;
    float loss_rate = 0.0;
    
    // Parse arguments
    for (int i = 2; i < argc; i++) {
        if (strcmp(argv[i], "--chat") == 0) {
            chat_mode = 1;
        } else {
            loss_rate = atof(argv[i]);
        }
    }
    
    // Initialize logging and random seed
    init_logging("server_log.txt");
    srand(time(NULL));
    
    // Create and bind socket
    int sockfd = sham_socket();
    if (sockfd < 0 || sham_bind(sockfd, port) < 0) {
        return 1;
    }
    
    printf("Server listening on port %d\n", port);
    
    // Accept connection
    sham_connection_t* conn = sham_accept(sockfd);
    if (!conn) {
        close(sockfd);
        return 1;
    }
    
    conn->loss_rate = loss_rate;
    conn->chat_mode = chat_mode;
    
    if (chat_mode) {
        // Chat mode
        printf("Chat mode enabled. Type /quit to exit.\n");
        
        char buffer[MAX_DATA_SIZE];
        fd_set readfds;
        
        while (1) {
            FD_ZERO(&readfds);
            FD_SET(STDIN_FILENO, &readfds);
            FD_SET(conn->socket_fd, &readfds);
            
            int max_fd = (conn->socket_fd > STDIN_FILENO) ? conn->socket_fd : STDIN_FILENO;
            
            if (select(max_fd + 1, &readfds, NULL, NULL, NULL) > 0) {
                if (FD_ISSET(STDIN_FILENO, &readfds)) {
                    // Read from stdin
                    if (fgets(buffer, sizeof(buffer), stdin)) {
                        buffer[strcspn(buffer, "\n")] = 0; // Remove newline
                        
                        if (strcmp(buffer, "/quit") == 0) {
                            break;
                        }
                        
                        sham_send(conn, buffer, strlen(buffer));
                    }
                }
                
                if (FD_ISSET(conn->socket_fd, &readfds)) {
                    // Receive data
                    int received = sham_recv(conn, buffer, sizeof(buffer) - 1);
                    if (received > 0) {
                        buffer[received] = '\0';
                        if (strcmp(buffer, "/quit") == 0) {
                            printf("Client disconnected.\n");
                            break;
                        }
                        printf("Client: %s\n", buffer);
                    }
                }
            }
        }
    } else {
        // File transfer mode
        printf("File transfer mode. Waiting for file...\n");
        
        FILE* output_file = fopen("received_file", "wb");
        if (!output_file) {
            perror("Failed to create output file");
            sham_close(conn);
            sham_cleanup(conn);
            close(sockfd);
            return 1;
        }
        
        char buffer[MAX_DATA_SIZE];
        int total_received = 0;
        
        while (1) {
            int received = sham_recv(conn, buffer, sizeof(buffer));
            if (received <= 0) break;
            
            fwrite(buffer, 1, received, output_file);
            total_received += received;
        }
        
        fclose(output_file);
        printf("File received successfully. Total bytes: %d\n", total_received);
        
        // Calculate and print MD5
        char md5_output[33];
        calculate_md5("received_file", md5_output);
        printf("MD5: %s\n", md5_output);
    }
    
    // Close connection
    sham_close(conn);
    sham_cleanup(conn);
    close(sockfd);
    cleanup_logging();
    
    return 0;
}