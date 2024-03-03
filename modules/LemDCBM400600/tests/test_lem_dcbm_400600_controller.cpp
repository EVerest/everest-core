// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

// Unit tests for the LemDCBM400600Controller

#include "http_client_interface.hpp"
#include "lem_dcbm_400600_controller.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <memory>

namespace module::main {

class HTTPClientMock : public HttpClientInterface {
public:
    MOCK_METHOD(HttpResponse, get, (const std::string& path), (override, const));
    MOCK_METHOD(HttpResponse, post, (const std::string& path, const std::string& body), (override, const));
    MOCK_METHOD(HttpResponse, put, (const std::string& path, const std::string& body), (override, const));
};

class LemDCBMTimeSyncHelperMock : public LemDCBMTimeSyncHelper {
public:
    MOCK_METHOD(void, sync_if_deadline_expired, (const HttpClientInterface& httpClient), (override));
    MOCK_METHOD(void, sync, (const HttpClientInterface& httpClient), (override));
    MOCK_METHOD(void, restart_unsafe_period, (), (override));
    LemDCBMTimeSyncHelperMock() : LemDCBMTimeSyncHelper({}, {}){};
};

// Fixture class providing
//   - a http client mock
//   - default responses & request objects
class LemDCBM400600ControllerTest : public ::testing::Test {

protected:
    std::unique_ptr<HTTPClientMock> http_client;
    std::unique_ptr<LemDCBMTimeSyncHelperMock> time_sync_helper;

    const std::string livemeasure_response{R"({
                                                "voltage": 4.2,
                                                "current": 4,
                                                "power": 3,
                                                "temperatureH": 0,
                                                "temperatureL": 0,
                                                "energyImportTotal": 1,
                                                "energyExportTotal": 2,
                                                "timestamp": "2023-09-10T21:10:08.068773"
                                                })"};

    const types::powermeter::TransactionReq transaction_request{
        "mock_evse_id", "mock_transaction_id", "mock_client_id", 42, 43, "mock_user_data"};

    const std::string expected_start_transaction_request_body{
        R"({"evseId":"mock_evse_id","transactionId":"mock_transaction_id","clientId":"mock_client_id","tariffId":42,"cableId":43,"userData":"mock_user_data"})"};

    const std::string put_legal_response = R"({
                                                "paginationCounter": 6,
                                                "transactionId": "mock_transaction_id",
                                                "evseId": "+49*DEF*E123ABC",
                                                "clientId": "C12",
                                                "tariffId": 2,
                                                "cableSp": {
                                                  "cableSpName": "2mR_Comp",
                                                  "cableSpId": 1,
                                                  "cableSpRes": 2
                                                },
                                                "userData": "",
                                                "meterValue": {
                                                  "timestampStart": "2020-12-10T16:39:15+01:00",
                                                  "timestampStop": "2020-12-10T16:39:15+01:00",
                                                  "transactionDuration": 70,
                                                  "intermediateRead": false,
                                                  "transactionStatus": 17,
                                                  "sampleValue": {
                                                    "energyUnit": "kWh",
                                                    "energyImport": 7.637,
                                                    "energyImportTotalStart": 188.977,
                                                    "energyImportTotalStop": 196.614,
                                                    "energyExport": 0.000,
                                                    "energyExportTotalStart": 0.000,
                                                    "energyExportTotalStop": 0.000
                                                  }},
                                                "meterId": "12024072805",
                                                "signature": "304502203DC38FBC722D216568D6ECB4B352577A999B6D184EA6AD48BDCAE7766DB1D628022100A7687B4CB5573829D407DD4B17D41C297917B7E8307E5017711B5A3A987F6801",
                                                "publicKey": "A80F10D968E1122F8820F288B23C4E1C0DA912F35B48481274ADFEFE66D7E87E130C7CF2B8047C45CF105041C8C3A57DD242782F755C9443F42DABA9404A67BF"
                                                })";

    const LemDCBM400600Controller::Conf controller_config{0, 0, 1, 0};

    void SetUp() override {
        this->http_client = std::make_unique<HTTPClientMock>();
        this->time_sync_helper = std::make_unique<LemDCBMTimeSyncHelperMock>();
    }
};

// Extended fixture for parametrizing tests for invalid response checks
class LemDCBM400600ControllerTestInvalidResponses
    : public LemDCBM400600ControllerTest,
      public ::testing::WithParamInterface<testing::internal::ReturnAction<HttpResponse>> {};

//****************************************************************
// Test get_powermeter behavior

/// \brief Test get_powermeter returns correct live measure
TEST_F(LemDCBM400600ControllerTest, test_get_powermeter) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->time_sync_helper, sync_if_deadline_expired(testing::_)).Times(1).InSequence(seq);
    EXPECT_CALL(*this->http_client, get("/v1/livemeasure"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{200, this->livemeasure_response}));
    LemDCBM400600Controller controller(std::move(this->http_client), std::move(this->time_sync_helper),
                                       this->controller_config);

    // Act
    const types::powermeter::Powermeter& powermeter = controller.get_powermeter();

    // Verify
    EXPECT_EQ(powermeter.timestamp, "2023-09-10T21:10:08.068773");
    EXPECT_THAT(powermeter.energy_Wh_import.total, testing::FloatEq(1.0));
    EXPECT_THAT(powermeter.energy_Wh_export->total, testing::FloatEq(2.0));
    EXPECT_THAT(powermeter.power_W->total, testing::FloatEq(3.0));
    EXPECT_THAT(powermeter.current_A->DC.value(), testing::FloatEq(4.0));
    EXPECT_THAT(powermeter.voltage_V->DC.value(), testing::FloatEq(4.2));
    EXPECT_THAT(powermeter.meter_id.value(), ""); // not initialized
}

/// \brief Test get_powermeter fails due to an invalid response status code
TEST_F(LemDCBM400600ControllerTest, test_get_powermeter_fail_invalid_response_code) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->time_sync_helper, sync_if_deadline_expired(testing::_)).Times(1).InSequence(seq);
    EXPECT_CALL(*this->http_client, get("/v1/livemeasure"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{403, this->livemeasure_response}));
    LemDCBM400600Controller controller(std::move(this->http_client), std::move(this->time_sync_helper),
                                       this->controller_config);

    // Act & Verify
    EXPECT_THROW(controller.get_powermeter(), LemDCBM400600Controller::DCBMUnexpectedResponseException);
}

/// \brief Test get_powermeter fails due to an invalid response status body
TEST_F(LemDCBM400600ControllerTest, test_get_powermeter_fail_invalid_response_body) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->time_sync_helper, sync_if_deadline_expired(testing::_)).Times(1).InSequence(seq);
    EXPECT_CALL(*this->http_client, get("/v1/livemeasure"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{200, "invalid"}));

    LemDCBM400600Controller controller(std::move(this->http_client), std::move(this->time_sync_helper),
                                       this->controller_config);
    // Act & Verify
    EXPECT_THROW(controller.get_powermeter(), LemDCBM400600Controller::DCBMUnexpectedResponseException);
}

/// \brief Test get_powermeter fails due to an http client error
TEST_F(LemDCBM400600ControllerTest, test_get_powermeter_fail_http_error) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->time_sync_helper, sync_if_deadline_expired(testing::_)).Times(1).InSequence(seq);
    EXPECT_CALL(*this->http_client, get("/v1/livemeasure"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Throw(HttpClientError("http client mock error")));

    LemDCBM400600Controller controller(std::move(this->http_client), std::move(this->time_sync_helper),
                                       this->controller_config);
    // Act & Verify
    EXPECT_THROW(controller.get_powermeter(), HttpClientError);
}

//****************************************************************
// Test start_transaction behavior

// \brief Test a successful start transaction
TEST_F(LemDCBM400600ControllerTest, test_start_transaction) {
    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->time_sync_helper, sync(testing::_)).Times(1).InSequence(seq);
    EXPECT_CALL(*this->http_client, post("/v1/legal", this->expected_start_transaction_request_body))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{201, R"({"running": true})"}));
    LemDCBM400600Controller controller(std::move(this->http_client), std::move(this->time_sync_helper),
                                       this->controller_config);

    // Act
    auto res = controller.start_transaction(this->transaction_request);

    // Verify
    EXPECT_EQ(transaction_request_status_to_string(res.status), "OK");
    EXPECT_TRUE(res.error->empty());
    EXPECT_THAT(res.transaction_min_stop_time.value(),
                testing::MatchesRegex("^[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{3}Z$"));
    EXPECT_THAT(res.transaction_max_stop_time.value(),
                testing::MatchesRegex("^[0-9]{4}-[0-9]{2}-[0-9]{2}T[0-9]{2}:[0-9]{2}:[0-9]{2}.[0-9]{3}Z$"));

    auto delta = Everest::Date::from_rfc3339(res.transaction_max_stop_time.value()) -
                 Everest::Date::from_rfc3339(res.transaction_min_stop_time.value());
    EXPECT_EQ(
        int(delta.count() / 1E9 / 60),
        48 * 60 -
            3); // delta of max and min stopping time should be 48 hours - 2 minutes wait time and 1 minute safety time
}

// \brief Test a failed start transaction with the DCBM returning an invalid response
TEST_P(LemDCBM400600ControllerTestInvalidResponses, test_start_transaction_fail_invalid_response) {

    // Setup
    // request fails due to an invalid response
    testing::Sequence seq;
    EXPECT_CALL(*this->time_sync_helper, sync(testing::_)).Times(1).InSequence(seq);
    EXPECT_CALL(*this->http_client, post("/v1/legal", this->expected_start_transaction_request_body))
        .Times(1)
        .InSequence(seq)
        .WillRepeatedly(GetParam());
    EXPECT_CALL(*this->time_sync_helper, sync(testing::_)).Times(1).InSequence(seq);
    EXPECT_CALL(*this->http_client, post("/v1/legal", this->expected_start_transaction_request_body))
        .Times(1)
        .InSequence(seq)
        .WillRepeatedly(GetParam());
    LemDCBM400600Controller controller(std::move(this->http_client), std::move(this->time_sync_helper),
                                       this->controller_config);

    // Act
    auto res = controller.start_transaction(this->transaction_request);

    // Verify
    EXPECT_EQ(transaction_request_status_to_string(res.status), "UNEXPECTED_ERROR");
    EXPECT_THAT(res.error.value(), testing::MatchesRegex("Failed to start transaction mock_transaction_id.*"));
    EXPECT_TRUE(res.transaction_min_stop_time->empty());
    EXPECT_TRUE(res.transaction_max_stop_time->empty());
}

// Setup parametrized invalid responses
static const std::string TEST_NAMES_START_TRANSACTION_INVALID_RESPONSES[2] = {"InvalidReturnCode",
                                                                              "InvalidResponseBody"};
INSTANTIATE_TEST_SUITE_P(
    LemDCBM400600ControllerTestStartTransactionInvalidResponses, LemDCBM400600ControllerTestInvalidResponses,
    testing::Values(testing::Return(HttpResponse{403, ""}), testing::Return(HttpResponse{201, "invalid"})),
    [](const testing::TestParamInfo<LemDCBM400600ControllerTestInvalidResponses::ParamType>& info) {
        return TEST_NAMES_START_TRANSACTION_INVALID_RESPONSES[info.index];
    });

// \brief Test a failed start transaction with the http request failing
TEST_F(LemDCBM400600ControllerTest, test_start_transaction_http_fail) {
    // Setup
    // request fails and throws an HttpClientError
    testing::Sequence seq;
    EXPECT_CALL(*this->time_sync_helper, sync(testing::_)).Times(1).InSequence(seq);
    EXPECT_CALL(*this->http_client, post("/v1/legal", this->expected_start_transaction_request_body))
        .Times(1)
        .InSequence(seq)
        .WillRepeatedly(testing::Throw(HttpClientError{"mock error"}));
    EXPECT_CALL(*this->time_sync_helper, sync(testing::_)).Times(1).InSequence(seq);
    EXPECT_CALL(*this->http_client, post("/v1/legal", this->expected_start_transaction_request_body))
        .Times(1)
        .InSequence(seq)
        .WillRepeatedly(testing::Throw(HttpClientError{"mock error"}));
    LemDCBM400600Controller controller(std::move(this->http_client), std::move(this->time_sync_helper),
                                       this->controller_config);

    // Act
    auto res = controller.start_transaction(this->transaction_request);

    // Verify
    EXPECT_EQ(transaction_request_status_to_string(res.status), "UNEXPECTED_ERROR");
    EXPECT_THAT(res.error.value(), testing::MatchesRegex("Failed to start transaction mock_transaction_id.*"));
    EXPECT_TRUE(res.transaction_min_stop_time->empty());
    EXPECT_TRUE(res.transaction_max_stop_time->empty());
}

//****************************************************************
// Test stop_transaction behavior

// \brief Test to stop a transaction and receive OCMF report.
TEST_F(LemDCBM400600ControllerTest, test_stop_transaction) {

    // Setup
    EXPECT_CALL(*this->time_sync_helper, sync(testing::_)).Times(0);

    EXPECT_CALL(*this->http_client, put("/v1/legal?transactionId=mock_transaction_id", R"({"running": false})"))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, this->put_legal_response}));

    EXPECT_CALL(*this->http_client, get("/v1/ocmf?transactionId=mock_transaction_id"))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, "mock_ocmf_string"}));
    LemDCBM400600Controller controller(std::move(this->http_client), std::move(this->time_sync_helper),
                                       this->controller_config);

    // Act
    auto res = controller.stop_transaction("mock_transaction_id");

    // Verify
    ASSERT_EQ(transaction_request_status_to_string(res.status), "OK");
    ASSERT_TRUE(res.signed_meter_value.has_value());
    ASSERT_EQ(res.signed_meter_value.value().signed_meter_data, "mock_ocmf_string");
}

// \brief Test a failed stop transaction with the DCBM returning an invalid response
TEST_P(LemDCBM400600ControllerTestInvalidResponses, test_stop_transaction_fail_invalid_response) {
    // Setup
    // request fails repeatedly
    EXPECT_CALL(*this->time_sync_helper, sync(testing::_)).Times(0);

    EXPECT_CALL(*this->http_client, put("/v1/legal?transactionId=mock_transaction_id", R"({"running": false})"))
        .Times(2)
        .WillRepeatedly(GetParam());
    LemDCBM400600Controller controller(std::move(this->http_client), std::move(this->time_sync_helper),
                                       this->controller_config);

    // Act
    auto res = controller.stop_transaction("mock_transaction_id");

    // Verify
    EXPECT_EQ(transaction_request_status_to_string(res.status), "UNEXPECTED_ERROR");
    EXPECT_THAT(res.error.value(), testing::MatchesRegex("Failed to stop transaction mock_transaction_id:.*"));
    EXPECT_FALSE(res.signed_meter_value.has_value());
}

// Setup parametrized invalid responses
static const std::string TEST_NAMES_STOP_TRANSACTION_INVALID_RESPONSES[2] = {"InvalidReturnCode",
                                                                             "InvalidResponseBody"};
INSTANTIATE_TEST_SUITE_P(
    LemDCBM400600ControllerTestStopTransactionInvalidResponses, LemDCBM400600ControllerTestInvalidResponses,
    testing::Values(testing::Return(HttpResponse{403, ""}), testing::Return(HttpResponse{200, "invalid"})),
    [](const testing::TestParamInfo<LemDCBM400600ControllerTestInvalidResponses::ParamType>& info) {
        return TEST_NAMES_STOP_TRANSACTION_INVALID_RESPONSES[info.index];
    });

// \brief Test a failed stop transaction with the http request failing
TEST_F(LemDCBM400600ControllerTest, test_stop_transaction_http_fail) {
    // Setup
    // request fails repeatedly
    EXPECT_CALL(*this->time_sync_helper, sync(testing::_)).Times(0);
    EXPECT_CALL(*this->http_client, put("/v1/legal?transactionId=mock_transaction_id", R"({"running": false})"))
        .Times(2)
        .WillRepeatedly(testing::Throw(HttpClientError{"mock error"}));
    LemDCBM400600Controller controller(std::move(this->http_client), std::move(this->time_sync_helper),
                                       this->controller_config);

    // Act
    auto res = controller.stop_transaction("mock_transaction_id");

    // Verify
    EXPECT_EQ(transaction_request_status_to_string(res.status), "UNEXPECTED_ERROR");
    EXPECT_THAT(res.error.value(), testing::MatchesRegex("Failed to stop transaction mock_transaction_id.*"));
    EXPECT_FALSE(res.signed_meter_value.has_value());
}

//****************************************************************
// Test init behavior

// \brief Test the init method fetches the meter_id
TEST_F(LemDCBM400600ControllerTest, test_init_meter_id) {
    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->time_sync_helper, sync_if_deadline_expired(testing::_)).Times(1).InSequence(seq);
    EXPECT_CALL(*this->http_client, get("/v1/livemeasure"))
        .Times(1)
        .InSequence(seq)
        .WillRepeatedly(testing::Return(HttpResponse{
            200,
            this->livemeasure_response,
        }));
    EXPECT_CALL(*this->http_client, get("/v1/status"))
        .Times(1)
        .InSequence(seq)
        .WillRepeatedly(testing::Return(HttpResponse{
            200,
            R"({ "meterId": "mock_meter_id",  "some_other_field": "other_value" })",
        }));
    EXPECT_CALL(*this->time_sync_helper, restart_unsafe_period()).Times(1).InSequence(seq);
    EXPECT_CALL(*this->time_sync_helper, sync_if_deadline_expired(testing::_)).Times(1).InSequence(seq);
    EXPECT_CALL(*this->http_client, get("/v1/livemeasure"))
        .Times(1)
        .InSequence(seq)
        .WillRepeatedly(testing::Return(HttpResponse{
            200,
            this->livemeasure_response,
        }));
    LemDCBM400600Controller controller(std::move(this->http_client), std::move(this->time_sync_helper),
                                       this->controller_config);

    // Assert: no meter id set before init call
    EXPECT_EQ(controller.get_powermeter().meter_id, "");

    // Act: initialize
    struct ntp_server_spec ntp_spec;
    controller.init();

    // verify by calling the powermeter interface that should provide the mocked metric id
    EXPECT_EQ(controller.get_powermeter().meter_id, "mock_meter_id");
}

// \brief Test the init method retries to fetch the meter id in case of a HttpClientError
TEST_F(LemDCBM400600ControllerTest, test_init_meter_id_retry_success) {

    // Setup
    int number_of_retries = 3;
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, get("/v1/status"))
        .Times(number_of_retries - 1)
        .InSequence(seq)
        .WillRepeatedly(testing::Throw(HttpClientError{"mock error"}));

    EXPECT_CALL(*this->http_client, get("/v1/status"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{
            200,
            R"({ "meterId": "mock_meter_id",  "some_other_field": "other_value" })",
        }));
    EXPECT_CALL(*this->time_sync_helper, restart_unsafe_period()).Times(1).InSequence(seq);
    EXPECT_CALL(*this->http_client, get("/v1/livemeasure"))
        .Times(1)
        .InSequence(seq)
        .WillRepeatedly(testing::Return(HttpResponse{
            200,
            this->livemeasure_response,
        }));
    const LemDCBM400600Controller::Conf controller_config{number_of_retries, 1, 1, 0};
    LemDCBM400600Controller controller(std::move(this->http_client), std::move(this->time_sync_helper),
                                       controller_config);

    // Act
    struct ntp_server_spec ntp_spec;
    controller.init();

    // Verify
    EXPECT_EQ(controller.get_powermeter().meter_id, "mock_meter_id");
}

// \brief Test at init the HttpClientError is re-raised after the provided number of attempts all failed
TEST_F(LemDCBM400600ControllerTest, test_init_meter_id_retry_fail_eventually) {
    // Setup
    int number_of_retries = 3;
    EXPECT_CALL(*this->http_client, get("/v1/status"))
        .Times(1 + number_of_retries)
        .WillRepeatedly(testing::Throw(HttpClientError{"mock error"}));
    EXPECT_CALL(*this->time_sync_helper, restart_unsafe_period()).Times(0);

    const LemDCBM400600Controller::Conf controller_config{number_of_retries, 1, 1, 0};
    LemDCBM400600Controller controller(std::move(this->http_client), std::move(this->time_sync_helper),
                                       controller_config);

    // Act & Verify
    struct ntp_server_spec ntp_spec;
    EXPECT_THROW(controller.init(), HttpClientError);
}

} // namespace module::main
