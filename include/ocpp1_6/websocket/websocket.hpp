// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_HPP
#define OCPP_WEBSOCKET_HPP

#include <fstream>
#include <iostream>
#include <thread>

#include <ocpp1_6/ocpp_logging.hpp>
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
    std::shared_ptr<ChargePointConfiguration> configuration;
    std::function<void()> connected_callback;
    std::function<void()> disconnected_callback;
    std::function<void(const std::string& message)> message_callback;
    std::function<void()> sign_certificate_callback;
    std::shared_ptr<MessageLogging> logging;

public:
    /// \brief Creates a new Websocket object with the providede \p configuration
    explicit Websocket(std::shared_ptr<ChargePointConfiguration> configuration, int32_t security_profile,
                       std::shared_ptr<MessageLogging> logging);
    ~Websocket();

    /// \brief connect to a websocket (TLS or non-TLS depending on the central system uri in the configuration)
    /// \returns true if the websocket is initialized and a connection attempt is made
    bool connect(int32_t security_profile);

    /// \brief disconnect the websocket
    void disconnect(websocketpp::close::status::value code);

    // \brief reconnects the websocket after the delay
    void reconnect(std::error_code reason, long delay);

    /// \brief indicates if the websocket is connected
    bool is_connected();

    /// \brief register a \p callback that is called when the websocket is connected successfully
    void register_connected_callback(const std::function<void()>& callback);

    /// \brief register a \p callback that is called when the websocket is disconnected
    void register_disconnected_callback(const std::function<void()>& callback);

    /// \brief register a \p callback that is called when the websocket receives a message
    void register_message_callback(const std::function<void(const std::string& message)>& callback);

    /// \brief register a \p callback that is called when the chargepoint should send a certificate signing request
    void register_sign_certificate_callback(const std::function<void()>& callback);

    /// \brief send a \p message over the websocket
    /// \returns true if the message was sent successfully
    bool send(const std::string& message);
};

} // namespace ocpp1_6
#endif // OCPP_WEBSOCKET_HPP
