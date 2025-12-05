// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_WEBSOCKET_TLS_TPM_HPP
#define OCPP_WEBSOCKET_TLS_TPM_HPP

#include <ocpp/common/evse_security.hpp>
#include <ocpp/common/safe_queue.hpp>
#include <ocpp/common/websocket/websocket_base.hpp>

#include <memory>
#include <optional>
#include <string>

struct ssl_ctx_st;

namespace ocpp {

struct ConnectionData;
struct WebsocketMessage;

/// \brief Experimental libwebsockets TLS connection
class WebsocketLibwebsockets final : public WebsocketBase {
public:
    /// \brief Creates a new Websocket object with the providede \p connection_options
    explicit WebsocketLibwebsockets(const WebsocketConnectionOptions& connection_options,
                                    std::shared_ptr<EvseSecurity> evse_security);

    ~WebsocketLibwebsockets();

    void set_connection_options(const WebsocketConnectionOptions& connection_options) override;

    bool start_connecting() override;

    void reconnect(long delay) override;

    void close(const WebsocketCloseReason code, const std::string& reason) override;

    bool send(const std::string& message) override;

    void ping() override;

    /// \brief Indicates if the websocket has a valid connection data and is trying to
    ///        connect/reconnect internally even if for the moment it might not be connected
    /// \return True if the websocket is connected or trying to connect, false otherwise
    bool is_trying_to_connect();

public:
    int process_callback(void* wsi_ptr, int callback_reason, void* user, void* in, size_t len);

private:
    bool is_trying_to_connect_internal();
    void close_internal(const WebsocketCloseReason code, const std::string& reason);

private:
    /// \brief Initializes the connection options, including the security info
    /// \return True if it was successful, false otherwise
    bool initialize_connection_options(std::shared_ptr<ConnectionData>& new_connection_data);

    bool tls_init(struct ssl_ctx_st* ctx, const std::string& path_chain, const std::string& path_key, bool custom_key,
                  std::optional<std::string>& password);

    /// \brief Websocket processing thread loop
    void thread_websocket_client_loop(std::shared_ptr<ConnectionData> local_data);

    /// \brief Function to handle received messages. Required since from the received message
    ///        callback we also send messages that must block and wait on the client thread
    void thread_websocket_message_recv_loop(std::shared_ptr<ConnectionData> local_data);

    /// \brief Function to handle the deferred callbacks
    void thread_deferred_callback_queue();

    /// \brief Called when a TLS websocket connection is established, calls the connected callback
    void on_conn_connected(ConnectionData* conn_data);

    /// \brief Called when a TLS websocket connection is closed
    void on_conn_close(ConnectionData* conn_data);

    /// \brief Called when a TLS websocket connection fails to be established
    void on_conn_fail(ConnectionData* conn_data);

    /// \brief When the connection can send data
    void on_conn_writable();

    /// \brief Called when a message is received over the TLS websocket, calls the message callback
    void on_conn_message(std::string&& message);

    /// \brief Requests a message write, awakes the websocket loop from 'poll'
    void request_write();

    void poll_message(const std::shared_ptr<WebsocketMessage>& msg);

    /// \brief Add a callback to the queue of callbacks to be executed. All will be executed from a single thread
    void push_deferred_callback(const std::function<void()>& callback);

    // \brief Safely closes the already running connection threads
    void safe_close_threads();

    /// \brief Clears all messages and message queues both incoming and outgoing
    void clear_all_queues();

private:
    std::shared_ptr<EvseSecurity> evse_security;

    // Connection related data
    Everest::SteadyTimer reconnect_timer_tpm;
    std::unique_ptr<std::thread> websocket_thread;
    std::shared_ptr<ConnectionData> conn_data;

    // Queue of outgoing messages, notify thread only when we remove messages
    SafeQueue<std::shared_ptr<WebsocketMessage>> message_queue;

    std::unique_ptr<std::thread> recv_message_thread;
    SafeQueue<std::string> recv_message_queue;
    std::string recv_buffered_message;

    std::unique_ptr<std::thread> deferred_callback_thread;
    SafeQueue<std::function<void()>> deferred_callback_queue;
    std::atomic_bool stop_deferred_handler;

    OcppProtocolVersion connected_ocpp_version;
};

} // namespace ocpp
#endif // OCPP_WEBSOCKET_HPP
