// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_HPP
#define OCPP_WEBSOCKET_HPP

#include <fstream>
#include <iostream>
#include <thread>

#include <ocpp/common/ocpp_logging.hpp>
#include <ocpp/common/pki_handler.hpp>
#include <ocpp/common/websocket/websocket_plain.hpp>
#include <ocpp/common/websocket/websocket_tls.hpp>

namespace ocpp {
///
/// \brief contains a websocket abstraction that can connect to TLS and non-TLS websocket endpoints
///
class Websocket {
private:
    std::unique_ptr<WebsocketBase> websocket;
    std::function<void(const int security_profile)> connected_callback;
    std::function<void()> disconnected_callback;
    std::function<void(const std::string& message)> message_callback;
    std::shared_ptr<MessageLogging> logging;

public:
    /// \brief Creates a new Websocket object with the provided \p connection_options
    explicit Websocket(const WebsocketConnectionOptions& connection_options, std::shared_ptr<PkiHandler> pki_handler,
                       std::shared_ptr<MessageLogging> logging);
    ~Websocket();

    /// \brief connect to a websocket (TLS or non-TLS depending on the central system uri in the configuration). If \p
    /// try_once is set to true, to establish a connection will only be tried once and will be no reconnect attempts.
    /// This mechanism is used to switch back to a working security profile if a connection for a new security profile
    /// cant be established. \returns true if the websocket is initialized and a connection attempt is made
    bool connect(int32_t security_profile, bool try_once = false);

    /// \brief disconnect the websocket
    void disconnect(websocketpp::close::status::value code);

    // \brief reconnects the websocket after the delay
    void reconnect(std::error_code reason, long delay);

    /// \brief indicates if the websocket is connected
    bool is_connected();

    /// \brief register a \p callback that is called when the websocket is connected successfully
    void register_connected_callback(const std::function<void(const int security_profile)>& callback);

    /// \brief register a \p callback that is called when the websocket is disconnected
    void register_disconnected_callback(const std::function<void()>& callback);

    /// \brief register a \p callback that is called when the websocket receives a message
    void register_message_callback(const std::function<void(const std::string& message)>& callback);

    /// \brief send a \p message over the websocket
    /// \returns true if the message was sent successfully
    bool send(const std::string& message);

    /// \brief set the websocket ping interval \p interval_s in seconds
    void set_websocket_ping_interval(int32_t interval_s);
};

} // namespace ocpp
#endif // OCPP_WEBSOCKET_HPP
