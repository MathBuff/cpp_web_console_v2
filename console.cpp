#include "console.h"
#include <iostream>

// ^Purpose: Implements console-like functions for WebSocket client interaction

static int server_fd = -1;
static int client_socket = -1;

bool init_websocket_connection(sockaddr_in& address) {
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

    // Send prompt
    //send_websocket_frame(client_socket, "<div>Enter input:</div>"); I want it to act like cin kinda

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
