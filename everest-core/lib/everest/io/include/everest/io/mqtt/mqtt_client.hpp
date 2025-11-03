// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */

#pragma once
#include <cstdint>
#include <everest/io/event/fd_event_client.hpp>
#include <everest/io/event/fd_event_handler.hpp>
#include <everest/io/event/timer_fd.hpp>
#include <everest/io/mqtt/mqtt_handler.hpp>

namespace everest::lib::io::mqtt {

/**
 * @var mqtt_client_base
 * @brief Base type for a client for MQTT implemented in terms of \ref event::fd_event_client
 * and \ref mqtt::mqtt_handler
 */
using mqtt_client_base = event::fd_event_client<mqtt_handler>::type;

/**
 * mqtt_client extends \ref mqtt_client_base for special handling needed for MQTT like
 * subscribe and unsubscribe as well as the special syncing mechanism need for
 * <a href="https://github.com/LiamBindle/MQTT-C?tab=readme-ov-file">MQTT-C</a>.
 */
class mqtt_client : public mqtt_client_base {
public:
    /**
     * @brief Create an mqtt_client and connect it to broker
     * @param[in] server The address of the broker
     * @param[in] port The port of the broker
     * @param[in] ping_interval_ms The interval for ping messages to keep the
     * connection alive.
     */
    mqtt_client(std::string const& server, std::uint16_t port, std::uint32_t ping_interval_ms = 1000);

    /**
     * @brief Subscribe to a topic
     * @param[in] topic The topic to subscribe to
     * @return True on success, false otherwise
     */
    bool subscribe(std::string const& topic);

    /**
     * @brief Unsubscribe from a topic
     * @param[in] topic The topic to unsubscribe from
     * @return True on success, false otherwise
     */
    bool unsubscribe(std::string const& topic);

    /**
     * @brief Register a callback for message handling
     * @details The default \ref mqtt_client_base::set_rx_handler is called on any readable information
     * on the socket. This method is only called for actual messages.
     * @param[in] handler The message handler to be registered.
     */
    void set_message_handler(cb_rx const& handler);

    /**
     * @brief Special sync function
     * @details Overrides inherited sync member
     * @return Result of the operation
     */
    event::sync_status sync() override;

    /**
     * @brief Get file descriptor of internal event handler
     * @details Hides inherited get_poll_fd member
     * @return The file descriptor to be used for event handling
     */
    int get_poll_fd() override;

private:
    event::fd_event_handler m_handler;
    event::timer_fd m_timer;
};

} // namespace everest::lib::io::mqtt
