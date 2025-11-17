// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#include "everest/io/event/fd_event_handler.hpp"
#include "everest/io/event/fd_event_sync_interface.hpp"
#include "everest/io/mqtt/mosquitto_cpp.hpp"
#include <bits/chrono.h>
#include <chrono>
#include <cstring>
#include <everest/io/mqtt/mqtt_client.hpp>
#include <memory>

namespace everest::lib::io::mqtt {

mqtt_client::mqtt_client(std::uint32_t reconnect_to_ms, [[maybe_unused]] std::string client_id) : mosquitto_cpp() {
    set_option_threaded(false);
    set_option_tcpnodelay(true);
    m_reconnect_timer.set_timeout_ms(reconnect_to_ms);
}

mqtt_client::~mqtt_client() {
    // This fixes late adding actions to the event_handler
    mosquitto_cpp::set_callback_disconnect_impl([](auto&, auto, auto const&) {});
}

void mqtt_client::handle_socket(event_list const& events) {
    ErrorCode ec{ErrorCode::Success};
    if (events.count(event::poll_events::read)) {
        ec = loop_read();
    }
    if (ec == ErrorCode::Success and events.count(event::poll_events::write)) {
        ec = loop_write();
        if (not want_write()) {
            listen_to_write_events(false);
        }
    }
    if (ec == ErrorCode::Success) {
        ec = loop_misc();
    }
    if (ec == ErrorCode::Success and want_write()) {
        listen_to_write_events(true);
    }
    if (ec not_eq ErrorCode::Success) {
    }
}

void mqtt_client::handle_reconnect_timer() {
    m_handler.add_action([this] {
        if (m_last_socket not_eq -1) {
            m_handler.unregister_event_handler(m_last_socket);
            m_last_socket = -1;
        }
        auto result = reconnect();
        if (result == ErrorCode::Success) {
            m_last_socket = socket();
            if (m_last_socket not_eq -1) {
                m_handler.register_event_handler(
                    socket(), [this](auto const& events) { handle_socket(events); }, event::poll_events::read);
                listen_to_write_events_if_wanted();
                listen_to_reconnect_timer(false);
            }
        }
        listen_to_write_events_if_wanted();
        handle_error(result);
    });
}

void mqtt_client::listen_to_reconnect_timer(bool enable) {
    m_handler.add_action([this, enable]() {
        if (enable) {
            m_reconnect_timer.reset();
            m_handler.register_event_handler(&m_reconnect_timer, [this](auto const&) { handle_reconnect_timer(); });
        } else {
            m_handler.unregister_event_handler(&m_reconnect_timer);
        }
    });
}

ErrorCode mqtt_client::handle_error(ErrorCode error) {
    if (not m_error_handler) {
        return error;
    }
    if (m_last_error == error) {
        return error;
    }
    m_last_error = error;
    if (error == ErrorCode::Errno) {
        m_error_handler(errno, strerror(errno));
    } else {
        m_error_handler(static_cast<int>(error), static_cast<std::string>(to_string(error)));
    }
    return error;
}

void mqtt_client::listen_to_write_events(bool enable) {
    m_handler.add_action([this, enable]() {
        auto action = enable ? event::event_modification::add : event::event_modification::remove;
        m_handler.modify_event_handler(socket(), event::poll_events::write, action);
    });
}

void mqtt_client::listen_to_write_events_if_wanted() {
    if (want_write()) {
        listen_to_write_events(true);
    }
}

void mqtt_client::set_error_handler(const cb_error& handler) {
    m_error_handler = handler;
    // setting dummy callbacks. Actual error_handling callbacks are integrated in the called functions
    // The check ensures that set_error_handler and set_callback_xxxxx can be called in any order
    if (not is_connect_callback_set()) {
        set_callback_connect([](auto&, auto, auto, auto const&) {});
    }
    if (not is_disconnect_callback_set()) {
        set_callback_disconnect([](auto&, auto, auto const&) {});
    }
}

///// fd_event_sync_interface implementation

int mqtt_client::get_poll_fd() {
    return m_handler.get_poll_fd();
}

everest::lib::io::event::sync_status mqtt_client::sync() {
    m_handler.run_once();
    return everest::lib::io::event::sync_status::ok;
};

///// mosquitto_cpp override

ErrorCode mqtt_client::connect_impl(const std::string_view& host, std::uint16_t port, std::uint16_t keepalive_seconds) {
    auto result = mosquitto_cpp::connect_impl(host, port, keepalive_seconds);
    if (result == ErrorCode::Success) {
        m_last_socket = socket();
        m_handler.register_event_handler(
            socket(), [this](auto const& events) { handle_socket(events); }, event::poll_events::read);
        listen_to_write_events_if_wanted();
    } else {
        listen_to_reconnect_timer(true);
    }
    return handle_error(result);
}

ErrorCode mqtt_client::set_will_impl(const std::string_view& topic, const std::string_view& payload, QoS qos,
                                     bool retain, PropertiesBase&& props) {
    auto result = mosquitto_cpp::set_will_impl(topic, payload, qos, retain, std::move(props));
    listen_to_write_events_if_wanted();
    return handle_error(result);
}

ErrorCode mqtt_client::publish_impl(int* mid, const std::string_view& topic, const std::string_view& payload, QoS qos,
                                    bool retain, const PropertiesBase& props) {
    auto result = ErrorCode::NoConnection;
    if (m_connected) {
        result = mosquitto_cpp::publish_impl(mid, topic, payload, qos, retain, props);
        listen_to_write_events_if_wanted();
        handle_error(result);
    }
    return result;
}

ErrorCode mqtt_client::subscribe_impl(const std::string_view& topic, QoS qos, int options, const PropertiesBase& props,
                                      subscribe_callback cb) {
    auto result = mosquitto_cpp::subscribe_impl(topic, qos, options, props, std::move(cb));
    listen_to_write_events_if_wanted();
    return handle_error(result);
}

ErrorCode mqtt_client::unsubscribe_impl(const std::string_view& topic, const PropertiesBase& props) {
    auto result = mosquitto_cpp::unsubscribe_impl(topic, props);
    listen_to_write_events_if_wanted();
    return handle_error(result);
}

void mqtt_client::set_callback_connect_impl(connect_callback cb) {
    mosquitto_cpp::set_callback_connect_impl(
        [this, cb = std::move(cb)](auto& cb_client, auto rc, auto flags, auto const& props) {
            m_connected = true;
            if (cb) {
                cb(cb_client, rc, flags, props);
            }
            handle_error(ErrorCode::Success);
        });
}

void mqtt_client::set_callback_disconnect_impl(disconnect_callback cb) {
    mosquitto_cpp::set_callback_disconnect_impl(
        [this, cb = std::move(cb)](auto& cb_client, auto ec, auto const& props) {
            m_connected = false;
            listen_to_reconnect_timer(true);
            if (cb) {
                cb(cb_client, ec, props);
            }
            handle_error(ec);
        });
}

void mqtt_client::set_callback_publish_impl(publish_callback cb) {
    mosquitto_cpp::set_callback_publish_impl(cb);
}

} // namespace everest::lib::io::mqtt
