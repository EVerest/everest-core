// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <ifaddrs.h>
#include <net/if.h>
#include <net/if_dl.h>

#include <goose-ethernet/driver.hpp>
#include <stdexcept>
#include <string>

using namespace goose_ethernet;

// This implementation uses libpcap to send and receive Ethernet frames.
// This implementation is not ideal, as we currently have to have a timeout of
// 10ms and retry receiving until we get a packet

// this should work with timeout 0, but it doesn't (in my case)
// as the macos implementation is not that important we can leave it like this
// and do more important things
// todo: improve this

EthernetInterface::EthernetInterface(const char* interface_name) {
    char errbuf[PCAP_ERRBUF_SIZE];
    // timeout of 100ms
    pcap = pcap_open_live(interface_name, 65535, 1, 250, errbuf);
    if (pcap == NULL) {
        throw std::runtime_error("pcap_open_live failed: " + std::string(errbuf));
    }

    if (pcap_datalink(pcap) != DLT_EN10MB) {
        pcap_close(pcap);
        throw std::runtime_error("pcap_datalink failed: not an Ethernet interface");
    }

    struct ifaddrs *ifaddr, *ifa;
    if (getifaddrs(&ifaddr) == -1) {
        pcap_close(pcap);
        throw std::runtime_error("cannot get interface address; getifaddrs failed");
    }

    bool found = false;
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == NULL)
            continue;

        // Check if the interface name matches the one we are looking for
        if (strcmp(ifa->ifa_name, interface_name) == 0 && ifa->ifa_addr->sa_family == AF_LINK) {
            struct sockaddr_dl* sdl = (struct sockaddr_dl*)ifa->ifa_addr;
            uint8_t* mac_address = (unsigned char*)LLADDR(sdl);
            memcpy(this->mac_address, mac_address, 6);
            found = true;
            break;
        }
    }
    freeifaddrs(ifaddr);

    if (!found) {
        pcap_close(pcap);
        throw std::runtime_error("cannot get interface address; interface not found");
    }

    // add filter to ignore outgoing packets (sent by us)
    char filter_exp[sizeof("not ether src XX:XX:XX:XX:XX:XX")];
    snprintf(filter_exp, sizeof(filter_exp), "not ether src %02X:%02X:%02X:%02X:%02X:%02X", this->mac_address[0],
             this->mac_address[1], this->mac_address[2], this->mac_address[3], this->mac_address[4],
             this->mac_address[5]);

    struct bpf_program fp;
    if (pcap_compile(pcap, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
        pcap_close(pcap);
        throw std::runtime_error("pcap_compile failed: " + std::string(pcap_geterr(pcap)));
    }

    if (pcap_setfilter(pcap, &fp) == -1) {
        pcap_close(pcap);
        throw std::runtime_error("pcap_setfilter failed: " + std::string(pcap_geterr(pcap)));
    }
}

void EthernetInterface::send_packet_raw(const uint8_t* packet, size_t size) {
    int ret = pcap_sendpacket(pcap, (uint8_t*)packet, size);
    if (ret != 0) {
        throw std::runtime_error("pcap_sendpacket failed: " + std::string(pcap_geterr(pcap)));
    }
}

std::vector<uint8_t> EthernetInterface::receive_packet_raw() {
    pcap_pkthdr* header;
    const uint8_t* data;
    while (1) {
        int ret = pcap_next_ex(pcap, &header, &data);

        switch (ret) {
        case 1:
            return std::vector<uint8_t>(data, data + header->caplen);
        case 0:
            // timeout, try again
            continue;
        case -1:
            throw std::runtime_error("pcap_next_ex failed: " + std::string(pcap_geterr(pcap)));
        case -2:
            throw std::runtime_error("pcap_next_ex failed: no more packets");
        default:
            throw std::runtime_error("pcap_next_ex failed: unknown error");
        }
    }
}

EthernetInterface::~EthernetInterface() {
    pcap_close(pcap);
}

const uint8_t* EthernetInterface::get_mac_address() const {
    return this->mac_address;
}
