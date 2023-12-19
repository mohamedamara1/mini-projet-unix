#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_BUFFER_SIZE 1024
#define USERNAME "user"
#define PASSWORD "password"

void sigchld_handler(int signo) {
    // Gestionnaire de signal pour éviter les zombies
    (void)signo;
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

int authenticate(int client_socket) {
    char username[MAX_BUFFER_SIZE];
    char password[MAX_BUFFER_SIZE];

    // Attente du nom d'utilisateur
    recv(client_socket, username, sizeof(username), 0);

    // Attente du mot de passe
    recv(client_socket, password, sizeof(password), 0);

    // Vérification des informations d'identification
    if (strcmp(username, USERNAME) == 0 && strcmp(password, PASSWORD) == 0) {
        send(client_socket, "Authentication successful.", strlen("Authentication successful."), 0);
        return 1;
    } else {
        send(client_socket, "Authentication failed.", strlen("Authentication failed."), 0);
        return 0;
    }
}

void send_date(int client_socket) {
    time_t current_time;
    time(&current_time);

    char date_string[MAX_BUFFER_SIZE];
    strftime(date_string, sizeof(date_string), "%Y-%m-%d %H:%M:%S", localtime(&current_time));

    send(client_socket, date_string, strlen(date_string), 0);
}

void send_file_list(int client_socket, const char *directory_path) {
    DIR *dir = opendir(directory_path);
    if (!dir) {
        perror("Erreur lors de l'ouverture du répertoire");
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        send(client_socket, entry->d_name, strlen(entry->d_name), 0);
        send(client_socket, "\n", 1, 0);
    }

    closedir(dir);
}

void send_file_content(int client_socket, const char *file_path) {
    FILE *file = fopen(file_path, "r");
    if (!file) {
        perror("Erreur lors de l'ouverture du fichier");
        return;
    }

    char buffer[MAX_BUFFER_SIZE];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(client_socket, buffer, bytesRead, 0);
    }

    fclose(file);
}

void send_elapsed_time(int client_socket, time_t start_time) {
    time_t current_time;
    time(&current_time);

    double elapsed_time = difftime(current_time, start_time);

    char time_string[MAX_BUFFER_SIZE];
    snprintf(time_string, sizeof(time_string), "Temps écoulé depuis le début de la connexion : %.2f secondes\n", elapsed_time);

    send(client_socket, time_string, strlen(time_string), 0);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Mise en place du gestionnaire de signal pour éviter les zombies
    signal(SIGCHLD, sigchld_handler);

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Erreur lors de la création de la socket");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(atoi(argv[1]));

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur lors de la liaison de la socket");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 5) == -1) {
        perror("Erreur lors de l'écoute sur la socket");
        exit(EXIT_FAILURE);
    }

    while (1) {
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_socket == -1) {
            perror("Erreur lors de l'acceptation de la connexion");
            continue;
        }

        // Crée un processus fils pour gérer le client
        pid_t child_pid = fork();

        if (child_pid == -1) {
            perror("Erreur lors de la création du processus fils");
            close(client_socket);
            continue;
        }

        if (child_pid == 0) {
            // Code du processus fils
            close(server_socket);  // Le fils n'a pas besoin de la socket du serveur

            time_t start_time;
            time(&start_time);

            int authenticated = authenticate(client_socket);

            if (authenticated) {
                while (1) {
                    int choice;
                    // Attente du choix du client
                    recv(client_socket, &choice, sizeof(choice), 0);

                    switch (choice) {
                        case 1:
                            send_date(client_socket);
                            break;
                        case 2:
                            send_file_list(client_socket, ".");  // Envoie la liste des fichiers du répertoire actuel
                            break;
                        case 3: {
                            char file_name[MAX_BUFFER_SIZE];
                            // Attente du nom du fichier du client
                            recv(client_socket, file_name, sizeof(file_name), 0);
                            send_file_content(client_socket, file_name);
                            break;
                        }
                        case 4:
                            send_elapsed_time(client_socket, start_time);
                            break;
                        case 5:
                            close(client_socket);
                            break;
                        default:
                            break;
                    }

                    if (choice == 5) {
                        break;  // Quitte la boucle si le client choisit "Fin"
                    }
                }
            }

            close(client_socket);  // Ferme la socket du client dans le processus fils
            exit(EXIT_SUCCESS);   // Termine le processus fils
        } else {
            // Code du processus parent
            close(client_socket);  // Le parent n'a pas besoin de la socket du client
        }
    }

    close(server_socket);

    return 0;
}
