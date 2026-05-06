/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2026 Pionix GmbH and Contributors to EVerest
 *
 * test_din.cpp - Round-trip tests for the DIN 70121 converter.
 *
 * Uses GoogleTest. The wrapper exposes a C ABI (extern "C" in the header);
 * cJSON is used here only for structural JSON-equality assertions on the
 * decoded output.
 */

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>

#include "cJSON.h"
#include "cbv2g_json_wrapper.h"

namespace {

/* RAII wrapper so cJSON pointers are released even when an assertion fires. */
struct CJsonOwner {
    cJSON* ptr{nullptr};
    explicit CJsonOwner(cJSON* p) : ptr(p) {
    }
    ~CJsonOwner() {
        if (ptr != nullptr) {
            cJSON_Delete(ptr);
        }
    }
    CJsonOwner(const CJsonOwner&) = delete;
    CJsonOwner& operator=(const CJsonOwner&) = delete;
};

::testing::AssertionResult JsonStructurallyEqual(cJSON* expected, cJSON* actual) {
    if (expected == nullptr || actual == nullptr) {
        return ::testing::AssertionFailure() << "one of the JSON sub-trees was null";
    }
    if (cJSON_Compare(expected, actual, /*case_sensitive=*/1) == 0) {
        return ::testing::AssertionFailure() << "JSON sub-trees differ structurally";
    }
    return ::testing::AssertionSuccess();
}

cJSON* GetByPath(cJSON* root, std::initializer_list<const char*> path) {
    cJSON* cur = root;
    for (const char* segment : path) {
        if (cur == nullptr) {
            return nullptr;
        }
        cur = cJSON_GetObjectItemCaseSensitive(cur, segment);
    }
    return cur;
}

} // namespace

TEST(DinConverter, SessionSetupReqRoundTrip) {
    /* DIN SessionSetupReq carries a hexBinary EVCCID. The wrapper expects a
     * V2G_Message envelope with Header (SessionID hex) and Body. */
    const char* json = "{\"V2G_Message\":{"
                       "\"Header\":{\"SessionID\":\"0000000000000000\"},"
                       "\"Body\":{\"SessionSetupReq\":{\"EVCCID\":\"010203040506\"}}"
                       "}}";

    uint8_t exi[256] = {0};
    size_t exi_len = 0;
    ASSERT_EQ(cbv2g_encode(json, NS_DIN_MSG_DEF, exi, sizeof(exi), &exi_len), CBV2G_SUCCESS);
    ASSERT_GT(exi_len, 0u);

    char decoded[1024] = {0};
    ASSERT_EQ(cbv2g_decode(exi, exi_len, NS_DIN_MSG_DEF, decoded, sizeof(decoded)), CBV2G_SUCCESS);

    CJsonOwner original(cJSON_Parse(json));
    CJsonOwner round(cJSON_Parse(decoded));
    ASSERT_NE(original.ptr, nullptr);
    ASSERT_NE(round.ptr, nullptr);

    cJSON* orig_body = GetByPath(original.ptr, {"V2G_Message", "Body"});
    cJSON* round_body = GetByPath(round.ptr, {"V2G_Message", "Body"});
    ASSERT_NE(orig_body, nullptr);
    ASSERT_NE(round_body, nullptr);
    EXPECT_TRUE(JsonStructurallyEqual(orig_body, round_body));
}

TEST(DinConverter, SessionSetupResRoundTrip) {
    const char* json = "{\"V2G_Message\":{"
                       "\"Header\":{\"SessionID\":\"1122334455667788\"},"
                       "\"Body\":{\"SessionSetupRes\":{"
                       "\"ResponseCode\":\"OK_NewSessionEstablished\","
                       "\"EVSEID\":\"AABBCCDDEE\""
                       "}}"
                       "}}";

    uint8_t exi[256] = {0};
    size_t exi_len = 0;
    ASSERT_EQ(cbv2g_encode(json, NS_DIN_MSG_DEF, exi, sizeof(exi), &exi_len), CBV2G_SUCCESS);

    char decoded[1024] = {0};
    ASSERT_EQ(cbv2g_decode(exi, exi_len, NS_DIN_MSG_DEF, decoded, sizeof(decoded)), CBV2G_SUCCESS);

    CJsonOwner original(cJSON_Parse(json));
    CJsonOwner round(cJSON_Parse(decoded));
    ASSERT_NE(original.ptr, nullptr);
    ASSERT_NE(round.ptr, nullptr);

    cJSON* orig_res = GetByPath(original.ptr, {"V2G_Message", "Body", "SessionSetupRes"});
    cJSON* round_res = GetByPath(round.ptr, {"V2G_Message", "Body", "SessionSetupRes"});
    ASSERT_NE(orig_res, nullptr);
    ASSERT_NE(round_res, nullptr);

    cJSON* rc_orig = cJSON_GetObjectItemCaseSensitive(orig_res, "ResponseCode");
    cJSON* rc_round = cJSON_GetObjectItemCaseSensitive(round_res, "ResponseCode");
    ASSERT_NE(rc_orig, nullptr);
    ASSERT_NE(rc_round, nullptr);
    EXPECT_STREQ(rc_orig->valuestring, rc_round->valuestring);
}

/* ServiceDiscovery — covers service negotiation. The Req has no payload,
 * the Res carries a ResponseCode and a ChargeService.ServiceTag.ServiceID. */
TEST(DinConverter, ServiceDiscoveryReqRoundTrip) {
    const char* req_json = "{\"V2G_Message\":{"
                           "\"Header\":{\"SessionID\":\"0011223344556677\"},"
                           "\"Body\":{\"ServiceDiscoveryReq\":{}}"
                           "}}";

    uint8_t exi[256] = {0};
    size_t exi_len = 0;
    ASSERT_EQ(cbv2g_encode(req_json, NS_DIN_MSG_DEF, exi, sizeof(exi), &exi_len), CBV2G_SUCCESS);

    char decoded[1024] = {0};
    ASSERT_EQ(cbv2g_decode(exi, exi_len, NS_DIN_MSG_DEF, decoded, sizeof(decoded)), CBV2G_SUCCESS);

    CJsonOwner original(cJSON_Parse(req_json));
    CJsonOwner round(cJSON_Parse(decoded));
    ASSERT_NE(original.ptr, nullptr);
    ASSERT_NE(round.ptr, nullptr);

    cJSON* orig_body = GetByPath(original.ptr, {"V2G_Message", "Body"});
    cJSON* round_body = GetByPath(round.ptr, {"V2G_Message", "Body"});
    EXPECT_TRUE(JsonStructurallyEqual(orig_body, round_body));
}

TEST(DinConverter, ServiceDiscoveryResRoundTrip) {
    const char* res_json = "{\"V2G_Message\":{"
                           "\"Header\":{\"SessionID\":\"0011223344556677\"},"
                           "\"Body\":{\"ServiceDiscoveryRes\":{"
                           "\"ResponseCode\":\"OK\","
                           "\"PaymentOptions\":{\"PaymentOption\":[\"ExternalPayment\"]},"
                           "\"ChargeService\":{"
                           "\"ServiceTag\":{\"ServiceID\":1},"
                           "\"FreeService\":false,"
                           "\"EnergyTransferType\":\"DC_extended\""
                           "}"
                           "}}"
                           "}}";

    uint8_t exi[512] = {0};
    size_t exi_len = 0;
    ASSERT_EQ(cbv2g_encode(res_json, NS_DIN_MSG_DEF, exi, sizeof(exi), &exi_len), CBV2G_SUCCESS);

    char decoded[2048] = {0};
    ASSERT_EQ(cbv2g_decode(exi, exi_len, NS_DIN_MSG_DEF, decoded, sizeof(decoded)), CBV2G_SUCCESS);

    CJsonOwner round(cJSON_Parse(decoded));
    ASSERT_NE(round.ptr, nullptr);

    cJSON* res = GetByPath(round.ptr, {"V2G_Message", "Body", "ServiceDiscoveryRes"});
    ASSERT_NE(res, nullptr);

    cJSON* rcode = cJSON_GetObjectItemCaseSensitive(res, "ResponseCode");
    ASSERT_NE(rcode, nullptr);
    ASSERT_TRUE(cJSON_IsString(rcode));
    EXPECT_STREQ(rcode->valuestring, "OK");

    cJSON* svc = cJSON_GetObjectItemCaseSensitive(res, "ChargeService");
    ASSERT_NE(svc, nullptr);
    cJSON* etype = cJSON_GetObjectItemCaseSensitive(svc, "EnergyTransferType");
    ASSERT_NE(etype, nullptr);
    ASSERT_TRUE(cJSON_IsString(etype));
    EXPECT_STREQ(etype->valuestring, "DC_extended");
}

/* ChargeParameterDiscovery — covers DC parameter negotiation. */
TEST(DinConverter, ChargeParameterDiscoveryRoundTrip) {
    const char* json = "{\"V2G_Message\":{"
                       "\"Header\":{\"SessionID\":\"AABBCCDDEEFF0011\"},"
                       "\"Body\":{\"ChargeParameterDiscoveryReq\":{"
                       "\"EVRequestedEnergyTransferType\":\"DC_extended\","
                       "\"DC_EVChargeParameter\":{"
                       "\"DC_EVStatus\":{\"EVReady\":true,\"EVErrorCode\":0,\"EVRESSSOC\":42},"
                       "\"EVMaximumCurrentLimit\":{\"Multiplier\":0,\"Value\":125,\"Unit\":\"A\"},"
                       "\"EVMaximumVoltageLimit\":{\"Multiplier\":0,\"Value\":400,\"Unit\":\"V\"}"
                       "}"
                       "}}"
                       "}}";

    uint8_t exi[1024] = {0};
    size_t exi_len = 0;
    ASSERT_EQ(cbv2g_encode(json, NS_DIN_MSG_DEF, exi, sizeof(exi), &exi_len), CBV2G_SUCCESS);

    char decoded[2048] = {0};
    ASSERT_EQ(cbv2g_decode(exi, exi_len, NS_DIN_MSG_DEF, decoded, sizeof(decoded)), CBV2G_SUCCESS);

    CJsonOwner round(cJSON_Parse(decoded));
    ASSERT_NE(round.ptr, nullptr);

    cJSON* msg = GetByPath(round.ptr, {"V2G_Message", "Body", "ChargeParameterDiscoveryReq"});
    ASSERT_NE(msg, nullptr);

    cJSON* etype = cJSON_GetObjectItemCaseSensitive(msg, "EVRequestedEnergyTransferType");
    ASSERT_NE(etype, nullptr);
    ASSERT_TRUE(cJSON_IsString(etype));
    EXPECT_STREQ(etype->valuestring, "DC_extended");

    cJSON* soc = GetByPath(msg, {"DC_EVChargeParameter", "DC_EVStatus", "EVRESSSOC"});
    ASSERT_NE(soc, nullptr);
    ASSERT_TRUE(cJSON_IsNumber(soc));
    EXPECT_EQ(soc->valueint, 42);
}

/* CurrentDemand — RFC-cited 250ms-latency hotspot. Strict round-trip
 * on the key body fields. Note: DIN's CurrentDemandReq does NOT have
 * EVTargetEnergyRequest (that is an ISO 15118-20 field). */
TEST(DinConverter, CurrentDemandRoundTrip) {
    const char* json = "{\"V2G_Message\":{"
                       "\"Header\":{\"SessionID\":\"AABBCCDDEEFF0011\"},"
                       "\"Body\":{\"CurrentDemandReq\":{"
                       "\"DC_EVStatus\":{\"EVReady\":true,\"EVErrorCode\":0,\"EVRESSSOC\":55},"
                       "\"EVTargetVoltage\":{\"Multiplier\":0,\"Value\":400,\"Unit\":\"V\"},"
                       "\"EVTargetCurrent\":{\"Multiplier\":0,\"Value\":80,\"Unit\":\"A\"},"
                       "\"EVMaximumVoltageLimit\":{\"Multiplier\":0,\"Value\":450,\"Unit\":\"V\"},"
                       "\"EVMaximumCurrentLimit\":{\"Multiplier\":0,\"Value\":125,\"Unit\":\"A\"},"
                       "\"EVMaximumPowerLimit\":{\"Multiplier\":3,\"Value\":50,\"Unit\":\"W\"},"
                       "\"BulkChargingComplete\":false,"
                       "\"ChargingComplete\":false,"
                       "\"RemainingTimeToFullSoC\":{\"Multiplier\":0,\"Value\":1800,\"Unit\":\"s\"},"
                       "\"RemainingTimeToBulkSoC\":{\"Multiplier\":0,\"Value\":900,\"Unit\":\"s\"}"
                       "}}"
                       "}}";

    uint8_t exi[1024] = {0};
    size_t exi_len = 0;
    ASSERT_EQ(cbv2g_encode(json, NS_DIN_MSG_DEF, exi, sizeof(exi), &exi_len), CBV2G_SUCCESS);

    char decoded[2048] = {0};
    ASSERT_EQ(cbv2g_decode(exi, exi_len, NS_DIN_MSG_DEF, decoded, sizeof(decoded)), CBV2G_SUCCESS);

    CJsonOwner original(cJSON_Parse(json));
    CJsonOwner round(cJSON_Parse(decoded));
    ASSERT_NE(original.ptr, nullptr);
    ASSERT_NE(round.ptr, nullptr);

    cJSON* orig_msg = GetByPath(original.ptr, {"V2G_Message", "Body", "CurrentDemandReq"});
    cJSON* round_msg = GetByPath(round.ptr, {"V2G_Message", "Body", "CurrentDemandReq"});
    ASSERT_NE(orig_msg, nullptr);
    ASSERT_NE(round_msg, nullptr);
    EXPECT_TRUE(JsonStructurallyEqual(orig_msg, round_msg));
}

/* Regression for the previously-incomplete din_string_to_response_code
 * mapping table: a code that lived in the to_string side but was missing
 * from the from_string side used to round-trip back as "FAILED" instead
 * of preserving its identity. */
TEST(DinConverter, ExtendedResponseCodesRoundTrip) {
    const char* codes[] = {
        "FAILED_ServiceSelectionInvalid", "FAILED_PaymentSelectionInvalid",   "FAILED_CertificateExpired",
        "FAILED_SignatureError",          "FAILED_ContractCanceled",          "FAILED_WrongChargeParameter",
        "FAILED_PowerDeliveryNotApplied", "FAILED_TariffSelectionInvalid",    "FAILED_ChargingProfileInvalid",
        "FAILED_EVSEPresentVoltageToLow", "FAILED_MeteringSignatureNotValid", "FAILED_WrongEnergyTransferType",
    };

    for (const char* code : codes) {
        char json[256];
        std::snprintf(json, sizeof(json),
                      "{\"V2G_Message\":{"
                      "\"Header\":{\"SessionID\":\"0000000000000000\"},"
                      "\"Body\":{\"SessionSetupRes\":{"
                      "\"ResponseCode\":\"%s\","
                      "\"EVSEID\":\"AABBCCDDEE\""
                      "}}"
                      "}}",
                      code);

        uint8_t exi[256] = {0};
        size_t exi_len = 0;
        ASSERT_EQ(cbv2g_encode(json, NS_DIN_MSG_DEF, exi, sizeof(exi), &exi_len), CBV2G_SUCCESS) << "code=" << code;

        char decoded[1024] = {0};
        ASSERT_EQ(cbv2g_decode(exi, exi_len, NS_DIN_MSG_DEF, decoded, sizeof(decoded)), CBV2G_SUCCESS)
            << "code=" << code;

        CJsonOwner round(cJSON_Parse(decoded));
        ASSERT_NE(round.ptr, nullptr) << "code=" << code;
        cJSON* rc = GetByPath(round.ptr, {"V2G_Message", "Body", "SessionSetupRes", "ResponseCode"});
        ASSERT_NE(rc, nullptr) << "code=" << code;
        ASSERT_TRUE(cJSON_IsString(rc));
        EXPECT_STREQ(rc->valuestring, code) << "round-trip lost the ResponseCode mapping";
    }
}
