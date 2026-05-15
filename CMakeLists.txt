cmake_minimum_required(VERSION 4.1)
project(webserver)

# Configure an "output" -- executable named PROJECT_NAME built from SOURCE_FILES
add_executable(${PROJECT_NAME} main.cpp)

# Link our libraries in
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SocketManager)

add_library(SocketManager)

# Build our libraries
target_sources(SocketManager
    PRIVATE sockets/socket_manager.cpp
    PUBLIC 
        FILE_SET HEADERS
        BASE_DIRS
            sockets
        FILES
            sockets/socket_manager.h
)