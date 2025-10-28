// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <everest/io/udp/udp_payload.hpp>
#include <charge_bridge/evse_bridge.hpp>
#include <charge_bridge/utilities/logging.hpp>
#include <cstring>
#include <iostream>
#include <protocol/evse_bsp_cb_to_host.h>
#include <protocol/evse_bsp_host_to_cb.h>

namespace charge_bridge {

evse_bridge::evse_bridge(evse_bridge_config const& config) :
    m_bsp(config.api, config.cb + "/" + config.item), m_udp(config.cb_remote, config.cb_port) {
    using namespace std::chrono_literals;
    m_timer.set_timeout(5s);

    m_bsp.set_cb_tx([this](auto& data) {
        everest::lib::io::udp::udp_payload pl;
        pl.set_message(&data, sizeof(data));
        m_udp.tx(pl);
    });

    m_udp.set_rx_handler([this](auto const& data, auto&) {
        evse_bsp_cb_to_host msg;
        std::memcpy(&msg, data.buffer.data(), data.size());
        m_bsp.set_cb_message(msg);
    });

    auto identifier = config.cb + "/" + config.item;
    m_udp.set_error_handler([this, identifier](auto id, auto const& msg) {
        utilities::print_error(identifier, "BSP/UDP", id) << msg << std::endl;
        m_udp_on_error = id not_eq 0;
    });
}

bool evse_bridge::register_events(everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return
        handler.register_event_handler(&m_bsp) &&
        handler.register_event_handler(&m_udp) &&
        handler.register_event_handler(&m_timer, [this](auto&) {
            if (m_udp_on_error) {
                m_udp.reset();
            }
        });
    // clang-format on
}

bool evse_bridge::unregister_events(everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return
        handler.unregister_event_handler(&m_bsp) &&
        handler.unregister_event_handler(&m_udp) &&
        handler.unregister_event_handler(&m_timer);
    // clang-format on
}

} // namespace charge_bridge
