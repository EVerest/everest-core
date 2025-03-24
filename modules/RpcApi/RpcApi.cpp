// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "RpcApi.hpp"

namespace module {

void RpcApi::init() {
    invoke_init(*p_main);
}

void RpcApi::ready() {
    invoke_ready(*p_main);

    // Start the WebSocket server
    m_websocket_server = std::make_unique<server::WebSocketServer>(false, 8080);
    m_websocket_server->start_server();
}

} // namespace module
