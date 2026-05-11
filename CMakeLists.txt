cmake_minimum_required(VERSION 4.1)
project(webserver)

# Create a list of the source files our executable needs
set(SOURCE_FILES
    main.cpp
    greeting.cpp)

# Configure an "output" -- executable named PROJECT_NAME built from SOURCE_FILES
add_executable(${PROJECT_NAME} ${SOURCE_FILES})