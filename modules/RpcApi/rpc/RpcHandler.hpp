// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef RPCHANDLER_H
#define RPCHANDLER_H

#include <atomic>
#include <boost/uuid/uuid.hpp>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <jsonrpccxx/server.hpp>
#include <memory>
#include <mutex>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "../server/TransportInterface.hpp"
#include "methods/Api.hpp"
#include "methods/ChargePoint.hpp"
#include "methods/Evse.hpp"

using namespace server;
using namespace jsonrpccxx;

namespace rpc {

static const std::chrono::seconds CLIENT_HELLO_TIMEOUT(5);

// struct to store json data, plus the tansport interface
struct ClientReq {
    std::shared_ptr<server::TransportInterface> transport_interface;
    std::deque<nlohmann::json> data; // Queue of requests
};

class RpcHandler {
public:
    // Consturctor and Destructor
    RpcHandler() = delete;
    // RpcHandler just needs just a tranport interface array
    RpcHandler(std::vector<std::shared_ptr<server::TransportInterface>> transportInterfaces, DataStoreCharger& dataobj);
    ~RpcHandler() = default;
    // Methods
    void start_server();
    void stop_server();

private:
    void init_rpc_api();
    void init_transport_interfaces();
    void client_connected(const std::shared_ptr<server::TransportInterface>& transport_interfaces,
                          const TransportInterface::ClientId& client_id, const TransportInterface::Address& address);
    void client_disconnected(const std::shared_ptr<server::TransportInterface>& transport_interfaces,
                             const server::TransportInterface::ClientId& client_id);
    void data_available(const std::shared_ptr<server::TransportInterface>& transport_interfaces,
                        const TransportInterface::ClientId& client_id, const TransportInterface::Data& data);
    inline bool is_api_hello_req(const TransportInterface::ClientId& client_id, const nlohmann::json& request) {
        // Check if the request is a hello request
        if (request.contains("method") && request["method"] == methods::METHOD_API_HELLO) {
            // If it's a API.Hello request, we set the api_hello_received flag to true
            // and notify the condition variable to unblock the waiting thread
            m_api_hello_received[client_id] = true;
            return true;
        }
        return false;
    }
    void process_client_requests();

    // Members
    std::vector<std::shared_ptr<TransportInterface>> m_transport_interfaces;
    std::mutex m_mtx;
    std::condition_variable m_cv_api_hello;
    std::condition_variable m_cv_data_available;
    std::unordered_map<TransportInterface::ClientId, bool> m_api_hello_received;
    std::unique_ptr<JsonRpc2Server> m_rpc_server;
    std::unordered_map<TransportInterface::ClientId, ClientReq> messages;
    std::chrono::steady_clock::time_point m_last_req_notification; // Last tick time
    std::atomic<bool> m_is_running{false};

    methods::Api m_methods_api;
    methods::ChargePoint m_methods_chargepoint;
    methods::Evse m_methods_evse;
};
} // namespace rpc

#endif // RPCHANDLER_H
