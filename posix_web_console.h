#ifndef CONSOLE_H
#define CONSOLE_H

#include "posix_websocket.h"
#include <string>

// ^Purpose: Declares console-like functions for WebSocket client interaction
// ^Dependencies: websocket.h for WebSocket functions, os_network_interface.h for network abstraction


void printwc(const std::string& message);
std::string wcin();


#endif // CONSOLE_H
