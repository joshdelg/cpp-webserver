#include <iostream>
#include <regex>
#include <string>
#include <string_view>

#include "socket_manager.h"


void SocketManager::_accept_loop() {
    if(bind(_primary_socket, (struct sockaddr*) &_server_addr, sizeof(_server_addr)) < 0) {
        std::cout << "Error on bind" << std::endl;
        exit(1);
    }

    listen(_primary_socket, 5);
    
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);

    while(_running) {
        int new_socket_fd = accept(_primary_socket, (struct sockaddr*) &client_addr, &client_len);
        
        if(new_socket_fd < 0) {
            std::cout << "Error on accept" << std::endl;
            continue;
        }

        _connection_threads.emplace_back([this, new_socket_fd]{ 
            this->_handle_connection(new_socket_fd); 
        });
    }
}

void SocketManager::_handle_connection(int fd) {
    char buffer[256];
    int bytes_read = read(fd, buffer, 255);
    buffer[bytes_read] = 0;

    if(bytes_read < 0) {
        std::cout << "Error reading from buffer" << std::endl;
    } else {
        std::cout << "Read " << bytes_read << " bytes" << std::endl;
    }

    std::cout << "Here is the message: " << buffer << std::endl;

    const std::regex r("GET.*\r\nHost: (.*)", std::regex_constants::ECMAScript | std::regex_constants::icase);
    std::cmatch match;

    if(std::regex_search(buffer, match, r)) {
        std::cout << "Request is valid GET" << std::endl;

        std::cout << "Host is: " << match[0] << std::endl;
    } else {
        std::cout << "invalid request" << std::endl;
        close(fd);
        return;
    }

    constexpr std::string_view body =
        "<!DOCTYPE html><html><head><title>Success</title></head><body>"
        "<h1>Hello, the request was successful!</h1></body></html>";

    const std::string resp =
        "HTTP/1.1 200 OK\r\n"
        "Date: Mon, 19 May 2026 21:18:00 GMT\r\n"
        "Server: cpp-webserver\r\n"
        "Content-Type: text/html; charset=UTF-8\r\n"
        "Content-Length: " + std::to_string(body.size()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" + std::string(body);

    bytes_read = write(fd, resp.data(), resp.size());
    if (bytes_read < 0) {
        std::cout << "Error writing to socket :(" << std::endl;
    } else {
        std::cout << "Wrote " << bytes_read << std::endl;
    }

    close(fd);
}

SocketManager::SocketManager(int port) : _port(port) {
    std::cout << "Initializing socket manager..." << std::endl;

    _primary_socket = socket(AF_INET, SOCK_STREAM, 0);
    _server_addr.sin_family = AF_INET;
    _server_addr.sin_addr.s_addr = INADDR_ANY;
    _server_addr.sin_port = htons(_port);
}

SocketManager::~SocketManager() {
    std::cout << "Tearing down socket manager..." << std::endl;

    // This will cause accept() to fail and should exit the loop
    close(_primary_socket);

    // Wait for accept loop thread to terminate
    _listener_thread.join();
    std::cout << "Joined listener thread" << std::endl;

    // Wait for connection threads to terminate
    for(std::thread& t : _connection_threads) {
        t.join();
    }
    std::cout << "Joined connection threads" << std::endl;
}

void SocketManager::start() {
    _running = true;

    std::cout << "Starting socket on port " << this->get_port() << std::endl;

    _listener_thread = std::thread([this]{
        this->_accept_loop();
    });

    std::cout << "Webserver started" << std::endl;
}

void SocketManager::stop() {
    _running = false;
    
    std::cout << "Stopping socket on port " << this->get_port() << std::endl;
}