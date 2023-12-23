cmake_minimum_required(VERSION 3.10)

project(TCPProject C)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)

set(CMAKE_C_STANDARD 11)

# Check if source files are provided as command line arguments
if(NOT DEFINED CLIENT_SOURCE)
    message(FATAL_ERROR "Please provide the client source file using -DCLIENT_SOURCE=<client_source>")
endif()

if(NOT DEFINED SERVER_SOURCE)
    message(FATAL_ERROR "Please provide the server source file using -DSERVER_SOURCE=<server_source>")
endif()

# Server executable
add_executable(server ${SERVER_SOURCE})
target_link_libraries(server pthread)

# Client executable
add_executable(client ${CLIENT_SOURCE})
target_link_libraries(client ${GTK3_LIBRARIES})
target_include_directories(client PUBLIC ${GTK3_INCLUDE_DIRS})
target_compile_options(client PUBLIC ${GTK3_CFLAGS_OTHER})
