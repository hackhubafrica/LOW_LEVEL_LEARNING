#include <stdio.h>
#include <winsock2.h>

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

void port_scan(const char *target_ip, int port) {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Failed. Error Code : %d", WSAGetLastError());
        return;
    }

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d", WSAGetLastError());
        return;
    }

    server.sin_addr.s_addr = inet_addr(target_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Port %d is closed\n", port);
    } else {
        printf("Port %d is open\n", port);
    }

    closesocket(sock);
    WSACleanup();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <IP> <PORT>\n", argv[0]);
        return 1;
    }

    const char *target_ip = argv[1];
    int port = atoi(argv[2]);

    port_scan(target_ip, port);

    return 0;
}


//gcc -o portscanner portscanner.c -lws2_32

//portscanner.exe <IP> <PORT>
//portscanner.exe 127.0.0.1 80
