// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_PLAIN_HPP
#define OCPP_WEBSOCKET_PLAIN_HPP

#include <thread>

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

#include <ocpp/common/websocket/websocket_base.hpp>

namespace ocpp {

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

///
/// \brief contains a websocket abstraction that can connect to plaintext websocket endpoints (ws://)
///
class WebsocketPlain final : public WebsocketBase {
private:
    client ws_client;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> websocket_thread;

    /// \brief Connect to a plain websocket
    void connect_plain();

    /// \brief Called when a plaintext websocket connection is established, calls the connected callback
    void on_open_plain(client* c, websocketpp::connection_hdl hdl);

    /// \brief Called when a message is received over the plaintext websocket, calls the message callback
    void on_message_plain(websocketpp::connection_hdl hdl, client::message_ptr msg);

    /// \brief Called when a plaintext websocket connection is closed
    void on_close_plain(client* c, websocketpp::connection_hdl hdl);

    /// \brief Called when a plaintext websocket connection fails to be established
    void on_fail_plain(client* c, websocketpp::connection_hdl hdl);

public:
    /// \brief Creates a new WebsocketPlain object with the providede \p connection_options
    explicit WebsocketPlain(const WebsocketConnectionOptions& connection_options);

    ~WebsocketPlain() {
        this->websocket_thread->join();
    }

    /// \brief connect to a plaintext websocket
    /// \returns true if the websocket is initialized and a connection attempt is made
    bool connect() override;

    /// \brief Reconnects the websocket using the delay, a reason for this reconnect can be provided with the
    /// \p reason parameter
    void reconnect(std::error_code reason, long delay) override;

    /// \brief Closes a plaintext websocket connection
    void close(websocketpp::close::status::value code, const std::string& reason) override;

    /// \brief send a \p message over the websocket
    /// \returns true if the message was sent successfully
    bool send(const std::string& message) override;

    /// \brief send a websocket ping
    void ping() override;
};

} // namespace ocpp
#endif // OCPP_WEBSOCKET_PLAIN_HPP
