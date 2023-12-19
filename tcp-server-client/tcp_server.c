#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

void handle_client(int client_socket) {
    // Handle client requests here
    // This is a placeholder, you need to implement the actual logic

    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    // Example: Receive data from the client
    bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received < 0) {
        perror("Error receiving data from client");
        close(client_socket);
        return;
    }

    // Example: Process the received data (you need to implement this part)
    // For simplicity, we just print the received data here
    buffer[bytes_received] = '\0';
    printf("Received from client: %s\n", buffer);

    // Example: Send a response back to the client (you need to implement this part)
    const char *response = "Server received your request.";
    send(client_socket, response, strlen(response), 0);

    // Close the client socket
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    // Create socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error binding socket");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_socket, 10) == -1) {
        perror("Error listening for connections");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server listeninggg on port %d...\n", PORT);

    // Accept and handle incoming connections in parallel
    while (1) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Error accepting connection");
            continue;
        }

        // Fork a child process to handle the client
        if (fork() == 0) {
            // In child process
            close(server_socket);  // Close the server socket in the child
            handle_client(client_socket);
            exit(EXIT_SUCCESS);
        } else {
            // In parent process
            close(client_socket);  // Close the client socket in the parent
        }
    }

    // Close the server socket
    close(server_socket);

    return 0;
}
