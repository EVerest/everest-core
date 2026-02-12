// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <chrono>
#include <everest/io/event/fd_event_register_interface.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/mdns/mdns.hpp>
#include <everest/io/mdns/mdns_client.hpp>
#include <functional>
#include <memory>
#include <set>

namespace charge_bridge {

enum class discovery_device_type {
    CB_EVSE,
    CB_EV
};

class discovery : public everest::lib::io::event::fd_event_register_interface {
public:
    using discovery_cb = std::function<void(std::string const&)>;

    discovery(discovery_device_type type);
    discovery(discovery_device_type type, std::set<std::string> const& interfaces, bool excluding);

    bool register_events(everest::lib::io::event::fd_event_handler& handler) override;
    bool unregister_events(everest::lib::io::event::fd_event_handler& handler) override;

    void set_discovery_callback(discovery_cb const& cb);

private:
    void add_client(std::string const& interface);
    void query_registry();

    std::vector<std::unique_ptr<everest::lib::io::mdns::mdns_client>> m_mdns;
    everest::lib::io::event::timer_fd m_timer;
    discovery_cb m_on_discover;
    everest::lib::io::mdns::mDNS_registry m_registry;
    discovery_device_type m_type;
    static const std::string discovery_id;
};

} // namespace charge_bridge
