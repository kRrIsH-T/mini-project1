#include "../include/sham.h"
#include "../include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <time.h>

int main(int argc, char* argv[]) {
    if (argc < 3) {
        print_usage_client();
        return 1;
    }
    
    char* server_ip = argv[1];
    int server_port = atoi(argv[2]);
    
    int chat_mode = 0;
    char* input_file = NULL;
    char* output_file = NULL;
    float loss_rate = 0.0;
    
    // Parse arguments
    if (argc >= 4 && strcmp(argv[3], "--chat") == 0) {
        chat_mode = 1;
        if (argc >= 5) {
            loss_rate = atof(argv[4]);
        }
    } else if (argc >= 5) {
        // File transfer mode
        input_file = argv[3];
        output_file = argv[4];
        if (argc >= 6) {
            loss_rate = atof(argv[5]);
        }
    } else {
        print_usage_client();
        return 1;
    }
    
    // Initialize logging and random seed
    init_logging("client_log.txt");
    srand(time(NULL));
    
    // Create socket and connect
    int sockfd = sham_socket();
    if (sockfd < 0) {
        return 1;
    }
    
    sham_connection_t* conn = sham_connect(sockfd, server_ip, server_port);
    if (!conn) {
        printf("Failed to connect to server\n");
        close(sockfd);
        return 1;
    }
    
    conn->loss_rate = loss_rate;
    conn->chat_mode = chat_mode;
    
    printf("Connected to server %s:%d\n", server_ip, server_port);
    
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
                            sham_send(conn, buffer, strlen(buffer));
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
                        printf("Server: %s\n", buffer);
                    }
                }
            }
        }
    } else {
        // File transfer mode - send file to server, then receive response file
        FILE* file = fopen(input_file, "rb");
        if (!file) {
            printf("Failed to open input file: %s\n", input_file);
            sham_close(conn);
            sham_cleanup(conn);
            close(sockfd);
            return 1;
        }
        
        printf("Sending file: %s\n", input_file);
        
        char buffer[MAX_DATA_SIZE];
        size_t bytes_read;
        int total_sent = 0;
        
        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            int sent = sham_send(conn, buffer, bytes_read);
            if (sent <= 0) {
                printf("Failed to send data\n");
                break;
            }
            total_sent += sent;
        }
        
        fclose(file);
        printf("File sent successfully. Total bytes: %d\n", total_sent);
        
        // Now receive the processed file back from server
        printf("Receiving processed file as: %s\n", output_file);
        
        FILE* output = fopen(output_file, "wb");
        if (!output) {
            printf("Failed to create output file: %s\n", output_file);
            sham_close(conn);
            sham_cleanup(conn);
            close(sockfd);
            return 1;
        }
        
        int total_received = 0;
        while (1) {
            int received = sham_recv(conn, buffer, sizeof(buffer));
            if (received <= 0) break;
            
            fwrite(buffer, 1, received, output);
            total_received += received;
        }
        
        fclose(output);
        printf("File received successfully. Total bytes: %d\n", total_received);
        
        // Calculate and print MD5 of received file
        char md5_output[33];
        calculate_md5(output_file, md5_output);
        printf("MD5: %s\n", md5_output);
    }
    
    // Close connection
    sham_close(conn);
    sham_cleanup(conn);
    close(sockfd);
    cleanup_logging();
    
    return 0;
}