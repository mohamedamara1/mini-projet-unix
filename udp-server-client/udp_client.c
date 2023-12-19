#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h> // Include the time.h header for time-related functions

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define MAX_BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;

    // Initialize the random number generator with the current time as the seed
    srand(time(NULL));

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(PORT);

    // Get a random number to send to the server
    int clientRequest = rand() % 10 + 1; // Send a random number between 1 and 10
    printf("Sending request to server: %d\n", clientRequest);

    // Send the random number to the server
    sendto(sockfd, &clientRequest, sizeof(clientRequest), 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));

    // Receive the array of random numbers from the server
    int responseArray[MAX_BUFFER_SIZE];
    ssize_t receivedBytes = recvfrom(sockfd, responseArray, sizeof(responseArray), 0, NULL, NULL);
    if (receivedBytes < 0) {
        perror("Error receiving data");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Print the received array of random numbers
    printf("Received array of random numbers from server:\n");
    for (int i = 0; i < receivedBytes / sizeof(int); ++i) {
        printf("%d ", responseArray[i]);
    }
    printf("\n");

    close(sockfd);
    return 0;
}
