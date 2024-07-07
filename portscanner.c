#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void port_scan(char *target_ip, int port) {
    int sock;
    struct sockaddr_in server;

    server.sin_addr.s_addr = inet_addr(target_ip);
    server.sin_family = AF_INET;
    server.sin_port = htons(port);

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        printf("Could not create socket\n");
        return;
    }

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        printf("Port %d is closed\n", port);
    } else {
        printf("Port %d is open\n", port);
    }

    close(sock);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <IP> <PORT>\n", argv[0]);
        return 1;
    }

    char *target_ip = argv[1];
    int port = atoi(argv[2]);

    port_scan(target_ip, port);

    return 0;
}





//The error you are seeing occurs because the sys/socket.h header file is not available on Windows systems. 
//The code provided is intended for Unix-like operating systems such as Linux. 
//To make it work on Windows, you need to use the Windows socket API.