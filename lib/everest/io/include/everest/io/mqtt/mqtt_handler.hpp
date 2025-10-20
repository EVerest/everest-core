// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

/** \file */
#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include <everest/io/event/unique_fd.hpp>
#include <everest/io/mqtt/dataset.hpp>

namespace everest::lib::io::mqtt {

class MqttClientHandle;

/**
 * mqtt_handler bundles basic <a href="https://mqtt.org/">MQTT</a> functionality implemented
 * via <a href="https://github.com/LiamBindle/MQTT-C?tab=readme-ov-file">MQTT-C</a>.
 * This includes lifetime management, reading, writing and fundamental error checking.<br>
 * Although this class can be used on its own, the main purpose is to implement the
 * \p ClientPolicy of \ref event::fd_event_client
 */
class mqtt_handler {
public:
    /**
     * @var PayloadT
     * @brief Type of the payload for TX and RX operations
     */
    using PayloadT = Dataset;

    /**
     * @brief The class is default constructed
     */
    mqtt_handler();
    ~mqtt_handler();

    /**
     * @brief Open a TCP socket and connect to mqtt broker
     * @details Sets the socket non blocking and enabled tcp no-delay.
     * Implementation for \p ClientPolicy
     * @param[in] server Adress of the mqtt-broker
     * @param[in] port Port of the mqtt-broker
     * @return Description
     */
    bool open(std::string const& server, std::uint16_t port);

    /**
     * @brief Send a dataset
     * @details  Implementation for \p ClientPolicy
     * @param[in] data Dataset with payload and destination
     * @return True on success, False otherwise
     */
    bool tx(PayloadT const& data);
    /**
     * @brief Receive a dataset
     * @details  Implementation for \p ClientPolicy
     * @param[in] data Dataset with payload and source
     * @return True on success, False otherwise
     */
    bool rx(PayloadT& data);

    /**
     * @brief Get the file descriptor of the socket
     * @details Implementation for \p ClientPolicy
     * @return The file descriptor of the socket
     */
    int get_fd() const;

    /**
     * @brief Get pending errors on the socket.
     * @details Implementation for \p ClientPolicy
     * @return The current errno of the socket. Zero with no pending error.
     */
    int get_error() const;

    /* Extra functionality  */

    /**
     * @brief Subscribe to a topic
     * @details Subscribe to a topic
     * @param[in] topic The topic to be subscribed.
     * @return True on success, False otherwise
     */
    bool subscribe(std::string const& topic);

    /**
     * @brief Unsubscribe from a topic
     * @details Unsubscribe from a topic
     * @param[in] topic The topic to unsubscribed from.
     * @return True on success, False otherwise
     */
    bool unsubscribe(std::string const& topic);

    /**
     * @brief Ping the broker
     * @details Ping regularly to keep the connection alive.
     * @return True on success, False otherwise.
     */
    bool ping();

    /**
     * @brief Sync the <a href="https://github.com/LiamBindle/MQTT-C?tab=readme-ov-file">MQTT-C</a>
     * implementation
     * @details The internal implementation queues all messages internally. Calling mqtt_sync
     * does the actual work
     * @return True on success, False otherwise.
     */
    bool mqtt_sync();

    /**
     * @brief Check and reset the current status of tx_data_received flag
     * @details Not all reads return mqtt data. Return the status and reset it to false;
     * @return The current data status
     */
    bool reset_current_item_is_data();

private:
    event::unique_fd m_socket{};
    std::unique_ptr<MqttClientHandle> m_handle{nullptr};
    PayloadT* rx_buffer{nullptr};
    bool rx_data_received{false};
};

} // namespace everest::lib::io::mqtt
