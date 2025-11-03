// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "httpClientInterface.hpp"
#include "isabellenhuetteIemDcrController.hpp"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace module::main {

class HTTPClientMock : public HttpClientInterface {
public:
    MOCK_METHOD(HttpResponse, get, (const std::string& path), (override, const));
    MOCK_METHOD(HttpResponse, post, (const std::string& path, const std::string& body), (override, const));
};

// Fixture class providing
//   - a http client mock
//   - default responses & request objects
class IsabellenhuetteIemDcrControllerTest : public ::testing::Test {

protected:
    std::unique_ptr<HTTPClientMock> http_client;

    const std::string gw_response{R"({
        "CT": "",
        "CI": "",
        "TM": "2024-12-16T19:00:22,000+0200 U"  
    })"};

    const std::string metervalue_response{R"({
        "MS": "1ISA0200000001",
        "TM": "2024-12-15T22:42:28,000+0200 U",
        "EF": "t",
        "ST": "G",
        "XT": false,
        "RD": [
            {
            "RV": "00000446.540",
            "RI": "1-b:1.8.e",
            "RU": "kWh",
            "RT": "DC"
            },
            {
            "RV": "00000004.881",
            "RI": "1-b:2.8.e",
            "RU": "kWh",
            "RT": "DC"
            },
            {
            "WV": "00000001.040",
            "WI": "transaction_1-b:1.8.e",
            "WU": "kWh",
            "WT": "DC"
            },
            {
            "WV": "00000002.503",
            "WI": "transaction_1-b:2.8.e",
            "WU": "kWh",
            "WT": "DC"
            }
        ],
        "U1": 1.089,
        "U2": 60.1,
        "U3": 0,
        "U4": 0,
        "I": 5,
        "P": 300,
        "XS": "0x0000004000000004",
        "XC": "0x0000, 0x00000000, 0x00, 0x00"
    })"};

    const std::string publickey = "3059301306072a8648ce3d020106082a8648ce3d"
                                  "03010703420004A97A28BE22DEDF619A497288FF"
                                  "F217832B37E44B8B1F8918C48EB5FBF5CB8B5FBB"
                                  "717D32CD2211534D968CA4425B9FCBF5A93E60F2"
                                  "CE97BCD63F9CAD287F5E08";

    const std::string publickey_response{R"({
        "SA": "ECDSA-secp256r1-SHA256",
        "PK": ")" + publickey + R"("
    })"};

    const std::string publickey_response_alt{R"({
        "SA": "ECDSA-secp256r1-SHA256",
        "PK": "abc"
    })"};

    const std::string receipt_response{R"({
        "OCMF|{"FV":"1.0","GI":"Isabellenhuette IEM-DCR-125-1000-32-00-006-B_000",
        "GS":"1ISA0200001132","GV":"DU-02.00.12_SU-02.00.08","MV":"Isabellenhuette",
        "MM":"IEM-DCR-125-1000-32-00-006-B_000","MS":"1ISA0200001132",
        "CT":"DC-Test-Charger","CI":"CP-DE-4711","IS":true,"IL":"CERTIFIED",
        "IF":["RFID_PSK","OCPP_CERTIFIED","ISO15118_NONE","PLMN_NONE"],
        "IT":"ISO15693","ID":"9109543224","PG":"T4","TT":"0,20 EUR/kWh",
        "RD":[{"TM":"2025-06-06T09:26:24,000+0200 I","TX":"B","EF":"",
        "ST":"G","RV":"00000446.540","RI":"1-b:1.8.e","RU":"kWh","RT":"DC"}
        ,{"TM":"2025-06-06T09:26:45,000+0200 I","TX":"E","EF":"","ST":"G",
        "RV":"00000446.540","RI":"1-b:1.8.e","RU":"kWh","RT":"DC"}]}|
        {"SA":"ECDSA-secp256r1-SHA256","SD":"304402203DE5BF9E3A5960935
        47AAEDA8CCEAFD88CAB59AC65FE616BD33158F2002545960220B3ED5B4A32E
        F8028577EF0E4F823DACF30DE75CA744C9FF9560FAE0134D19ABF"}
    })"};

    const IsaIemDcrController::SnapshotConfig controller_config{"+0100", true,    12,      10000,       2,
                                                                250,     "ctVal", "ciVal", "ttInitial", true};

    void SetUp() override {
        this->http_client = std::make_unique<HTTPClientMock>();
    }
};

//****************************************************************
// Test init behavior

/// \brief Test init() interacts propely with HttpClient
TEST_F(IsabellenhuetteIemDcrControllerTest, test_init) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, get("/counter/v1/ocmf/gw"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{200, this->gw_response}));
    EXPECT_CALL(
        *this->http_client,
        post("/counter/v1/ocmf/gw", testing::MatchesRegex(R"(\{.*"CT":")" + this->controller_config.CT + R"(","CI":")" +
                                                          this->controller_config.CI + R"(","TM":.*\}.*)")))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, this->gw_response}));
    EXPECT_CALL(*this->http_client,
                post("/counter/v1/ocmf/tariff",
                     testing::MatchesRegex(R"(\{.*"TT":")" + this->controller_config.TT_initial + R"("\}.*)")))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, ""}));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act
    bool retVal = controller.init();

    // Verify
    EXPECT_EQ(retVal, true);
}

/// \brief Test init() interacts propely with HttpClient with gw already set
TEST_F(IsabellenhuetteIemDcrControllerTest, test_init_with_gw_already_set) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, get("/counter/v1/ocmf/gw"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{200, this->gw_response}));
    EXPECT_CALL(
        *this->http_client,
        post("/counter/v1/ocmf/gw", testing::MatchesRegex(R"(\{.*"CT":")" + this->controller_config.CT + R"(","CI":")" +
                                                          this->controller_config.CI + R"(","TM":.*\}.*)")))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{428, ""}));
    EXPECT_CALL(*this->http_client,
                post("/counter/v1/ocmf/tariff",
                     testing::MatchesRegex(R"(\{.*"TT":")" + this->controller_config.TT_initial + R"("\}.*)")))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, ""}));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act
    bool retVal = controller.init();

    // Verify
    EXPECT_EQ(retVal, true);
}

/// \brief Test init() returns false on missing connection
TEST_F(IsabellenhuetteIemDcrControllerTest, test_init_timeout) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, get("/counter/v1/ocmf/gw"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{408, ""})); // 408 Request timeout
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act
    bool retVal = controller.init();

    // Verify
    EXPECT_EQ(retVal, false);
}

//****************************************************************
// Test get powermeter behavior

/// \brief Test get_metervalue() returns correct values
TEST_F(IsabellenhuetteIemDcrControllerTest, test_get_metervalue) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, get("/counter/v1/ocmf/metervalue"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{200, this->metervalue_response}));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act
    auto meter_value_response = controller.get_metervalue();
    types::powermeter::Powermeter tmp_powermeter;
    std::string tmp_status;
    bool tmp_transaction_active;
    std::tie(tmp_powermeter, tmp_status, tmp_transaction_active) = meter_value_response;

    // Verify
    EXPECT_EQ(tmp_transaction_active, false);
    EXPECT_EQ(tmp_status, "0x0000, 0x00000000, 0x00, 0x00");
    EXPECT_EQ(tmp_powermeter.timestamp, "2024-12-15T22:42:28,000+0200");
    EXPECT_THAT(tmp_powermeter.energy_Wh_import.total, testing::FloatEq(1040));
    EXPECT_THAT(tmp_powermeter.energy_Wh_export->total, testing::FloatEq(2503));
    EXPECT_THAT(tmp_powermeter.power_W->total, testing::FloatEq(300.0));
    EXPECT_THAT(tmp_powermeter.current_A->DC.value(), testing::FloatEq(5.0));
    EXPECT_THAT(tmp_powermeter.voltage_V->DC.value(), testing::FloatEq(60.1));
    EXPECT_THAT(tmp_powermeter.meter_id.value(), "1ISA0200000001");
}

/// \brief Test get_metervalue() fails due to an invalid response status code
TEST_F(IsabellenhuetteIemDcrControllerTest, test_get_metervalue_invalid_response_code) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, get("/counter/v1/ocmf/metervalue"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{403, this->metervalue_response}));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act & Verify
    EXPECT_THROW(controller.get_metervalue(), IsaIemDcrController::UnexpectedIemDcrResponseCode);
}

/// \brief Test get_metervalue() fails due to an invalid response status body
TEST_F(IsabellenhuetteIemDcrControllerTest, test_get_metervalue_fail_invalid_response_body) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, get("/counter/v1/ocmf/metervalue"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{200, "invalid"}));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act & Verify
    EXPECT_THROW(controller.get_metervalue(), IsaIemDcrController::UnexpectedIemDcrResponseBody);
}

/// \brief get_metervalue() fails due to an http client error
TEST_F(IsabellenhuetteIemDcrControllerTest, test_get_metervalue_fail_http_error) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, get("/counter/v1/ocmf/metervalue"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Throw(HttpClientError("http client mock error")));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act & Verify
    EXPECT_THROW(controller.get_metervalue(), HttpClientError);
}

//****************************************************************
// Test get publickey behavior

/// \brief Test get_publickey() returns correct value with enabled caching
TEST_F(IsabellenhuetteIemDcrControllerTest, test_get_publickey) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, get("/counter/v1/ocmf/publickey"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{200, this->publickey_response}));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act & Verify
    auto retVal = controller.get_publickey(true);
    EXPECT_EQ(retVal, this->publickey);
    retVal = controller.get_publickey(true);
    EXPECT_EQ(retVal, this->publickey); // Expect previous value (only one GET request allowed above)
}

/// \brief Test get_publickey() returns correct value with disabled caching
TEST_F(IsabellenhuetteIemDcrControllerTest, test_get_publickey_no_cache) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, get("/counter/v1/ocmf/publickey"))
        .Times(2)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{200, this->publickey_response}))
        .WillOnce(testing::Return(HttpResponse{200, this->publickey_response_alt}));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act & Verify
    auto retVal = controller.get_publickey(false);
    EXPECT_EQ(retVal, this->publickey);
    retVal = controller.get_publickey(false);
    EXPECT_EQ(retVal, "abc"); // Expect fresh value of 2nd call (publickey_response_alt)
}

/// \brief get_publickey() fails due to an http client error
TEST_F(IsabellenhuetteIemDcrControllerTest, test_get_publickey_fail_http_error) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, get("/counter/v1/ocmf/publickey"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Throw(HttpClientError("http client mock error")));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act & Verify
    EXPECT_THROW(controller.get_publickey(false), HttpClientError);
}

//****************************************************************
// Test start transaction behavior

/// \brief Test post_receipt("B") starts transaction properly
TEST_F(IsabellenhuetteIemDcrControllerTest, test_post_receipt_B) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, post("/counter/v1/ocmf/receipt", testing::MatchesRegex(R"(\{.*"TX":"B"*\}.*)")))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, ""}));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act & Verify
    controller.post_receipt("B");
}

/// \brief Test post_receipt("B") fails due to an invalid response status code
TEST_F(IsabellenhuetteIemDcrControllerTest, test_post_receipt_B_invalid_response_code) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, post("/counter/v1/ocmf/receipt", testing::MatchesRegex(R"(\{.*"TX":"B"*\}.*)")))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{428, ""}));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act & Verify
    EXPECT_THROW(controller.post_receipt("B"), IsaIemDcrController::UnexpectedIemDcrResponseCode);
}

/// \brief Test post_receipt("B") fails due to an http client error
TEST_F(IsabellenhuetteIemDcrControllerTest, test_post_receipt_B_fail_http_error) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, post("/counter/v1/ocmf/receipt", testing::MatchesRegex(R"(\{.*"TX":"B"*\}.*)")))
        .Times(1)
        .WillOnce(testing::Throw(HttpClientError("http client mock error")));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act & Verify
    EXPECT_THROW(controller.post_receipt("B"), HttpClientError);
}

//****************************************************************
// Test end transaction behavior

/// \brief Test post_receipt("E") ends transaction properly
TEST_F(IsabellenhuetteIemDcrControllerTest, test_post_receipt_E) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, post("/counter/v1/ocmf/receipt", testing::MatchesRegex(R"(\{.*"TX":"E"*\}.*)")))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{200, ""}));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act & Verify
    controller.post_receipt("E");
}

/// \brief Test post_receipt("E") fails due to an invalid response status code
TEST_F(IsabellenhuetteIemDcrControllerTest, test_post_receipt_E_invalid_response_code) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, post("/counter/v1/ocmf/receipt", testing::MatchesRegex(R"(\{.*"TX":"E"*\}.*)")))
        .Times(1)
        .WillOnce(testing::Return(HttpResponse{428, ""}));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act & Verify
    EXPECT_THROW(controller.post_receipt("E"), IsaIemDcrController::UnexpectedIemDcrResponseCode);
}

/// \brief Test post_receipt("B") fails due to an http client error
TEST_F(IsabellenhuetteIemDcrControllerTest, test_post_receipt_E_fail_http_error) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, post("/counter/v1/ocmf/receipt", testing::MatchesRegex(R"(\{.*"TX":"E"*\}.*)")))
        .Times(1)
        .WillOnce(testing::Throw(HttpClientError("http client mock error")));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act & Verify
    EXPECT_THROW(controller.post_receipt("E"), HttpClientError);
}

/// \brief Test get_receipt() returns correct values
TEST_F(IsabellenhuetteIemDcrControllerTest, test_get_receipt) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, get("/counter/v1/ocmf/receipt"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{200, this->receipt_response}));
    EXPECT_CALL(*this->http_client, get("/counter/v1/ocmf/publickey"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{200, this->publickey_response}));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act
    auto signed_meter_value = controller.get_receipt();

    // Verify
    EXPECT_EQ(signed_meter_value.signed_meter_data, this->receipt_response);
    EXPECT_EQ(signed_meter_value.signing_method, "");
    EXPECT_EQ(signed_meter_value.encoding_method, "OCMF");
    EXPECT_EQ(signed_meter_value.public_key, this->publickey);
}

/// \brief Test get_receipt() fails due to an invalid response status code
TEST_F(IsabellenhuetteIemDcrControllerTest, test_get_receipt_invalid_response_code) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, get("/counter/v1/ocmf/receipt"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Return(HttpResponse{403, this->receipt_response}));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act & Verify
    EXPECT_THROW(controller.get_receipt(), IsaIemDcrController::UnexpectedIemDcrResponseCode);
}

/// \brief get_receipt() fails due to an http client error
TEST_F(IsabellenhuetteIemDcrControllerTest, test_get_receipt_fail_http_error) {

    // Setup
    testing::Sequence seq;
    EXPECT_CALL(*this->http_client, get("/counter/v1/ocmf/receipt"))
        .Times(1)
        .InSequence(seq)
        .WillOnce(testing::Throw(HttpClientError("http client mock error")));
    IsaIemDcrController controller(std::move(this->http_client), this->controller_config);

    // Act & Verify
    EXPECT_THROW(controller.get_receipt(), HttpClientError);
}

} // namespace module::main