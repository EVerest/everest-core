// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef RPCHANDLER_H
#define RPCHANDLER_H

#include <string>
#include <vector>
#include <functional>
#include <cstdint>
#include <optional>
#include <stdexcept>
#include <memory>
#include <boost/uuid/uuid.hpp>

#include "server/transportInterface.hpp"

using namespace server;

namespace rpc {

class RpcHandler
{
public:
    // Consturctor and Destructor
    RpcHandler() = delete;
    // RpcHandler just needs just a tranport interface array
    RpcHandler(std::vector<std::shared_ptr<server::TransportInterface>> transportInterfaces);
    ~RpcHandler() = default;
    // Methods
    void start_server();
    void stop_server();

    void client_connected(const server::TransportInterface::ClientId &client_id,
                        const server::TransportInterface::Address &address);
    void client_disconnected(const server::TransportInterface::ClientId &client_id);
    void data_available(const server::TransportInterface::ClientId &client_id,
                        const server::TransportInterface::Data &data);

    // Members
    std::vector<std::shared_ptr<server::TransportInterface>> m_transport_interfaces;

};
}

#endif // RPCHANDLER_H
