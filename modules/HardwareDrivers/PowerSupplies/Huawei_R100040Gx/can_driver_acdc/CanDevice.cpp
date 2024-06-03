// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <fcntl.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>

#include <linux/can.h>
#include <linux/can/raw.h>

#include <iostream>
#include <vector>

#include "CanDevice.hpp"

CanDevice::CanDevice() : exit_rx_thread{false} {
    can_fd = 0;
}

CanDevice::~CanDevice() {
    close_device();
}

void CanDevice::open_device(const std::string& dev) {
    can_device = dev;
    try_open_device_internal();
    // spawn read thread
    exit_rx_thread = false;
    rx_thread_handle = std::thread(&CanDevice::rx_thread, this);
}

bool CanDevice::try_open_device_internal() {

    if (is_open) {
        return false;
    }

    is_open = false;
    if ((can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        return false;

    } else {

        // retrieve interface index from interface name
        struct ifreq ifr;
        strcpy(ifr.ifr_name, can_device.c_str());
        if (ioctl(can_fd, SIOCGIFINDEX, &ifr) < 0) {
            close(can_fd);
            return false;
        }

        // bind to the interface
        struct sockaddr_can addr;
        memset(&addr, 0, sizeof(addr));
        addr.can_family = AF_CAN;
        addr.can_ifindex = ifr.ifr_ifindex;

        if (bind(can_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            close(can_fd);
            return false;
        }

        // set non blocking
        int v = fcntl(can_fd, F_GETFD, 0);
        fcntl(can_fd, F_SETFD, v | O_NONBLOCK);

        is_open = true;
        return true;
    }
}

bool CanDevice::close_device() {
    is_open = false;
    if (can_fd != 0 && close(can_fd) == 0) {
        can_fd = 0;
        exit_rx_thread = true;
        rx_thread_handle.join();
        return true;
    } else {
        return false;
    }
}

void CanDevice::close_device_internal() {
    is_open = false;
    if (can_fd != 0 && close(can_fd) == 0) {
        can_fd = 0;
    }
}

void CanDevice::rx_thread() {
    can_frame frame;

    while (!exit_rx_thread) {
        if (is_open) {
            /* Read non-blocking from CAN Bus.
            Note that an implementation with select/poll seems to be unreliable on socket CAN.
            Sometimes select() returns 0 even though there are bytes to read. If that happens, it will never return
            anything but 0 again. Sometimes it returns 1, but the subsequent read blocks (even in non blocking mode).
            Maybe this can be fixed by adjusting some buffering in ioctl, but no working solution has been found.
            */
            size_t nbytes = read(can_fd, &frame, sizeof(struct can_frame));

            if (nbytes <= 0) {
                // If there is nothing to read, give it some time before we try again.
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            } else if (nbytes > 0) {
                // Received a new CAN packet...
                std::vector<uint8_t> payload;
                payload.assign(frame.data, frame.data + frame.can_dlc);
                rx_handler(frame.can_id, payload);
            }
        } else {
            std::this_thread::sleep_for(std::chrono::milliseconds(250));
        }
    }
}

void CanDevice::rx_handler(uint32_t can_id, const std::vector<uint8_t>& payload) {
    std::cout << "CAN frame received" << std::endl;
}

void CanDevice::connection_established() {
    std::cout << "Connection established" << std::endl;
}

bool CanDevice::_tx(uint32_t can_id, const std::vector<uint8_t>& payload) {

    try_open_device_internal();

    if (not is_open) {
        return false;
    }

    // std::cout << "CAN frame TX" << std::endl;
    struct can_frame frame;
    if (payload.size() > sizeof(frame.data)) {
        return false;
    }

    frame.can_id = can_id;
    frame.can_dlc = payload.size();
    memcpy(frame.data, payload.data(), payload.size());

    if (write(can_fd, &frame, sizeof(can_frame)) != sizeof(can_frame)) {
        // On CAN, frames cannot be partially written. If we cannot write the packet, something is wrong with the bus.
        // Close device.
        close_device_internal();
        return false;
    }
    return true;
}
