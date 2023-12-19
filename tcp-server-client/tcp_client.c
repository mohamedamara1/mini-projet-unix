#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

GtkWidget *username_entry, *password_entry, *service_combobox, *login_button, *service_label;
GtkWidget *login_grid, *service_grid;
GtkWidget *login_window, *service_window;

void send_request(const char *request) {
    int client_socket;
    struct sockaddr_in server_addr;

    // Create socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Send request to the server
    send(client_socket, request, strlen(request), 0);

    // Receive and process the server's response (you need to implement this part)
    // For simplicity, we just print the response here
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        printf("Server response: %s\n", buffer);

        // Check if authentication is successful
        if (strcmp(buffer, "AUTH_SUCCESS") == 0) {
            // Authentication successful, show the services menu
            gtk_widget_show(service_combobox);
            gtk_widget_show(service_label);
        } else {
            // Authentication failed, hide the services menu
            gtk_widget_hide(service_combobox);
            gtk_widget_hide(service_label);
        }
    }

    // Close the client socket
    close(client_socket);
}

void handle_login_response(const char *response) {
    // Check if authentication is successful
    if (strcmp(response, "AUTH_SUCCESS") == 0) {
        // Authentication successful, hide the main window
        printf("auth sucess");
        gtk_widget_hide(login_window);

        gtk_widget_show_all(service_window);
        // Show the service window
    } else {
        // Authentication failed, hide the services menu
        gtk_widget_hide(service_combobox);
        gtk_widget_hide(service_label);
    }
}

void on_login_button_clicked(GtkButton *button, gpointer user_data) {
    const char *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    // Formulate the login request
    char request[BUFFER_SIZE];
    snprintf(request, BUFFER_SIZE, "LOGIN %s %s", username, password);

    // Send the request to the server
    send_request(request);

    // Handle the server response
    handle_login_response("AUTH_SUCCESS");  // For testing, replace with actual response handling
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);


    // Create the main window
    login_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(login_window), "TCP Client");
    gtk_container_set_border_width(GTK_CONTAINER(login_window), 10);
    gtk_widget_set_size_request(login_window, 300, 200);
    g_signal_connect(login_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    // Create the service window
    service_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(service_window), "Services");
    gtk_container_set_border_width(GTK_CONTAINER(service_window), 10);
    gtk_widget_set_size_request(service_window, 300, 200);
    g_signal_connect(service_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);


    // Create a grid for layout
    login_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(login_window), login_grid);

    // Create and add entry widgets for username and password
    username_entry = gtk_entry_new();
    password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE); // Hide password
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Username");
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Password");

    gtk_grid_attach(GTK_GRID(login_grid), username_entry, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(login_grid), password_entry, 0, 1, 1, 1);

    // Create and add a login button
    login_button = gtk_button_new_with_label("Login");
    g_signal_connect(login_button, "clicked", G_CALLBACK(on_login_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(login_grid), login_button, 0, 2, 1, 1);

    // Create and add a label for the services menu
    service_label = gtk_label_new("Select a Servicee:");
    gtk_grid_attach(GTK_GRID(service_grid), service_label, 0, 3, 1, 1);
    gtk_widget_hide(service_label);  // Initially hide the service label

    // Create and add a combo box for service selection
    service_combobox = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(service_combobox), "Service A");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(service_combobox), "Service B");
    // Add more services as needed
    gtk_grid_attach(GTK_GRID(service_grid), service_combobox, 0, 4, 1, 1);
    gtk_widget_hide(service_combobox);  // Initially hide the service combobox

    // Show all widgets
    gtk_widget_show_all(login_window);

    // Start the GTK main loop
    gtk_main();

    return 0;
}
