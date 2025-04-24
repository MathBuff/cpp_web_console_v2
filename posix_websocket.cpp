#include "posix_websocket.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <openssl/buffer.h>
#include <algorithm>
#include <string>
#include "posix_socket.h"
#include "posix_server_setup.h"

// ^Purpose: Implements WebSocket functions for handshake and frames
// ^Dependencies: websocket.h, fstream for read_file

std::string read_file(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool init_websocket_connection() {
    sockaddr_in address; // Address to initialize
    
    // Set up server socket
    server_fd = setup_server_socket(address);
    if (server_fd == -1) {
        return false;
    }

    // Accept first connection
    
    socklen_t addrlen = sizeof(address);
    int first_socket = accept_connection(server_fd, &address, &addrlen);
    if (first_socket < 0) {
        close_socket(server_fd);
        server_fd = -1;
        return false;
    }
    std::cout << "New client connected, FD: " << first_socket << "\n";

    // Receive request
    char buffer[1024] = {0};
    ssize_t bytes_received = receive_data(first_socket, buffer, sizeof(buffer));
    if (bytes_received <= 0) {
        close_socket(first_socket);
        close_socket(server_fd);
        server_fd = -1;
        return false;
    }
    buffer[bytes_received] = '\0';
    std::cout << "Received request:\n" << buffer << "\n";

    // Check if it's a WebSocket upgrade
    std::string request(buffer);
    if (handle_websocket_upgrade(first_socket, request)) {
        client_socket = first_socket;
        return true;
    }

    // Otherwise, assume HTTP and serve index.html
    std::string html = read_file("index.html");
    if (html.empty()) {
        std::cerr << "Failed to load HTML\n";
        html = "<h1>Error: Could not load page</h1>";
    }
    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Content-Length: " + std::to_string(html.length()) + "\r\n"
        "\r\n" + html;
    if (send_data(first_socket, response) < 0) {
        std::cerr << "Failed to send HTML response\n";
    }
    std::cout << "HTML response sent\n";
    close_socket(first_socket);

    // Accept WebSocket connection
    client_socket = accept_connection(server_fd, &address, &addrlen);
    if (client_socket < 0) {
        close_socket(server_fd);
        server_fd = -1;
        return false;
    }
    std::cout << "New client connected (WebSocket), FD: " << client_socket << "\n";

    // Receive WebSocket upgrade request
    bytes_received = receive_data(client_socket, buffer, sizeof(buffer));
    if (bytes_received <= 0) {
        close_socket(client_socket);
        close_socket(server_fd);
        client_socket = -1;
        server_fd = -1;
        return false;
    }
    buffer[bytes_received] = '\0';
    std::cout << "Received request:\n" << buffer << "\n";

    // Handle WebSocket upgrade
    request = buffer;
    if (!handle_websocket_upgrade(client_socket, request)) {
        std::cerr << "Failed to upgrade to WebSocket\n";
        close_socket(client_socket);
        close_socket(server_fd);
        client_socket = -1;
        server_fd = -1;
        return false;
    }

    return true;
}

void close_websocket_connection() {
    if (client_socket >= 0) {
        close_socket(client_socket);
        client_socket = -1;
        std::cout << "WebSocket client socket closed\n";
    }
    if (server_fd >= 0) {
        close_socket(server_fd);
        server_fd = -1;
        std::cout << "Server socket closed\n";
    }
}

bool contains_header_value(const std::string& request, const std::string& header, const std::string& value) {
    std::string request_lower = request;
    std::transform(request_lower.begin(), request_lower.end(), request_lower.begin(), ::tolower);
    std::string header_lower = header;
    std::transform(header_lower.begin(), header_lower.end(), header_lower.begin(), ::tolower);
    std::string value_lower = value;
    std::transform(value_lower.begin(), value_lower.end(), value_lower.begin(), ::tolower);

    std::string header_search = header_lower + ":";
    auto header_pos = request_lower.find(header_search);
    if (header_pos == std::string::npos) {
        return false;
    }

    auto value_start = header_pos + header_search.length();
    auto value_end = request_lower.find("\r\n", value_start);
    if (value_end == std::string::npos) {
        return false;
    }
    std::string header_value = request_lower.substr(value_start, value_end - value_start);

    header_value.erase(0, header_value.find_first_not_of(" \t"));
    header_value.erase(header_value.find_last_not_of(" \t") + 1);

    std::string header_value_lower = header_value;
    std::transform(header_value_lower.begin(), header_value_lower.end(), header_value_lower.begin(), ::tolower);
    return header_value_lower.find(value_lower) != std::string::npos;
}

std::string compute_websocket_accept(const std::string& key) {
    const std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    std::string input = key + magic;
    unsigned char hash[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(input.c_str()), input.length(), hash);
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
    char header[2];
    if (recv(client_socket, header, 2, 0) <= 0) {
        std::cerr << "Failed to read frame header\n";
        return false;
    }
    frame.fin = (header[0] & 0x80) != 0;
    frame.opcode = header[0] & 0x0F;
    frame.masked = (header[1] & 0x80) != 0;
    frame.payload_length = header[1] & 0x7F;
    if (frame.payload_length > 125) {
        std::cerr << "Payload too large\n";
        return false;
    }
    if (frame.masked) {
        if (recv(client_socket, frame.masking_key, 4, 0) <= 0) {
            std::cerr << "Failed to read masking key\n";
            return false;
        }
    }
    frame.payload.resize(frame.payload_length);
    if (frame.payload_length > 0) {
        if (recv(client_socket, &frame.payload[0], frame.payload_length, 0) <= 0) {
            std::cerr << "Failed to read payload\n";
            return false;
        }
        if (frame.masked) {
            for (size_t i = 0; i < frame.payload_length; ++i) {
                frame.payload[i] ^= frame.masking_key[i % 4];
            }
        }
    }
    return true;
}

void send_websocket_frame(int client_socket, const std::string& message) {
    if (message.length() > 125) {
        std::cerr << "Message too large to send\n";
        return;
    }
    std::string frame;
    frame += char(0x81);
    frame += char(message.length());
    frame += message;
    if (send(client_socket, frame.c_str(), frame.length(), 0) < 0) {
        std::cerr << "Failed to send WebSocket frame\n";
    }
}

bool handle_websocket_upgrade(int client_socket, const std::string& request) {
    if (!contains_header_value(request, "Upgrade", "websocket")) {
        std::cerr << "Missing or invalid Upgrade: websocket header\n";
        return false;
    }
    if (!contains_header_value(request, "Connection", "upgrade")) {
        std::cerr << "Missing or invalid Connection: upgrade header\n";
        return false;
    }
    std::string ws_key;
    std::string::size_type key_start = request.find("Sec-WebSocket-Key: ");
    if (key_start == std::string::npos) {
        std::cerr << "No Sec-WebSocket-Key found\n";
        return false;
    }
    key_start += 19;
    std::string::size_type key_end = request.find("\r\n", key_start);
    ws_key = request.substr(key_start, key_end - key_start);
    std::string accept = compute_websocket_accept(ws_key);
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
