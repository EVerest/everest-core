#include <gtest/gtest.h>
#include <thread>
#include <everest/logging.hpp>

#include "../helpers/WebSocketTestClient.hpp"
#include "../rpc/RpcHandler.hpp"
#include "../server/WebsocketServer.hpp"
#include "../data/DataStore.hpp"

using namespace server;
using namespace rpc;

class RpcHandlerTest : public ::testing::Test {
protected:
    int test_port = 8080;
    void SetUp() override {
        // Start the WebSocket server
        m_websocket_server = std::make_unique<server::WebSocketServer>(false, test_port);
        lws_set_log_level(LLL_ERR | LLL_WARN, NULL);

        // Create RpcHandler instance. Move the transport interfaces to the RpcHandler
        std::vector<std::shared_ptr<server::TransportInterface>> transport_interfaces;
        transport_interfaces.push_back(std::shared_ptr<server::TransportInterface>(std::move(m_websocket_server)));
        m_rpc_server = std::make_unique<RpcHandler>(std::move(transport_interfaces), data_store);
        m_rpc_server->start_server();

        data_store.chargerinfo.set_data({
            {"firmware_version", "1.1.1beta"},
            {"model", "Test Charger"},
            {"serial", "1"},
            {"vendor", "Test Company"}
        });
        data_store.everest_version = "2025.1.0";
        data_store.evses.emplace_back().connectors.emplace_back();
        types::json_rpc_api::ConnectorInfoObj connector_info;
        connector_info.id = 1;
        connector_info.type = types::json_rpc_api::ConnectorTypeEnum::DCExtended;

        data_store.evses[0].connectors[0].connectorinfo.set_data(connector_info);
        types::json_rpc_api::EVSEInfoObj evse_info;
        evse_info.id = "DE1235251";
        //TODO: evse_info.available_connectors = ...
        data_store.evses[0].evseinfo.set_data(evse_info);
    }

    void TearDown() override {
        m_rpc_server->stop_server();
    }

    std::unique_ptr<server::WebSocketServer> m_websocket_server;
    std::unique_ptr<rpc::RpcHandler> m_rpc_server;

    //Condition variable to wait for response
    std::condition_variable cv;
    std::mutex cv_mutex;

    // data store object
    data::DataStoreCharger data_store;
};

// Test: Connect to WebSocket server and check if API.hello timeout occurs
TEST_F(RpcHandlerTest, ClientHelloTimeout) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.is_connected());

    // Wait for the client hello timeout
    EVLOG_info << "Waiting for client hello timeout...";
    std::this_thread::sleep_for(std::chrono::seconds(CLIENT_HELLO_TIMEOUT) + std::chrono::milliseconds(100));

    // Check if the client is still connected
    ASSERT_FALSE(client.is_connected());
}

// Test: Connect to WebSocket server and send API.Hello request
TEST_F(RpcHandlerTest, ClientHelloRequest) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.is_connected());

    // Send API.Hello request
    nlohmann::json hello_request = {
        {"jsonrpc", "2.0"},
        {"method", "API.Hello"},
        {"id", 1}
    };
    // Expected response
    nlohmann::json expected_response = {
        {"jsonrpc", "2.0"},
        {"result", {
            {"authentication_required", false},
            {"authenticated", true},
            {"api_version", "1.0"},
            {"everest_version", "2024.9.0"},
            {"charger_info", {
                {"firmware_version", "1.1.1beta"},
                {"model", "DavyBox"},
                {"serial", "1"},
                {"vendor", "chargebyte"}
            }}
        }},
        {"id", 1}
    };
    client.send(hello_request.dump());
    // Wait for the response
    std::unique_lock<std::mutex> lock(cv_mutex);
    cv.wait_for(lock, std::chrono::seconds(1), [&] { return !client.get_received_data().empty(); });
    // Check if the response is not empty
    ASSERT_FALSE(client.get_received_data().empty());
    // Check if the response is valid
    nlohmann::json response = nlohmann::json::parse(client.get_received_data());
    ASSERT_EQ(response, expected_response);
    // Wait for API.Hello timeout
    std::this_thread::sleep_for(std::chrono::seconds(CLIENT_HELLO_TIMEOUT) + std::chrono::milliseconds(100));
    // Check if the client is still connected
    ASSERT_TRUE(client.is_connected());
}

// Test: Connect to WebSocket server and send invalid request
TEST_F(RpcHandlerTest, InvalidRequest) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.is_connected());

    // Send invalid request
    nlohmann::json invalid_request = {
        {"jsonrpc", "2.0"},
        {"method", "API.InvalidMethod"},
        {"id", 1}
    };

    // Expected response
    nlohmann::json expected_response = {
        {"jsonrpc", "2.0"},
        {"error", {
            {"code", -32601},
            {"message", "method not found: API.InvalidMethod"}
        }},
        {"id", 1}
    };
    client.send(invalid_request.dump());
    // Wait for the response
    std::unique_lock<std::mutex> lock(cv_mutex);
    cv.wait_for(lock, std::chrono::seconds(1), [&] { return !client.get_received_data().empty(); });
    // Check if the response is not empty
    ASSERT_FALSE(client.get_received_data().empty());
    // Check if the response is valid
    nlohmann::json response = nlohmann::json::parse(client.get_received_data());
    ASSERT_EQ(response, expected_response);
}
