#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024

void generateRandomNumbers(int count, int* numbers) {
    srand(time(NULL));
    for (int i = 0; i < count; ++i) {
        numbers[i] = rand() % 100; // Generate random numbers between 0 and 99
    }
}

int main() {
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    int clientRequest;  // Updated: variable to store the client's request

    // Create socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    // Bind socket
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Error binding socket");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP server is listening onnn port %d...\n", PORT);

    while (1) {
        // Receive data from client
        ssize_t receivedBytes = recvfrom(sockfd, &clientRequest, sizeof(clientRequest), 0, (struct sockaddr*)&clientAddr, &addrLen);
        if (receivedBytes < 0) {
            perror("Error receiving data");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        printf("Received request from %s:%d: %d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), clientRequest);

        // Generate random numbers
        int responseSize = clientRequest; // Size of the array is the number sent by the client
        int responseArray[responseSize];
        generateRandomNumbers(responseSize, responseArray);

        // Send the array of random numbers back to the client
        sendto(sockfd, responseArray, sizeof(responseArray), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
    }

}
