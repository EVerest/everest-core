// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <netinet/in.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <csignal>
#include <cstring>
#include <goose-ethernet/driver.hpp>
#include <optional>
#include <stdexcept>

#include "poll.h"

using namespace goose_ethernet;

EthernetInterface::EthernetInterface(const char* interface_name) {
    this->fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
    if (this->fd == -1) {
        throw std::runtime_error("Failed to open socket for ethernet interface: " + std::string(interface_name) +
                                 ", maybe add CAP_NET_RAW capability to executble");
    }

    int ifindex;

    {
        struct ifreq ifr;
        strncpy(ifr.ifr_name, interface_name, IFNAMSIZ);

        int ret = ioctl(this->fd, SIOCGIFINDEX, &ifr);
        if (ret == -1) {
            throw std::runtime_error("Failed to get interface index");
        }

        ifindex = ifr.ifr_ifindex;

        ret = ioctl(fd, SIOCGIFHWADDR, &ifr);
        if (ret == -1) {
            throw std::runtime_error("Failed to get interface MAC address");
        }

        memcpy(this->mac_address, ifr.ifr_hwaddr.sa_data, 6);
    }

    struct sockaddr_ll sll;
    sll.sll_family = AF_PACKET;
    sll.sll_protocol = htons(ETH_P_ALL);
    sll.sll_ifindex = ifindex;

    int ret = bind(this->fd, (struct sockaddr*)&sll, sizeof(sll));
    if (ret == -1) {
        throw std::runtime_error("Failed to bind socket");
    }
}

EthernetInterface::~EthernetInterface() {
    close(this->fd);
}

void EthernetInterface::send_packet_raw(const uint8_t* packet, size_t size) {
    ssize_t ret = write(this->fd, packet, size);
    if (ret == -1) {
        throw std::runtime_error("Failed to send packet, errno: " + std::to_string(errno));
    }
}

std::optional<std::vector<uint8_t>> EthernetInterface::receive_packet_raw() {
    std::vector<uint8_t> buffer(2000); // more than enough for an ethernet frame
                                       //
                                       //
    struct pollfd pfd[1];
    pfd[0].fd = this->fd;
    pfd[0].events = POLLIN;

    auto result_code = poll(pfd, 1, 50);
    auto error = errno;
    if (result_code < 0) {
        throw std::runtime_error("Failed to poll ethernet frame, errno: " + std::to_string(error));
    }

    if (result_code == 0) {
        return std::nullopt;
    }

    ssize_t ret = read(this->fd, buffer.data(), buffer.size());
    if (ret == -1) {
        throw std::runtime_error("Failed to receive packet, errno: " + std::to_string(errno));
    }

    buffer.resize(ret);
    return buffer;
}

const uint8_t* EthernetInterface::get_mac_address() const {
    return this->mac_address;
}
