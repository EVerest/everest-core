// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "RpcApi.hpp"
#include "server/transportInterface.hpp"

#include <boost/uuid/uuid_io.hpp>

namespace module {

void RpcApi::init() {
    invoke_init(*p_main);
}

void RpcApi::ready() {
    invoke_ready(*p_main);

    using namespace server;

    // Start the WebSocket server
    m_websocket_server = std::make_unique<server::WebSocketServer>(config.websocket_tls_enabled, config.websocket_port);
    m_websocket_server->on_client_connected = [](const TransportInterface::ClientId& client_id, const server::TransportInterface::Address& address) {
        // Handle client connected logic here
        EVLOG_info << "on_client_connected: Client " << boost::uuids::to_string(client_id) << " connected from " << address;
    };

    m_websocket_server->on_client_disconnected = [](const TransportInterface::ClientId& client_id) {
        // Handle client disconnected logic here
        EVLOG_info << "on_client_disconnected: Client " << boost::uuids::to_string(client_id) << " disconnected";
    };

    m_websocket_server->on_data_available = [](const TransportInterface::ClientId& client_id, const server::TransportInterface::Data& data) {
        // Handle data available logic here
        EVLOG_info << "on_data_available: Received data from client " << boost::uuids::to_string(client_id) << " with size " << data.size();
    };
    m_websocket_server->start_server();
}

} // namespace module
