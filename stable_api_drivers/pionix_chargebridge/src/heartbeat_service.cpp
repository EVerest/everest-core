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
    m_last_heartbeat_reply = std::chrono::steady_clock::time_point::max();

    m_udp.set_rx_handler([this](auto const& data, auto&) { handle_udp_rx(data); });

    m_udp.set_error_handler([this](auto id, auto const& msg) {
        if (m_inital_cb_commcheck and id == 0) {
            utilities::print_error(m_identifier, "HEARTBEAT/UDP", 1) << "Waiting for ChargeBridge" << std::endl;
        } else {
            utilities::print_error(m_identifier, "HEARTBEAT/UDP", id) << msg << std::endl;
        }
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
    auto timeout = std::chrono::steady_clock::now() - m_last_heartbeat_reply > 2s ||
                   m_last_heartbeat_reply == std::chrono::steady_clock::time_point::max();
    if (timeout and m_cb_connected) {
        utilities::print_error(m_identifier, "HEARTBEAT/UDP", 1) << "ChargeBridge connection lost" << std::endl;
        m_cb_connected = false;
    }

    else if (not timeout and not m_cb_connected) {
        utilities::print_error(m_identifier, "HEARTBEAT/UDP", 0) << "ChargeBridge connected" << std::endl;
        m_cb_connected = true;
    }
}

void heartbeat_service::handle_udp_rx(everest::lib::io::udp::udp_payload const& payload) {
    CbManagementPacket<CbHeartbeatReplyPacket> data;
    if (payload.size() == sizeof(data)) {
        std::memcpy(&data, payload.buffer.data(), sizeof(data));
        m_last_heartbeat_reply = std::chrono::steady_clock::now();

        // TODO: do something with this data;
        /*printf(
            "CP: %.2f/%.2f PP: %i MCU_temp %i degC\nVoltages: 12V: %.2f, -12V: %.2f, ref %.3f, 3.3V: %.3f, core:
           %.3f\n", data.data.cp_hi_mV / 1000., data.data.cp_lo_mV / 1000., (int)data.data.pp_mOhm / 1000,
            data.data.temperature_mcu_C, data.data.vdd_12V/1000., data.data.vdd_N12V/1000., data.data.vdd_refint/1000.,
           data.data.vdd_3v3/1000., data.data.vdd_core/1000.);*/
    } else {
        std::cout << "INVALID DATA SIZE in UDP RX of HEARTBEAT: " << payload.size() << " vs " << sizeof(data)
                  << std::endl;
    }
}

} // namespace charge_bridge
