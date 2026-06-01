#include <iostream>

#include "sockets/socket_manager.h"
#include "parser/parser.h"

int main(int argc, char* argv[]) {

    static const char kExampleRequest[] =
        "GET /index.html?foo=bar HTTP/1.1\r\n"
        "Host: localhost:8080\r\n"
        "User-Agent: cpp-webserver-example/1.0\r\n"
        "Accept: */*\r\n"
        "Connection: close\r\n"
        "\r\n";

    Parser parser(kExampleRequest);
    HTTPRequest req = parser.parse_request();

    std::cout << req << std::endl;

    // if (argc < 2) {
    //     std::cout << "Usage: " << argv[0] << " <port>" << std::endl;
    //     exit(1);
    // }
    
    // int port = atoi(argv[1]);

    // SocketManager socket_manager(port);

    // socket_manager.start();

    
    // std::string cmd;

    // do {
    //     std::cout << "Enter a command [stop]: ";
    //     std::cin >> cmd;
    // } while (cmd != "stop");

    // socket_manager.stop();

    return 0;
}