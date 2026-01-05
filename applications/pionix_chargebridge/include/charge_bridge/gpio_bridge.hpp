// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include "everest/io/mqtt/mosquitto_cpp.hpp"
#include <array>
#include <everest/io/can/can_payload.hpp>
#include <everest/io/event/fd_event_register_interface.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/mqtt/mqtt_client.hpp>
#include <everest/io/udp/udp_client.hpp>
#include <protocol/cb_management.h>

namespace charge_bridge {

struct gpio_config {
    std::string cb;
    std::string item;
    std::uint16_t cb_port;
    std::string cb_remote;
    std::uint16_t interval_s;
    std::string mqtt_remote;
    std::string mqtt_bind;
    std::uint16_t mqtt_port;
    std::uint32_t mqtt_ping_interval_ms;
};

class gpio_bridge : public everest::lib::io::event::fd_event_register_interface {
public:
    gpio_bridge(gpio_config const& config);
    ~gpio_bridge();

    bool register_events(everest::lib::io::event::fd_event_handler& handler) override;
    bool unregister_events(everest::lib::io::event::fd_event_handler& handler) override;

private:
    void handle_error_timer();
    void handle_heartbeat_timer();
    void handle_udp_rx(everest::lib::io::udp::udp_payload const& payload);
    void dispatch(everest::lib::io::mqtt::mqtt_client::message const& data);
    void send_mqtt(std::string const& topic, std::string const& message);
    void send_udp();

    everest::lib::io::udp::udp_client m_udp;
    bool m_udp_on_error{false};
    everest::lib::io::event::timer_fd m_heartbeat_timer;
    std::chrono::steady_clock::time_point last_heartbeat;
    CbManagementPacket<CbGpioPacket> m_message;
    std::string m_identifier;
    bool m_mqtt_on_error{false};
    everest::lib::io::mqtt::mqtt_client m_mqtt;
    std::string m_receive_topic;
    std::string m_send_topic;
};

} // namespace charge_bridge
