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
extern int server_fd;
extern int client_socket;

bool init_websocket_connection();
void close_websocket_connection();

std::string compute_websocket_accept(const std::string& key);

struct WebSocketFrame {
    bool fin;
    uint8_t opcode;
    bool masked;
    uint64_t payload_length;
    uint8_t masking_key[4];
    std::string payload;
};

bool read_websocket_frame(int client_socket, WebSocketFrame& frame);
void send_websocket_frame(int client_socket, const std::string& message);
bool handle_websocket_upgrade(int client_socket, const std::string& request);

#endif // WEBSOCKET_H
