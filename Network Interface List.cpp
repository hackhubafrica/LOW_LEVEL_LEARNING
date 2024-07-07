#include <pcap.h>
#include <stdio.h>

int main() {
    pcap_if_t *alldevs, *dev;
    char errbuf[PCAP_ERRBUF_SIZE];

    if (pcap_findalldevs(&alldevs, errbuf) == -1) {
        fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
        return 1;
    }

    for (dev = alldevs; dev != NULL; dev = dev->next) {
        printf("Device: %s\n", dev->name);
        if (dev->description)
            printf("Description: %s\n", dev->description);
        else
            printf("No description available\n");
    }

    pcap_freealldevs(alldevs);
    return 0;
}
