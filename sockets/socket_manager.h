#pragma once

class SocketManager {
private:
    int _port;

public:
    SocketManager(int port);

    // Getters and setters
    int getPort();

    void start();
    void stop();
};