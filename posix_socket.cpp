#include "posix_socket.h"
#include <iostream>
#include <unistd.h>

// ^Purpose: Implements POSIX socket functions for TCP socket management within posix_soc namespace

int create_socket() {
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        std::cerr << "Socket creation failed\n";
    }
    return sockfd;
}

int configure_socket(int sockfd) {
    int opt = 1;
    int result = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    if (result < 0) {
        std::cerr << "Setsockopt failed\n";
    }
    return result;
}

int bind_socket(int sockfd, sockaddr_in* address) {
    address->sin_family = AF_INET;
    address->sin_addr.s_addr = INADDR_ANY;
    address->sin_port = htons(8080);
    int result = bind(sockfd, (sockaddr*)address, sizeof(*address));
    if (result < 0) {
        std::cerr << "Bind failed\n";
    }
    return result;
}

int listen_socket(int sockfd) {
    int result = listen(sockfd, 3);
    if (result < 0) {
        std::cerr << "Listen failed\n";
    }
    return result;
}

ssize_t receive_data(int sockfd, char* buffer, size_t buffer_size) {
    ssize_t bytes_received = recv(sockfd, buffer, buffer_size - 1, 0);
    if (bytes_received < 0) {
        std::cerr << "Receive failed\n";
    } else if (bytes_received == 0) {
        std::cerr << "Client disconnected\n";
    }
    return bytes_received;
}

ssize_t send_data(int sockfd, const std::string& data) {
    ssize_t bytes_sent = send(sockfd, data.c_str(), data.length(), 0);
    if (bytes_sent < 0) {
        std::cerr << "Send failed\n";
    }
    return bytes_sent;
}

void close_socket(int sockfd) {
    close(sockfd);
}
