// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <algorithm>
#include <everest/io/can/socket_can_handler.hpp>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/unique_fd.hpp>
#include <everest/io/socket/socket.hpp>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

namespace everest::lib::io::can {

bool socket_can_handler::data_valid(can_payload const& payload) {
    return payload.size() <= CAN_MAX_DLEN;
}

int socket_can_handler::tx(uint32_t can_id, uint8_t len8_dlc, can_payload const& payload) {
    if (not is_open()) {
        return ENETDOWN;
    }
    if (not data_valid(payload)) {
        return EINVAL;
    }
    struct can_frame frame;

    frame.can_id = can_id;
    auto const max_dlc_value = 15;
    frame.len8_dlc = std::min<uint8_t>(len8_dlc, max_dlc_value);
    frame.len = std::min<uint8_t>(CAN_MAX_DLEN, payload.size());
    memcpy(frame.data, payload.data(), frame.len);

    auto bytes_written = write(m_owned_can_fd, &frame, sizeof(can_frame));
    if (bytes_written != sizeof(can_frame)) {
        if (bytes_written == -1) {
            return errno;
        } else {
            return EAGAIN;
        }
    }
    return 0;
}

int socket_can_handler::rx(uint32_t& can_id, uint8_t& len8_dlc, can_payload& payload) {
    if (is_open()) {
        static auto const framesize = sizeof(struct can_frame);
        can_frame frame;
        auto nbytes = read(m_owned_can_fd, &frame, framesize);

        if (nbytes == framesize) {
            payload.clear();
            payload.assign(frame.data, frame.data + frame.can_dlc);
            can_id = frame.can_id;
            len8_dlc = frame.len8_dlc;
            return 0;
        } else if (nbytes == -1) {
            return errno;
        } else {
            return EINVAL;
        }
    }
    return ENETDOWN;
}

bool socket_can_handler::tx(can_dataset const& data) {
    auto can_id = data.get_can_id_with_flags();
    auto status = tx(can_id, data.len8_dlc, data.payload);
    return status == 0;
}

bool socket_can_handler::rx(can_dataset& data) {
    uint32_t can_id;
    auto status = rx(can_id, data.len8_dlc, data.payload);
    data.set_can_id_with_flags(can_id);
    return status == 0;
}

bool socket_can_handler::open(std::string const& can_device) {
    m_can_dev = can_device;
    return open_device() == 0;
}

int socket_can_handler::open_device() {
    auto can_fd = event::unique_fd(::socket(PF_CAN, SOCK_RAW, CAN_RAW));
    if (can_fd < 0) {
        perror("Socket");
        return errno;
    }
    struct ifreq ifr;
    strcpy(ifr.ifr_name, m_can_dev.c_str());
    if (ioctl(can_fd, SIOCGIFINDEX, &ifr) < 0) {
        perror(m_can_dev.c_str());
        return errno;
    }

    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(can_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind");
        return errno;
    }

    socket::set_non_blocking(can_fd);
    socket::set_socket_send_buffer_to_min(can_fd);
    m_owned_can_fd = std::move(can_fd);
    return 0;
}

bool socket_can_handler::is_open() const {
    return m_owned_can_fd.is_fd();
}

void socket_can_handler::close() {
    m_owned_can_fd.close();
}

int socket_can_handler::get_fd() const {
    return m_owned_can_fd;
}

int socket_can_handler::get_error() const {
    return socket::get_pending_error(m_owned_can_fd);
}

} // namespace everest::lib::io::can
