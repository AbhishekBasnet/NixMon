cmake_minimum_required(VERSION 3.10)
project(NixMon VERSION 1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# Include directories
include_directories(include)

# Find necessary packages (Ncurses for the client)
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

# Server Executable
add_executable(nixmon-server src/server/main.c)

# Client Executable
add_executable(nixmon-client src/client/main.c)
target_link_libraries(nixmon-client ${CURSES_LIBRARIES})
