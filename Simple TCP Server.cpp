#include <iostream>
#include <winsock2.h>
#include <thread>
#pragma comment(lib, "ws2_32.lib")

//g++ -std=c++11 -o tcp_server "Simple TCP Server.cpp" -lws2_32

void handle_client(SOCKET client_socket) {
    char buffer[1024];
    int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        std::cout << "Received: " << buffer << "\n";
        const char *response = "Hello from server!";
        send(client_socket, response, strlen(response), 0);
    }
    closesocket(client_socket);
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int c;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "WSAStartup failed. Error Code: " << WSAGetLastError() << "\n";
        return 1;
    }

    // Create a socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Could not create socket. Error Code: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    // Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    // Bind the socket
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error code: " << WSAGetLastError() << "\n";
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Listen on the socket
    if (listen(server_socket, 3) == SOCKET_ERROR) {
        std::cerr << "Listen failed with error code: " << WSAGetLastError() << "\n";
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Server listening on port 8080\n";

    // Accept and handle incoming connections
    c = sizeof(struct sockaddr_in);
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client, &c)) != INVALID_SOCKET) {
        std::cout << "Connection accepted\n";
        std::thread(handle_client, client_socket).detach();
    }

    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Accept failed with error code: " << WSAGetLastError() << "\n";
    }

    // Cleanup
    closesocket(server_socket);
    WSACleanup();
    return 0;
}
