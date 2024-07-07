#include <pcap.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>

void packet_handler(u_char *user_data, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
    struct ether_header *eth_header;
    eth_header = (struct ether_header *) packet;

    if (ntohs(eth_header->ether_type) == ETHERTYPE_ARP) {
        struct ether_arp *arp_packet;
        arp_packet = (struct ether_arp *)(packet + sizeof(struct ether_header));

        printf("ARP packet from %s ", inet_ntoa(*(struct in_addr *)&arp_packet->arp_spa));
        printf("is trying to claim it has IP address %s\n", inet_ntoa(*(struct in_addr *)&arp_packet->arp_tpa));
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <interface>\n", argv[0]);
        return 1;
    }

    char *dev = argv[1];
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);

    if (handle == NULL) {
        printf("Couldn't open device %s: %s\n", dev, errbuf);
        return 2;
    }

    pcap_loop(handle, 0, packet_handler, NULL);

    pcap_close(handle);
    return 0;
}
