// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <iostream>
#include <vector>

#include "CanDevice.hpp"

#include <everest/logging.hpp>

CanDevice::CanDevice() : exit_rx_thread{false} {
    can_fd = 0;
}

CanDevice::~CanDevice() {
    close_device();
}

bool CanDevice::open_device(const char* dev) {
    if (!dev || std::strlen(dev) >= IFNAMSIZ) {
        fprintf(stderr, "Interface name is invalid or too long: %s\n", dev ? dev : "NULL");
        return false;
    }

    if ((can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket");
        return false;
    }

    // retrieve interface index from interface name
    struct ifreq ifr;
    std::memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, dev, IFNAMSIZ - 1);
    if (ioctl(can_fd, SIOCGIFINDEX, &ifr) < 0) {
        perror(dev);
        close(can_fd);
        return false;
    }

    // bind to the interface
    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(can_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind");
        close(can_fd);
        return false;
    }

    // spawn read thread
    exit_rx_thread = false;
    rx_thread_handle = std::thread(&CanDevice::rx_thread, this);

    return true;
}

bool CanDevice::close_device() {
    if (can_fd != 0 && close(can_fd) == 0) {
        can_fd = 0;
        exit_rx_thread = true;
        rx_thread_handle.join();
        return true;
    } else {
        return false;
    }
}

void CanDevice::rx_thread() {
    can_frame frame;
    while (!exit_rx_thread) {

        size_t nbytes = read(can_fd, &frame, sizeof(struct can_frame));

        if (nbytes < 0) {
            perror("Read");
        } else if (nbytes > 0) {
            // Received a new CAN packet...
            std::vector<uint8_t> payload;
            payload.assign(frame.data, frame.data + frame.can_dlc);
            rx_handler(frame.can_id, payload);
        }
    }
}

void CanDevice::rx_handler(uint32_t can_id, const std::vector<uint8_t>& payload) {
    EVLOG_debug << "CAN frame received";
}

bool CanDevice::_tx(uint32_t can_id, const std::vector<uint8_t>& payload) {
    if (can_fd == 0)
        return false;

    struct can_frame frame;
    if (payload.size() > sizeof(frame.data)) {
        throw std::runtime_error("Size of can payload data to large (" + std::to_string(payload.size()) + " bytes)");
    }

    frame.can_id = can_id;
    frame.can_dlc = payload.size();
    memcpy(frame.data, payload.data(), payload.size());

    if (write(can_fd, &frame, sizeof(can_frame)) != sizeof(can_frame)) {
        throw std::runtime_error(std::string("Failed to send can packet :") + strerror(errno));
        return false;
    }

    return true;
}
