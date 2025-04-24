#include "server_setup.h"

// ^Purpose: Implements server socket setup function

int setup_server_socket(sockaddr_in& address) {
    // Create socket
    int server_fd = create_socket();
    if (server_fd == -1) {
        std::cerr << "Failed to create socket\n";
        return -1;
    }
    std::cout << "Socket created, FD: " << server_fd << "\n";

    // Configure socket
    if (configure_socket(server_fd) < 0) {
        close_socket(server_fd);
        return -1;
    }
    std::cout << "Socket options set\n";

    // Bind socket
    if (bind_socket(server_fd, &address) < 0) {
        close_socket(server_fd);
        return -1;
    }
    std::cout << "Socket bound to port 8080\n";

    // Listen for connections
    if (listen_socket(server_fd) < 0) {
        close_socket(server_fd);
        return -1;
    }
    std::cout << "Server listening on port 8080\n";

    return server_fd;
}
