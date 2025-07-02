// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#include "WebsocketServer.hpp"

#include <iostream>
#include <unordered_map>
#include <everest/logging.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace server {

static const int PER_SESSION_DATA_SIZE {4096};

static void log_callback(int level, const char *line) {
    switch (level) {
        case LLL_ERR:
            EVLOG_error << line;
            break;
        case LLL_WARN:
            EVLOG_warning << line;
            break;
        default:
        case LLL_DEBUG:
            EVLOG_debug << line;
            break;
    }
}

int WebSocketServer::callback_ws(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    struct lws_context *context = lws_get_context(wsi);
    WebSocketServer *server = static_cast<WebSocketServer *>(lws_context_user(context));

    if (!server) {
        throw std::runtime_error("Error: WebSocketServer instance not found!");
    }

    std::unique_lock<std::mutex> lock(server->m_clients_mutex); // To protect access to m_clients

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: {
            // Generate a random UUID for the client
            boost::uuids::uuid client_id = boost::uuids::random_generator()();
            server->m_clients[client_id] = wsi;

            char ip_address[INET6_ADDRSTRLEN] {0};
            if (lws_get_peer_simple(wsi, ip_address, sizeof(ip_address)) == NULL) {
                EVLOG_warning << "Failed to get client IP address";
            }

            lock.unlock();  // Unlock before calling the callback
            server->on_client_connected(client_id, ip_address);  // Call the on_client_connected callback
            lock.lock();  // Lock again after the callback
            EVLOG_info << "Client " << boost::uuids::to_string(client_id) << " connected"
                       << (strlen(ip_address) > 0 ? (" from " + std::string(ip_address)) : "");
            break;
        }
        case LWS_CALLBACK_CLOSED: {
            for (auto it = server->m_clients.begin(); it != server->m_clients.end(); ++it) {
                if (it->second == wsi) {
                    lock.unlock();  // Unlock before calling the callback
                    EVLOG_info << "Client " << it->first << " disconnected";
                    server->on_client_disconnected(it->first);  // Call the on_client_disconnected callback
                    lock.lock();  // Lock again after the callback
                    server->m_clients.erase(it);
                    break;
                }
            }
            break;
        }
        case LWS_CALLBACK_RECEIVE: {
            for (auto it = server->m_clients.begin(); it != server->m_clients.end(); ++it) {
                if (it->second == wsi) {
                    unsigned char *data = (unsigned char *)in;
                    std::vector<uint8_t> received_data(data, data + len);
                    lock.unlock();  // Unlock before calling the callback
                    server->on_data_available(it->first, received_data);  // Call the on_data_available callback
                    lock.lock();  // Lock again after the callback
                    break;
                }
            }
            break;
        }
        default:
            break;
    }

    lock.unlock();  // Unlock the mutex after processing the callback

    return 0;
}

WebSocketServer::WebSocketServer(bool ssl_enabled, int port, const std::string& iface)
    : m_ssl_enabled(ssl_enabled) {
    // Constructor implementation
    lws_set_log_level(LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO | LLL_DEBUG, log_callback);
    m_info.port = port;
    if (iface != "") {
        // create a persistent copy
        m_iface = new char[iface.size() + 1];
        memcpy(m_iface, iface.c_str(), iface.size() + 1);
        m_info.iface = m_iface;
    }
    m_lws_protocols[0] = { "EVerestRpcApi", callback_ws, PER_SESSION_DATA_SIZE, 0 };
    m_lws_protocols[1] = LWS_PROTOCOL_LIST_TERM;

    m_info.protocols = m_lws_protocols;
    m_info.options = LWS_SERVER_OPTION_FAIL_UPON_UNABLE_TO_BIND;
    m_info.options |= (m_ssl_enabled ? LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT : 0);
    m_info.user = this; // To access WebSocketServer instance in callback
}

WebSocketServer::~WebSocketServer() {
    stop_server();
    delete[] m_iface;
}

bool WebSocketServer::running() const {
    return m_running;
}

// send data to all connected clients
void WebSocketServer::send_data(const std::vector<uint8_t>& data) {
    std::lock_guard<std::mutex> lock(m_clients_mutex);

    for (const auto& client : m_clients) {
        struct lws* wsi = client.second;
        send_data(wsi, data);
    }
}

// send data to client identified by ClientId
void WebSocketServer::send_data(const ClientId& client_id, const std::vector<uint8_t>& data) {
    try {
        std::lock_guard<std::mutex> lock(m_clients_mutex);

        auto it = m_clients.find(client_id);
        if (it == m_clients.end()) {
            EVLOG_error << "Client " << client_id << " not found";
            return;
        }

        struct lws* wsi = it->second;
        send_data(wsi, data);
    } catch (const std::exception& e) {
        EVLOG_error << "Exception occurred while sending data to client " << client_id << ": " << e.what();
    }
}

// send data to client identified by libwebsockets wsi
void WebSocketServer::send_data(struct lws* wsi, const std::vector<uint8_t>& data) {
    try {
        std::vector<unsigned char> buf(LWS_PRE + data.size());
        memcpy(buf.data() + LWS_PRE, data.data(), data.size());

        if (lws_write(wsi, buf.data() + LWS_PRE, data.size(), LWS_WRITE_BINARY) < 0) {
            EVLOG_error << "Failed to send data to client";
        }
    } catch (const std::exception& e) {
        // Note: the code in the try{} block probably cannot throw an exception
        EVLOG_error << "Exception occurred while sending data to client: " << e.what();
    }
}

void WebSocketServer::kill_client_connection(const ClientId &client_id, const std::string &kill_reason) {
    std::lock_guard<std::mutex> lock(m_clients_mutex);

    auto it = m_clients.find(client_id);
    if (it != m_clients.end()) {
        struct lws *wsi = it->second;
        std::string close_reason = kill_reason.empty() ? "Connection closed by server" : kill_reason;
        lws_close_reason(wsi, LWS_CLOSE_STATUS_PROTOCOL_ERR,
            reinterpret_cast<unsigned char*>(const_cast<char*>(close_reason.data())), close_reason.size());
        lws_set_timeout(wsi, PENDING_TIMEOUT_CLOSE_SEND, LWS_TO_KILL_ASYNC); // Set timeout to close the connection
        lws_callback_on_writable(wsi); // Notify the event loop to close the connection
        EVLOG_info << "Client " << client_id << " connection closed (reason: " << kill_reason << ")";
        m_clients.erase(it);  // Remove client from map
    } else {
        EVLOG_error << "Client ID " << client_id << " not found!";
    }
}

uint WebSocketServer::connections_count() const {
    std::lock_guard<std::mutex> lock(m_clients_mutex);
    return m_clients.size();
}

bool WebSocketServer::start_server() {
    if (m_running) {
        EVLOG_warning << "WebSocket Server is already running";
        return true;
    }

    m_context = lws_create_context(&m_info);
    if (!m_context) {
        EVLOG_error << "Failed to create WebSocket context";
        return false;
    }

    EVLOG_info << "WebSocket Server running on port " << m_info.port
               << (m_info.iface ? " (interface \"" + std::string(m_info.iface) + "\" only)" : "")
               << (m_ssl_enabled ? " with" : " without") << " TLS";

    m_server_thread = std::thread([this]() {
        m_running = true;
        while (m_running) {
            lws_service(m_context, 1000);
        }
    });

    while(!m_running) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10)); // Wait for server to start
    }

    return true; // Server started successfully
}

bool WebSocketServer::stop_server() {
    if (!m_running) {
        return true;
    }
    m_running = false;

    lws_cancel_service(m_context); // To unblock the server thread loop immediately

    if (m_server_thread.joinable()) {
        m_server_thread.join();  // Wait for server thread to finish
    }

    if (m_context) {
        lws_context_destroy(m_context);
        m_context = nullptr;
    }
    EVLOG_info << "WebSocket Server stopped";

    return true;
}

} // namespace server
