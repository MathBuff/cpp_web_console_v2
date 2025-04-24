#include "websocket.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <openssl/buffer.h> // Added for BUF_MEM definition

// ^Purpose: Implements WebSocket functions for handshake, frames, and file reading

std::string compute_websocket_accept(const std::string& key) {
    const std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string input = key + magic;
    // Compute SHA-1 hash
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);
    // Base64 encode
    BIO* b64 = BIO_new(BIO_f_base64());
    BIO* mem = BIO_new(BIO_s_mem());
    BIO_push(b64, mem);
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    BIO_write(b64, hash, SHA_DIGEST_LENGTH);
    BIO_flush(b64);
    BUF_MEM* bptr;
    BIO_get_mem_ptr(mem, &bptr);
    std::string result(bptr->data, bptr->length);
    BIO_free_all(b64);
    return result;
}

bool read_websocket_frame(int client_socket, WebSocketFrame& frame) {
    // Read 2-byte header
    char header[2];
    if (recv(client_socket, header, 2, 0) <= 0) {
        std::cerr << "Failed to read frame header\n";
        return false;
    }
    // Parse header
    frame.fin = (header[0] & 0x80) != 0;
    frame.opcode = header[0] & 0x0F;
    frame.masked = (header[1] & 0x80) != 0;
    frame.payload_length = header[1] & 0x7F;
    // Only handle small payloads
    if (frame.payload_length > 125) {
        std::cerr << "Payload too large\n";
        return false;
    }
    // Read masking key
    if (frame.masked) {
        if (recv(client_socket, frame.masking_key, 4, 0) <= 0) {
            std::cerr << "Failed to read masking key\n";
            return false;
        }
    }
    // Read payload
    frame.payload.resize(frame.payload_length);
    if (frame.payload_length > 0) {
        if (recv(client_socket, &frame.payload[0], frame.payload_length, 0) <= 0) {
            std::cerr << "Failed to read payload\n";
            return false;
        }
        // Unmask payload
        if (frame.masked) {
            for (size_t i = 0; i < frame.payload_length; ++i) {
                frame.payload[i] ^= frame.masking_key[i % 4];
            }
        }
    }
    return true;
}

void send_websocket_frame(int client_socket, const std::string& message) {
    // Check message length
    if (message.length() > 125) {
        std::cerr << "Message too large to send\n";
        return;
    }
    // Build frame: FIN=1, opcode=0x1, no mask
    std::string frame;
    frame += char(0x81);
    frame += char(message.length());
    frame += message;
    // Send frame
    if (send(client_socket, frame.c_str(), frame.length(), 0) < 0) {
        std::cerr << "Failed to send WebSocket frame\n";
    }
}

bool handle_websocket_upgrade(int client_socket, const std::string& request) {
    // Check for WebSocket upgrade headers
    if (request.find("Upgrade: websocket") == std::string::npos ||
        request.find("Connection: Upgrade") == std::string::npos) {
        return false;
    }
    // Extract Sec-WebSocket-Key
    std::string ws_key;
    std::string::size_type key_start = request.find("Sec-WebSocket-Key: ");
    if (key_start == std::string::npos) {
        std::cerr << "No Sec-WebSocket-Key found\n";
        return false;
    }
    key_start += 19;
    std::string::size_type key_end = request.find("\r\n", key_start);
    ws_key = request.substr(key_start, key_end - key_start);
    // Compute Sec-WebSocket-Accept
    std::string accept = compute_websocket_accept(ws_key);
    // Build and send handshake response
    std::string handshake =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: " + accept + "\r\n"
        "\r\n";
    if (send(client_socket, handshake.c_str(), handshake.length(), 0) < 0) {
        std::cerr << "Failed to send handshake response\n";
        return false;
    }
    std::cout << "WebSocket handshake completed\n";
    return true;
}

void handle_websocket(int client_socket) {
    while (true) {
        WebSocketFrame frame;
        if (!read_websocket_frame(client_socket, frame)) {
            std::cerr << "WebSocket read failed or client disconnected\n";
            break;
        }
        if (frame.opcode == 0x8) {
            std::cerr << "Client sent close frame\n";
            break;
        }
        if (frame.opcode == 0x1) {
            std::cout << "Received WebSocket message: " << frame.payload << "\n";
            // Echo back
            std::string response = "Server received: " + frame.payload;
            send_websocket_frame(client_socket, response);
        }
    }
    close_socket(client_socket); // Use POSIX module's close_socket
    std::cout << "WebSocket client socket closed\n";
}

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}
