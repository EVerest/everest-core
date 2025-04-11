// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "RpcApi.hpp"

#include <boost/uuid/uuid_io.hpp>

namespace module {

void RpcApi::init() {
    invoke_init(*p_main);

    for (const auto& evse_manager : r_evse_manager) {
        // create on DataStore object per EVSE
        this->data.emplace_back();
    }

    m_websocket_server = std::make_unique<server::WebSocketServer>(config.websocket_tls_enabled, config.websocket_port);
    // Create RpcHandler instance. Move the transport interfaces to the RpcHandler
    std::vector<std::shared_ptr<server::TransportInterface>> transport_interfaces;
    transport_interfaces.push_back(std::shared_ptr<server::TransportInterface>(std::move(m_websocket_server)));
    m_rpc_server = std::make_unique<rpc::RpcHandler>(std::move(transport_interfaces));
}

void RpcApi::ready() {
    // Start the WebSocket server
    m_rpc_server->start_server();

    invoke_ready(*p_main);
}

} // namespace module
