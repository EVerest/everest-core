// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <charge_bridge/gpio_bridge.hpp>
#include <charge_bridge/utilities/logging.hpp>
#include <charge_bridge/utilities/platform_utils.hpp>
#include <charge_bridge/utilities/string.hpp>
#include <chrono>
#include <cstdint>
#include <cstring>
#include <everest/io/event/fd_event_handler.hpp>
#include <iostream>
#include <limits>
#include <memory>
#include <protocol/cb_management.h>
#include <stdexcept>
#include <string>

namespace charge_bridge {
using namespace std::chrono_literals;

gpio_bridge::gpio_bridge(gpio_config const& config) :
    m_udp(config.cb_remote, config.cb_port, 1000),
    m_mqtt(config.mqtt_remote, config.mqtt_port, config.mqtt_ping_interval_ms)

{
    m_identifier = config.cb + "/" + config.item;

    m_heartbeat_timer.set_timeout(std::chrono::seconds(config.interval_s));

    m_udp.set_rx_handler([this](auto const& data, auto&) { handle_udp_rx(data); });

    m_udp.set_error_handler([this](auto id, auto const& msg) {
        utilities::print_error(m_identifier, "GPIO/UDP", id) << msg << std::endl;
        m_udp_on_error = id not_eq 0;
    });

    m_receive_topic = "pionix/chargebridge/" + config.cb + "/gpio/output/";
    m_send_topic = "pionix/chargebridge/" + config.cb + "/gpio/input/";
    m_mqtt.subscribe(m_receive_topic + "#");
    m_mqtt.set_message_handler([this](auto& data, auto&) { dispatch(data); });
    m_mqtt.set_error_handler([this, config](int id, std::string const& msg) {
        utilities::print_error(m_identifier, "GPIO/MQTT", id) << msg << std::endl;
        m_mqtt_on_error = id not_eq 0;
    });

    m_mqtt_timer.set_timeout(5s);

    m_message.type = CbStructType::CST_HostToCb_Gpio;
    m_message.data.number_of_gpios = CB_NUMBER_OF_GPIOS;
    std::memset(m_message.data.gpio_values, 0, sizeof(m_message.data.gpio_values));
}

gpio_bridge::~gpio_bridge() {
}

bool gpio_bridge::register_events(everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return
        handler.register_event_handler(&m_udp) &&
        handler.register_event_handler(&m_mqtt) &&
        handler.register_event_handler(&m_heartbeat_timer, [this](auto&) { handle_heartbeat_timer(); }) &&
        handler.register_event_handler(&m_mqtt_timer, [this](auto&) { handle_mqtt_timer(); });
;
    // clang-format on
}

bool gpio_bridge::unregister_events(everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return
        handler.unregister_event_handler(&m_udp) &&
        handler.unregister_event_handler(&m_mqtt) &&
        handler.unregister_event_handler(&m_heartbeat_timer) &&
        handler.unregister_event_handler(&m_mqtt_timer);
    // clang-format on
}

void gpio_bridge::dispatch(everest::lib::io::mqtt::Dataset const& data) {
    auto& topic = data.topic;
    auto& payload = data.message;
    auto operation = utilities::string_after_pattern(topic, m_receive_topic);
    uint16_t value = 0;
    int id = 0;

    auto stous = [](std::string const& data) {
        auto val = stoi(data);
        if (val < 0 or val > std::numeric_limits<uint16_t>::max()) {
            throw std::range_error("");
        }
        return static_cast<uint16_t>(val);
    };

    try {
        value = stous(payload);
    } catch (...) {
        std::cout << "INVALID DATA on MQTT for GPIO DATA" << std::endl;
        return;
    }
    try {
        id = std::stoi(operation);
    } catch (...) {
        std::cout << "INVALID DATA on MQTT for GPIO ID" << std::endl;
        return;
    }
    if (id < 0 or id >= CB_NUMBER_OF_GPIOS) {
        std::cout << "INVALID GPIO ID" << std::endl;
        return;
    }

    m_message.data.gpio_values[id] = value;
    send_udp();
}

void gpio_bridge::send_mqtt(std::string const& topic, std::string const& message) {
    everest::lib::io::mqtt::Dataset payload;
    payload.topic = m_send_topic + topic;
    payload.message = message;
    m_mqtt.tx(payload);
}

void gpio_bridge::send_udp() {
    if (not m_udp_on_error) {
        everest::lib::io::udp::udp_payload payload;
        utilities::struct_to_vector(m_message, payload.buffer);
        m_udp.tx(payload);
    }
}

void gpio_bridge::handle_error_timer() {
    if (m_udp_on_error) {
        m_udp.reset();
    }
}

void gpio_bridge::handle_mqtt_timer() {
    if (m_mqtt_on_error) {
        m_mqtt.reset();
    }
}

void gpio_bridge::handle_heartbeat_timer() {
    send_udp();
}

void gpio_bridge::handle_udp_rx(everest::lib::io::udp::udp_payload const& payload) {
    CbManagementPacket<CbGpioPacket> data;
    if (payload.size() == sizeof(data)) {
        std::memcpy(&data, payload.buffer.data(), sizeof(data));
        for (size_t i = 0; i < sizeof(data.data.gpio_values) / sizeof(data.data.gpio_values[0]); ++i) {
            send_mqtt(std::to_string(i), std::to_string(data.data.gpio_values[i]));
        }
    } else {
        std::cout << "INVALID DATA SIZE in UDP RX of GPIO: " << payload.size() << " vs " << sizeof(data) << std::endl;
    }
}

} // namespace charge_bridge
