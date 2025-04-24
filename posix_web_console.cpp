#include "posix_web_console.h"
#include "posix_websocket.h"
#include <iostream>
#include <errno.h>

// ^Purpose: Implements console-like functions for WebSocket client interaction 

void printwc(const std::string& message) {
    std::cerr << "printwc: client_socket = " << client_socket << "\n";
    if (client_socket >= 0) {
        send_websocket_frame(client_socket, "<div>" + message + "</div>");
    } else {
        std::cerr << "No client connected for printwc\n";
    }
}

std::string wcin() {
    std::cerr << "wcin: client_socket = " << client_socket << "\n";
    if (client_socket < 0) {
        std::cerr << "No client connected for wcin\n";
        return "";
    }

    // Wait for input
    WebSocketFrame frame;
    while (true) {
        std::cerr << "wcin: Waiting for input on client_socket " << client_socket << "\n";
        if (!read_websocket_frame(client_socket, frame)) {
            std::cerr << "Failed to read client input, errno: " << errno << "\n";
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // Temporary error, retry
                continue;
            }
            // Fatal error or invalid socket, return empty
            std::cerr << "Invalid or closed client socket\n";
            return "";
        }

        if (frame.opcode == 0x8) {
            std::cerr << "Client sent close frame\n";
            close_socket(client_socket);
            client_socket = -1;
            return "";
        }

        if (frame.opcode == 0x1) {
            std::cout << "Received WebSocket message: " << frame.payload << "\n";
            return frame.payload;
        }
        // Other opcodes (e.g., ping/pong), continue waiting
    }

    return ""; // Unreachable
}
