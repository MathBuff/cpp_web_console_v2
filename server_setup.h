#ifndef SERVER_SETUP_H
#define SERVER_SETUP_H

#include "posix_socket.h"
#include <netinet/in.h>
#include <iostream>

// ^Purpose: Declares server socket setup function for WebSocket server
// ^Dependencies: posix_socket.h for socket functions, netinet/in.h for sockaddr_in

// Sets up server socket (create, configure, bind, listen)
// ^Purpose: Initializes server socket for single-client WebSocket connections
// ^Input: address - sockaddr_in to store server address (port 8080)
// ^Output: Server socket FD (>=0) or -1 on failure
int setup_server_socket(sockaddr_in& address);

#endif // SERVER_SETUP_H
