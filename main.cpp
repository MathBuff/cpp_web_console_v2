#include "posix_socket.h"
#include "websocket.h"
#include <iostream>

// ^Purpose: Runs WebSocket server, using POSIX and WebSocket modules
// ^Orchestrates socket setup, HTTP serving, and WebSocket communication

int main() {
    // Create socket
    int server_fd = create_socket();
    if (server_fd == -1) {
        return 1;
    }
    std::cout << "Socket created, FD: " << server_fd << "\n";

    // Configure socket
    if (configure_socket(server_fd) < 0) {
        close_socket(server_fd);
        return 1;
    }
    std::cout << "Socket options set\n";

    // Bind socket
    sockaddr_in address;
    if (bind_socket(server_fd, &address) < 0) {
        close_socket(server_fd);
        return 1;
    }
    std::cout << "Socket bound to port 8080\n";

    // Listen for connections
    if (listen_socket(server_fd) < 0) {
        close_socket(server_fd);
        return 1;
    }
    std::cout << "Server listening on port 8080\n";

    // Accept connections
    while (true) {
        socklen_t addrlen = sizeof(address);
        int new_socket = accept_connection(server_fd, &address, &addrlen);
        // ^Accepts new client
        if (new_socket < 0) {
            continue;
        }
        std::cout << "New client connected, FD: " << new_socket << "\n";

        // Receive client request
        char buffer[1024] = {0};
        ssize_t bytes_received = receive_data(new_socket, buffer, sizeof(buffer));
        // ^Reads client request
        if (bytes_received <= 0) {
            close_socket(new_socket);
            continue;
        }
        buffer[bytes_received] = '\0';
        std::cout << "Received request:\n" << buffer << "\n";

        // Check for WebSocket upgrade
        std::string request(buffer);
        if (handle_websocket_upgrade(new_socket, request)) {
            // Enter WebSocket mode
            handle_websocket(new_socket);
        } else {
            // Serve HTML
            std::string html = read_file("index.html");
            if (html.empty()) {
                std::cerr << "Failed to load HTML\n";
                html = "<h1>Error: Could not load page</h1>";
            }
            std::string response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html\r\n"
                "Content-Length: " + std::to_string(html.length()) + "\r\n"
                "\r\n" + html;
            if (send_data(new_socket, response) < 0) {
                std::cerr << "Failed to send HTML response\n";
            }
            std::cout << "HTML response sent\n";
            close_socket(new_socket);
            std::cout << "Client socket closed\n";
        }
    }

    // Close server socket (unreachable)
    close_socket(server_fd);
    return 0;
}
