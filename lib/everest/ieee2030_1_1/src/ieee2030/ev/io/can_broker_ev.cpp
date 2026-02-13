// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <ieee2030/common/io/time.hpp>
#include <ieee2030/ev/io/can_broker_ev.hpp>

#include <cstring>
#include <linux/can.h>
#include <net/if.h>
#include <poll.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

// Todo(sl): should be in a helper file
static void throw_with_error(const std::string& msg) {
    throw std::runtime_error(msg + ": (" + std::string(strerror(errno)) + ")");
}

namespace ieee2030::ev::io {

CanBrokerEv::CanBrokerEv(const std::string& interface_name) : tx_active(false), rx_active{false} {
    can_fd = socket(PF_CAN, SOCK_RAW, CAN_RAW);

    if (can_fd == -1) {
        throw_with_error("Failed to open socket");
    }

    // retrieve interface index from interface name
    struct ifreq ifr;

    if (interface_name.size() >= sizeof(ifr.ifr_name)) {
        throw_with_error("Interface name too long: " + interface_name);
    } else {
        strcpy(ifr.ifr_name, interface_name.c_str());
    }

    if (ioctl(can_fd, SIOCGIFINDEX, &ifr) == -1) {
        throw_with_error("Failed with ioctl/SIOCGIFINDEX on interface " + interface_name);
    }

    // bind to the interface
    struct sockaddr_can addr;
    memset(&addr, 0, sizeof(addr));
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(can_fd, reinterpret_cast<struct sockaddr*>(&addr), sizeof(addr)) == -1) {
        throw_with_error("Failed with bind");
    }

    rx_loop_thread = std::thread(&CanBrokerEv::rx_loop, this);
    tx_loop_thread = std::thread(&CanBrokerEv::tx_loop, this);
}

CanBrokerEv::~CanBrokerEv() {

    tx_active = false;
    rx_active = false;
    exit_rx_loop = true;
    exit_tx_loop = true;

    rx_loop_thread.join();
    tx_loop_thread.join();

    close(can_fd);
}

void CanBrokerEv::set_event_callback(const CanEventCallback& callback) {
    event_callback = callback;
}

void CanBrokerEv::rx_loop() {

    struct pollfd pfds = {can_fd, POLLIN, 0};

    while (!exit_rx_loop) {

        const auto poll_result = poll(&pfds, 1, 1);

        if (poll_result == 0) {
            continue;
        }

        if (pfds.revents & POLLIN) {
            struct can_frame frame;
            read(can_fd, &frame, sizeof(frame));

            std::vector<uint8_t> payload;
            payload.assign(frame.data, frame.data + frame.can_dlc);
            handle_can_input(frame.can_id, payload);
        }
    }
}

void CanBrokerEv::handle_can_input(uint32_t can_id, const std::vector<uint8_t>& payload) {

    if (!(can_id == messages::CHARGER_ID_108 || can_id == messages::CHARGER_ID_109)) {
        // Todo(sl): Check standard if a error is provided
        // Todo(sl): What is with the extended can ids (v1.2, 2.0, 3,1 and V2H)?
        return;
    }

    // Todo(sl): send can charger is active event -> Active means the first 108, 109 are received?
    // Todo(sl): How to detect charger can is shutdown? -> Receiving nothing over a certain time?

    // Todo(sl): Check if correct sequence 108 -> 109 -> 108 ...
    // Todo(sl): Check Timer for message 108, 109

    if (can_id == messages::CHARGER_ID_108) {
        message_108 = messages::Charger108(payload);
    } else if (can_id == messages::CHARGER_ID_109) {
        message_109 = messages::Charger109(payload);
    }

    publish_event(CanEvent::NEW_DATA);
}

void CanBrokerEv::tx_loop() {

    static constexpr auto SEND_CAN_TIMEOUT_MS = 100;
    auto actual_time_100 = ieee2030::io::get_current_time_point();
    auto actual_time_101 = ieee2030::io::get_current_time_point();
    auto actual_time_102 = ieee2030::io::get_current_time_point();

    while (!exit_tx_loop) {

        if (tx_active) {
            switch (tx_state) {
            case SendState::ID_100:
                if (ieee2030::io::get_current_time_point() >=
                    ieee2030::io::offset_time_point_by_ms(actual_time_100, SEND_CAN_TIMEOUT_MS)) {
                    send(messages::EV_ID_100, message_100);
                    actual_time_100 = ieee2030::io::get_current_time_point();
                    tx_state = SendState::ID_101;
                }
                break;

            case SendState::ID_101:
                if (ieee2030::io::get_current_time_point() >=
                    ieee2030::io::offset_time_point_by_ms(actual_time_101, SEND_CAN_TIMEOUT_MS)) {
                    send(messages::EV_ID_101, message_101);
                    actual_time_101 = ieee2030::io::get_current_time_point();
                    tx_state = SendState::ID_101;
                }
                break;

            case SendState::ID_102:
                if (ieee2030::io::get_current_time_point() >=
                    ieee2030::io::offset_time_point_by_ms(actual_time_102, SEND_CAN_TIMEOUT_MS)) {
                    send(messages::EV_ID_100, message_102);
                    actual_time_102 = ieee2030::io::get_current_time_point();
                    tx_state = SendState::ID_102;
                }
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void CanBrokerEv::send(uint32_t id, const std::vector<uint8_t>& payload) {

    struct can_frame frame;

    // Check payload length
    if (payload.size() > sizeof(frame.data)) {
        throw_with_error("Payload data length is too large");
    }

    frame.can_id = id;
    frame.can_dlc = payload.size();

    memcpy(frame.data, payload.data(), payload.size());

    const auto wrote_bytes = write(can_fd, &frame, sizeof(frame));

    if (wrote_bytes != sizeof(frame)) {
        throw_with_error("Failed to send can packet!");
    }
}

} // namespace ieee2030::ev::io
