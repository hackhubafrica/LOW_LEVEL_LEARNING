#include <iostream>
#include <thread>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

void handle_client(SOCKET client_socket) {
    const char *response = "HTTP/1.1 200 OK\r\n"
                           "Content-Type: text/plain\r\n"
                           "Content-Length: 13\r\n"
                           "\r\n"
                           "Hello, World!";
    send(client_socket, response, strlen(response), 0);
    closesocket(client_socket);
}

int main() {
    WSADATA wsa;
    SOCKET server_socket, client_socket;
    struct sockaddr_in server, client;
    int c;

    std::cout << "Initializing Winsock...\n";
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "Failed. Error Code: " << WSAGetLastError() << "\n";
        return 1;
    }

    std::cout << "Winsock initialized.\n";

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Could not create socket: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    std::cout << "Socket created.\n";

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8080);

    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR) {
        std::cerr << "Bind failed with error code: " << WSAGetLastError() << "\n";
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    std::cout << "Bind done.\n";

    listen(server_socket, 3);

    std::cout << "Waiting for incoming connections...\n";
    c = sizeof(struct sockaddr_in);
    while ((client_socket = accept(server_socket, (struct sockaddr *)&client, &c)) != INVALID_SOCKET) {
        std::cout << "Connection accepted.\n";
        std::thread(handle_client, client_socket).detach();
    }

    if (client_socket == INVALID_SOCKET) {
        std::cerr << "Accept failed with error code: " << WSAGetLastError() << "\n";
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}
