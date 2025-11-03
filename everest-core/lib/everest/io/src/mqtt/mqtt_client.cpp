// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#include "everest/io/event/fd_event_client.hpp"
#include "everest/io/event/fd_event_handler.hpp"
#include <everest/io/mqtt/mqtt_client.hpp>

namespace everest::lib::io::mqtt {

mqtt_client::mqtt_client(std::string const& server, std::uint16_t port, std::uint32_t ping_interval_ms) :
    mqtt_client_base(server, port) {
    m_timer.set_timeout_ms(ping_interval_ms);
    m_handler.register_event_handler(&m_timer, [this](auto) {
        auto const& ptr = get_raw_handler();
        if (ptr) {
            ptr->ping();
        }
    });
    m_handler.register_event_handler(mqtt_client_base::get_poll_fd(), [this](auto) { mqtt_client_base::sync(); },
                                     {event::poll_events::read});
}

int mqtt_client::get_poll_fd() {
    return m_handler.get_poll_fd();
}

event::sync_status mqtt_client::sync() {
    m_handler.poll();
    return event::sync_status::ok;
}

void mqtt_client::set_message_handler(cb_rx const& handler) {
    set_rx_handler([handler, this](auto const& data, auto& tx) {
        auto const& ptr = get_raw_handler();
        if (ptr && ptr->reset_current_item_is_data()) {
            handler(data, tx);
        }
    });
}

bool mqtt_client::subscribe(std::string const& topic) {
    auto const& ptr = get_raw_handler();
    if (not ptr) {
        return false;
    }
    return ptr->subscribe(topic);
}

bool mqtt_client::unsubscribe(std::string const& topic) {
    auto const& ptr = get_raw_handler();
    if (not ptr) {
        return false;
    }
    return ptr->unsubscribe(topic);
}

} // namespace everest::lib::io::mqtt
