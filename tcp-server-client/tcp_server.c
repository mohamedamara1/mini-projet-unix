#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <time.h>

#define PORT 5020
#define BUFFER_SIZE 1024

#define USER "admin"
#define PASSWORD "password"

bool authenticate_user(const char *username, const char *password) {
    return (strcmp(username, USER) == 0) && (strcmp(password, PASSWORD) == 0);
}

void handle_login(int client_socket, const char *username, const char *password) {
    //if (authenticate_user(username, password)) {
      if (1){
        const char *auth_success_response = "AUTH_SUCCESS";
        send(client_socket, auth_success_response, strlen(auth_success_response), 0);
    } else {
        const char *auth_fail_response = "AUTH_FAIL";
        send(client_socket, auth_fail_response, strlen(auth_fail_response), 0);
    }
}


void handle_send_date(int client_socket) {
    time_t rawtime;
    struct tm *timeinfo;
    char date_response[50];  // Adjust the buffer size as needed

    // Get the current time
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    // Format the date and time as a string
    strftime(date_response, sizeof(date_response), "Current Date: %Y-%m-%d %H:%M:%S", timeinfo);
    printf("printing time%s\n", date_response);
    // Send the response to the client
    send(client_socket, date_response, strlen(date_response), 0);
}
void handle_list_files(int client_socket) {
    // Implement logic for the "LIST_FILES" service
    // This is just a placeholder; replace it with your actual implementation

    const char *files_response = "File1.txt\nFile2.txt\nFile3.txt";
    send(client_socket, files_response, strlen(files_response), 0);
}

void handle_show_file_content(int client_socket, const char *filename) {
    // Implement logic for the "SHOW_FILE_CONTENT" service
    // This is just a placeholder; replace it with your actual implementation

    // Example: Read the content of the file (replace with actual file reading logic)
    const char *file_content = "Content of the file...";
    send(client_socket, file_content, strlen(file_content), 0);
}


void handle_send_session_duration(int client_socket, time_t login_time) {
    // Calculate elapsed seconds since login time
    time_t current_time;
    time(&current_time);
    int elapsed_seconds = (int)difftime(current_time, login_time);

    // Format the session duration response
    char session_duration_response[BUFFER_SIZE];
    snprintf(session_duration_response, BUFFER_SIZE, "Session duration: %02d:%02d:%02d",
             elapsed_seconds / 3600, (elapsed_seconds % 3600) / 60, elapsed_seconds % 60);

    // Send the response to the client
    send(client_socket, session_duration_response, strlen(session_duration_response), 0);
}
void handle_client(int client_socket) {
    // Global variable to store login time
    time_t login_time;

    time(&login_time);
    printf("Login time: %s", ctime(&login_time));

    while (1) {
        char buffer[BUFFER_SIZE];
        ssize_t bytes_received;

        // Receive data from the client
        bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            if (bytes_received < 0) {
                perror("Error receiving data from client");
            }
            break;  // Break the loop on error or client disconnect
        }

        // Process the received data
        buffer[bytes_received] = '\0';
        printf("Received request %s \n", buffer);
        // Check the request type and call the appropriate service handler
        if (strcmp(buffer, "SEND_DATE") == 0) {
            handle_send_date(client_socket);
        } else if (strcmp(buffer, "LIST_FILES") == 0) {
            handle_list_files(client_socket);
        } else if (strncmp(buffer, "SHOW_FILE_CONTENT", strlen("SHOW_FILE_CONTENT")) == 0) {
            // Extract the filename from the request
            const char *filename = buffer + strlen("SHOW_FILE_CONTENT") + 1;
            handle_show_file_content(client_socket, filename);
        } else if (strcmp(buffer, "SESSION_DURATION") == 0) {
            handle_send_session_duration(client_socket, login_time);
        } else if (strncmp(buffer, "LOGIN", strlen("LOGIN")) == 0) {
            // Extract the username and password from the request
            const char *username = strtok(buffer + strlen("LOGIN") + 1, " ");
            const char *password = strtok(NULL, " ");
            handle_login(client_socket, username, password);
        } else {
            // Unknown service request
            const char *unknown_response = "Unknown service request";
            send(client_socket, unknown_response, strlen(unknown_response), 0);
        }
    }

    // Close the client socket outside the loop
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

        printf("Client connected\n");

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

    // Close the server socket (this part is unreachable in the current code)
    close(server_socket);

    return 0;
}
