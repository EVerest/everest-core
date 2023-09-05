// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_HPP
#define OCPP_WEBSOCKET_HPP

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
    // unique_ptr holds address of base - requires WebSocketBase to have a virtual destructor
    std::unique_ptr<WebsocketBase> websocket;
    std::function<void(const int security_profile)> connected_callback;
    std::function<void(const websocketpp::close::status::value reason)> closed_callback;
    std::function<void(const std::string& message)> message_callback;
    std::shared_ptr<MessageLogging> logging;

public:
    /// \brief Creates a new Websocket object with the provided \p connection_options
    explicit Websocket(const WebsocketConnectionOptions& connection_options, std::shared_ptr<PkiHandler> pki_handler,
                       std::shared_ptr<MessageLogging> logging);
    ~Websocket();

    /// \brief connect to a websocket (TLS or non-TLS depending on the central system uri in the configuration).
    bool connect();

    void set_connection_options(const WebsocketConnectionOptions& connection_options);

    /// \brief disconnect the websocket
    void disconnect(websocketpp::close::status::value code);

    // \brief reconnects the websocket after the delay
    void reconnect(std::error_code reason, long delay);

    /// \brief indicates if the websocket is connected
    bool is_connected();

    /// \brief register a \p callback that is called when the websocket is connected successfully
    void register_connected_callback(const std::function<void(const int security_profile)>& callback);

    /// \brief register a \p callback that is called when the websocket connection has been closed and will not attempt
    /// to reconnect
    void register_closed_callback(const std::function<void(const websocketpp::close::status::value reason)>& callback);

    /// \brief register a \p callback that is called when the websocket receives a message
    void register_message_callback(const std::function<void(const std::string& message)>& callback);

    /// \brief send a \p message over the websocket
    /// \returns true if the message was sent successfully
    bool send(const std::string& message);

    /// \brief set the websocket ping interval \p interval_s in seconds
    void set_websocket_ping_interval(int32_t interval_s);

    /// \brief set the \p authorization_key of the connection_options
    void set_authorization_key(const std::string& authorization_key);
};

} // namespace ocpp
#endif // OCPP_WEBSOCKET_HPP
