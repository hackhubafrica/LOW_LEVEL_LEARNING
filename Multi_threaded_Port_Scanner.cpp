#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

std::mutex mtx;

void scan_port(const std::string &ip, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in target;

    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &target.sin_addr);

    if (connect(sock, (struct sockaddr *)&target, sizeof(target)) == 0) {
        std::lock_guard<std::mutex> lock(mtx);
        std::cout << "Port " << port << " is open" << std::endl;
    }
    close(sock);
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
