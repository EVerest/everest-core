// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_TLS_TPM_HPP
#define OCPP_WEBSOCKET_TLS_TPM_HPP

#include <ocpp/common/evse_security.hpp>
#include <ocpp/common/websocket/websocket_base.hpp>

#include <condition_variable>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <string>

struct ssl_ctx_st;

namespace ocpp {

struct ConnectionData;
struct WebsocketMessage;

/// \brief Experimental libwebsockets TLS connection
class WebsocketTlsTPM final : public WebsocketBase {
public:
    /// \brief Creates a new Websocket object with the providede \p connection_options
    explicit WebsocketTlsTPM(const WebsocketConnectionOptions& connection_options,
                             std::shared_ptr<EvseSecurity> evse_security);

    ~WebsocketTlsTPM();

    void set_connection_options(const WebsocketConnectionOptions& connection_options) override;

    /// \brief connect to a TLS websocket
    /// \returns true if the websocket is initialized and a connection attempt is made
    bool connect() override;

    /// \brief Reconnects the websocket using the delay, a reason for this reconnect can be provided with the
    /// \param reason parameter
    /// \param delay delay of the reconnect attempt
    void reconnect(std::error_code reason, long delay) override;

    /// \brief closes the websocket
    void close(websocketpp::close::status::value code, const std::string& reason) override;

    /// \brief send a \p message over the websocket
    /// \returns true if the message was sent successfully
    bool send(const std::string& message) override;

    /// \brief send a websocket ping
    void ping() override;

public:
    int process_callback(void* wsi_ptr, int callback_reason, void* user, void* in, size_t len);

private:
    void tls_init(struct ssl_ctx_st* ctx, const std::string& path_chain, const std::string& path_key, bool tpm_key,
                  std::optional<std::string>& password);
    void client_loop();
    void recv_loop();

    /// \brief Called when a TLS websocket connection is established, calls the connected callback
    void on_conn_connected();

    /// \brief Called when a TLS websocket connection is closed
    void on_conn_close();

    /// \brief Called when a TLS websocket connection fails to be established
    void on_conn_fail();

    /// \brief When the connection can send data
    void on_writable();

    /// \brief Called when a message is received over the TLS websocket, calls the message callback
    void on_message(std::string&& message);

    void request_write();

    void poll_message(const std::shared_ptr<WebsocketMessage>& msg);

private:
    std::shared_ptr<EvseSecurity> evse_security;

    // Connection related data
    Everest::SteadyTimer reconnect_timer_tpm;
    std::unique_ptr<std::thread> websocket_thread;
    std::shared_ptr<ConnectionData> conn_data;
    std::condition_variable conn_cv;

    std::mutex queue_mutex;

    std::queue<std::shared_ptr<WebsocketMessage>> message_queue;
    std::condition_variable msg_send_cv;
    std::mutex msg_send_cv_mutex;

    std::unique_ptr<std::thread> recv_message_thread;
    std::mutex recv_mutex;
    std::queue<std::string> recv_message_queue;
    std::condition_variable recv_message_cv;
    std::string recv_buffered_message;
};

} // namespace ocpp
#endif // OCPP_WEBSOCKET_HPP
