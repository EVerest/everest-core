// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef REMOTE_CHARGING_TAP_DEVICE_HPP
#define REMOTE_CHARGING_TAP_DEVICE_HPP

#include <arpa/inet.h>
#include <errno.h>
#include <netinet/if_ether.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

#include <everest/logging.hpp>
#include <functional>
#include <string>

namespace tap {

int open_devices(const std::string& eth_device) {
    // open the clone device
    int fd = open("/dev/net/tun", O_RDWR);
    if (fd < 0) {
        return fd;
    }

    // preparation of the paramter struct ifr
    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    strncpy(ifr.ifr_name, eth_device.c_str(), IFNAMSIZ);

    // create the actual device
    int err = ioctl(fd, TUNSETIFF, (void*)&ifr);
    if (err < 0) {
        close(fd);
        return err;
    }

    return fd;
}

inline void send_eth_packet(int fd, const std::string& packet) {
    write(fd, packet.data(), packet.size());
}

inline void run_receive_loop(int fd, std::function<void(std::string packet)> rx_callback) {
    char packet[2048];

    while (true) {
        // Read one packet
        int nread = read(fd, packet, sizeof(packet));
        if (nread < 0) {
            perror("Reading from interface");
            close(fd);
            return;
        }

        // call callback
        rx_callback(std::string(packet, nread));
    }
}

} // namespace tap

#endif
