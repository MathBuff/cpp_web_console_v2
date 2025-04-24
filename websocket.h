#ifndef WEBSOCKET_H
#define WEBSOCKET_H

#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include "posix_socket.h" // Added for close_socket

// ^Purpose: Declares WebSocket functions and struct for handshake and frame handling
// ^Dependencies: sys/socket.h, netinet/in.h, openssl for SHA-1/Base64, posix_socket.h for close_socket, string for data

// Compute WebSocket Accept header
// ^Purpose: Creates Sec-WebSocket-Accept for handshake
// ^Input: key - Client's Sec-WebSocket-Key
// ^Output: Base64-encoded SHA-1 hash of key + magic string
std::string compute_websocket_accept(const std::string& key);

// WebSocket frame struct
// ^Purpose: Stores WebSocket frame data for parsing/encoding
struct WebSocketFrame {
    bool fin;            // True if final frame
    uint8_t opcode;      // Frame type (0x1=text, 0x8=close)
    bool masked;         // True if payload masked (client-to-server)
    uint64_t payload_length; // Payload length
    uint8_t masking_key[4];  // Masking key
    std::string payload;     // Data (e.g., JSON, HTML)
};

// Read a WebSocket frame
// ^Purpose: Reads/decodes client frame (text/close, length<=125)
// ^Input: client_socket - Socket FD
// ^Input: frame - WebSocketFrame to store parsed frame
// ^Output: True if successful, false if failed/disconnected
bool read_websocket_frame(int client_socket, WebSocketFrame& frame);

// Send a WebSocket frame
// ^Purpose: Sends text frame to client
// ^Input: client_socket - Socket FD
// ^Input: message - Text to send (<=125 bytes)
// ^Output: None, logs error if failed
void send_websocket_frame(int client_socket, const std::string& message);

// Handle WebSocket upgrade
// ^Purpose: Checks for Upgrade, sends handshake response
// ^Input: client_socket - Socket FD
// ^Input: request - HTTP request
// ^Output: True if handshake successful, false if not WebSocket
bool handle_websocket_upgrade(int client_socket, const std::string& request);

// Handle WebSocket connection
// ^Purpose: Manages bidirectional WebSocket communication
// ^Input: client_socket - Socket FD
// ^Output: None, closes socket when done
void handle_websocket(int client_socket);

// Read file content
// ^Purpose: Reads file into string
// ^Input: filename - Path to file
// ^Output: File content, empty if failed
std::string read_file(const std::string& filename);

#endif // WEBSOCKET_H
