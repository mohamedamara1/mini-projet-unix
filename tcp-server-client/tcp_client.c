#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <gtk/gtk.h>

#define SERVER_IP "127.0.0.1"
#define PORT 5010
#define BUFFER_SIZE 1024

GtkWidget *username_entry, *password_entry, *service_combobox, *login_button, *service_label;
GtkWidget *login_grid, *service_grid;
GtkWidget *login_window, *service_window;

struct ServerConnection {
    int client_socket;
};

// Initialize the connection before using it
struct ServerConnection server_connection;

void initialize_server_connection() {
    server_connection.client_socket = -1; // Set to an invalid value
}

int connect_to_server() {
    // Create socket
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Error creating socket");
        return -1;
    }

    // Set up server address structure
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(PORT);

    // Connect to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error connecting to server");
        close(client_socket);
        return -1;
    }

    return client_socket;
}

void close_connection() {
    if (server_connection.client_socket != -1) {
        printf("Closing connection \n");
        close(server_connection.client_socket);
        server_connection.client_socket = -1;
    }
}


void send_request(struct ServerConnection *connection, const char *request, char *response, size_t response_size) {
    printf("Sending request: %s\n", request);
    send(connection->client_socket, request, strlen(request), 0);

    // Receive and process the server's response
    ssize_t bytes_received = recv(connection->client_socket, response, response_size - 1, 0);
    if (bytes_received > 0) {
        response[bytes_received] = '\0';
        printf("Server response: %s\n", response);
    }

}


// Corrected on_list_files_submit_button_clicked function
void on_list_files_submit_button_clicked(GtkButton *button, gpointer user_data) {
    // Get the directory path from the entry widget
    const char *directory_path = gtk_entry_get_text(GTK_ENTRY(user_data));

    // Formulate the request for the "List Files" service
    char request[BUFFER_SIZE];
    snprintf(request, BUFFER_SIZE, "LIST_FILES %s", directory_path);

    // Allocate a buffer for the response
    char response[BUFFER_SIZE];
    size_t response_size = sizeof(response);

    // Call the send_request function
    send_request(&server_connection, request, response, response_size);

    // Create a text view to display the list of files
    GtkWidget *files_text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(files_text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(files_text_view), GTK_WRAP_WORD);

    // Create a scrolled window to handle scrolling and add the text view
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), files_text_view);

    // Set properties for the scrolled window
    gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 10);
    gtk_widget_set_size_request(scrolled_window, 300, 200);

    // Show all widgets in the scrolled window
    gtk_widget_show_all(scrolled_window);

    // Set the response text to the text view
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(files_text_view)), response, -1);

    // Create a new pop-up window
    GtkWidget *popup_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(popup_window), "List Files");
    gtk_container_set_border_width(GTK_CONTAINER(popup_window), 10);
    gtk_widget_set_size_request(popup_window, 300, 200);

    // Create a grid for layout
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(popup_window), grid);

    // Add the scrolled window to the grid
    gtk_grid_attach(GTK_GRID(grid), scrolled_window, 0, 0, 1, 1);

    // Show all widgets in the pop-up window
    gtk_widget_show_all(popup_window);
}

void handle_login_response(const char *response) {
    // Check if authentication is successful
    if (strcmp(response, "AUTH_SUCCESS") == 0) {
        // Authentication successful, hide the main window
        printf("auth success\n");
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
    // Ensure the server connection is initialized
    initialize_server_connection();

    // Connect to the server
    server_connection.client_socket = connect_to_server();
    if (server_connection.client_socket == -1) {
        // Handle connection error, show message to the user, etc.
        fprintf(stderr, "Failed to connect to the server\n");
        return;
    }

    const char *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(password_entry));

    // Formulate the login request
    char request[BUFFER_SIZE];
    snprintf(request, BUFFER_SIZE, "LOGIN %s %s", username, password);

    // Allocate a buffer for the response
    char response[BUFFER_SIZE];
    size_t response_size = sizeof(response);

    // Call the send_request function
    send_request(&server_connection, request, response, response_size);

    handle_login_response(response);

}



// Modify the on_send_date_button_clicked function
void on_send_date_button_clicked(GtkButton *button, gpointer user_data) {
    // Formulate the request for the "Send Date" service
    const char *request = "SEND_DATE";

    // Allocate a buffer for the response
    char response[BUFFER_SIZE];
    size_t response_size = sizeof(response);

    // Call the send_request function
    send_request(&server_connection, request, response, response_size);

    // Create a new pop-up window
    GtkWidget *popup_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(popup_window), "Send Date");
    gtk_container_set_border_width(GTK_CONTAINER(popup_window), 10);
    gtk_widget_set_size_request(popup_window, 200, 100);

    // Create a label to display the date
    GtkWidget *date_label = gtk_label_new(response);
    gtk_container_add(GTK_CONTAINER(popup_window), date_label);

    // Show all widgets in the pop-up window
    gtk_widget_show_all(popup_window);
}
// Modify the on_list_files_button_clicked function
void on_list_files_button_clicked(GtkButton *button, gpointer user_data) {
    // Create a new pop-up window
    GtkWidget *popup_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(popup_window), "List Files");
    gtk_container_set_border_width(GTK_CONTAINER(popup_window), 10);
    gtk_widget_set_size_request(popup_window, 300, 200);

    // Create a grid for layout
    GtkWidget *grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(popup_window), grid);

    // Create and add an entry widget for the directory path
    GtkWidget *directory_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(directory_entry), "Enter Directory Path");
    gtk_grid_attach(GTK_GRID(grid), directory_entry, 0, 0, 1, 1);

    // Create and add a button to list files
    GtkWidget *list_files_button = gtk_button_new_with_label("List Files");
    g_signal_connect(list_files_button, "clicked", G_CALLBACK(on_list_files_submit_button_clicked), directory_entry);
    gtk_grid_attach(GTK_GRID(grid), list_files_button, 0, 1, 1, 1);

    // Show all widgets in the pop-up window
    gtk_widget_show_all(popup_window);
}


// Function to display file content in a pop-up window
void display_file_content(const char *file_content) {
    // Create a pop-up window
    GtkWidget *popup_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(popup_window), "File Content");
    gtk_container_set_border_width(GTK_CONTAINER(popup_window), 10);
    gtk_widget_set_size_request(popup_window, 400, 300);

    // Create a text view to display the file content
    GtkWidget *file_content_text_view = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(file_content_text_view), FALSE);
    gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(file_content_text_view), GTK_WRAP_WORD);

    // Create a scrolled window to handle scrolling and add the text view
    GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled_window), file_content_text_view);

    // Set properties for the scrolled window
    gtk_container_set_border_width(GTK_CONTAINER(scrolled_window), 10);
    gtk_widget_set_size_request(scrolled_window, 300, 200);

    // Show all widgets in the scrolled window
    gtk_widget_show_all(scrolled_window);

    // Set the file content to the text view
    gtk_text_buffer_set_text(gtk_text_view_get_buffer(GTK_TEXT_VIEW(file_content_text_view)), file_content, -1);

    // Add the scrolled window to the pop-up window
    gtk_container_add(GTK_CONTAINER(popup_window), scrolled_window);

    // Show all widgets in the pop-up window
    gtk_widget_show_all(popup_window);
}

// New function to send a request for file content
void send_file_content_request(const char *filename) {
    // Formulate the request for the "Show File Content" service
    char request[BUFFER_SIZE];
    snprintf(request, BUFFER_SIZE, "SHOW_FILE_CONTENT %s", filename);

    // Allocate a buffer for the response
    char response[BUFFER_SIZE];
    size_t response_size = sizeof(response);

    // Call the send_request function
    send_request(&server_connection, request, response, response_size);

    // Display the file content in a pop-up window
    display_file_content(response);
}

// The on_show_file_content_button_clicked function
void on_show_file_content_button_clicked(GtkButton *button, gpointer user_data) {
    // Get the filename from the user (you can use a GtkEntry for this)
    const char *filename = gtk_entry_get_text(GTK_ENTRY(user_data));

    // Call the new function to send the request for file content
    send_file_content_request(filename);
}


void on_send_session_duration_button_clicked(GtkButton *button, gpointer user_data) {
    // Formulate the request for the "Send Date" service
    const char *request = "SESSION_DURATION";

    // Allocate a buffer for the response
    char response[BUFFER_SIZE];
    size_t response_size = sizeof(response);

    // Call the send_request function
    send_request(&server_connection, request, response, response_size);

    // Create a new pop-up window
    GtkWidget *popup_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(popup_window), "Session Duration");
    gtk_container_set_border_width(GTK_CONTAINER(popup_window), 10);
    gtk_widget_set_size_request(popup_window, 200, 100);

    // Create a label to display the date
    GtkWidget *date_label = gtk_label_new(response);
    gtk_container_add(GTK_CONTAINER(popup_window), date_label);

    // Show all widgets in the pop-up window
    gtk_widget_show_all(popup_window);
}



int main(int argc, char *argv[]) {
    initialize_server_connection();

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

    // Create a grid for layout
    service_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(service_window), service_grid);

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

// Create the service window
    service_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(service_window), "Services");
    gtk_container_set_border_width(GTK_CONTAINER(service_window), 10);
    gtk_widget_set_size_request(service_window, 300, 200);
    g_signal_connect(service_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

// Create a grid for layout
    service_grid = gtk_grid_new();
    gtk_container_add(GTK_CONTAINER(service_window), service_grid);

// Create and add a label for the services menu
    service_label = gtk_label_new("Select a Service:");
    gtk_grid_attach(GTK_GRID(service_grid), service_label, 0, 0, 1, 1);

// ... (existing code)

// Create and add buttons for service selection
    GtkWidget *send_date_button = gtk_button_new_with_label("Send Date");
    g_signal_connect(send_date_button, "clicked", G_CALLBACK(on_send_date_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(service_grid), send_date_button, 0, 1, 1, 1);

    GtkWidget *list_files_button = gtk_button_new_with_label("List Files");
    g_signal_connect(list_files_button, "clicked", G_CALLBACK(on_list_files_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(service_grid), list_files_button, 0, 2, 1, 1);

    GtkWidget *show_file_content_button = gtk_button_new_with_label("Show File Content");
    g_signal_connect(show_file_content_button, "clicked", G_CALLBACK(on_show_file_content_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(service_grid), show_file_content_button, 0, 3, 1, 1);

    GtkWidget *send_session_duration_button = gtk_button_new_with_label("Send Session Duration");
    g_signal_connect(send_session_duration_button, "clicked", G_CALLBACK(on_send_session_duration_button_clicked), NULL);
    gtk_grid_attach(GTK_GRID(service_grid), send_session_duration_button, 0, 4, 1, 1);



    // Show all widgets
    gtk_widget_show_all(login_window);

    // Start the GTK main loop
    gtk_main();

    // Close the connection when the user quits the program
    close_connection();

    return 0;
}
