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

int WebSocketServer::callback_ws(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
    WebSocketServer *server = static_cast<WebSocketServer *>(lws_context_user(lws_get_context(wsi)));

    if (!server) {
        EVLOG_error << "Error: WebSocketServer instance not found!";
        return 0;
    }

    switch (reason) {
        case LWS_CALLBACK_ESTABLISHED: {
            // Generate a random UUID for the client
            boost::uuids::uuid client_id = boost::uuids::random_generator()();

            {
                std::lock_guard<std::mutex> lock(server->m_clients_mutex);
                server->m_clients[client_id] = wsi;
            }

            EVLOG_info << "Client " << boost::uuids::to_string(client_id) << " connected";
            break;
        }
        case LWS_CALLBACK_CLOSED: {
            std::lock_guard<std::mutex> lock(server->m_clients_mutex);
            for (auto it = server->m_clients.begin(); it != server->m_clients.end(); ++it) {
                if (it->second == wsi) {
                    EVLOG_info << "Client " << it->first << " disconnected";
                    server->m_clients.erase(it);
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
    m_info.port = port;
    m_lws_protocols[0] = { "http", callback_ws, 0, PER_SESSION_DATA_SIZE };
    m_lws_protocols[1] = { NULL, NULL, 0, 0 };

    m_info.protocols = m_lws_protocols;
    m_info.options = m_ssl_enabled ? LWS_SERVER_OPTION_DO_SSL_GLOBAL_INIT : 0;
}

WebSocketServer::~WebSocketServer() {
    stop_server();
}

bool WebSocketServer::running() const {
    return m_running;
}

void WebSocketServer::send_data(const ClientId &client_id, const std::vector<uint8_t> &data) {
    std::lock_guard<std::mutex> lock(m_clients_mutex);

    auto it = m_clients.find(client_id);
    if (it == m_clients.end()) {
        EVLOG_error << "Client " << client_id << " not found" << std::endl;
        return;
    }

    struct lws *wsi = it->second;
    unsigned char *buf = new unsigned char[LWS_PRE + data.size()];
    memcpy(buf + LWS_PRE, data.data(), data.size());

    lws_write(wsi, buf + LWS_PRE, data.size(), LWS_WRITE_BINARY);
    delete[] buf;
}

void WebSocketServer::kill_client_connection(const ClientId &client_id, const std::string &kill_reason) {
    std::lock_guard<std::mutex> lock(m_clients_mutex);

    auto it = m_clients.find(client_id);
    if (it != m_clients.end()) {
        struct lws *wsi = it->second;

        unsigned char * close_msg = new unsigned char[kill_reason.size() + 1];
        strncpy((char *)close_msg, kill_reason.c_str(), kill_reason.size());

        lws_close_reason(wsi, LWS_CLOSE_STATUS_ABNORMAL_CLOSE, close_msg, kill_reason.size());
        m_clients.erase(it);  // Remove client from map

        EVLOG_info << "Client " << client_id << " connection closed (reason:" << kill_reason << ")";
    } else {
        EVLOG_error << "Client ID " << client_id << " not found!";
    }
}

uint WebSocketServer::connections_count() const {
}

bool WebSocketServer::start_server() {
    m_context = lws_create_context(&m_info);
    if (!m_context) {
        EVLOG_error << "Failed to create WebSocket context";
        return false;
    }

    EVLOG_info << "WebSocket Server running on port " << m_info.port << m_ssl_enabled? " with TLS": " without TLS";

    m_server_thread = std::thread([this]() {
        while (m_running) {
            lws_service(m_context, 1000);
        }
    });

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

    return true;
}

void WebSocketServer::on_client_connected() {
}

void WebSocketServer::on_client_disconnected(const ClientId &client_id) {
}

void WebSocketServer::on_data_received(const ClientId &client_id, const std::vector<uint8_t> &data) {
}
} // namespace server
