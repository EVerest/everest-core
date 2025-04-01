// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "RpcApi.hpp"

#include <boost/uuid/uuid_io.hpp>

namespace module {

void RpcApi::init() {
    invoke_init(*p_main);
}

void RpcApi::ready() {
    invoke_ready(*p_main);

    using namespace server;
    using namespace rpc;

    // Start the WebSocket server
    m_websocket_server = std::make_unique<server::WebSocketServer>(config.websocket_tls_enabled, config.websocket_port);
    // Create RpcHandler instance. Move the transport interfaces to the RpcHandler
    std::vector<std::shared_ptr<server::TransportInterface>> transport_interfaces;
    transport_interfaces.push_back(std::shared_ptr<server::TransportInterface>(std::move(m_websocket_server)));
    m_rpc_server = std::make_unique<RpcHandler>(std::move(transport_interfaces));
    m_rpc_server->start_server();
}

} // namespace module
