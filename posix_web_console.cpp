#include "posix_web_console.h"
#include "posix_websocket.h"
#include <iostream>

// ^Purpose: Implements console-like functions for WebSocket client interaction 

void printwc(const std::string& message) {
    if (client_socket >= 0) {
        send_websocket_frame(client_socket, "<div>" + message + "</div>");
    } else {
        std::cerr << "No client connected for printwc\n";
    }
}

std::string wcin() {
    if (client_socket < 0) {
        std::cerr << "No client connected for wcin\n";
        return "";
    }

    // Send prompt (commented out to mimic cin behavior)
    //send_websocket_frame(client_socket, "<div>Enter input:</div>");

    // Wait for input
    WebSocketFrame frame;
    if (!read_websocket_frame(client_socket, frame)) {
        std::cerr << "Failed to read client input\n";
        close_socket(client_socket);
        close_socket(server_fd);
        client_socket = -1;
        server_fd = -1;
        return "";
    }

    if (frame.opcode == 0x8) {
        std::cerr << "Client sent close frame\n";
        close_socket(client_socket);
        close_socket(server_fd);
        client_socket = -1;
        server_fd = -1;
        return "";
    }

    if (frame.opcode == 0x1) {
        std::cout << "Received WebSocket message: " << frame.payload << "\n";
        return frame.payload;
    }

    return "";
}
