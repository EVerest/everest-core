// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <chrono>
#include <everest/io/can/can_payload.hpp>
#include <everest/io/can/socket_can.hpp>
#include <everest/io/event/fd_event_register_interface.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/udp/udp_client.hpp>
#include <memory>

extern "C" struct cb_can_message;

namespace charge_bridge {

struct can_bridge_config {
    std::string cb;
    std::string item;
    uint16_t cb_port;
    std::string cb_remote;
    std::string can_device;
};

class can_bridge : public everest::lib::io::event::fd_event_register_interface {
public:
    can_bridge(can_bridge_config const& config);
    ~can_bridge();

    bool register_events(everest::lib::io::event::fd_event_handler& handler) override;
    bool unregister_events(everest::lib::io::event::fd_event_handler& handler) override;

private:
    void handle_error_timer();
    void handle_heartbeat_timer();
    void send_can_to_udp(cb_can_message const& pl);
    std::unique_ptr<everest::lib::io::can::socket_can> m_can;
    everest::lib::io::udp::udp_client m_udp;
    std::string m_can_device;
    std::string identifier;
    everest::lib::io::event::timer_fd m_heartbeat_timer;
    std::chrono::steady_clock::time_point m_last_msg_to_cb;
};

} // namespace charge_bridge
