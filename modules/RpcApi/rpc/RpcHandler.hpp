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
#include <unordered_map>
#include <mutex>
#include <condition_variable>

#include "../server/TransportInterface.hpp"

using namespace server;

namespace rpc {

static const std::chrono::seconds CLIENT_HELLO_TIMEOUT(5);

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

    void client_connected(const std::shared_ptr<server::TransportInterface> &transport_interfaces, const TransportInterface::ClientId &client_id, const TransportInterface::Address &address);
    void client_disconnected(const server::TransportInterface::ClientId &client_id);
    void data_available(const TransportInterface::ClientId &client_id,
                        const TransportInterface::Data &data);

    // Members
    std::vector<std::shared_ptr<TransportInterface>> m_transport_interfaces;

private:
    std::mutex m_mtx;
    std::condition_variable m_cv;
    std::unordered_map<TransportInterface::ClientId, bool> m_client_hello_received;

};
}

#endif // RPCHANDLER_H
