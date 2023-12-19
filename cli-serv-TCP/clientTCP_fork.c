#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define MAX_BUFFER_SIZE 1024

void display_menu() {
    printf("\n===== Menu des Services =====\n");
    printf("1. Afficher la date du serveur\n");
    printf("2. Afficher la liste des fichiers dans un répertoire\n");
    printf("3. Afficher le contenu d'un fichier\n");
    printf("4. Afficher la durée depuis le début de la connexion\n");
    printf("5. Fin\n");
    printf("=============================\n");
    printf("Entrez le numéro du service souhaité : ");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_address> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Erreur lors de la création de la socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));

    if (inet_pton(AF_INET, argv[1], &server_addr.sin_addr) <= 0) {
        perror("Adresse du serveur invalide");
        exit(EXIT_FAILURE);
    }

    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur lors de la connexion au serveur");
        exit(EXIT_FAILURE);
    }

    char username[MAX_BUFFER_SIZE];
    char password[MAX_BUFFER_SIZE];

    printf("Nom d'utilisateur : ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = '\0';  // Supprime le caractère de nouvelle ligne

    printf("Mot de passe : ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';  // Supprime le caractère de nouvelle ligne

    // Envoie du nom d'utilisateur au serveur
    send(sockfd, username, strlen(username), 0);

    // Envoie du mot de passe au serveur
    send(sockfd, password, strlen(password), 0);

    int authenticated = 0;
    char auth_response[MAX_BUFFER_SIZE];

    // Attente de la réponse d'authentification du serveur
    recv(sockfd, auth_response, sizeof(auth_response), 0);

    printf("%s\n", auth_response);

    if (strcmp(auth_response, "Authentication successful.") == 0) {
        authenticated = 1;
    }

    while (authenticated) {
        display_menu();

        int choice;
        scanf("%d", &choice);

        // Envoie du choix au serveur
        send(sockfd, &choice, sizeof(choice), 0);

        if (choice == 5) {
            break;  // Quitte la boucle si l'utilisateur choisit "Fin"
        }

        // Attente de la réponse du serveur et traitement en fonction du choix
        char response_buffer[MAX_BUFFER_SIZE];
        recv(sockfd, response_buffer, sizeof(response_buffer), 0);

        printf("Réponse du serveur : %s\n", response_buffer);
    }

    close(sockfd);

    return 0;
}
