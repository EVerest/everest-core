// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "charge_bridge/utilities/string.hpp"
#include <charge_bridge/discovery.hpp>
#include <charge_bridge/utilities/logging.hpp>
#include <everest/io/event/fd_event_handler.hpp>
#include <type_traits>

namespace charge_bridge {

namespace {
std::string to_string(discovery_device_type val) {
    switch (val) {
    case discovery_device_type::CB_EV:
        return "CB-CCS-EV-LU";

    case discovery_device_type::CB_EVSE:
        return "CB-CCS-EVSE-LU";
    default:
        return "INVALID";
    }
}

bool is_cb_match(std::string const& board_type, discovery_device_type discriminator) {
    auto result = board_type == to_string(discriminator);
    return result;
}

} // namespace

const std::string discovery::discovery_id = "_chargebridge._udp.local";

discovery::discovery(discovery_device_type type) : m_type(type) {
    using namespace std::chrono_literals;
    m_timer.set_timeout(1s);

    for (auto const& item : everest::lib::io::socket::get_all_interaces()) {
        add_client(item.name);
    }
}

discovery::discovery(discovery_device_type type, std::set<std::string> const& interfaces, bool excluding) :
    m_type(type) {
    using namespace std::chrono_literals;
    m_timer.set_timeout(1s);

    for (auto const& item : everest::lib::io::socket::get_all_interaces()) {
        if (not interfaces.empty()) {
            if (interfaces.count(item.name) == 1 and excluding) {
                continue;
            }
            if (interfaces.count(item.name) == 0 and not excluding) {
                continue;
            }
        }
        std::cout << "Adding: " << item.name << std::endl;
        add_client(item.name);
    }
}

void discovery::add_client(std::string const& interface) {
    auto client = std::make_unique<everest::lib::io::mdns::mdns_client>(interface);
    client->set_rx_handler([&](auto const& data, auto&) {
        auto discovery = everest::lib::io::mdns::parse_mdns_packet(data.buffer);
        if (discovery.has_value()) {
            if (m_registry.update(discovery.value())) {
                query_registry();
            }
        }
    });
    m_mdns.push_back(std::move(client));
}

void discovery::query_registry() {
    auto obj = m_registry.get();
    for (auto const& [key, value] : obj) {
        if (not utilities::string_ends_with(key, discovery_id)) {
            continue;
        }
        if (not value.txt.count("board_type") or not is_cb_match(value.txt.at("board_type"), m_type)) {
            continue;
        }
        if (not m_on_discover) {
            continue;
        }
        m_on_discover(value.ip);
        return;
    }
}

void discovery::set_discovery_callback(discovery_cb const& cb) {
    m_on_discover = cb;
}

bool discovery::register_events(everest::lib::io::event::fd_event_handler& handler) {
    auto result = true;
    for (auto& item : m_mdns) {
        if (item) {
            result = handler.register_event_handler(item.get()) && result;
        }
    }
    handler.register_event_handler(&m_timer, [&](auto) {
        for (auto& item : m_mdns) {
            item->get_raw_handler()->query(discovery_id);
        }
    });

    return result;
}

bool discovery::unregister_events(everest::lib::io::event::fd_event_handler& handler) {
    auto result = true;
    for (auto& item : m_mdns) {
        if (item) {
            result = handler.unregister_event_handler(item.get()) && result;
        }
    }
    handler.unregister_event_handler(&m_timer);
    return result;
}

} // namespace charge_bridge
