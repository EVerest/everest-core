#include <gtest/gtest.h>
#include <thread>
#include <everest/logging.hpp>

#include "../helpers/WebSocketTestClient.hpp"
#include "../rpc/RpcHandler.hpp"
#include "../server/WebsocketServer.hpp"
#include "../data/DataStore.hpp"
#include "../helpers/JsonRpcUtils.hpp"

using namespace server;
using namespace rpc;
using namespace json_rpc_utils;

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
        auto& ptr = data_store.evses.emplace_back(std::make_unique<data::DataStoreEvse>()); // Properly initialize EVSE objects
        ptr->connectors.emplace_back(std::make_unique<data::DataStoreConnector>());
    }

    std::unique_ptr<server::WebSocketServer> m_websocket_server;
    std::unique_ptr<rpc::RpcHandler> m_rpc_server;

    //Condition variable to wait for response
    std::condition_variable cv;
    std::mutex cv_mutex;

    // data store object
    data::DataStoreCharger data_store;
};

// Test: Connect to WebSocket server and check if API.Hello timeout occurs
TEST_F(RpcHandlerTest, ApiHelloTimeout) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

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
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

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
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));
}

// Test: Connect to WebSocket server and send EVSEInfo request
TEST_F(RpcHandlerTest, ChargePointGetEVSEInfosReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Set up the data store with test data
    //RPCDataTypes::chargerinfo charger_info;
    RPCDataTypes::ChargePointGetEVSEInfosResObj result; // Expected response
    data_store.evses.emplace_back(std::make_unique<data::DataStoreEvse>());
    data_store.evses.emplace_back(std::make_unique<data::DataStoreEvse>());
    RPCDataTypes::EVSEInfoObj evse_info;
    evse_info.id = "DEA12E000001"; ///< Unique identifier
    evse_info.available_connectors.emplace_back();
    evse_info.available_connectors[0].id = 1;
    evse_info.available_connectors[0].type = types::json_rpc_api::ConnectorTypeEnum::cCCS2;
    evse_info.available_connectors.emplace_back();
    evse_info.available_connectors[1].id = 2;
    evse_info.available_connectors[1].type = types::json_rpc_api::ConnectorTypeEnum::cCCS1;
    evse_info.bidi_charging = false; ///< Whether bidirectional charging is supported
    evse_info.description = "Test EVSE 1";
    data_store.evses[0]->evseinfo.set_data(evse_info);
    result.infos.push_back(evse_info);
    evse_info.id = "DEA12E000002"; ///< Unique identifier
    evse_info.description = "Test EVSE 2";
    data_store.evses[1]->evseinfo.set_data(evse_info);
    
    result.infos.push_back(evse_info);
    result.error = RPCDataTypes::ResponseErrorEnum::NoError; ///< No error

    // Set up request and expected response
    nlohmann::json charge_point_get_evse_infos_req = create_json_rpc_request("ChargePoint.GetEVSEInfos", {}, 1);
    nlohmann::json expected_response = create_json_rpc_response(result, 1);

    // Send Api.Hello request
    client.sendApiHelloReq();
    client.wait_for_data(std::chrono::seconds(1));
    // Send ChargePoint.GetEVSEInfos request
    client.send(charge_point_get_evse_infos_req.dump());
    // Wait for the response
    std::string received_data = client.wait_for_data(std::chrono::seconds(1));
    // Check if the response is not empty
    ASSERT_FALSE(received_data.empty());
    // Check if the response is valid
    nlohmann::json response = nlohmann::json::parse(received_data);
    ASSERT_EQ(response, expected_response);
    //sleep(10);
}

// Test: Connect to WebSocket server and send EVSE.Infos request with valid evse_id
TEST_F(RpcHandlerTest, EvseGetEVSEInfosReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Set up the data store with test data
    RPCDataTypes::EVSEInfoObj evse_info;
    evse_info.id = "DEA12E000001"; ///< Unique identifier
    evse_info.available_connectors.emplace_back();
    evse_info.available_connectors[0].id = 1;
    evse_info.available_connectors[0].type = types::json_rpc_api::ConnectorTypeEnum::cCCS2;
    evse_info.available_connectors.emplace_back();
    evse_info.available_connectors[1].id = 2;
    evse_info.available_connectors[1].type = types::json_rpc_api::ConnectorTypeEnum::cCCS1;
    evse_info.bidi_charging = false; ///< Whether bidirectional charging is supported
    evse_info.description = "Test EVSE 1";
    data_store.evses[0]->evseinfo.set_data(evse_info);

    // Expected response 1
    RPCDataTypes::EVSEGetInfoResObj result_1;
    result_1.info = evse_info;
    result_1.error = RPCDataTypes::ResponseErrorEnum::NoError; ///< No error

    evse_info.id = "DEA12E000002";
    evse_info.available_connectors[0].type = types::json_rpc_api::ConnectorTypeEnum::cType2;
    evse_info.available_connectors[1].type = types::json_rpc_api::ConnectorTypeEnum::sType2;
    data_store.evses.emplace_back(std::make_unique<data::DataStoreEvse>());
    data_store.evses[1]->evseinfo.set_data(evse_info);

    nlohmann::json expected_response_1 = create_json_rpc_response(result_1, 1);

    // Expected response 2
    RPCDataTypes::EVSEGetInfoResObj result_2;
    result_2.info = evse_info;
    result_2.error = RPCDataTypes::ResponseErrorEnum::NoError; ///< No error

    nlohmann::json expected_response_2 = create_json_rpc_response(result_2, 1);
    
    // Set up the request
    nlohmann::json evse_get_evse_infos_req_1 = create_json_rpc_request("EVSE.GetInfo", {{"evse_id", "DEA12E000001"}}, 1);
    nlohmann::json evse_get_evse_infos_req_2 = create_json_rpc_request("EVSE.GetInfo", {{"evse_id", "DEA12E000002"}}, 1);

    // Send Api.Hello request
    client.sendApiHelloReq();
    client.wait_for_data(std::chrono::seconds(1));
    // Send EVSE.GetEVSEInfos request 1
    client.send(evse_get_evse_infos_req_1.dump());
    // Wait for the response
    std::string received_data = client.wait_for_data(std::chrono::seconds(1));
    // Check if the response is not empty
    ASSERT_FALSE(received_data.empty());
    // Check if the response is valid
    nlohmann::json response = nlohmann::json::parse(received_data);
    ASSERT_EQ(response, expected_response_1);
    // Send EVSE.GetEVSEInfos request 2
    client.send(evse_get_evse_infos_req_2.dump());
    // Wait for the response
    received_data = client.wait_for_data(std::chrono::seconds(1));
    // Check if the response is not empty
    ASSERT_FALSE(received_data.empty());
    // Check if the response is valid
    response = nlohmann::json::parse(received_data);
    ASSERT_EQ(response, expected_response_2);
}

// Test: Connect to WebSocket server and send EVSE.Infos request with invalid evse_id
TEST_F(RpcHandlerTest, EvseGetEVSEInfosReqInvalidID) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Set up the data store with test data
    RPCDataTypes::EVSEInfoObj evse_info;
    evse_info.id = "DEA12E000001"; ///< Unique identifier
    evse_info.available_connectors.emplace_back();
    evse_info.available_connectors[0].id = 1;
    evse_info.available_connectors[0].type = types::json_rpc_api::ConnectorTypeEnum::cCCS2;
    evse_info.available_connectors.emplace_back();
    evse_info.available_connectors[1].id = 2;
    evse_info.available_connectors[1].type = types::json_rpc_api::ConnectorTypeEnum::cCCS1;
    evse_info.bidi_charging = false; ///< Whether bidirectional charging is supported
    evse_info.description = "Test EVSE 1";
    data_store.evses[0]->evseinfo.set_data(evse_info);

    // Set up the request
    nlohmann::json evse_get_evse_infos_req = create_json_rpc_request("EVSE.GetInfo", {{"evse_id", "INVALID_ID"}}, 1);

    // Send Api.Hello request
    client.sendApiHelloReq();
    client.wait_for_data(std::chrono::seconds(1));
    // Send EVSE.GetEVSEInfos request
    client.send(evse_get_evse_infos_req.dump());
    // Wait for the response
    std::string received_data = client.wait_for_data(std::chrono::seconds(1));
    // Check if the response is not empty
    ASSERT_FALSE(received_data.empty());
    // Check if the response is valid
    nlohmann::json response = nlohmann::json::parse(received_data);
    // Check only if the error is present in the json response. All other fields are not relevant.
    // Create a json object with the expected error
    nlohmann::json expected_error =  {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEID)}};
    ASSERT_TRUE(is_key_value_in_json_rpc_result(response, expected_error));
}

// Test: Connect to WebSocket server and send EVSEStatus request with valid evse_id
TEST_F(RpcHandlerTest, EvseGetStatusReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Set up the data store with test data
    RPCDataTypes::EVSEStatusObj evse_status;
    evse_status.charged_energy_wh = 123.45; ///< charged_energy_wh
    evse_status.discharged_energy_wh = 123.45; ///< discharged_energy_wh
    evse_status.charging_duration_s = 500; ///< charging_duration_s
    evse_status.charging_allowed = true; ///< charging_allowed
    evse_status.available = true; ///< available
    evse_status.active_connector_id = 1; ///< active_connector_id
    evse_status.evse_error = types::json_rpc_api::EVSEErrorEnum::NoError; ///< evse_error
    evse_status.charge_protocol = types::json_rpc_api::ChargeProtocolEnum::ISO15118; ///< charge_protocol
    evse_status.state = types::json_rpc_api::EVSEStateEnum::Charging;
    evse_status.ac_charge_loop.emplace();
    evse_status.ac_charge_loop.value().evse_active_phase_count = 3;
    data_store.evses[0]->evsestatus.set_data(evse_status);

    // Expected response
    RPCDataTypes::EVSEGetStatusResObj result;
    result.status = evse_status;
    result.error = RPCDataTypes::ResponseErrorEnum::NoError; ///< No error

    nlohmann::json expected_response = create_json_rpc_response(result, 1);

    // Set up the request
    nlohmann::json evse_get_evse_status_req = create_json_rpc_request("EVSE.GetStatus", {{"evse_id", "DEA12E000001"}}, 1);

    // Send Api.Hello request
    client.sendApiHelloReq();
    client.wait_for_data(std::chrono::seconds(1));
    // Send EVSE.GetEVSEInfos request
    client.send(evse_get_evse_status_req.dump());
    // Wait for the response
    std::string received_data = client.wait_for_data(std::chrono::seconds(1));
    // Check if the response is not empty
    ASSERT_FALSE(received_data.empty());
    // Check if the response is valid
    nlohmann::json response = nlohmann::json::parse(received_data);
    ASSERT_EQ(response, expected_response);
}

// Test: Connect to WebSocket server and send invalid request
TEST_F(RpcHandlerTest, InvalidRequest) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Send Api.Hello request
    client.sendApiHelloReq();
    // Wait for the response
    client.wait_for_data(std::chrono::seconds(1));
    // Send invalid request
    nlohmann::json invalid_request = create_json_rpc_request("API.InvalidMethod", {}, 1);
    // Expected response
    nlohmann::json expected_response = create_json_rpc_error_response(-32601, "method not found: API.InvalidMethod", 1);
    // Send invalid request
    client.send(invalid_request.dump());
    // Wait for the response
    std::string received_data = client.wait_for_data(std::chrono::seconds(1));
    // Check if the response is not empty
    ASSERT_FALSE(received_data.empty());
    // Check if the response is valid
    nlohmann::json response = nlohmann::json::parse(received_data);
    ASSERT_EQ(response, expected_response);
}
