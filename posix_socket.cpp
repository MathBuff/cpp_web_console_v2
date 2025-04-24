#include "posix_socket.h"
#include <iostream>
#include <unistd.h>

// ^Purpose: Implements POSIX socket functions for TCP socket management

int create_socket() {
    // Create IPv4 TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // ^Executes TCP socket creation for IPv4
    if (sockfd == -1) {
        std::cerr << "Socket creation failed\n";
    }
    // ^Logic to probe for socket creation failure
    return sockfd;
}

int configure_socket(int sockfd) {
    // Set SO_REUSEADDR to avoid "address already in use"
    int opt = 1;
    int result = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // ^Configures socket to allow port reuse
    if (result < 0) {
        std::cerr << "Setsockopt failed\n";
    }
    // ^Logic to probe for socket configuration failure
    return result;
}

int bind_socket(int sockfd, sockaddr_in* address) {
    // Initialize IPv4 address
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(8080);
    // Bind socket
    int result = bind(sockfd, (sockaddr*)address, sizeof(*address));
    // ^Binds socket to port 8080 and INADDR_ANY
    if (result < 0) {
        std::cerr << "Bind failed\n";
    }
    // ^Logic to probe for binding failure
    return result;
}

int listen_socket(int sockfd) {
    // Listen with backlog of 3
    int result = listen(sockfd, 3);
    // ^Sets socket to listen for clients
    if (result < 0) {
        std::cerr << "Listen failed\n";
    }
    // ^Logic to probe for listening failure
    return result;
}

int accept_connection(int sockfd, sockaddr_in* address, socklen_t* addrlen) {
    // Accept new client connection
    int new_sockfd = accept(sockfd, (sockaddr*)address, addrlen);
    // ^Creates new socket for client
    if (new_sockfd < 0) {
        std::cerr << "Accept failed\n";
    }
    // ^Logic to probe for accept failure
    return new_sockfd;
}

ssize_t receive_data(int sockfd, char* buffer, size_t buffer_size) {
    // Receive data into buffer
    ssize_t bytes_received = recv(sockfd, buffer, buffer_size - 1, 0);
    // ^Reads data from client
    if (bytes_received < 0) {
        std::cerr << "Receive failed\n";
    } else if (bytes_received == 0) {
        std::cerr << "Client disconnected\n";
    }
    // ^Logic to probe for receive failure
    return bytes_received;
}

ssize_t send_data(int sockfd, const std::string& data) {
    // Send data to client
    ssize_t bytes_sent = send(sockfd, data.c_str(), data.length(), 0);
    // ^Sends data through socket
    if (bytes_sent < 0) {
        std::cerr << "Send failed\n";
    }
    // ^Logic to probe for send failure
    return bytes_sent;
}

void close_socket(int sockfd) {
    // Close socket
    close(sockfd);
    // ^Frees socket resources
}
