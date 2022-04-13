// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_PLAIN_HPP
#define OCPP_WEBSOCKET_PLAIN_HPP

#include <thread>

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

#include <ocpp1_6/websocket/websocket_base.hpp>

namespace ocpp1_6 {

class ChargePointConfiguration;

typedef websocketpp::client<websocketpp::config::asio_client> client;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

///
/// \brief contains a websocket abstraction that can connect to plaintext websocket endpoints (ws://)
///
class WebsocketPlain : public WebsocketBase {
private:
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> websocket_thread;
    client ws_client;
    std::string uri;
    websocketpp::connection_hdl handle;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> ws_thread;
    std::mutex reconnect_mutex;
    long reconnect_interval_ms;
    websocketpp::transport::timer_handler reconnect_callback;
    websocketpp::lib::shared_ptr<boost::asio::steady_timer> reconnect_timer;

    /// \brief Reconnects the websocket using the reconnect timer, a reason for this reconnect can be provided with the
    /// \p reason parameter
    void reconnect(std::error_code reason);

    /// \brief Connect to a plaintext websocket
    void connect_plain();

    /// \brief Called when a plaintext websocket connection is established, calls the connected callback
    void on_open_plain(client* c, websocketpp::connection_hdl hdl);

    /// \brief Called when a message is received over the plaintext websocket, calls the message callback
    void on_message_plain(websocketpp::connection_hdl hdl, client::message_ptr msg);

    /// \brief Called when a plaintext websocket connection is closed, tries to reconnect
    void on_close_plain(client* c, websocketpp::connection_hdl hdl);

    /// \brief Called when a plaintext websocket connection fails to be established, tries to reconnect
    void on_fail_plain(client* c, websocketpp::connection_hdl hdl);

    /// \brief Closes a plaintext websocket connection
    void close_plain(websocketpp::close::status::value code, const std::string& reason);

public:
    /// \brief Creates a new WebsocketPlain object with the providede \p configuration
    explicit WebsocketPlain(std::shared_ptr<ChargePointConfiguration> configuration);

    /// \brief connect to a plaintext websocket
    /// \returns true if the websocket is initialized and a connection attempt is made
    bool connect() override;

    /// \brief disconnect the websocket
    void disconnect() override;

    /// \brief send a \p message over the websocket
    /// \returns true if the message was sent successfully
    bool send(const std::string& message) override;
};

} // namespace ocpp1_6
#endif // OCPP_WEBSOCKET_PLAIN_HPP
