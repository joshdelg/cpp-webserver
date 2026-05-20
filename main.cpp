#include <iostream>

#include "sockets/socket_manager.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
        exit(1);
    }
    
    int port = atoi(argv[1]);

    SocketManager socket_manager(port);

    socket_manager.start();

    
    std::string cmd;

    do {
        std::cout << "Enter a command [stop]: ";
        std::cin >> cmd;
    } while (cmd != "stop");

    socket_manager.stop();

    return 0;
}