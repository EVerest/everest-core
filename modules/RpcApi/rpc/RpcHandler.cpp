// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#include "RpcHandler.hpp"

#include <jsonrpccxx/client.hpp>
#include <jsonrpccxx/common.hpp>
#include <jsonrpccxx/server.hpp>
#include <jsonrpccxx/typemapper.hpp>

#include <boost/uuid/uuid_io.hpp>
#include <everest/logging.hpp>

namespace rpc {

static const std::chrono::milliseconds REQ_COLLECTION_TIMEOUT(
    10); // Timeout for collecting client requests. After this timeout, the requests will be processed.
static const std::chrono::milliseconds
    REQ_PROCESSING_TIMEOUT(50); // Timeout for processing requests. After this timeout, the request will be processed.

// RpcHandler just needs just a tranport interface array
RpcHandler::RpcHandler(std::vector<std::shared_ptr<server::TransportInterface>> transport_interfaces,
                       DataStoreCharger& dataobj) :
    m_transport_interfaces(std::move(transport_interfaces)),
    m_methods_api(dataobj),
    m_methods_chargepoint(dataobj),
    m_methods_evse(dataobj) {
    init_rpc_api();
    init_transport_interfaces();
}

struct HelloResponse {
    bool success;
    bool authenticated;
};

void to_json(nlohmann::json& j, const HelloResponse& r) {
    j = nlohmann::json{{"success", r.success}, {"authenticated", r.authenticated}};
}

void from_json(const nlohmann::json& j, HelloResponse& r) {
    j.at("success").get_to(r.success);
    j.at("authenticated").get_to(r.authenticated);
}

void RpcHandler::init_rpc_api() {
    // Initialize the RPC API here
    m_methods_api.set_authentication_required(false);
    m_methods_api.set_api_version(API_VERSION);
    m_rpc_server = std::make_unique<JsonRpc2Server>();
    m_rpc_server->Add(methods::METHOD_API_HELLO, GetHandle(&methods::Api::hello, m_methods_api), {});
    m_rpc_server->Add(methods::METHOD_CHARGEPOINT_GET_EVSE_INFOS,
                      GetHandle(&methods::ChargePoint::getEVSEInfos, m_methods_chargepoint), {});
    m_rpc_server->Add(methods::METHOD_EVSE_GET_INFO, GetHandle(&methods::Evse::getInfo, m_methods_evse),
                      {"evse_id"});
    m_rpc_server->Add(methods::METHOD_EVSE_GET_STATUS, GetHandle(&methods::Evse::getStatus, m_methods_evse),
                      {"evse_id"});
    m_rpc_server->Add(methods::METHOD_EVSE_GET_HARDWARE_CAPABILITIES,
                      GetHandle(&methods::Evse::getHardwareCapabilities, m_methods_evse), {"evse_id"});
    m_rpc_server->Add(methods::METHOD_EVSE_SET_CHARGING_ALLOWED,
                      GetHandle(&methods::Evse::setChargingAllowed, m_methods_evse), {"evse_id", "charging_allowed"});
}

void RpcHandler::init_transport_interfaces() {
    for (const auto& transport_interface : m_transport_interfaces) {
        if (!transport_interface) {
            throw std::runtime_error("Transport interface is null");
        }
        m_last_req_notification = std::chrono::steady_clock::now();

        transport_interface->on_client_connected =
            [this, transport_interface](const server::TransportInterface::ClientId& client_id,
                                        const server::TransportInterface::Address& address) {
                this->client_connected(transport_interface, client_id, address);
            };

        transport_interface->on_client_disconnected =
            [this, transport_interface](const server::TransportInterface::ClientId& client_id) {
                this->client_disconnected(transport_interface, client_id);
            };

        transport_interface->on_data_available =
            [this, transport_interface](const server::TransportInterface::ClientId& client_id,
                                        const server::TransportInterface::Data data) {
                this->data_available(transport_interface, client_id, data);
            };
    }
}

void RpcHandler::client_connected(const std::shared_ptr<server::TransportInterface>& transport_interface,
                                  const server::TransportInterface::ClientId& client_id,
                                  const server::TransportInterface::Address& address) {
    // In case of a new client, we expect that the client will send an API.Hello request within 5 seconds.
    // The API.Hello request is a handshake message sent by the client to establish a connection and verify
    // compatibility. If the API.Hello request is not received within the timeout period, the connection will be
    // terminated.

    // Launch a detached thread to wait for the client hello message
    std::thread([this, client_id, transport_interface]() {
        std::unique_lock<std::mutex> lock(m_mtx);
        if (m_cv_api_hello.wait_for(lock, CLIENT_HELLO_TIMEOUT, [this, client_id] {
                return m_api_hello_received.find(client_id) != m_api_hello_received.end();
            }) == false) {
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

void RpcHandler::client_disconnected(const std::shared_ptr<server::TransportInterface>& transport_interface,
                                     const server::TransportInterface::ClientId& client_id) {
    if (transport_interface) {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_api_hello_received.erase(client_id);
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

void RpcHandler::data_available(const std::shared_ptr<server::TransportInterface>& transport_interface,
                                const server::TransportInterface::ClientId& client_id,
                                const server::TransportInterface::Data& data) {
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

        auto elapsed = duration_cast<milliseconds>(now - m_last_req_notification);
        if (elapsed >= REQ_COLLECTION_TIMEOUT) {
            m_last_req_notification = now; // Timer neu starten
            m_cv_data_available.notify_all();
        }
    } catch (const nlohmann::json::parse_error& e) {
        EVLOG_error << "Failed to parse JSON request from client " << client_id << ": " << e.what();
    } catch (const std::exception& e) {
        EVLOG_error << "Exception occurred while handling data available: " << e.what();
    }
}

void RpcHandler::process_client_requests() {
    while (m_is_running) {
        std::unique_lock<std::mutex> lock(m_mtx);
        // Wait for data to be available or timeout
        m_cv_data_available.wait_for(lock, REQ_PROCESSING_TIMEOUT, [this]() {
            // Interate over all clients and check if data is available
            for (const auto& [client_id, client_req] : messages) {
                if (!client_req.data.empty()) {
                    return true;
                }
            }
            return false;
        });

        // Process requests for each client
        bool all_requests_processed; // Flag to check if all requests are processed
        do {
            all_requests_processed = true;
            for (auto& [client_id, client_req] : messages) {
                if (client_req.data.empty()) {
                    continue; // Skip if no data available
                }
                // Process the data for this client
                auto transport_interface = client_req.transport_interface;

                if (!transport_interface) {
                    EVLOG_error << "Skip data. Transport interface is null for client " << client_id;
                    continue; // Skip if transport interface is null
                }

                // Get the first request from the client
                nlohmann::json request = client_req.data.front();
                client_req.data.pop_front(); // Remove the processed request

                // Check if next request is available
                if (client_req.data.empty()) {
                    all_requests_processed = false;
                }

                // Check if the request is an API.Hello request
                if (is_api_hello_req(client_id, request)) {
                    // Notify condition variable to unblock the waiting thread
                    m_cv_api_hello.notify_all();
                    EVLOG_info << "API.Hello request received from client " << client_id;
                } else {
                    // If it is not an API.Hello request, we need to check if the client has already sent an API.Hello
                    // request, if not, close the connection
                    if (m_api_hello_received.find(client_id) == m_api_hello_received.end()) {
                        EVLOG_debug << "Client " << client_id << " did not send API.Hello request. Closing connection.";
                        transport_interface->kill_client_connection(client_id,
                                                                    "Disconnected due to missing API.Hello request");
                        continue; // Skip processing this request
                    }
                }

                // Call the RPC server with the request
                std::string res = m_rpc_server->HandleRequest(request.dump());
                // Send the response back to the client
                transport_interface->send_data(client_id, res);
                EVLOG_debug << "Sent response to client " << client_id << ": " << res;
            }
        } while (!all_requests_processed && m_is_running);
    }
}

void RpcHandler::start_server() {
    m_is_running = true;
    // Start all transport interfaces
    for (const auto& transport_interface : m_transport_interfaces) {
        if (!transport_interface->start_server()) {
            throw std::runtime_error("Failed to start transport interface server");
        }
    }

    // Start RPC receiver thread
    m_rpc_recv_thread = std::thread([this]() {
        this->process_client_requests(); 
    });
}

void RpcHandler::stop_server() {
    m_is_running = false;
    // Notify all threads to stop
    m_cv_data_available.notify_all();
    m_cv_api_hello.notify_all();

    // Wait for the RPC receiver thread to finish
    if (m_rpc_recv_thread.joinable()) {
        m_rpc_recv_thread.join();
    }

    for (const auto& transport_interface : m_transport_interfaces) {
        if (!transport_interface->stop_server()) {
            throw std::runtime_error("Failed to stop transport interface server");
        }
    }
}
} // namespace rpc
