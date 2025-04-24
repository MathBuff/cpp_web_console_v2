#ifndef POSIX_SOCKET_H
#define POSIX_SOCKET_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>

// ^Purpose: Declares POSIX socket functions for TCP socket management within posix_soc namespace
// ^Dependencies: sys/socket.h, netinet/in.h for socket operations, string for send_data


// Create a TCP socket
// ^Purpose: Creates a new IPv4 TCP socket
// ^Output: Socket file descriptor (int), or -1 if failed
int create_socket();

// Configure socket to avoid "address already in use"
// ^Purpose: Sets SO_REUSEADDR to allow port reuse
// ^Input: sockfd - Socket file descriptor
// ^Output: 0 if successful, -1 if failed
int configure_socket(int sockfd);

// Bind socket to port and IP
// ^Purpose: Binds socket to IPv4 address (INADDR_ANY) and port 8080
// ^Input: sockfd - Socket file descriptor
// ^Input: address - Pointer to sockaddr_in structure
// ^Output: 0 if successful, -1 if failed
int bind_socket(int sockfd, sockaddr_in* address);

// Listen for incoming connections
// ^Purpose: Sets socket to listen with a backlog of 3
// ^Input: sockfd - Socket file descriptor
// ^Output: 0 if successful, -1 if failed
int listen_socket(int sockfd);

// Receive data from socket
// ^Purpose: Reads data into a buffer from a client socket
// ^Input: sockfd - Client socket file descriptor
// ^Input: buffer - Char array to store data
// ^Input: buffer_size - Maximum bytes to read
// ^Output: Number of bytes received, 0 if disconnected, -1 if failed
ssize_t receive_data(int sockfd, char* buffer, size_t buffer_size);

// Send data over socket
// ^Purpose: Sends data to a client socket
// ^Input: sockfd - Client socket file descriptor
// ^Input: data - String to send
// ^Output: Number of bytes sent, or -1 if failed
ssize_t send_data(int sockfd, const std::string& data);

// Close a socket
// ^Purpose: Closes a socket file descriptor
// ^Input: sockfd - Socket file descriptor
void close_socket(int sockfd);

#endif // POSIX_SOCKET_H
