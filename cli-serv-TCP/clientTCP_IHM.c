//gcc -o clientTCP_IHM clientTCP_IHM.c `pkg-config --cflags --libs gtk+-3.0`


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>

#define MAX_BUFFER_SIZE 1024

GtkWidget *window;
GtkWidget *username_entry;
GtkWidget *password_entry;
GtkWidget *status_label;

int client_socket;

void on_button_clicked(GtkWidget *widget, gpointer data) {
    // Récupère le nom d'utilisateur et le mot de passe depuis les champs de l'IHM
    const char *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    // Authentification
    send(client_socket, username, strlen(username), 0);
    send(client_socket, password, strlen(password), 0);

    char auth_response[MAX_BUFFER_SIZE];
    recv(client_socket, auth_response, sizeof(auth_response), 0);

    // Mise à jour de l'étiquette de statut dans l'IHM
    gtk_label_set_text(GTK_LABEL(status_label), auth_response);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <server_address> <server_port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Crée la fenêtre principale
    gtk_init(&argc, &argv);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Client TCP avec IHM");
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Crée des widgets pour l'IHM
    GtkWidget *vbox = gtk_vbox_new(TRUE, 5);
    username_entry = gtk_entry_new();
    password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);  // Masque les caractères du mot de passe
    GtkWidget *button = gtk_button_new_with_label("Se connecter");
    status_label = gtk_label_new("");

    // Connecte le gestionnaire de clic de bouton à la fonction on_button_clicked
    g_signal_connect(button, "clicked", G_CALLBACK(on_button_clicked), NULL);

    // Ajoute les widgets à la boîte verticale
    gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new("Nom d'utilisateur:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), username_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), gtk_label_new("Mot de passe:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), password_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), status_label, FALSE, FALSE, 0);

    // Ajoute la boîte verticale à la fenêtre
    gtk_container_add(GTK_CONTAINER(window), vbox);

    // Affiche tous les widgets
    gtk_widget_show_all(window);

    // Crée la socket du client
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
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

    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erreur lors de la connexion au serveur");
        exit(EXIT_FAILURE);
    }

    // Démarre la boucle principale GTK
    gtk_main();

    close(client_socket);

    return 0;
}
