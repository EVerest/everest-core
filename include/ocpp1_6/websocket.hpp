// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_HPP
#define OCPP_WEBSOCKET_HPP

#include <thread>

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

#include <everest/timer.hpp>

#include <ocpp1_6/charge_point_configuration.hpp>

namespace ocpp1_6 {

typedef websocketpp::client<websocketpp::config::asio_client> client;
typedef websocketpp::client<websocketpp::config::asio_tls_client> tls_client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> tls_context;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

///
/// \brief contains a websocket abstraction that can connect to TLS and non-TLS websocket endpoints
///
class Websocket {
private:
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> websocket_thread;
    ChargePointConfiguration* configuration;
    client ws_client;
    tls_client wss_client;
    std::string uri;
    bool tls;
    bool shutting_down;
    websocketpp::connection_hdl handle;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> ws_thread;
    std::mutex reconnect_mutex;
    long reconnect_interval_ms;
    websocketpp::transport::timer_handler reconnect_callback;
    websocketpp::lib::shared_ptr<boost::asio::steady_timer> reconnect_timer;
    std::function<void()> connected_callback;
    std::function<void()> disconnected_callback;
    std::function<void(const std::string& message)> message_callback;

    bool initialized();
    void reconnect(std::error_code reason);
    std::string get_hostname(std::string uri);

    // plaintext websocket
    void connect_plain();
    void on_open_plain(client* c, websocketpp::connection_hdl hdl);
    void on_message_plain(websocketpp::connection_hdl hdl, client::message_ptr msg);
    void on_close_plain(client* c, websocketpp::connection_hdl hdl);
    void on_fail_plain(client* c, websocketpp::connection_hdl hdl);
    void close_plain(websocketpp::close::status::value code, const std::string& reason);

    // TLS websocket
    bool verify_certificate(std::string hostname, bool preverified, boost::asio::ssl::verify_context& context);
    tls_context on_tls_init(std::string hostname, websocketpp::connection_hdl hdl);
    void connect_tls(std::string authorization_header);
    void on_open_tls(tls_client* c, websocketpp::connection_hdl hdl);
    void on_message_tls(websocketpp::connection_hdl hdl, tls_client::message_ptr msg);
    void on_close_tls(tls_client* c, websocketpp::connection_hdl hdl);
    void on_fail_tls(tls_client* c, websocketpp::connection_hdl hdl);
    void close_tls(websocketpp::close::status::value code, const std::string& reason);

public:
    Websocket(ChargePointConfiguration* configuration);
    ///
    /// \brief connect to a websocket (TLS or non-TLS depending on the uri)
    ///
    bool connect();

    ///
    /// \brief disconnect the websocket
    ///
    void disconnect();

    ///
    /// \brief register a callback that is called when the websocket is connected successfully
    ///
    void register_connected_callback(const std::function<void()>& callback);

    ///
    /// \brief register a callback that is called when the websocket is disconnected
    ///
    void register_disconnected_callback(const std::function<void()>& callback);

    ///
    /// \brief register a callback that is called when the websocket receives a message
    ///
    void register_message_callback(const std::function<void(const std::string& message)>& callback);

    ///
    /// \brief send a message over the websocket
    ///
    bool send(const std::string& message);
};

} // namespace ocpp1_6
#endif // OCPP_WEBSOCKET_HPP
