#include <iostream>
#include <winsock2.h>
#include <iphlpapi.h>
#include <icmpapi.h>
#include <chrono>
#include <thread>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "iphlpapi.lib")

void ping(const std::string &ip) {
    HANDLE hIcmpFile;
    unsigned long ipAddr = inet_addr(ip.c_str());
    char sendData[32] = "Data Buffer";
    LPVOID replyBuffer;
    DWORD replySize;

    hIcmpFile = IcmpCreateFile();
    if (hIcmpFile == INVALID_HANDLE_VALUE) {
        std::cerr << "Unable to open handle.\n";
        return;
    }

    replySize = sizeof(ICMP_ECHO_REPLY) + sizeof(sendData);
    replyBuffer = (VOID *)malloc(replySize);
    if (replyBuffer == NULL) {
        std::cerr << "Unable to allocate memory.\n";
        IcmpCloseHandle(hIcmpFile);
        return;
    }

    while (true) {
        DWORD dwRetVal = IcmpSendEcho(hIcmpFile, ipAddr, sendData, sizeof(sendData), NULL, replyBuffer, replySize, 1000);
        if (dwRetVal != 0) {
            PICMP_ECHO_REPLY pEchoReply = (PICMP_ECHO_REPLY)replyBuffer;
            struct in_addr addr;
            addr.S_un.S_addr = pEchoReply->Address;
            std::cout << "Received " << dwRetVal << " ICMP response(s) from " << inet_ntoa(addr) << ".\n";
        } else {
            std::cerr << "IcmpSendEcho failed: " << GetLastError() << "\n";
        }

        // Sleep for 1 second before sending the next packet
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    free(replyBuffer);
    IcmpCloseHandle(hIcmpFile);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <IP>\n";
        return 1;
    }

    std::string ip = argv[1];
    ping(ip);
    return 0;
}
