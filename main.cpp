#include <iostream>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

int main() {
// Create socket
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    // ^This captures return and executes TCP socket Creation for IPv4
    if (server_fd == -1) {
        std::cerr << "Socket creation failed\n";
        return 1;
    }
    // ^Logic to probe for socket creation failure
    std::cout << "Socket created, FD: " << server_fd << "\n";

// Configure Socket ("address already in use" error)
    int opt = 1; // Used to pass in argument for setsockopt below
    int setsockopt_check = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    // ^Executes setsockopt and puts return value into setsockopt_check
    // Configures socket to avoid address already in use failure in the case of restart
    if (setsockopt_check < 0) {
        std::cerr << "Setsockopt failed\n";
        close(server_fd);
        return 1;
    }
    // ^Logic to probe for socket configuration failure
    std::cout << "Socket options set\n";

// Bind socket
    // initializing the address and port of the new socket
    sockaddr_in address; // Creates an address structure called address
    address.sin_family = AF_INET; // Sets address family to IPv4
    address.sin_addr.s_addr = INADDR_ANY; // Sets to self hosting ip
    address.sin_port = htons(8080); // Sets to self hosting port
    int check = bind(server_fd, (sockaddr*)&address, sizeof(address));
    // ^calls bind function and receives the return to check bind status
    if (check < 0) {
        std::cerr << "Bind failed\n";
        close(server_fd);
        return 1;
    }
    // ^logic to probe for binding failure
    std::cout << "Socket bound to port 8080\n";

// Listen For connections
    int listen_check = listen(server_fd, 3);
    // ^Calls and returns listen function failure value
    if (listen_check < 0) {
        std::cerr << "Listen failed\n";
        close(server_fd);
        return 1;
    }
    // ^Logic to probe for listening Failure
    std::cout << "Server listening on port 8080\n";

// Accept connections (in a loop to allow multiple connections)
    while (true) {
        int addrlen = sizeof(address);
        int new_socket = accept(server_fd, (sockaddr*)&address, (socklen_t*)&addrlen);
        // ^New_socket receives the file descriptor for the new socket.
        // It does this adjacent to doing the call of accept.
        if (new_socket < 0) {
            std::cerr << "Accept failed\n";
            continue;
        }
        // ^Logic to probe for connection acceptance failure
        std::cout << "New client connected, FD: " << new_socket << "\n";

// Receive client request
        char buffer[1024] = {0}; // Buffer to store received data
        ssize_t bytes_received = recv(new_socket, buffer, sizeof(buffer) - 1, 0);
        // ^Reads data from the client into buffer
        //Also captures the return and executes simultaniously. 
        if (bytes_received < 0) {
            std::cerr << "Receive failed\n";
        } else if (bytes_received == 0) {
            std::cerr << "Client disconnected\n";
        } else {
            buffer[bytes_received] = '\0'; // Null-terminate the string
            std::cout << "Received request:\n" << buffer << "\n";
        }
        // ^Logic to probe for receive failure and print the request

// Sending Information (HTTP response)
        std::string response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Content-Length: 27\r\n"
            "\r\n"
            "<h1>Yo, this is a webpage! </h1>";
        ssize_t send_check = send(new_socket, response.c_str(), response.length(), 0);
        // ^send message function return and execution
        // This message is being sent through a socket created by accept()
        if (send_check < 0) {
            std::cerr << "Send failed\n";
        }
        // ^Logic to probe for sent message failure
        std::cout << "Response sent\n";
        // This is able to send messages to a self-hosted browser if you
        // enter this into the search bar: http://localhost:8080

        // Close client socket
        close(new_socket);
        // ^Closes the client connection after handling the request
        std::cout << "Client socket closed\n";
    }

// Close server socket (unreachable due to infinite loop)
    close(server_fd);
    return 0;
}
