#!/bin/bash

# Compile UDP server
gcc udp_server.c -o udp_server -Wall -Werror
if [ $? -ne 0 ]; then
    echo "Error compiling UDP server"
    exit 1
fi

# Compile UDP client
gcc udp_client.c -o udp_client -Wall -Werror
if [ $? -ne 0 ]; then
    echo "Error compiling UDP client"
    exit 1
fi

# Create symbolic links
ln -sf ./udp_server ./server
ln -sf ./udp_client ./client

echo "Build and link successful"
