cmake_minimum_required(VERSION 3.10)

project(TCPProject C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

set(CMAKE_C_STANDARD 11)

# Server executable
add_executable(server tcp_server.c)
target_link_libraries(server pthread)

# Client executable
add_executable(client tcp_client.c)
target_link_libraries(client ${GTK3_LIBRARIES})
target_include_directories(client PUBLIC ${GTK3_INCLUDE_DIRS})
target_compile_options(client PUBLIC ${GTK3_CFLAGS_OTHER})
