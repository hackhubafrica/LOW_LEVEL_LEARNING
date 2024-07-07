#include <iostream>
#include <pcap.h>
#include <chrono>
#include <thread>

void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    long *packet_count = (long *)user_data;
    (*packet_count)++;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <interface>\n";
        return 1;
    }

    char *dev = argv[1];
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);

    if (handle == NULL) {
        std::cerr << "Couldn't open device " << dev << ": " << errbuf << "\n";
        return 2;
    }

    long packet_count = 0;
    std::thread t([&handle, &packet_count]() {
        pcap_loop(handle, 0, packet_handler, (u_char *)&packet_count);
    });

    std::this_thread::sleep_for(std::chrono::seconds(10));
    pcap_breakloop(handle);

    t.join();
    pcap_close(handle);

    std::cout << "Total packets captured in 10 seconds: " << packet_count << "\n";

    return 0;
}
