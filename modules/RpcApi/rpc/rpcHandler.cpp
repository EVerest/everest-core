#include "rpcHandler.hpp"

#include <jsonrpccxx/server.hpp>
#include <thread>


namespace rpc {
// RpcHandler just needs just a tranport interface array
RpcHandler::RpcHandler(std::vector<std::shared_ptr<server::TransportInterface>> transport_interfaces)
    : m_transport_interfaces(std::move(transport_interfaces)) {
    for (const auto& transport_interface : m_transport_interfaces) {
        if (!transport_interface) {
            throw std::runtime_error("Transport interface is null");
        }
        transport_interface->on_client_connected = [this, transport_interface](
            const server::TransportInterface::ClientId &client_id,
            const server::TransportInterface::Address &address) {
            this->client_connected(transport_interface, client_id, address);
        };

        transport_interface->on_client_disconnected = std::bind(&RpcHandler::client_disconnected, this,
            std::placeholders::_1);

        transport_interface->on_data_available = std::bind(&RpcHandler::data_available, this,
            std::placeholders::_1, std::placeholders::_2);
    }
}

void RpcHandler::client_connected(const std::shared_ptr<server::TransportInterface> &transport_interfaces, const server::TransportInterface::ClientId &client_id,
                                  const server::TransportInterface::Address &address) {
    // In case of a new client, we expect that the client will send a API.Hello request within 5 seconds.
    // If not, we will close the connection.
    // Start a timer for 5 seconds and wait for condition variable

    std::thread wait_for_hello([this, client_id, transport_interfaces]() {
        std::unique_lock<std::mutex> lock(m_mtx);
        if (m_cv.wait_for(lock, std::chrono::seconds(5), [this, client_id] {
            return m_client_hello_received.find(client_id) != m_client_hello_received.end();
        })) {
            // Client sent hello
            m_client_hello_received.erase(client_id);
        } else {
            // Client did not send hello, close connection
            transport_interfaces->kill_client_connection(client_id, "Disconnected due to timeout");
        }
    });
    wait_for_hello.detach();
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
