#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include <vector>

class SocketManager {
private:
    int _port;
    int _primary_socket;
    struct sockaddr_in _server_addr;

    std::thread _listener_thread;
    std::vector<std::thread> _connection_threads;

    bool _running;

    void _accept_loop();
    void _handle_connection(int fd);

public:
    SocketManager(int port);
    ~SocketManager();

    // Google Style Guide: Define accessors inline in header
    int get_port() { return _port; }

    void start();
    void stop();
};