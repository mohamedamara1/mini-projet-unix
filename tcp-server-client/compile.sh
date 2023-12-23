#!/bin/bash

# Parse command line arguments
while [ "$#" -gt 0 ]; do
  case "$1" in
    -s|--server-source)
      SERVER_SOURCE="$2"
      shift 2
      ;;
    -c|--client-source)
      CLIENT_SOURCE="$2"
      shift 2
      ;;
    *)
      echo "Unknown argument: $1"
      exit 1
      ;;
  esac
done

# Check if necessary arguments are provided
if [ -z "$SERVER_SOURCE" ] || [ -z "$CLIENT_SOURCE" ]; then
  echo "Usage: $0 -s <server_source> -c <client_source>"
  exit 1
fi

# Create build directory
mkdir -p build
cd build

# Run CMake with provided source files
cmake .. -DSERVER_SOURCE="${SERVER_SOURCE}" -DCLIENT_SOURCE="${CLIENT_SOURCE}"

# Build the project
make
