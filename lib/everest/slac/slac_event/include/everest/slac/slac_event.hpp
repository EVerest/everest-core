// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2026 Pionix GmbH and Contributors to EVerest
#pragma once

#include <array>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/slac/slac_socket.hpp>
#include <slac/slac.hpp>

namespace everest::lib::slac {
class SlacEvent : public everest::lib::io::event::fd_event_register_interface {
public:
    using HomeplugMessage = slac_client::ClientPayloadT;
    using HomeplugMessageHandler = std::function<void(HomeplugMessage const&)>;
    using MacAddress = slac_socket::MacAddress;

    SlacEvent(std::string const& if_name);
    void send(::slac::messages::HomeplugMessage& msg);

    const uint8_t* get_mac_addr();

    void set_callback(HomeplugMessageHandler const& callback);

    bool register_events(everest::lib::io::event::fd_event_handler& handler) override;
    bool unregister_events(everest::lib::io::event::fd_event_handler& handler) override;

private:
    void handle_socket_error(int id, std::string const& msg);
    void handle_socket_rx(HomeplugMessage const& data, slac_client::interface& client);
    void handle_error_timer();

    slac_client m_connection;
    io::event::timer_fd m_error_timer;
    bool m_on_error{false};
    HomeplugMessageHandler m_callback;
    MacAddress m_mac_address;
    std::string m_if_name;
};
} // namespace everest::lib::slac
