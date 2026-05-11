cmake_minimum_required(VERSION 4.1)
project(webserver)

# Workaround for CLang and Raylib's compound literals
# See: https://github.com/raysan5/raylib/issues/1343
# @joshdelg: Does this prevent using modern C++?
set(CMAKE_CXX_STANDARD 11)

# Dependencies: allow use of FetchContent
include(FetchContent)

# Declare a constant for the version. Make sure every consumer pulls the SAME version
set(RAYLIB_VERSION 6.0) 

# Configure how to search for lib and install if not found
FetchContent_Declare(
    raylib # Our name for the package
    URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    FIND_PACKAGE_ARGS ${RAYLIB_VERSION} EXACT # Enforce exact version match when searching
)

# BUILD_EXAMPLES is a flag exposed by raylib's CMakeLists.txt file. We don't want to use it.
set(BUILD_EXAMPLES OFF CACHE INTERNAL "")

# Mark the lib as configured/available
FetchContent_MakeAvailable(raylib)

# --- My Project ---

# Create a list of the source files our executable needs
set(SOURCE_FILES
    main.cpp
    greeting.cpp)

# Configure an "output" -- executable named PROJECT_NAME built from SOURCE_FILES
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

# Specifies that the output target PROJECT_NAME must be linked with raylib
target_link_libraries(${PROJECT_NAME} raylib)

# Checks if OSX and links appropriate frameworks (Only required on MacOS)
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()