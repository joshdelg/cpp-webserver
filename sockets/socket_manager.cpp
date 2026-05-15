#include <iostream>

#include "socket_manager.h"

SocketManager::SocketManager(int port) : _port(port) {
    std::cout << "Initializing socket manager..." << std::endl;
}

int SocketManager::getPort() {
    return this->_port;
}

void SocketManager::start() {
    std::cout << "Starting socket on port " << this->getPort() << std::endl;
}

void SocketManager::stop() {
    std::cout << "Stopping socket on port " << this->getPort() << std::endl;
}