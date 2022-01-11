// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
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
    std::shared_ptr<ChargePointConfiguration> configuration;
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

    /// \brief Indicates if the required callbacks are registered and the websocket is not shutting down
    /// \returns true if the websocket is properly initialized
    bool initialized();

    /// \brief Reconnects the websocket using the reconnect timer, a reason for this reconnect can be provided with the
    /// \p reason parameter
    void reconnect(std::error_code reason);

    /// \brief Extracts the hostname from the provided \p uri
    /// FIXME(kai): this only works with a very limited subset of hostnames and should be extended to work spec conform
    /// \returns the extracted hostname
    std::string get_hostname(std::string uri);

    // plaintext websocket

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

    // TLS websocket

    /// \brief FIXME: this should verify the certificate
    /// FIXME(kai): implement validation!
    /// \returns FIXME: always true at the moment because no certificate verification is performed
    bool verify_certificate(std::string hostname, bool preverified, boost::asio::ssl::verify_context& context);

    /// \brief Called when a TLS websocket connection gets initialized, manages the supported TLS versions, cipher lists
    /// and how verification of the server certificate is handled
    tls_context on_tls_init(std::string hostname, websocketpp::connection_hdl hdl);

    /// \brief Connect to a TLS websocket, if the provided \p authorization_header is not empty use it to add a HTTP
    /// Authorization header
    void connect_tls(std::string authorization_header);

    /// \brief Called when a TLS websocket connection is established, calls the connected callback
    void on_open_tls(tls_client* c, websocketpp::connection_hdl hdl);

    /// \brief Called when a message is received over the TLS websocket, calls the message callback
    void on_message_tls(websocketpp::connection_hdl hdl, tls_client::message_ptr msg);

    /// \brief Called when a TLS websocket connection is closed, tries to reconnect
    void on_close_tls(tls_client* c, websocketpp::connection_hdl hdl);

    /// \brief Called when a TLS websocket connection fails to be established, tries to reconnect
    void on_fail_tls(tls_client* c, websocketpp::connection_hdl hdl);

    /// \brief Closes a TLS websocket connection
    void close_tls(websocketpp::close::status::value code, const std::string& reason);

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
