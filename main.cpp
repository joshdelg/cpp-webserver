#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char* argv[]) {
    std::cout << "Initializing server..." << std::endl;

    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    int port = atoi(argv[1]);

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

    int new_socket_fd = accept(socket_fd, (struct sockaddr*) &client_addr, &client_len);
    if(new_socket_fd < 0) {
        std::cout << "Error on accept" << std::endl;
    }

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

    close(new_socket_fd);
    close(socket_fd);

    return 0;
}