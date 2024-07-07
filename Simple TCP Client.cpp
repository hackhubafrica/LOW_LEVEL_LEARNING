#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <IP> <Port>\n";
        return 1;
    }

    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;
    char *ip = argv[1];
    int port = std::stoi(argv[2]);

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        std::cerr << "Failed. Error Code: " << WSAGetLastError() << "\n";
        return 1;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Could not create socket: " << WSAGetLastError() << "\n";
        WSACleanup();
        return 1;
    }

    server.sin_addr.s_addr = inet_addr(ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        std::cerr << "Connection error\n";
        closesocket(sock);
        WSACleanup();
        return 1;
    }

    std::cout << "Connected to server\n";
    const char *message = "GET / HTTP/1.1\r\n\r\n";
    send(sock, message, strlen(message), 0);
    char buffer[1024];
    int bytes_received = recv(sock, buffer, sizeof(buffer), 0);
    if (bytes_received > 0) {
        buffer[bytes_received] = '\0';
        std::cout << buffer << "\n";
    }

    closesocket(sock);
    WSACleanup();
    return 0;
}
