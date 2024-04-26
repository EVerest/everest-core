// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_TLS_HPP
#define OCPP_WEBSOCKET_TLS_HPP

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

#include <ocpp/common/evse_security.hpp>
#include <ocpp/common/websocket/websocket_base.hpp>

namespace ocpp {

typedef websocketpp::client<websocketpp::config::asio_tls_client> tls_client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> tls_context;

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

///
/// \brief contains a websocket abstraction that can connect to TLS and non-TLS websocket endpoints
///
class WebsocketTLS final : public WebsocketBase {
private:
    tls_client wss_client;
    std::shared_ptr<EvseSecurity> evse_security;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> websocket_thread;

    websocketpp::connection_hdl handle;
    websocketpp::transport::timer_handler reconnect_callback;

    /// \brief Called when a TLS websocket connection gets initialized, manages the supported TLS versions, cipher lists
    /// and how verification of the server certificate is handled
    tls_context on_tls_init(std::string hostname, websocketpp::connection_hdl hdl, int32_t security_profile);

    /// \brief Verify that the csms certificate's commonName matches the CSMS FQDN
    bool verify_csms_cn(const std::string& hostname, bool preverified, boost::asio::ssl::verify_context& ctx);

    /// \brief Connect to a TLS websocket
    void connect_tls();

    /// \brief Called when a TLS websocket connection is established, calls the connected callback
    void on_open_tls(tls_client* c, websocketpp::connection_hdl hdl);

    /// \brief Called when a message is received over the TLS websocket, calls the message callback
    void on_message_tls(websocketpp::connection_hdl hdl, tls_client::message_ptr msg);

    /// \brief Called when a TLS websocket connection is closed
    void on_close_tls(tls_client* c, websocketpp::connection_hdl hdl);

    /// \brief Called when a TLS websocket connection fails to be established
    void on_fail_tls(tls_client* c, websocketpp::connection_hdl hdl);

    void set_connection_options(const WebsocketConnectionOptions& connection_options) override;

public:
    /// \brief Creates a new Websocket object with the providede \p connection_options
    explicit WebsocketTLS(const WebsocketConnectionOptions& connection_options,
                          std::shared_ptr<EvseSecurity> evse_security);

    ~WebsocketTLS() {
        this->websocket_thread->join();
    }

    /// \brief connect to a TLS websocket
    /// \returns true if the websocket is initialized and a connection attempt is made
    bool connect() override;

    /// \brief Reconnects the websocket using the delay, a reason for this reconnect can be provided with the
    /// \param reason parameter
    /// \param delay delay of the reconnect attempt
    void reconnect(long delay) override;

    /// \brief closes the websocket
    void close(const WebsocketCloseReason code, const std::string& reason) override;

    /// \brief send a \p message over the websocket
    /// \returns true if the message was sent successfully
    bool send(const std::string& message) override;

    /// \brief send a websocket ping
    void ping() override;
};

} // namespace ocpp
#endif // OCPP_WEBSOCKET_HPP
