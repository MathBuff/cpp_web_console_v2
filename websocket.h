#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include "posix_socket.h"

// ^Purpose: Declares WebSocket functions and struct for handshake and frame handling
// ^Dependencies: sys/socket.h, netinet/in.h, openssl for SHA-1/Base64, posix_socket.h for close_socket

// Compute WebSocket Accept header
std::string compute_websocket_accept(const std::string& key);

// WebSocket frame struct
struct WebSocketFrame {
    bool fin;
    uint8_t opcode;
    bool masked;
    uint64_t payload_length;
    uint8_t masking_key[4];
    std::string payload;
};

// Read a WebSocket frame
bool read_websocket_frame(int client_socket, WebSocketFrame& frame);

// Send a WebSocket frame
void send_websocket_frame(int client_socket, const std::string& message);

// Handle WebSocket upgrade
bool handle_websocket_upgrade(int client_socket, const std::string& request);

// Read file content
std::string read_file(const std::string& filename);

#endif // WEBSOCKET_H
