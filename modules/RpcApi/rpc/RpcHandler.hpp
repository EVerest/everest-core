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
#include <jsonrpccxx/server.hpp>
#include <deque>
#include <atomic>

#include "../server/TransportInterface.hpp"
#include "methods/Api.hpp"

using namespace server;
using namespace jsonrpccxx;

namespace rpc {

static const std::chrono::seconds CLIENT_HELLO_TIMEOUT(5);

// struct to store json data, plus the tansport interface
struct ClientReq
{
    std::shared_ptr<server::TransportInterface> transport_interface;
    std::deque<nlohmann::json> data; // Queue of requests
};

class RpcHandler
{
public:
    // Consturctor and Destructor
    RpcHandler() = delete;
    // RpcHandler just needs just a tranport interface array
    RpcHandler(std::vector<std::shared_ptr<server::TransportInterface>> transportInterfaces, DataStoreCharger &dataobj);
    ~RpcHandler() = default;
    // Methods
    void start_server();
    void stop_server();

private:
    void init_rpc_api();
    void init_transport_interfaces();
    void client_connected(const std::shared_ptr<server::TransportInterface> &transport_interfaces, const TransportInterface::ClientId &client_id, const TransportInterface::Address &address);
    void client_disconnected(const std::shared_ptr<server::TransportInterface> &transport_interfaces, const server::TransportInterface::ClientId &client_id);
    void data_available(const std::shared_ptr<server::TransportInterface> &transport_interfaces,
                        const TransportInterface::ClientId &client_id,
                        const TransportInterface::Data &data);
    inline bool is_client_hello_req(const TransportInterface::ClientId &client_id,
                                    const nlohmann::json &request) {
        // Check if the request is a hello request
        if (request.contains("method") && request["method"] == methods::METHOD_API_HELLO) {
            // If it's a API.Hello request, we set the client_hello_received flag to true
            // and notify the condition variable to unblock the waiting thread
            m_client_hello_received[client_id] = true;
            return true;
        }
        return false;
    }
    void process_client_requests();

    // Members
    std::vector<std::shared_ptr<TransportInterface>> m_transport_interfaces;
    std::mutex m_mtx;
    std::condition_variable m_cv_client_hello;
    std::condition_variable m_cv_data_available;
    std::unordered_map<TransportInterface::ClientId, bool> m_client_hello_received;
    std::unique_ptr<JsonRpc2Server> m_rpc_server;
    std::unordered_map<TransportInterface::ClientId, ClientReq> messages;
    std::chrono::steady_clock::time_point m_last_req_notification; // Last tick time
    std::atomic<bool> m_is_running{false};

    methods::Api m_methods_api;
};
} // namespace rpc

#endif // RPCHANDLER_H
