// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once
#include <chrono>
#include <cstdint>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/fd_event_sync_interface.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/mqtt/mosquitto_cpp.hpp>

namespace everest::lib::io::mqtt {

class mqtt_client : public mosquitto_cpp, public event::fd_event_sync_interface {
public:
    using event_list = event::fd_event_handler::event_list;
    using cb_error = std::function<void(int, std::string const)>;

    mqtt_client(std::uint32_t reconnect_to_ms, std::string client_id = "");
    ~mqtt_client();

    everest::lib::io::event::sync_status sync() override;
    int get_poll_fd() override;

    void set_error_handler(cb_error const& handler);

protected:
    ErrorCode set_will_impl(std::string_view const& topic, std::string_view const& payload, QoS qos, bool retain,
                            PropertiesBase&& props) override;

    ErrorCode publish_impl(int* mid, std::string_view const& topic, std::string_view const& payload, QoS qos,
                           bool retain, PropertiesBase const& props) override;

    ErrorCode subscribe_impl(std::string_view const& topic, QoS qos, int options, PropertiesBase const& props,
                             subscribe_callback cb) override;

    ErrorCode unsubscribe_impl(std::string_view const& topic, PropertiesBase const& props) override;
    ErrorCode connect_impl(std::string_view const& host, std::uint16_t port, std::uint16_t keepalive_seconds) override;

    void set_callback_connect_impl(connect_callback cb) override;
    void set_callback_disconnect_impl(disconnect_callback cb) override;
    void set_callback_publish_impl(publish_callback cb) override;

private:
    void handle_socket(event_list const& events);
    void handle_reconnect_timer();
    ErrorCode handle_error(ErrorCode error);

    void listen_to_reconnect_timer(bool enable);
    void listen_to_write_events(bool enable);
    void listen_to_write_events_if_wanted();

    event::fd_event_handler m_handler;
    event::timer_fd m_reconnect_timer;

    cb_error m_error_handler;
    ErrorCode m_last_error{ErrorCode::Unknown};
    int m_last_socket{-1};
    bool m_connected{false};
};

} // namespace everest::lib::io::mqtt
