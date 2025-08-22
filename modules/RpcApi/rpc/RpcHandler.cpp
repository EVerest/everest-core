#include "RpcHandler.hpp"

#include <jsonrpccxx/client.hpp>
#include <jsonrpccxx/server.hpp>
#include <jsonrpccxx/typemapper.hpp>
#include <jsonrpccxx/common.hpp>

#include <thread>
#include <everest/logging.hpp>
#include <boost/uuid/uuid_io.hpp>
#include "../data/DataTypes.hpp"
#include "rpcApi.hpp"

namespace rpc {

static const std::chrono::milliseconds REQ_COLLECTION_TIMEOUT(10); // Timeout for collecting client requests. After this timeout, the requests will be processed.
static const std::chrono::milliseconds REQ_PROCESSING_TIMEOUT(50); // Timeout for processing requests. After this timeout, the request will be processed.

// RpcHandler just needs just a tranport interface array
RpcHandler::RpcHandler(std::vector<std::shared_ptr<server::TransportInterface>> transport_interfaces)
    : m_transport_interfaces(std::move(transport_interfaces)) {
    init_rpc_api();
    init_transport_interfaces();
}

struct HelloResponse {
    bool success;
    bool authenticated;
};

void to_json(nlohmann::json& j, const HelloResponse& r) {
    j = nlohmann::json{
        {"success", r.success},
        {"authenticated", r.authenticated}
    };
}

void from_json(const nlohmann::json& j, HelloResponse& r) {
    j.at("success").get_to(r.success);
    j.at("authenticated").get_to(r.authenticated);
}

nlohmann::json hello(const nlohmann::json& params) {
    HelloResponse res{true, false}; // Beispielwerte
    return nlohmann::json(res);
}
using namespace jsonrpccxx;

void RpcHandler::init_rpc_api() {
    // Initialize the RPC API here
    RpcApi api;
    m_rpc_server = std::make_unique<JsonRpc2Server>();
//    m_rpc_server->Add("API.Hello1", GetHandle(&data::API_Hello, this), {});
    m_rpc_server->Add("API.Hello", GetHandle(&rpc::RpcApi::hello, api), {});
}

void RpcHandler::init_transport_interfaces() {
    for (const auto& transport_interface : m_transport_interfaces) {
        if (!transport_interface) {
            throw std::runtime_error("Transport interface is null");
        }
        m_last_tick = std::chrono::steady_clock::now();

        transport_interface->on_client_connected = [this, transport_interface](
            const server::TransportInterface::ClientId &client_id,
            const server::TransportInterface::Address &address) {
            this->client_connected(transport_interface, client_id, address);
        };

        transport_interface->on_client_disconnected = [this, transport_interface](
            const server::TransportInterface::ClientId &client_id) {
            this->client_disconnected(transport_interface, client_id);
        };

        transport_interface->on_data_available = [this, transport_interface](
            const server::TransportInterface::ClientId &client_id,
            const server::TransportInterface::Data data) {
            this->data_available(transport_interface, client_id, data);
        };
    }
}

void RpcHandler::client_connected(const std::shared_ptr<server::TransportInterface> &transport_interface, const server::TransportInterface::ClientId &client_id,
                                  const server::TransportInterface::Address &address) {
    // In case of a new client, we expect that the client will send an API.Hello request within 5 seconds.
    // The API.Hello request is a handshake message sent by the client to establish a connection and verify compatibility.
    // If the API.Hello request is not received within the timeout period, the connection will be terminated.

    // Launch a detached thread to wait for the client hello message
    std::thread([this, client_id, transport_interface]() {
        std::unique_lock<std::mutex> lock(m_mtx);
        if (m_cv_client_hello.wait_for(lock, CLIENT_HELLO_TIMEOUT, [this, client_id] {
            return m_client_hello_received.find(client_id) != m_client_hello_received.end();
        })) {
            // Client sent hello
            m_client_hello_received.erase(client_id);
        } else {
            // Client did not send hello, close connection
            if (transport_interface) {
                transport_interface->kill_client_connection(client_id, "Disconnected due to timeout");
            } else {
                // Log the error instead of throwing an exception in a detached thread
                // to avoid undefined behavior.
                EVLOG_error << "Transport interface is null during client connection timeout handling";
            }
        }
    }).detach();
}

void RpcHandler::client_disconnected(const std::shared_ptr<server::TransportInterface> &transport_interface, const server::TransportInterface::ClientId &client_id) {
    if (transport_interface) {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_client_hello_received.erase(client_id);
    } else {
        // Log the error instead of throwing an exception in a detached thread
        // to avoid undefined behavior.
        EVLOG_error << "Transport interface is null during client disconnection handling";
    }
    // Clean up the client data
    std::lock_guard<std::mutex> lock(m_mtx);
    auto it = messages.find(client_id);
    if (it != messages.end()) {
        messages.erase(it);
    }
}

void RpcHandler::data_available(const std::shared_ptr<server::TransportInterface> &transport_interface,
                                const server::TransportInterface::ClientId &client_id,
                                const server::TransportInterface::Data &data) {
    // Handle request
    using namespace std::chrono;

    try {
        auto now = steady_clock::now();

        nlohmann::json request = nlohmann::json::parse(data);
        if (request.is_null()) {
            EVLOG_error << "Received null request from client " << client_id;
            return;
        }
        // Store message in a map with client_id as key
        std::lock_guard<std::mutex> lock(m_mtx);
        messages[client_id].data.push_back(request);
        messages[client_id].transport_interface = transport_interface;
        EVLOG_debug << "Received message from client " << client_id << ": " << request.dump();

        auto elapsed = duration_cast<milliseconds>(now - m_last_tick);
        if (elapsed >= REQ_COLLECTION_TIMEOUT) {
            m_last_tick = now;  // Timer neu starten
            m_cv_data_available.notify_all();
            EVLOG_info << "Processing data available for client " << client_id;
        }
    } catch (const nlohmann::json::parse_error& e) {
        EVLOG_error << "Failed to parse JSON request from client " << client_id << ": " << e.what();
    } catch (const std::exception& e) {
        EVLOG_error << "Exception occurred while handling data available: " << e.what();
    }
}

void RpcHandler::start_server() {
    for (const auto& transport_interface : m_transport_interfaces) {
        if (!transport_interface->start_server()) {
            throw std::runtime_error("Failed to start transport interface server");
        }
    }

    // Start RPC receiver thread
    std::thread([this]() {
        while (true) {
            std::unique_lock<std::mutex> lock(m_mtx);
            // Wait for data to be available or timeout
            m_cv_data_available.wait_for(lock, REQ_PROCESSING_TIMEOUT, [this]() {
                // Interate over all clients and check if data is available
                for (const auto& [client_id, client_data] : messages) {
                    if (!client_data.data.empty()) {
                        return true;
                    }
                }
                return false;
            });

            // Process data for all clients

            bool all_requests_processed;
            do {
                all_requests_processed = true;
                for(auto& [client_id, client_data] : messages) {
                    if (!client_data.data.empty()) {
                        // Process the data for this client
                        auto transport_interface = client_data.transport_interface;
                        if (transport_interface) {
                            // Get the first request from the client
                            nlohmann::json request = client_data.data.front();
                            client_data.data.pop_front();  // Remove the processed request

                            // Check if next request is available
                            if (client_data.data.empty()) {
                                all_requests_processed = false;
                            }

                            // Check if it's a hello request
                            if (request.contains("method") && request["method"] == "API.Hello") {
                                // If it's a API.Hello request, we set the client_hello_received flag to true
                                // and notify the condition variable to unblock the waiting thread
                                //std::lock_guard<std::mutex> lock(m_mtx);
                                m_client_hello_received[client_id] = true;
                                EVLOG_debug << "Received API.Hello request from client " << client_id;
                                // Notify condition variable to unblock the waiting thread
                                m_cv_client_hello.notify_all();
                            }

                            // Call the RPC server with the request
                            std::string res = m_rpc_server->HandleRequest(request.dump());
                            // Send the response back to the client
                            transport_interface->send_data(client_id, res);
                            EVLOG_info << "Sent response to client " << client_id << ": " << res;
                        } else {
                            EVLOG_error << "Transport interface is null for client " << client_id;
                        }
                    }
                }
            } while (!all_requests_processed);
        }
    }).detach();
}

void RpcHandler::stop_server() {
    for (const auto& transport_interface : m_transport_interfaces) {
        if (!transport_interface->stop_server()) {
            throw std::runtime_error("Failed to stop transport interface server");
        }
    }
}
}
