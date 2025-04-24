#ifndef CONSOLE_H
#define CONSOLE_H

#include "websocket.h"
#include "server_setup.h"
#include <string>
#include <netinet/in.h>

// ^Purpose: Declares console-like functions for WebSocket client interaction
// ^Dependencies: websocket.h for WebSocket functions, server_setup.h for socket setup

// Initializes WebSocket connection
// ^Purpose: Sets up server and connects to a single client
// ^Input: address - sockaddr_in for server address
// ^Output: True if connected, false if failed
bool init_websocket_connection(sockaddr_in& address);

// Sends a message to the client
// ^Purpose: Displays message in client's #console div
// ^Input: message - Text to send
void printwc(const std::string& message);

// Requests and waits for client input
// ^Purpose: Sends prompt and blocks until client sends input
// ^Output: Client's input as string, empty if failed/disconnected
std::string wcin();

// Closes the WebSocket connection
// ^Purpose: Closes client and server sockets
void close_websocket_connection();

#endif // CONSOLE_H
