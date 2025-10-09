// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <charge_bridge/heartbeat_service.hpp>
#include <charge_bridge/utilities/logging.hpp>
#include <charge_bridge/utilities/platform_utils.hpp>
#include <chrono>
#include <cstring>
#include <everest/io/event/fd_event_handler.hpp>
#include <iostream>
#include <memory>
#include <protocol/cb_management.h>

namespace charge_bridge {
using namespace std::chrono_literals;

heartbeat_service::heartbeat_service(heartbeat_config const& config) : m_udp(config.cb_remote, config.cb_port) {
    m_identifier = config.cb + "/" + config.item;
    std::memcpy(&m_config_message.data, &config.cb_config, sizeof(CbConfig));
    m_config_message.type = CbStructType::CST_HostToCb_Heartbeat;
    m_heartbeat_timer.set_timeout(std::chrono::seconds(config.interval_s));

    m_udp.set_rx_handler([this](auto const& , auto&) { std::cout << "HEARTBEAT reply" << std::endl; });

    m_udp.set_error_handler([this](auto id, auto const& msg) {
        utilities::print_error(m_identifier, "HEARTBEAT/UDP", id) << msg << std::endl;
        m_udp_on_error = id not_eq 0;
    });
}

heartbeat_service::~heartbeat_service() {
}

bool heartbeat_service::register_events(everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return
        handler.register_event_handler(&m_udp) &&
        handler.register_event_handler(&m_heartbeat_timer, [this](auto&) { handle_heartbeat_timer(); });
    // clang-format on
}

bool heartbeat_service::unregister_events(everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return
        handler.unregister_event_handler(&m_udp) &&
        handler.unregister_event_handler(&m_heartbeat_timer);
    // clang-format on
}

void heartbeat_service::handle_error_timer() {
    if (m_udp_on_error) {
        m_udp.reset();
    }
}

void heartbeat_service::handle_heartbeat_timer() {
    if (not m_udp_on_error) {
        everest::lib::io::udp::udp_payload payload;
        utilities::struct_to_vector(m_config_message, payload.buffer);
        m_udp.tx(payload);
    }
}

} // namespace charge_bridge
