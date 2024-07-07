#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <winsock2.h>
#include <ws2tcpip.h>

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

std::mutex mtx;

void scan_port(const std::string &ip, int port) {
    WSADATA wsa;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in target;

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
        return;
    }

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Socket creation failed with error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &target.sin_addr);

    if (connect(sock, (struct sockaddr *)&target, sizeof(target)) == 0) {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Port " << port << " is open" << std::endl;
    }

    closesocket(sock);
    WSACleanup();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <IP> <PORTS>" << std::endl;
        return 1;
    }

    std::string ip = argv[1];
    int max_port = std::stoi(argv[2]);

    std::vector<std::thread> threads;

    for (int port = 1; port <= max_port; ++port) {
        threads.emplace_back(scan_port, ip, port);
    }

    for (auto &t : threads) {
        t.join();
    }

    return 0;
}
 //Enter the maximum number of port to scan for a range from 0 to the MAX port