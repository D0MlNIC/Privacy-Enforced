// © Copyright 2023, D0MlNIC, All Rights Reserved.

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "file_operations.h"
#include "Message_Header.h"
#include "Miscallenous.h"
#include "server_receiver.h"
#include "server_sock.h"
#include <string.h>

#define RECEIVE_BUFFER 1024

int main_server_socket();
int create_server_socket();
int bind_server_socket(int socket_fd, int port_num);
int listen_server_socket(int socket_fd, int max_conns);
int accept_server_socket(int socket_fd, struct sockaddr *client_addr);
int close_server_socket(int socket_fd);

int main_server_socket() {
    uint8_t type;
    int server_socket_fd, client_socket_fd, code, port, max_conns, received_data_len, reply_size;
    struct sockaddr client_addr;
    struct info_mes_det command_success;
    char temp_command_buffer[RECEIVE_BUFFER];
    char *command_buffer;
    void *reply;

    port = 5457; // Use Config.ini
    max_conns = 5; // Use Config.ini

    // Creating Server Socket
    server_socket_fd = create_server_socket();
    if (server_socket_fd== 200) {return server_socket_fd;}
        
    code = bind_server_socket(server_socket_fd, port);
    if (code == 201) {close(server_socket_fd); return code;}

    code = listen_server_socket(server_socket_fd, max_conns);
    if (code == 202) {close(server_socket_fd); return code;}

    while (1) {
        client_socket_fd = accept_server_socket(server_socket_fd, &client_addr);
        if (code == 203) {close(server_socket_fd); return code;}

        while((received_data_len = recv(client_socket_fd, temp_command_buffer, sizeof(temp_command_buffer), 0)), received_data_len > 0) {
            // Call command parser
            // Call relevant functions.....
            command_buffer = strip(temp_command_buffer);
            main_server_receiver(command_buffer, RECEIVE_BUFFER, &command_success);
            free(command_buffer);

            // Ask for a resend if the command is not in the correct format.
            if (command_success.resend == 0) {
                reply = header_init(command_success.type, command_success.file_path, command_success.section_num, &command_success, &reply_size);
                send(client_socket_fd, reply, reply_size, 0);
                free(reply);
            } else {
                // Call resend function.
                reply = header_init(255, NULL, 0, &command_success, &reply_size);
                send(client_socket_fd, reply, reply_size, 0);
                free(reply);
            }
            memset(temp_command_buffer, '\0', RECEIVE_BUFFER);
        }
        close(client_socket_fd);
    }
    // Need to return some integer 
    return 1;  
}

int create_server_socket() {
    int server_sock_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock_fd == -1) {
        log_info_message(200, true);
        return 200;
    }
    return server_sock_fd;
}

int bind_server_socket(int socket_fd, int port_num) {
    struct sockaddr_in sock_addr;
    
    // Defining Struct details for socket
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_port = htons(port_num); // Will be getting port from config.ini
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    int bind_success = bind(socket_fd,(struct sockaddr *)&sock_addr, sizeof(sock_addr)); //(struct sockaddr *) was casting sock_addr 2nd param with this.
    if (bind_success == -1) {
        log_info_message(201, true);
        return 201;
    }
    return 0;
}

int listen_server_socket(int socket_fd, int max_conns) {
    int listen_success = listen(socket_fd, max_conns);
    if (listen_success == -1) {
        log_info_message(202, true);
        return 202;
    }
    return 0;
}

int accept_server_socket(int socket_fd, struct sockaddr *client_addr) {
    //struct sockaddr client_addr;
    socklen_t client_size;
    client_size = sizeof(struct sockaddr);
    int accept_success = accept(socket_fd, client_addr, &client_size);
    if (accept_success == -1) {
        log_info_message(203, true);
        return 203;
    }
    return accept_success;
}

int close_server_socket(int socket_fd) {
    int success = close(socket_fd);
    // Need to setup error codes.
    return success;
}