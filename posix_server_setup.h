#ifndef LINUX_SERVER_SETUP_H
#define LINUX_SERVER_SETUP_H

#include <netinet/in.h>

// ^Purpose: Declares Linux-specific server socket setup functions
// ^Dependencies: netinet/in.h for sockaddr_in

// Creates and binds server socket, returns socket FD
int setup_server_socket(sockaddr_in& address);

// Accepts client connection, returns client socket FD
int accept_connection(int server_fd, sockaddr_in* address, socklen_t* addrlen);


#endif // LINUX_SERVER_SETUP_H
