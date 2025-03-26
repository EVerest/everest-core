// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#include "websocketServer.hpp"

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

    std::lock_guard<std::mutex> lock(server->m_clients_mutex);

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: {
            // Generate a random UUID for the client
            boost::uuids::uuid client_id = boost::uuids::random_generator()();
            server->m_clients[client_id] = wsi;

            char ip_address[INET6_ADDRSTRLEN] {0};
            if (lws_get_peer_simple(wsi, ip_address, sizeof(ip_address)) == NULL) {
                EVLOG_warning << "Failed to get client IP address";
            }

            server->on_client_connected(client_id, ip_address);  // Call the on_client_connected callback
            EVLOG_info << "Client " << boost::uuids::to_string(client_id) << " connected";
            break;
        }
        case LWS_CALLBACK_CLOSED: {
            for (auto it = server->m_clients.begin(); it != server->m_clients.end(); ++it) {
                if (it->second == wsi) {
                    server->on_client_disconnected(it->first);  // Call the on_client_disconnected callback
                    EVLOG_info << "Client " << it->first << " disconnected";
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
                    server->on_data_available(it->first, received_data);  // Call the on_data_available callback
                    break;
                }
            }
            break;
        }
        default:
            break;
    }
    return 0;
}

WebSocketServer::WebSocketServer(bool ssl_enabled, int port)
    : m_ssl_enabled(ssl_enabled) {
    // Constructor implementation
    lws_set_log_level(LLL_ERR | LLL_WARN | LLL_NOTICE | LLL_INFO | LLL_DEBUG, log_callback);
    m_info.port = port;
    m_lws_protocols[0] = { "EVerestRpcApi", callback_ws, PER_SESSION_DATA_SIZE, 0 };
    m_lws_protocols[1] = LWS_PROTOCOL_LIST_TERM;

    m_info.protocols = m_lws_protocols;
    m_info.options = m_ssl_enabled ? LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT : 0;
    m_info.user = this; // To access WebSocketServer instance in callback
}

WebSocketServer::~WebSocketServer() {
    stop_server();
}

bool WebSocketServer::running() const {
    return m_running;
}

void WebSocketServer::send_data(const ClientId &client_id, const std::vector<uint8_t> &data) {
    try {
        std::lock_guard<std::mutex> lock(m_clients_mutex);

        auto it = m_clients.find(client_id);
        if (it == m_clients.end()) {
            EVLOG_error << "Client " << client_id << " not found";
            return;
        }

        struct lws *wsi = it->second;
        std::vector<unsigned char> buf(LWS_PRE + data.size());
        memcpy(buf.data() + LWS_PRE, data.data(), data.size());

        if (lws_write(wsi, buf.data() + LWS_PRE, data.size(), LWS_WRITE_BINARY) < 0) {
            EVLOG_error << "Failed to send data to client " << client_id;
        }
    } catch (const std::exception &e) {
        EVLOG_error << "Exception occurred while sending data to client " << client_id << ": " << e.what();
    }
}

void WebSocketServer::kill_client_connection(const ClientId &client_id, const std::string &kill_reason) {
    std::lock_guard<std::mutex> lock(m_clients_mutex);

    auto it = m_clients.find(client_id);
    if (it != m_clients.end()) {
        struct lws *wsi = it->second;
        std::string close_reason = kill_reason.empty() ? "Connection closed by server" : kill_reason;

        lws_close_reason(wsi, LWS_CLOSE_STATUS_ABNORMAL_CLOSE,
            reinterpret_cast<unsigned char*>(const_cast<char*>(close_reason.data())), close_reason.size());
        m_clients.erase(it);  // Remove client from map

        EVLOG_info << "Client " << client_id << " connection closed (reason:" << kill_reason << ")";
    } else {
        EVLOG_error << "Client ID " << client_id << " not found!";
    }
}

uint WebSocketServer::connections_count() const {
    std::lock_guard<std::mutex> lock(m_clients_mutex);
    return m_clients.size();
}

bool WebSocketServer::start_server() {
    m_context = lws_create_context(&m_info);
    if (!m_context) {
        EVLOG_error << "Failed to create WebSocket context";
        return false;
    }

    EVLOG_info << "WebSocket Server running on port " << m_info.port << (m_ssl_enabled? " with TLS": " without TLS");

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

    if (m_server_thread.joinable()) {
        m_server_thread.join();  // Wait for server thread to finish
    }
    if (m_context) {
        lws_context_destroy(m_context);
        m_context = nullptr;
    }

    m_clients.clear();  // Clear the clients map
    EVLOG_info << "WebSocket Server stopped";

    return true;
}

} // namespace server
