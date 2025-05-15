#include "../data/DataStore.hpp"
#include <gtest/gtest.h>
#include <thread>

#include "../data/DataStore.hpp"
#include "../helpers/JsonRpcUtils.hpp"
#include "../helpers/RequestHandlerDummy.hpp"
#include "../helpers/WebSocketTestClient.hpp"
#include "../rpc/RpcHandler.hpp"
#include "../server/WebsocketServer.hpp"

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

        // Create RpcHandler instance. Move the transport interfaces and request handler to the RpcHandler
        std::vector<std::shared_ptr<server::TransportInterface>> transport_interfaces;
        request_handler = std::make_unique<RequestHandlerDummy>();
        transport_interfaces.push_back(std::shared_ptr<server::TransportInterface>(std::move(m_websocket_server)));
        m_rpc_handler = std::make_unique<RpcHandler>(std::move(transport_interfaces), data_store, std::move(request_handler));
        m_rpc_handler->start_server();
        initialize_data_store();
    }

    void TearDown() override {
        m_rpc_handler->stop_server();
    }

    void initialize_data_store() {
        // Set up the data store with test data
        RPCDataTypes::ChargerInfoObj charger_info;
        charger_info.firmware_version = "1.0.0";
        charger_info.model = "Test Charger";
        charger_info.serial = "123456789";
        charger_info.vendor = "Test Vendor";
        data_store.chargerinfo.set_data(charger_info);
        data_store.everest_version = "2025.1.0";
        // Properly initialize EVSE objects
        const auto& ptr = data_store.evses.emplace_back(std::make_unique<data::DataStoreEvse>());
        ptr->connectors.emplace_back(std::make_unique<data::DataStoreConnector>());
    }

    void send_req_and_validate_res(
        WebSocketTestClient& client, const nlohmann::json& request, const nlohmann::json& expected_response,
        bool (*cmp_f)(const nlohmann::json&, const nlohmann::json&) = nullptr) {
        // Send the request
        client.send(request.dump());
        // Wait for the response
        std::string data = client.wait_for_data(std::chrono::seconds(1));
        // Check if the response is not empty
        ASSERT_FALSE(data.empty());
        nlohmann::json response = nlohmann::json::parse(data);
        // Check if the response is valid
        if (cmp_f != nullptr) {
            // Compare the response with the expected response using the provided comparison function
            bool res = cmp_f(response, expected_response);
            if (!res) {
                // If the comparison fails, print the response and expected response for debugging
                EVLOG_error << "Expected equality of these values: response: " << response.dump();
                EVLOG_error << "Expected response: " << expected_response.dump();
            }
            ASSERT_TRUE(res);
        } else {
            // Compare the response with the expected response
            ASSERT_EQ(response, expected_response);
        }
    }

    std::unique_ptr<server::WebSocketServer> m_websocket_server;
    std::unique_ptr<rpc::RpcHandler> m_rpc_handler;

    //Condition variable to wait for response
    std::condition_variable cv;
    std::mutex cv_mutex;

    // Data store object used to manage and access charger-related data, including EVSEs, connectors, and charger info.
    data::DataStoreCharger data_store;
    // Dummy request handler. Needed to create the reponses of synchronous requests
    std::unique_ptr<request_interface::RequestHandlerInterface> request_handler;
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
    client.send_api_hello_req();
    // Wait for the response
    std::string data = client.wait_for_data(std::chrono::seconds(1));
    // Check if the response is not empty
    ASSERT_FALSE(data.empty());
    // Check if the response is valid
    nlohmann::json response = nlohmann::json::parse(data);
    ASSERT_EQ(response, expected_response);
    // Check if the client is still connected
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));
}

// Test: Connect to WebSocket server and send EVSEInfo request
TEST_F(RpcHandlerTest, ChargePointGetEVSEInfosReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Set up the data store with test data
    RPCDataTypes::ChargePointGetEVSEInfosResObj result; // Expected response
    data_store.evses.emplace_back(std::make_unique<data::DataStoreEvse>());
    data_store.evses.emplace_back(std::make_unique<data::DataStoreEvse>());
    RPCDataTypes::EVSEInfoObj evse_info;
    evse_info.index = 1;
    evse_info.available_connectors.emplace_back();
    evse_info.available_connectors[0].id = 1;
    evse_info.available_connectors[0].type = types::json_rpc_api::ConnectorTypeEnum::cCCS2;
    evse_info.available_connectors.emplace_back();
    evse_info.available_connectors[1].id = 2;
    evse_info.available_connectors[1].type = types::json_rpc_api::ConnectorTypeEnum::cCCS1;
    evse_info.bidi_charging = false; ///< Whether bidirectional charging is supported
    evse_info.description = "Test EVSE 1";
    
    result.error = RPCDataTypes::ResponseErrorEnum::NoError; ///< No error

    // Set up request and expected response
    nlohmann::json charge_point_get_evse_infos_req = create_json_rpc_request("ChargePoint.GetEVSEInfos", {}, 1);
    nlohmann::json expected_error_no_data = {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorNoDataAvailable)}};

    // Send Api.Hello request
    client.send_api_hello_req();
    client.wait_for_data(std::chrono::seconds(1));
    // Send ChargePoint.GetEVSEInfos request and validate response, no data available
    send_req_and_validate_res(client, charge_point_get_evse_infos_req, expected_error_no_data, is_key_value_in_json_rpc_result);
    // Set up the data store with test data
    data_store.evses[0]->evseinfo.set_data(evse_info);
    result.infos.push_back(evse_info);
    evse_info.index = 2;
    evse_info.description = "Test EVSE 2";
    data_store.evses[1]->evseinfo.set_data(evse_info);
    result.infos.push_back(evse_info);
    // Set up expected response
    nlohmann::json expected_response = create_json_rpc_response(result, 1);
    // Send ChargePoint.GetEVSEInfos request and validate response
    send_req_and_validate_res(client, charge_point_get_evse_infos_req, expected_response);
}

// Test: Connect to WebSocket server and send EVSE.Infos request with valid and invalid index
TEST_F(RpcHandlerTest, EvseGetEVSEInfosReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

     // Set up requests
     nlohmann::json evse_get_evse_infos_req_1 = create_json_rpc_request("EVSE.GetInfo", {{"evse_index", 1}}, 1);
     nlohmann::json evse_get_evse_infos_req_2 = create_json_rpc_request("EVSE.GetInfo", {{"evse_index", 2}}, 1);
     nlohmann::json evse_get_infos_req_invalid_id = create_json_rpc_request("EVSE.GetInfo", {{"evse_index", 99}}, 1);
    
    // Set up the data store with test data
    RPCDataTypes::EVSEInfoObj evse_info;
    evse_info.index = 1; ///< Unique identifier
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

    // Set up the second EVSE info
    evse_info.index = 2;
    evse_info.available_connectors[0].type = types::json_rpc_api::ConnectorTypeEnum::cType2;
    evse_info.available_connectors[1].type = types::json_rpc_api::ConnectorTypeEnum::sType2;
    data_store.evses.emplace_back(std::make_unique<data::DataStoreEvse>());
    data_store.evses[1]->evseinfo.set_data(evse_info);

    // Set up the expected responses
    nlohmann::json expected_response_index_1 = create_json_rpc_response(result_1, 1);

    // Expected response 2
    RPCDataTypes::EVSEGetInfoResObj result_2;
    result_2.info = evse_info;
    result_2.error = RPCDataTypes::ResponseErrorEnum::NoError; ///< No error
    nlohmann::json expected_response_index_2 = create_json_rpc_response(result_2, 1);

    // Expected error object in case of invalid ID
    nlohmann::json expected_error = {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex)}};

    // Send Api.Hello request
    client.send_api_hello_req();
    client.wait_for_data(std::chrono::seconds(1));
    // Send EVSE.GetEVSEInfos request 1 and validate response
    send_req_and_validate_res(client, evse_get_evse_infos_req_1, expected_response_index_1);
    // Send EVSE.GetEVSEInfos request 2 and validate response
    send_req_and_validate_res(client, evse_get_evse_infos_req_2, expected_response_index_2);
    // Send EVSE.GetEVSEInfos request with invalid ID and validate response
    send_req_and_validate_res(client, evse_get_infos_req_invalid_id, expected_error, is_key_value_in_json_rpc_result);
}

// Test: Connect to WebSocket server and send Evse.GetStatusReq request with valid and invalid index
TEST_F(RpcHandlerTest, EvseGetStatusReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));
    // Set up the requests
    nlohmann::json evse_get_status_req_valid_id = create_json_rpc_request("EVSE.GetStatus", {{"evse_index", 1}}, 1);
    nlohmann::json evse_get_status_req_invalid_id = create_json_rpc_request("EVSE.GetStatus", {{"evse_index", 99}}, 1);

    // Set up the data store with test data
    RPCDataTypes::EVSEInfoObj evse_info;
    evse_info.index = 1; ///< Unique identifier
    data_store.evses[0]->evseinfo.set_data(evse_info);

    RPCDataTypes::EVSEStatusObj evse_status;
    evse_status.charged_energy_wh = 123.45;
    evse_status.discharged_energy_wh = 123.45;
    evse_status.charging_duration_s = 600;
    evse_status.charging_allowed = true;
    evse_status.available = true;
    evse_status.active_connector_id = 1;
    evse_status.evse_error = types::json_rpc_api::EVSEErrorEnum::NoError; ///< evse_error
    evse_status.charge_protocol = types::json_rpc_api::ChargeProtocolEnum::ISO15118; ///< charge_protocol
    evse_status.state = types::json_rpc_api::EVSEStateEnum::Charging;
    evse_status.ac_charge_loop.emplace().evse_active_phase_count = 3;

    // Set up the expected responses
    RPCDataTypes::EVSEGetStatusResObj res_valid_id;
    res_valid_id.status = evse_status;
    res_valid_id.error = RPCDataTypes::ResponseErrorEnum::NoError;
    nlohmann::json expected_error_no_data = {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorNoDataAvailable)}};
    nlohmann::json res_obj_invalid_index = {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex)}};
    nlohmann::json expected_response = create_json_rpc_response(res_valid_id, 1);

    // Send Api.Hello request
    client.send_api_hello_req();
    client.wait_for_data(std::chrono::seconds(1));
    // Send EVSE.GetStatus request with valid ID, but no data available
    send_req_and_validate_res(client, evse_get_status_req_valid_id, expected_error_no_data, is_key_value_in_json_rpc_result);
    // Set the EVSE status in the data store
    data_store.evses[0]->evsestatus.set_data(evse_status);
    // Send EVSE.GetStatus request with valid ID
    send_req_and_validate_res(client, evse_get_status_req_valid_id, expected_response);
    // Send EVSE.GetStatus request with invalid ID
    send_req_and_validate_res(client, evse_get_status_req_invalid_id, res_obj_invalid_index, is_key_value_in_json_rpc_result);
}

// Test: Connect to WebSocket server and send EVSE.GetHardwareCapabilities request with valid and invalid index
TEST_F(RpcHandlerTest, EvseGetHardwareCapabilitiesReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Set up requests
    nlohmann::json evse_get_hardware_capabilities_req_valid_id = create_json_rpc_request("EVSE.GetHardwareCapabilities", {{"evse_index", 1}}, 1);
    nlohmann::json evse_get_hardware_capabilities_req_invalid_id = create_json_rpc_request("EVSE.GetHardwareCapabilities", {{"evse_index", 99}}, 1);

    // Set up the data store with test data
    RPCDataTypes::EVSEInfoObj evse_info;
    evse_info.index = 1;
    data_store.evses[0]->evseinfo.set_data(evse_info);

    RPCDataTypes::EVSEGetHardwareCapabilitiesResObj result;
    result.error = RPCDataTypes::ResponseErrorEnum::NoError;
    result.hardware_capabilities.max_current_A_export = 32.0;
    result.hardware_capabilities.max_current_A_import = 16.0;
    result.hardware_capabilities.max_phase_count_export = 3;
    result.hardware_capabilities.max_phase_count_import = 3;
    result.hardware_capabilities.min_current_A_export = 6.0;
    result.hardware_capabilities.min_current_A_import = 6.0;
    result.hardware_capabilities.min_phase_count_export = 1;
    result.hardware_capabilities.min_phase_count_import = 1;
    result.hardware_capabilities.phase_switch_during_charging = true;

    // Set up the expected responses
    nlohmann::json expected_response = create_json_rpc_response(result, 1);
    nlohmann::json expected_error_no_data =  {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorNoDataAvailable)}};
    nlohmann::json expected_error_invalid_index =  {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex)}};

    // Send Api.Hello request
    client.send_api_hello_req();
    client.wait_for_data(std::chrono::seconds(1));

    // Send EVSE.GetHardwareCapabilities request with valid ID, but no hardware capabilities available
    send_req_and_validate_res(client, evse_get_hardware_capabilities_req_valid_id, expected_error_no_data, is_key_value_in_json_rpc_result);
    // Set the hardware capabilities in the data store
    data_store.evses[0]->connectors[0]->hardwarecapabilities.set_data(result.hardware_capabilities);
    // Send EVSE.GetHardwareCapabilities request with valid ID
    send_req_and_validate_res(client, evse_get_hardware_capabilities_req_valid_id, expected_response);
    // Send EVSE.GetHardwareCapabilities request with invalid ID
    send_req_and_validate_res(client, evse_get_hardware_capabilities_req_invalid_id, expected_error_invalid_index, is_key_value_in_json_rpc_result);
}

// Test: Connect to WebSocket server and send EVSE.SetChargingAllowed request with valid and invalid index
TEST_F(RpcHandlerTest, EvseSetChargingAllowedReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Set up requests
    nlohmann::json evse_set_charging_allowed_req_valid_id = create_json_rpc_request("EVSE.SetChargingAllowed", {{"evse_index", 1}, {"charging_allowed", true}}, 1);
    nlohmann::json evse_set_charging_allowed_req_invalid_id = create_json_rpc_request("EVSE.SetChargingAllowed", {{"evse_index", 99}, {"charging_allowed", true}}, 1);

    // Set up the data store with test data
    RPCDataTypes::EVSEInfoObj evse_info;
    evse_info.index = 1;
    data_store.evses[0]->evseinfo.set_data(evse_info);

    RPCDataTypes::EVSEStatusObj evse_status;
    evse_status.available = false;
    data_store.evses[0]->evsestatus.set_data(evse_status);

    // Set up the expected responses
    types::json_rpc_api::ErrorResObj result {RPCDataTypes::ResponseErrorEnum::NoError};
    nlohmann::json expected_response = create_json_rpc_response(result, 1);
    nlohmann::json expected_error = {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex)}};

    // Send Api.Hello request
    client.send_api_hello_req();
    client.wait_for_data(std::chrono::seconds(1));
    // Send EVSE.SetChargingAllowed request with valid ID
    send_req_and_validate_res(client, evse_set_charging_allowed_req_valid_id, expected_response);
    // Check if the EVSE status is updated
    ASSERT_TRUE(data_store.evses[0]->evsestatus.get_data().has_value());
    ASSERT_TRUE(data_store.evses[0]->evsestatus.get_data().value().charging_allowed);

    // Send EVSE.SetChargingAllowed request with invalid ID
    send_req_and_validate_res(client, evse_set_charging_allowed_req_invalid_id, expected_error, is_key_value_in_json_rpc_result);
}

// Test: Connect to WebSocket server and send EVSE.MeterData request with valid and invalid index
TEST_F(RpcHandlerTest, EvseMeterDataReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Set up requests
    nlohmann::json evse_meter_data_req_valid_id = create_json_rpc_request("EVSE.GetMeterData", {{"evse_index", 1}}, 1);
    nlohmann::json evse_meter_data_req_invalid_id = create_json_rpc_request("EVSE.GetMeterData", {{"evse_index", 99}}, 1);

    // Set up the data store with test data
    RPCDataTypes::EVSEInfoObj evse_info;
    evse_info.index = 1;
    data_store.evses[0]->evseinfo.set_data(evse_info);

    // Configure meter data, but do not set it in the data store
    RPCDataTypes::MeterDataObj meter_data {};
    meter_data.energy_Wh_import.total = 123.45;
    meter_data.timestamp = 1234567890.0;

    // Set up the expected responses
    types::json_rpc_api::EVSEGetMeterDataResObj result {{meter_data}, RPCDataTypes::ResponseErrorEnum::NoError};
    nlohmann::json expected_response_no_error = create_json_rpc_response(result, 1);
    nlohmann::json expected_error_no_data = {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorNoDataAvailable)}};
    nlohmann::json expected_error_invalid_index = {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex)}};

    // Send Api.Hello request
    client.send_api_hello_req();
    client.wait_for_data(std::chrono::seconds(1));
    // Send EVSE.MeterData request with valid ID, but no meter data available
    send_req_and_validate_res(client, evse_meter_data_req_valid_id, expected_error_no_data, is_key_value_in_json_rpc_result);

    // Set the meter data in the data store
    data_store.evses[0]->meterdata.set_data(meter_data);

    // Send EVSE.MeterData request with valid ID and meter data available
    send_req_and_validate_res(client, evse_meter_data_req_valid_id, expected_response_no_error);
    
    // Send EVSE.MeterData request with invalid ID
    send_req_and_validate_res(client, evse_meter_data_req_invalid_id, expected_error_invalid_index, is_key_value_in_json_rpc_result);
}

// Test: Connect to WebSocket server and send EVSE.SetACCharging request with valid and invalid index
TEST_F(RpcHandlerTest, EvseSetACChargingReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Set up requests
    nlohmann::json evse_set_ac_charging_req_valid_id = create_json_rpc_request("EVSE.SetACCharging", 
        {{"evse_index", 1},
         {"charging_allowed", true},
         {"max_current", 12.3},
         {"phase_count", 3}}, 1);

    // As long as the method is not implemented, we expect an error response that the method is not implemented
    nlohmann::json expected_res = create_json_rpc_error_response(-32601, "method not found: EVSE.SetACCharging", 1);

    // Send Api.Hello request
    client.send_api_hello_req();
    client.wait_for_data(std::chrono::seconds(1));
    // Send EVSE.SetACCharging request with valid ID
    client.send(evse_set_ac_charging_req_valid_id.dump());
    // Wait for the response
    std::string received_data = client.wait_for_data(std::chrono::seconds(1));
    // Check if the response is valid
    nlohmann::json response = nlohmann::json::parse(received_data);
    ASSERT_EQ(response, expected_res);
}

// Test: Connect to WebSocket server and send EVSE.SetACChargingCurrent request with valid and invalid index
TEST_F(RpcHandlerTest, EvseSetACChargingCurrentReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Set up requests
    nlohmann::json evse_set_ac_charging_current_req_valid_id = create_json_rpc_request("EVSE.SetACChargingCurrent", {{"evse_index", 1}, {"max_current", 12.3}}, 1);
    nlohmann::json evse_set_ac_charging_current_req_invalid_id = create_json_rpc_request("EVSE.SetACChargingCurrent", {{"evse_index", 99}, {"max_current", 12.3}}, 1);

    // Set up the data store with test data
    RPCDataTypes::EVSEInfoObj evse_info;
    evse_info.index = 1;
    data_store.evses[0]->evseinfo.set_data(evse_info);

    RPCDataTypes::EVSEStatusObj evse_status;
    evse_status.available = false;
    evse_status.ac_charge_param.emplace();
    evse_status.ac_charge_param->evse_max_current.emplace(12.3);
    data_store.evses[0]->evsestatus.set_data(evse_status);

    // Set up the expected responses
    types::json_rpc_api::ErrorResObj result {RPCDataTypes::ResponseErrorEnum::NoError};
    nlohmann::json expected_response = create_json_rpc_response(result, 1);
    nlohmann::json expected_error = {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex)}};

    // Send Api.Hello request
    client.send_api_hello_req();
    client.wait_for_data(std::chrono::seconds(1));
    // Send EVSE.SetACChargingCurrent request with valid ID
    send_req_and_validate_res(client, evse_set_ac_charging_current_req_valid_id, expected_response);
    // Check if the EVSE status is updated
    ASSERT_TRUE(data_store.evses[0]->evsestatus.get_data().has_value());
    ASSERT_TRUE(data_store.evses[0]->evsestatus.get_data().value().charging_allowed);

    // Send EVSE.SetACChargingCurrent request with invalid ID
    send_req_and_validate_res(client, evse_set_ac_charging_current_req_invalid_id, expected_error, is_key_value_in_json_rpc_result);
}

// Test: Connect to WebSocket server and send EVSE.SetACChargingPhaseCount request with valid and invalid index
TEST_F(RpcHandlerTest, EvseSetACChargingPhaseCountReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Set up requests
    nlohmann::json evse_set_ac_charging_phase_count_req_valid_id = create_json_rpc_request("EVSE.SetACChargingPhaseCount", {{"evse_index", 1}, {"phase_count", 3}}, 1);
    nlohmann::json evse_set_ac_charging_phase_count_req_invalid_id = create_json_rpc_request("EVSE.SetACChargingPhaseCount", {{"evse_index", 99}, {"phase_count", 3}}, 1);

    // Set up the data store with test data
    RPCDataTypes::EVSEInfoObj evse_info;
    evse_info.index = 1;
    data_store.evses[0]->evseinfo.set_data(evse_info);

    RPCDataTypes::EVSEStatusObj evse_status;
    evse_status.available = false;
    evse_status.ac_charge_param.emplace();
    evse_status.ac_charge_param->evse_max_current.emplace(12.3);
    data_store.evses[0]->evsestatus.set_data(evse_status);

    // Set up the expected responses
    types::json_rpc_api::ErrorResObj result {RPCDataTypes::ResponseErrorEnum::NoError};
    nlohmann::json expected_response = create_json_rpc_response(result, 1);
    nlohmann::json expected_error = {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex)}};

    // Send Api.Hello request
    client.send_api_hello_req();
    client.wait_for_data(std::chrono::seconds(1));
    // Send EVSE.SetACChargingPhaseCount request with valid ID
    send_req_and_validate_res(client, evse_set_ac_charging_phase_count_req_valid_id, expected_response);
    // Check if the EVSE status is updated
    ASSERT_TRUE(data_store.evses[0]->evsestatus.get_data().has_value());
    ASSERT_TRUE(data_store.evses[0]->evsestatus.get_data().value().charging_allowed);

    // Send EVSE.SetACChargingPhaseCount request with invalid ID
    send_req_and_validate_res(client, evse_set_ac_charging_phase_count_req_invalid_id, expected_error, is_key_value_in_json_rpc_result);
}

// Test: Connect to WebSocket server and send EVSE.SetDCCharging request with valid and invalid index
TEST_F(RpcHandlerTest, EvseSetDCChargingReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Set up requests
    nlohmann::json evse_set_dc_charging_req_valid_id = create_json_rpc_request("EVSE.SetDCCharging", {{"evse_index", 1}, {"charging_allowed", true}, {"max_power", 12.3}}, 1);

    // As long as the method is not implemented, we expect an error response that the method is not implemented
    nlohmann::json expected_res = create_json_rpc_error_response(-32601, "method not found: EVSE.SetDCCharging", 1);
    // Send Api.Hello request
    client.send_api_hello_req();
    client.wait_for_data(std::chrono::seconds(1));
    // Send EVSE.SetDCCharging request with valid ID
    client.send(evse_set_dc_charging_req_valid_id.dump());
    // Wait for the response
    std::string received_data = client.wait_for_data(std::chrono::seconds(1));
    // Check if the response is valid
    nlohmann::json response = nlohmann::json::parse(received_data);
    ASSERT_EQ(response, expected_res);
}


// Test: Connect to WebSocket server and send EVSE.SetDCChargingPower request with valid and invalid index
TEST_F(RpcHandlerTest, EvseSetDCChargingPowerReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Set up requests
    nlohmann::json evse_set_dc_charging_power_req_valid_id = create_json_rpc_request("EVSE.SetDCChargingPower", {{"evse_index", 1}, {"max_power", 12.3}}, 1);
    nlohmann::json evse_set_dc_charging_power_req_invalid_id = create_json_rpc_request("EVSE.SetDCChargingPower", {{"evse_index", 99}, {"max_power", 12.3}}, 1);

    // Set up the data store with test data
    RPCDataTypes::EVSEInfoObj evse_info;
    evse_info.index = 1;
    data_store.evses[0]->evseinfo.set_data(evse_info);

    RPCDataTypes::EVSEStatusObj evse_status;
    evse_status.available = false;
    evse_status.dc_charge_param.emplace();
    data_store.evses[0]->evsestatus.set_data(evse_status);

    // Set up the expected responses
    types::json_rpc_api::ErrorResObj result {RPCDataTypes::ResponseErrorEnum::NoError};
    nlohmann::json expected_response = create_json_rpc_response(result, 1);
    nlohmann::json expected_error = {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex)}};

    // Send Api.Hello request
    client.send_api_hello_req();
    client.wait_for_data(std::chrono::seconds(1));
    // Send EVSE.SetDCChargingPower request with valid ID
    send_req_and_validate_res(client, evse_set_dc_charging_power_req_valid_id, expected_response);
    // Send EVSE.SetDCChargingPower request with invalid ID
    send_req_and_validate_res(client, evse_set_dc_charging_power_req_invalid_id, expected_error, is_key_value_in_json_rpc_result);
}

// Test: Connect to WebSocket server and send EVSE.EnableConnector request with valid and invalid index
TEST_F(RpcHandlerTest, EvseEnableConnectorReq) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Set up requests
    nlohmann::json evse_enable_connector_req_valid_id = create_json_rpc_request("EVSE.EnableConnector", {{"evse_index", 1}, {"enable", true}, {"priority", 1}, {"connector_id", 1}}, 1);
    nlohmann::json evse_enable_connector_req_invalid_id = create_json_rpc_request("EVSE.EnableConnector", {{"evse_index", 99}, {"enable", true}, {"priority", 1}, {"connector_id", 1}}, 1);
    nlohmann::json evse_enable_connector_req_invalid_connector_id = create_json_rpc_request("EVSE.EnableConnector", {{"evse_index", 1}, {"enable", true}, {"priority", 1}, {"connector_id", 99}}, 1);

    // Set up the expected responses
    types::json_rpc_api::ErrorResObj result {RPCDataTypes::ResponseErrorEnum::NoError};
    nlohmann::json expected_response = create_json_rpc_response(result, 1);
    nlohmann::json expected_error = {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorInvalidEVSEIndex)}};
    nlohmann::json expected_error_invalid_connector_id = {{"error", response_error_enum_to_string(RPCDataTypes::ResponseErrorEnum::ErrorInvalidConnectorID)}};

    // Set up the data store with test data
    RPCDataTypes::EVSEInfoObj evse_info;
    evse_info.index = 1;
    data_store.evses[0]->evseinfo.set_data(evse_info);

    RPCDataTypes::ConnectorInfoObj connectorinfo;
    connectorinfo.id = 1;
    connectorinfo.type = types::json_rpc_api::ConnectorTypeEnum::cCCS2;
    data_store.evses[0]->connectors[0]->connectorinfo.set_data(connectorinfo);

    RPCDataTypes::EVSEStatusObj evse_status;
    evse_status.available = false;
    data_store.evses[0]->evsestatus.set_data(evse_status);

    // Send Api.Hello request
    client.send_api_hello_req();
    client.wait_for_data(std::chrono::seconds(1));
    // Send EVSE.EnableConnector request with valid ID
    send_req_and_validate_res(client, evse_enable_connector_req_valid_id, expected_response);
    // Send EVSE.EnableConnector request with invalid ID
    send_req_and_validate_res(client, evse_enable_connector_req_invalid_id, expected_error, is_key_value_in_json_rpc_result);
    // Send EVSE.EnableConnector request with invalid connector ID
    send_req_and_validate_res(client, evse_enable_connector_req_invalid_connector_id, expected_error_invalid_connector_id, is_key_value_in_json_rpc_result);
}

// Test: Connect to WebSocket server and send invalid request
TEST_F(RpcHandlerTest, InvalidRequest) {
    WebSocketTestClient client("localhost", test_port);
    ASSERT_TRUE(client.connect());
    ASSERT_TRUE(client.wait_until_connected(std::chrono::milliseconds(100)));

    // Send Api.Hello request
    client.send_api_hello_req();
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
