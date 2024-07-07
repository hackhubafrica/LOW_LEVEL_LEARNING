#include <stdio.h>
#include <string.h>
#include <winsock2.h>
#include <stdlib.h>

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

void port_scan(const char *target_ip, int port) {
    WSADATA wsa;
    SOCKET sock;
    struct sockaddr_in server;

    if (WSAStartup(MAKEWORD(2,2),&wsa) != 0) {
        printf("Failed. Error Code : %d\n", WSAGetLastError());
        return;
    }

    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        printf("Could not create socket : %d\n", WSAGetLastError());
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

void parse_ports(const char *port_str, int **ports, int *num_ports) {
    char *token;
    char *port_str_copy = strdup(port_str);
    char *rest = port_str_copy;

    *num_ports = 0;
    *ports = NULL;

    while ((token = strtok_r(rest, ",", &rest))) {
        if (strchr(token, '-')) {
            int start, end;
            sscanf(token, "%d-%d", &start, &end);
            for (int i = start; i <= end; i++) {
                (*ports) = realloc(*ports, (*num_ports + 1) * sizeof(int));
                (*ports)[*num_ports] = i;
                (*num_ports)++;
            }
        } else {
            (*ports) = realloc(*ports, (*num_ports + 1) * sizeof(int));
            (*ports)[*num_ports] = atoi(token);
            (*num_ports)++;
        }
    }

    free(port_str_copy);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <IP> <PORTS>\n", argv[0]);
        printf("Example: %s 127.0.0.1 22,80,443 or %s 127.0.0.1 1-1000\n", argv[0], argv[0]);
        return 1;
    }

    const char *target_ip = argv[1];
    const char *port_str = argv[2];

    int *ports;
    int num_ports;

    parse_ports(port_str, &ports, &num_ports);

    for (int i = 0; i < num_ports; i++) {
        port_scan(target_ip, ports[i]);
    }

    free(ports);

    return 0;
}
