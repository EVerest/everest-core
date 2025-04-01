#include "rpcHandler.hpp"

#include <jsonrpccxx/server.hpp>

namespace rpc {
// RpcHandler just needs just a tranport interface array
RpcHandler::RpcHandler(std::vector<std::shared_ptr<server::TransportInterface>> transportInterfaces)
    : m_transport_interfaces(std::move(transportInterfaces)) {
    for (const auto& transport_interface : m_transport_interfaces) {
        if (!transport_interface) {
            throw std::runtime_error("Transport interface is null");
        }
        transport_interface->on_client_connected = std::bind(&RpcHandler::client_connected, this,
            std::placeholders::_1, std::placeholders::_2); 

        transport_interface->on_client_disconnected = std::bind(&RpcHandler::client_disconnected, this,
            std::placeholders::_1);

        transport_interface->on_data_available = std::bind(&RpcHandler::data_available, this,
            std::placeholders::_1, std::placeholders::_2);
    }

}

void RpcHandler::client_connected(const server::TransportInterface::ClientId &client_id,
                         const server::TransportInterface::Address &address) {
    // Handle client connected logic here
}
void RpcHandler::client_disconnected(const server::TransportInterface::ClientId &client_id) {
    // Handle client disconnected logic here
}
void RpcHandler::data_available(const server::TransportInterface::ClientId &client_id,
                         const server::TransportInterface::Data &data) {
    // Handle data available logic here
    for (const auto& transport_interface : m_transport_interfaces) {
        transport_interface->send_data(client_id, data);
    }
}

void RpcHandler::start_server() {
    for (const auto& transport_interface : m_transport_interfaces) {
        if (!transport_interface->start_server()) {
            throw std::runtime_error("Failed to start transport interface server");
        }
    }
}

void RpcHandler::stop_server() {
    for (const auto& transport_interface : m_transport_interfaces) {
        if (!transport_interface->stop_server()) {
            throw std::runtime_error("Failed to stop transport interface server");
        }
    }
}
}
