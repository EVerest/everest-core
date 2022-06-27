// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_BASE_HPP
#define OCPP_WEBSOCKET_BASE_HPP

#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

namespace ocpp1_6 {

class ChargePointConfiguration;

///
/// \brief contains a websocket abstraction
///
class WebsocketBase {
protected:
    bool shutting_down;
    bool m_is_connected;
    std::shared_ptr<ChargePointConfiguration> configuration;
    std::function<void()> connected_callback;
    std::function<void()> disconnected_callback;
    std::function<void(const std::string& message)> message_callback;
    std::function<void()> sign_certificate_callback;

    websocketpp::lib::shared_ptr<websocketpp::lib::thread> websocket_thread;
    std::string uri;
    websocketpp::connection_hdl handle;
    std::mutex reconnect_mutex;
    long reconnect_interval_ms;
    websocketpp::transport::timer_handler reconnect_callback;
    websocketpp::lib::shared_ptr<boost::asio::steady_timer> reconnect_timer;

    /// \brief Indicates if the required callbacks are registered and the websocket is not shutting down
    /// \returns true if the websocket is properly initialized
    bool initialized();

    /// \brief getter for authorization header for connection with basic authentication
    boost::optional<std::string> getAuthorizationHeader();

public:
    /// \brief Creates a new WebsocketBase object with the providede \p configuration
    explicit WebsocketBase(std::shared_ptr<ChargePointConfiguration> configuration);
    ~WebsocketBase();

    /// \brief connect to a websocket
    /// \returns true if the websocket is initialized and a connection attempt is made
    virtual bool connect(int32_t security_profile) = 0;

    /// \brief reconnect the websocket after the delay
    virtual void reconnect(std::error_code reason, long delay) = 0;

    /// \brief disconnect the websocket
    void disconnect(websocketpp::close::status::value code);

    /// \brief indicates if the websocket is connected
    bool is_connected();

    /// \brief closes the websocket
    virtual void close(websocketpp::close::status::value code, const std::string& reason) = 0;

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
    virtual bool send(const std::string& message) = 0;
};

} // namespace ocpp1_6
#endif // OCPP_WEBSOCKET_BASE_HPP
