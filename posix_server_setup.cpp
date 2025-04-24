#include "posix_server_setup.h"
#include "posix_socket.h"
#include <iostream>
#include <fcntl.h>

// ^Purpose: Implements Linux-specific server socket setup
// ^Dependencies: posix_socket.h for socket functions
int setup_server_socket(sockaddr_in& address) {

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);    
    
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return -1;
    }
    std::cout << "Socket created, FD: " << server_fd << "\n";

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        std::cerr << "Setsockopt failed\n";
        close_socket(server_fd);
        return -1;
    }
    std::cout << "Socket options set\n";

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == -1) {
        std::cerr << "Bind failed\n";
        close_socket(server_fd);
        return -1;
    }
    std::cout << "Socket bound to port 8080\n";

    if (listen(server_fd, 3) == -1) {
        std::cerr << "Listen failed\n";
        close_socket(server_fd);
        return -1;
    }
    std::cout << "Server listening on port 8080\n";

    return server_fd;
}

int accept_connection(int server_fd, sockaddr_in* address, socklen_t* addrlen) {
    int client_socket = accept(server_fd, (struct sockaddr*)address, addrlen);
    if (client_socket == -1) {
        std::cerr << "Accept failed\n";
        return -1;
    }
    return client_socket;

}//posix_soc namspace
