// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_BASE_HPP
#define OCPP_WEBSOCKET_BASE_HPP

#include <functional>
#include <memory>
#include <mutex>
#include <thread>

#include <everest/timer.hpp>
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

#include <ocpp/common/types.hpp>
#include <ocpp/common/websocket/websocket_uri.hpp>

namespace ocpp {

struct WebsocketConnectionOptions {
    OcppProtocolVersion ocpp_version;
    Uri csms_uri;         // the URI of the CSMS
    int security_profile; // FIXME: change type to `SecurityProfile`
    std::optional<std::string> authorization_key;
    int retry_backoff_random_range_s;
    int retry_backoff_repeat_times;
    int retry_backoff_wait_minimum_s;
    int max_connection_attempts;
    std::string supported_ciphers_12;
    std::string supported_ciphers_13;
    int ping_interval_s;
    std::string ping_payload;
    int pong_timeout_s;
    bool use_ssl_default_verify_paths;
    std::optional<bool> additional_root_certificate_check;
    std::optional<std::string> hostName;
    bool verify_csms_common_name;
    bool use_tpm_tls;
};

enum class ConnectionFailedReason {
    InvalidCSMSCertificate = 0,
};

///
/// \brief contains a websocket abstraction
///
class WebsocketBase {
protected:
    bool m_is_connected;
    WebsocketConnectionOptions connection_options;
    std::function<void(const int security_profile)> connected_callback;
    std::function<void()> disconnected_callback;
    std::function<void(const websocketpp::close::status::value reason)> closed_callback;
    std::function<void(const std::string& message)> message_callback;
    std::function<void(ConnectionFailedReason)> connection_failed_callback;
    websocketpp::lib::shared_ptr<boost::asio::steady_timer> reconnect_timer;
    std::unique_ptr<Everest::SteadyTimer> ping_timer;
    websocketpp::connection_hdl handle;
    std::mutex reconnect_mutex;
    std::mutex connection_mutex;
    long reconnect_backoff_ms;
    websocketpp::transport::timer_handler reconnect_callback;
    int connection_attempts;
    bool shutting_down;
    bool reconnecting;

    /// \brief Indicates if the required callbacks are registered
    /// \returns true if the websocket is properly initialized
    bool initialized();

    /// \brief getter for authorization header for connection with basic authentication
    std::optional<std::string> getAuthorizationHeader();

    /// \brief Logs websocket connection error
    void log_on_fail(const std::error_code& ec, const boost::system::error_code& transport_ec, const int http_status);

    /// \brief Calculates and returns the reconnect interval based on int retry_backoff_random_range_s,
    /// retry_backoff_repeat_times, int retry_backoff_wait_minimum_s of the WebsocketConnectionOptions
    long get_reconnect_interval();

    // \brief cancels the reconnect timer
    void cancel_reconnect_timer();

    /// \brief send a websocket ping
    virtual void ping() = 0;

    /// \brief Called when a websocket pong timeout is received
    void on_pong_timeout(websocketpp::connection_hdl hdl, std::string msg);

public:
    /// \brief Creates a new WebsocketBase object. The `connection_options` must be initialised with
    /// `set_connection_options()`
    explicit WebsocketBase();
    virtual ~WebsocketBase();

    /// \brief connect to a websocket
    /// \returns true if the websocket is initialized and a connection attempt is made
    virtual bool connect() = 0;

    /// \brief sets this connection_options to the given \p connection_options and resets the connection_attempts
    virtual void set_connection_options(const WebsocketConnectionOptions& connection_options) = 0;
    void set_connection_options_base(const WebsocketConnectionOptions& connection_options);

    /// \brief reconnect the websocket after the delay
    virtual void reconnect(std::error_code reason, long delay) = 0;

    /// \brief disconnect the websocket
    void disconnect(websocketpp::close::status::value code);

    /// \brief indicates if the websocket is connected
    bool is_connected();

    /// \brief closes the websocket
    virtual void close(websocketpp::close::status::value code, const std::string& reason) = 0;

    /// \brief register a \p callback that is called when the websocket is connected successfully
    void register_connected_callback(const std::function<void(const int security_profile)>& callback);

    /// \brief register a \p callback that is called when the websocket connection is disconnected
    void register_disconnected_callback(const std::function<void()>& callback);

    /// \brief register a \p callback that is called when the websocket connection has been closed and will not attempt
    /// to reconnect
    void register_closed_callback(const std::function<void(const websocketpp::close::status::value reason)>& callback);

    /// \brief register a \p callback that is called when the websocket receives a message
    void register_message_callback(const std::function<void(const std::string& message)>& callback);

    /// \brief register a \p callback that is called when the websocket could not connect with a specific reason
    void register_connection_failed_callback(const std::function<void(ConnectionFailedReason)>& callback);

    /// \brief send a \p message over the websocket
    /// \returns true if the message was sent successfully
    virtual bool send(const std::string& message) = 0;

    /// \brief starts a timer that sends a websocket ping at the given \p interval_s
    void set_websocket_ping_interval(int32_t interval_s);

    /// \brief set the \p authorization_key of the connection_options
    void set_authorization_key(const std::string& authorization_key);
};

} // namespace ocpp
#endif // OCPP_WEBSOCKET_BASE_HPP
