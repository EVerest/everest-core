// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/udp/udp_payload.hpp>
#include <charge_bridge/plc_bridge.hpp>
#include <charge_bridge/utilities/logging.hpp>
#include <iostream>

namespace charge_bridge {

plc_bridge::plc_bridge(plc_bridge_config const& config) :
    m_tap(config.plc_tap, config.plc_ip, config.plc_netmaks, config.plc_mtu), m_udp(config.cb_remote, config.cb_port, 1000) {
    using namespace std::chrono_literals;
    m_timer.set_timeout(5s);

    m_tap.set_rx_handler([this](auto const& data, auto&) {
        everest::lib::io::udp::udp_payload pl;
        pl.buffer = data;
        m_udp.tx(pl);
    });

    m_udp.set_rx_handler([this](auto const& data, auto&) { m_tap.tx(data.buffer); });

    auto identifier = config.cb + "/" + config.item;
    m_tap.set_error_handler([this, identifier](auto id, auto const& msg) {
        utilities::print_error(identifier, "PLC/TAP", id) << msg << std::endl;
        m_tap_on_error = id not_eq 0;
    });

    m_udp.set_error_handler([this, identifier](auto id, auto const& msg) {
        utilities::print_error(identifier, "PLC/UDP", id) << msg << std::endl;
        m_udp_on_error = id not_eq 0;
    });
}

bool plc_bridge::register_events(everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return
        handler.register_event_handler(&m_tap) &&
        handler.register_event_handler(&m_udp) &&
        handler.register_event_handler(&m_timer, [this](auto&) {
            if (m_udp_on_error) {
                m_udp.reset();
            }
            if (m_tap_on_error) {
                m_tap.reset();
            }
        });
    // clang-format on
}

bool plc_bridge::unregister_events(everest::lib::io::event::fd_event_handler& handler) {
    // clang-format off
    return
        handler.unregister_event_handler(&m_tap) &&
        handler.unregister_event_handler(&m_udp) &&
        handler.unregister_event_handler(&m_timer);
    // clang-format on
}

} // namespace charge_bridge
