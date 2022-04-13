// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_HPP
#define OCPP_WEBSOCKET_HPP

#include <thread>

#include <ocpp1_6/websocket/websocket_plain.hpp>
#include <ocpp1_6/websocket/websocket_tls.hpp>

namespace ocpp1_6 {

class ChargePointConfiguration;

///
/// \brief contains a websocket abstraction that can connect to TLS and non-TLS websocket endpoints
///
class Websocket {
private:
    std::unique_ptr<WebsocketBase> websocket;
    std::string uri;
    bool shutting_down;

public:
    /// \brief Creates a new Websocket object with the providede \p configuration
    explicit Websocket(std::shared_ptr<ChargePointConfiguration> configuration);

    /// \brief connect to a websocket (TLS or non-TLS depending on the central system uri in the configuration)
    /// \returns true if the websocket is initialized and a connection attempt is made
    bool connect();

    /// \brief disconnect the websocket
    void disconnect();

    /// \brief register a \p callback that is called when the websocket is connected successfully
    void register_connected_callback(const std::function<void()>& callback);

    /// \brief register a \p callback that is called when the websocket is disconnected
    void register_disconnected_callback(const std::function<void()>& callback);

    /// \brief register a \p callback that is called when the websocket receives a message
    void register_message_callback(const std::function<void(const std::string& message)>& callback);

    /// \brief send a \p message over the websocket
    /// \returns true if the message was sent successfully
    bool send(const std::string& message);
};

} // namespace ocpp1_6
#endif // OCPP_WEBSOCKET_HPP
