// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_TLS_HPP
#define OCPP_WEBSOCKET_TLS_HPP

#include <thread>

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

#include <ocpp1_6/websocket/websocket_base.hpp>

namespace ocpp1_6 {

class ChargePointConfiguration;

typedef websocketpp::client<websocketpp::config::asio_tls_client> tls_client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> tls_context;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

///
/// \brief contains a websocket abstraction that can connect to TLS and non-TLS websocket endpoints
///
class WebsocketTLS : public WebsocketBase {
private:
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> websocket_thread;
    tls_client wss_client;
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

    /// \brief Extracts the hostname from the provided \p uri
    /// FIXME(kai): this only works with a very limited subset of hostnames and should be extended to work spec conform
    /// \returns the extracted hostname
    std::string get_hostname(std::string uri);

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
    explicit WebsocketTLS(std::shared_ptr<ChargePointConfiguration> configuration);

    /// \brief connect to a TLS websocket
    /// \returns true if the websocket is initialized and a connection attempt is made
    bool connect() override;

    /// \brief disconnect the websocket
    void disconnect() override;

    /// \brief send a \p message over the websocket
    /// \returns true if the message was sent successfully
    bool send(const std::string& message) override;
};

} // namespace ocpp1_6
#endif // OCPP_WEBSOCKET_HPP
