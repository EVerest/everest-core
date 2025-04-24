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
        initilize_data_store();
    }

    void TearDown() override {
        m_rpc_server->stop_server();
    }

    void initilize_data_store() {
        // Set up the data store with test data
        RPCDataTypes::ChargerInfoObj charger_info;
        charger_info.firmware_version = "1.0.0";
        charger_info.model = "Test Charger";
        charger_info.serial = "123456789";
        charger_info.vendor = "Test Vendor";
        data_store.chargerinfo.set_data(charger_info);
        data_store.everest_version = "2025.1.0";
        data_store.evses.emplace_back().connectors.emplace_back();
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
TEST_F(RpcHandlerTest, ApiHelloTimeout) {
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
TEST_F(RpcHandlerTest, ApiHelloReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.is_connected());

    // Set up the expected response
    RPCDataTypes::HelloResObj result;
    result.authentication_required = false;
    result.api_version = API_VERSION;
    result.charger_info = data_store.chargerinfo.get_data().value();
    result.everest_version = data_store.everest_version;

    nlohmann::json expected_response = {
        {"jsonrpc", "2.0"},
        {"result", result},
        {"id", 1}
    };
    
    // Send Api.Hello request
    client.sendApiHelloReq();
    // Wait for the response
    std::string data = client.wait_for_data(std::chrono::seconds(1));
    // Check if the response is not empty
    ASSERT_FALSE(data.empty());
    // Check if the response is valid
    nlohmann::json response = nlohmann::json::parse(data);
    ASSERT_EQ(response, expected_response);
    // Wait for API.Hello timeout
    std::this_thread::sleep_for(std::chrono::seconds(CLIENT_HELLO_TIMEOUT) + std::chrono::milliseconds(100));
    // Check if the client is still connected
    ASSERT_TRUE(client.is_connected());
}

// Test: Connect to WebSocket server and send EVSEInfo request
TEST_F(RpcHandlerTest, ChargePointGetEVSEInfosReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.is_connected());

    // Set up the data store with test data
    //RPCDataTypes::chargerinfo charger_info;
    RPCDataTypes::ChargePointGetEVSEInfosResObj result; // Expected response
    data_store.evses.resize(2); // Ensure at least two EVSEs exist
    RPCDataTypes::EVSEInfoObj evse_info;
    evse_info.id = "DEA12E000001"; ///< Unique identifier
    evse_info.available_connectors.emplace_back();
    evse_info.available_connectors[0].id = 1;
    evse_info.available_connectors[0].type = types::json_rpc_api::ConnectorTypeEnum::cCCS2;
    evse_info.available_connectors[1].id = 2;
    evse_info.available_connectors[1].type = types::json_rpc_api::ConnectorTypeEnum::cCCS1;
    evse_info.bidi_charging = false; ///< Whether bidirectional charging is supported
    evse_info.description = "Test EVSE 1";
    data_store.evses[0].evseinfo.set_data(evse_info);
    result.infos.push_back(evse_info);
    evse_info.id = "DEA12E000002"; ///< Unique identifier
    evse_info.description = "Test EVSE 2";
    data_store.evses[1].evseinfo.set_data(evse_info);
    
    result.infos.push_back(evse_info);
    result.error = RPCDataTypes::ResponseErrorEnum::NoError; ///< No error

    nlohmann::json expected_response = {
        {"jsonrpc", "2.0"},
        {"result", result},
        {"id", 1}
    };
    nlohmann::json chargePointgetEVSEInfosReq = {
        {"jsonrpc", "2.0"},
        {"method", "ChargePoint.GetEVSEInfos"},
        {"id", 1}
    };

    // Send Api.Hello request
    client.sendApiHelloReq();
    client.wait_for_data(std::chrono::seconds(1));
    // Send ChargePoint.GetEVSEInfos request
    client.send(chargePointgetEVSEInfosReq.dump());
    // Wait for the response
    std::string received_data = client.wait_for_data(std::chrono::seconds(1));
    // Check if the response is not empty
    ASSERT_FALSE(received_data.empty());
    // Check if the response is valid
    nlohmann::json response = nlohmann::json::parse(received_data);
    ASSERT_EQ(response, expected_response);
    //sleep(10);
}

// Test: Connect to WebSocket server and send invalid request
TEST_F(RpcHandlerTest, InvalidRequest) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.is_connected());

    // Send Api.Hello request
    client.sendApiHelloReq();

    // Wait for the response
    client.wait_for_data(std::chrono::seconds(1));

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
    std::string received_data = client.wait_for_data(std::chrono::seconds(1));
    // Check if the response is not empty
    ASSERT_FALSE(received_data.empty());
    // Check if the response is valid
    nlohmann::json response = nlohmann::json::parse(received_data);
    ASSERT_EQ(response, expected_response);
}
