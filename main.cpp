#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

void handle_connection(int new_socket_fd) {
    char buffer[256];
    int bytes_read = read(new_socket_fd, buffer, 255);

    if(bytes_read < 0) {
        std::cout << "Error reading from buffer" << std::endl;
    }

    std::cout << "Here is the message: " << buffer << std::endl;

    bytes_read = write(new_socket_fd, "I got your message", 18);
    if (bytes_read < 0) {
        std::cout << "Error writing to socket :(" << std::endl;
    }
}

int main(int argc, char* argv[]) {
    
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        exit(1);
    }
    
    int port = atoi(argv[1]);
    
    std::cout << "Initializing server on port " << port << std::endl;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if(bind(socket_fd, (struct sockaddr*) &server_addr, sizeof(server_addr)) < 0) {
        std::cout << "Error on bind" << std::endl;
        exit(1);
    }

    listen(socket_fd, 5);
    
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while(1) {
        int new_socket_fd = accept(socket_fd, (struct sockaddr*) &client_addr, &client_len);
        if(new_socket_fd < 0) {
            std::cout << "Error on accept" << std::endl;
        }

        // Recall: File descriptors are duplicated, but open file table is shared
        pid_t child_process = fork();

        if(child_process == 0) {
            // Child's code
            close(socket_fd);
            handle_connection(new_socket_fd);
            exit(0);
        } else {
            close(new_socket_fd);
        }
    }

    close(socket_fd);

    return 0;
}