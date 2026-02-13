// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <ieee2030/charger/io/can_broker_charger.hpp>
#include <ieee2030/common/io/time.hpp>

#include <cstring>
#include <linux/can.h>
#include <net/if.h>
#include <poll.h>
#include <stdexcept>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

// Todo: should be in a helper file
static void throw_with_error(const std::string& msg) {
    throw std::runtime_error(msg + ": (" + std::string(strerror(errno)) + ")");
}

namespace ieee2030::charger::io {

CanBrokerCharger::CanBrokerCharger() {
}

CanBrokerCharger::CanBrokerCharger(const std::string& interface_name) : tx_active(false), rx_active{false} {
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

    rx_loop_thread = std::thread(&CanBrokerCharger::rx_loop, this);
    tx_loop_thread = std::thread(&CanBrokerCharger::tx_loop, this);
}

CanBrokerCharger::~CanBrokerCharger() {

    tx_active = false;
    rx_active = false;
    exit_rx_loop = true;
    exit_tx_loop = true;

    rx_loop_thread.join();
    tx_loop_thread.join();

    close(can_fd);
}

void CanBrokerCharger::set_event_callback(const CanEventCallback& callback) {
    event_callback = callback;
}

void CanBrokerCharger::rx_loop() {

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

void CanBrokerCharger::handle_can_input(uint32_t can_id, const std::vector<uint8_t>& payload) {

    if (!(can_id == messages::EV_ID_100 || can_id == messages::EV_ID_101 || can_id == messages::EV_ID_102)) {
        // Todo: Check standard if a error is provided
        return;
    }

    // Todo: How to detect ev can is active -> Active means the first 100, 101, 102 are received?
    // Todo: How to detect ev can is not active? -> Receiving nothing over a certain time?

    // Todo: Check if correct sequence 100 -> 101 -> 102 -> 100 ...
    // Todo: Check Timer for message 100, 101, 102

    if (can_id == messages::EV_ID_100) {
        message_100 = messages::EV100(payload);
    } else if (can_id == messages::EV_ID_101) {
        message_101 = messages::EV101(payload);
    } else if (can_id == messages::EV_ID_102) {
        message_102 = messages::EV102(payload);
    }

    publish_event(CanEvent::NEW_DATA);
}

void CanBrokerCharger::tx_loop() {

    static constexpr auto SEND_CAN_TIMEOUT_MS = 100;
    auto actual_time_108 = ieee2030::io::get_current_time_point();
    auto actual_time_109 = ieee2030::io::get_current_time_point();

    while (!exit_tx_loop) {

        if (tx_active) {
            switch (tx_state) {
            case SendState::ID_108:
                if (ieee2030::io::get_current_time_point() >=
                    ieee2030::io::offset_time_point_by_ms(actual_time_108, SEND_CAN_TIMEOUT_MS)) {
                    send(messages::CHARGER_ID_108, message_108);
                    actual_time_108 = ieee2030::io::get_current_time_point();
                    tx_state = SendState::ID_109;
                }
                break;

            case SendState::ID_109:
                if (ieee2030::io::get_current_time_point() >=
                    ieee2030::io::offset_time_point_by_ms(actual_time_109, SEND_CAN_TIMEOUT_MS)) {
                    send(messages::CHARGER_ID_109, message_109);
                    actual_time_109 = ieee2030::io::get_current_time_point();
                    tx_state = SendState::ID_108;
                }
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void CanBrokerCharger::send(uint32_t id, const std::vector<uint8_t>& payload) {

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

void CanBrokerCharger::update_status_error_flag(defs::ChargerStatusError status, bool active) {
    switch (status) {
    case defs::ChargerStatusError::CHARGER_STATUS:
        message_109.charger_status = active;
        break;
    case defs::ChargerStatusError::CHARGER_MALFUNCTION:
        message_109.charger_malfunction = active;
        break;
    case defs::ChargerStatusError::CONNECTOR_LOCK:
        message_109.connector_lock = active;
        break;
    case defs::ChargerStatusError::BATTERY_INCOMPATIBILITY:
        message_109.battery_incompatibility = active;
        break;
    case defs::ChargerStatusError::SYSTEM_MALFUNCTION:
        message_109.system_malfunction = active;
        break;
    case defs::ChargerStatusError::STOP_CONTROL:
        message_109.stop_control = active;
        break;
    }
}

void CanBrokerCharger::update_reamining_time_10s(uint16_t time_10s) {
    if (time_10s < 0xFF) {
        message_109.reamining_time_10s = (uint8_t)time_10s;
        message_109.reamining_time_1min = 0;
    } else {
        message_109.reamining_time_10s = 0xFF;
        message_109.reamining_time_1min = (uint8_t)(time_10s / 6);
    }
}

} // namespace ieee2030::charger::io
