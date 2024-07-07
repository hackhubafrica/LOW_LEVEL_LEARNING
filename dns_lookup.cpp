// g++ -o dns_lookup dns_lookup.cpp -lws2_32
// dns_lookup forward example.com
// dns_lookup reverse 93.184.216.34


#include <iostream>
#include <vector>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

// Link with Ws2_32.lib
#pragma comment(lib, "Ws2_32.lib")

void forward_lookup(const std::string &hostname) {
    WSADATA wsa;
    struct addrinfo hints, *results, *p;
    char ipstr[INET6_ADDRSTRLEN];

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
        return;
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // AF_INET or AF_INET6 to force version
    hints.ai_socktype = SOCK_STREAM;

    if (getaddrinfo(hostname.c_str(), NULL, &hints, &results) != 0) {
        std::cerr << "getaddrinfo: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return;
    }

    std::cout << "IP addresses for " << hostname << ":" << std::endl;

    for(p = results; p != NULL; p = p->ai_next) {
        void *addr;
        std::string ipver;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        std::cout << "  " << ipver << ": " << ipstr << std::endl;
    }

    freeaddrinfo(results);
    WSACleanup();
}

void reverse_lookup(const std::string &ip_address) {
    WSADATA wsa;
    struct sockaddr_in sa4;
    struct sockaddr_in6 sa6;
    char host[1024];
    char serv[20];
    int result;

    if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
        std::cerr << "WSAStartup failed with error: " << WSAGetLastError() << std::endl;
        return;
    }

    memset(&sa4, 0, sizeof(sa4));
    memset(&sa6, 0, sizeof(sa6));

    if (inet_pton(AF_INET, ip_address.c_str(), &(sa4.sin_addr)) == 1) {
        sa4.sin_family = AF_INET;
        result = getnameinfo((struct sockaddr*)&sa4, sizeof(sa4), host, sizeof(host), serv, sizeof(serv), 0);
    } else if (inet_pton(AF_INET6, ip_address.c_str(), &(sa6.sin6_addr)) == 1) {
        sa6.sin6_family = AF_INET6;
        result = getnameinfo((struct sockaddr*)&sa6, sizeof(sa6), host, sizeof(host), serv, sizeof(serv), 0);
    } else {
        std::cerr << "Not a valid IP address" << std::endl;
        WSACleanup();
        return;
    }

    if (result != 0) {
        std::cerr << "getnameinfo: " << WSAGetLastError() << std::endl;
    } else {
        std::cout << "Host name for IP address " << ip_address << " is " << host << std::endl;
    }

    WSACleanup();
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <forward/reverse> <hostname/IP>" << std::endl;
        return 1;
    }

    std::string mode = argv[1];
    std::string input = argv[2];

    if (mode == "forward") {
        forward_lookup(input);
    } else if (mode == "reverse") {
        reverse_lookup(input);
    } else {
        std::cerr << "Unknown mode: " << mode << ". Use 'forward' or 'reverse'." << std::endl;
        return 1;
    }

    return 0;
}



