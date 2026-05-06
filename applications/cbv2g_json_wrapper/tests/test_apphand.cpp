/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2026 Pionix GmbH and Contributors to EVerest
 *
 * test_apphand.cpp - Round-trip tests for the App Handshake (SAP) converter.
 *
 * Uses GoogleTest. The wrapper exposes a C ABI (extern "C" in the header);
 * cJSON is used here only for structural JSON-equality assertions on the
 * decoded output.
 */

#include <gtest/gtest.h>

#include <cstdint>
#include <cstring>
#include <string>

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

::testing::AssertionResult JsonStructurallyEqual(const char* expected_json, const char* actual_json) {
    CJsonOwner expected(cJSON_Parse(expected_json));
    CJsonOwner actual(cJSON_Parse(actual_json));
    if (expected.ptr == nullptr) {
        return ::testing::AssertionFailure() << "expected JSON did not parse";
    }
    if (actual.ptr == nullptr) {
        return ::testing::AssertionFailure() << "actual JSON did not parse";
    }
    if (cJSON_Compare(expected.ptr, actual.ptr, /*case_sensitive=*/1) == 0) {
        return ::testing::AssertionFailure()
               << "JSON differs.\n  expected: " << expected_json << "\n  actual:   " << actual_json;
    }
    return ::testing::AssertionSuccess();
}

} // namespace

TEST(AppHandConverter, ReqRoundTrip) {
    const char* req_json = "{\"supportedAppProtocolReq\":{\"AppProtocol\":["
                           "{\"ProtocolNamespace\":\"urn:din:70121:2012:MsgDef\","
                           "\"VersionNumberMajor\":2,\"VersionNumberMinor\":0,"
                           "\"SchemaID\":1,\"Priority\":1}"
                           "]}}";

    uint8_t exi[256] = {0};
    size_t exi_len = 0;
    ASSERT_EQ(cbv2g_encode(req_json, NS_SAP, exi, sizeof(exi), &exi_len), CBV2G_SUCCESS);
    ASSERT_GT(exi_len, 0u);

    char decoded[1024] = {0};
    ASSERT_EQ(cbv2g_decode(exi, exi_len, NS_SAP, decoded, sizeof(decoded)), CBV2G_SUCCESS);
    ASSERT_GT(std::strlen(decoded), 0u);
    EXPECT_TRUE(JsonStructurallyEqual(req_json, decoded));
}

TEST(AppHandConverter, ResRoundTrip) {
    const char* res_json = "{\"supportedAppProtocolRes\":{"
                           "\"ResponseCode\":\"OK_SuccessfulNegotiation\","
                           "\"SchemaID\":1"
                           "}}";

    uint8_t exi[64] = {0};
    size_t exi_len = 0;
    ASSERT_EQ(cbv2g_encode(res_json, NS_SAP, exi, sizeof(exi), &exi_len), CBV2G_SUCCESS);
    ASSERT_GT(exi_len, 0u);

    char decoded[512] = {0};
    ASSERT_EQ(cbv2g_decode(exi, exi_len, NS_SAP, decoded, sizeof(decoded)), CBV2G_SUCCESS);
    EXPECT_TRUE(JsonStructurallyEqual(res_json, decoded));
}

TEST(AppHandConverter, ResFailedRoundTrip) {
    const char* res_json = "{\"supportedAppProtocolRes\":{"
                           "\"ResponseCode\":\"Failed_NoNegotiation\""
                           "}}";

    uint8_t exi[64] = {0};
    size_t exi_len = 0;
    ASSERT_EQ(cbv2g_encode(res_json, NS_SAP, exi, sizeof(exi), &exi_len), CBV2G_SUCCESS);

    char decoded[512] = {0};
    ASSERT_EQ(cbv2g_decode(exi, exi_len, NS_SAP, decoded, sizeof(decoded)), CBV2G_SUCCESS);
    EXPECT_TRUE(JsonStructurallyEqual(res_json, decoded));
}

TEST(AppHandConverter, UnknownResponseCodeStringRejected) {
    /* Regression for the silent fallback that previously turned an
     * unrecognised string into ResponseCode=0 (OK_SuccessfulNegotiation). */
    const char* bad_json = "{\"supportedAppProtocolRes\":{"
                           "\"ResponseCode\":\"NotARealCode\""
                           "}}";

    uint8_t exi[64] = {0};
    size_t exi_len = 0;
    EXPECT_EQ(cbv2g_encode(bad_json, NS_SAP, exi, sizeof(exi), &exi_len), CBV2G_ERROR_JSON_PARSE);
}

TEST(AppHandConverter, NumericResponseCodeAccepted) {
    /* A numeric ResponseCode is the alternate canonical form. 0 ==
     * OK_SuccessfulNegotiation in the appHand_responseCodeType enum. */
    const char* numeric_json = "{\"supportedAppProtocolRes\":{"
                               "\"ResponseCode\":0"
                               "}}";

    uint8_t exi[64] = {0};
    size_t exi_len = 0;
    ASSERT_EQ(cbv2g_encode(numeric_json, NS_SAP, exi, sizeof(exi), &exi_len), CBV2G_SUCCESS);

    char decoded[512] = {0};
    ASSERT_EQ(cbv2g_decode(exi, exi_len, NS_SAP, decoded, sizeof(decoded)), CBV2G_SUCCESS);

    /* The decoder always emits the symbolic form. */
    const char* expected = "{\"supportedAppProtocolRes\":{"
                           "\"ResponseCode\":\"OK_SuccessfulNegotiation\""
                           "}}";
    EXPECT_TRUE(JsonStructurallyEqual(expected, decoded));
}

TEST(AppHandConverter, NullInputsReturnInvalidParam) {
    uint8_t exi[64] = {0};
    size_t exi_len = 0;
    char json[64] = {0};

    EXPECT_EQ(cbv2g_encode(nullptr, NS_SAP, exi, sizeof(exi), &exi_len), CBV2G_ERROR_INVALID_PARAM);
    EXPECT_EQ(cbv2g_encode("{}", nullptr, exi, sizeof(exi), &exi_len), CBV2G_ERROR_INVALID_PARAM);
    EXPECT_EQ(cbv2g_encode("{}", NS_SAP, nullptr, sizeof(exi), &exi_len), CBV2G_ERROR_INVALID_PARAM);
    EXPECT_EQ(cbv2g_encode("{}", NS_SAP, exi, sizeof(exi), nullptr), CBV2G_ERROR_INVALID_PARAM);
    EXPECT_EQ(cbv2g_encode("{}", NS_SAP, exi, 0, &exi_len), CBV2G_ERROR_INVALID_PARAM);

    EXPECT_EQ(cbv2g_decode(nullptr, 1, NS_SAP, json, sizeof(json)), CBV2G_ERROR_INVALID_PARAM);
    EXPECT_EQ(cbv2g_decode(exi, 1, nullptr, json, sizeof(json)), CBV2G_ERROR_INVALID_PARAM);
    EXPECT_EQ(cbv2g_decode(exi, 1, NS_SAP, nullptr, sizeof(json)), CBV2G_ERROR_INVALID_PARAM);
    EXPECT_EQ(cbv2g_decode(exi, 0, NS_SAP, json, sizeof(json)), CBV2G_ERROR_INVALID_PARAM);
    EXPECT_EQ(cbv2g_decode(exi, 1, NS_SAP, json, 0), CBV2G_ERROR_INVALID_PARAM);
}

TEST(AppHandConverter, UnknownNamespaceRejected) {
    const char* req_json = "{\"supportedAppProtocolReq\":{\"AppProtocol\":[]}}";
    uint8_t exi[64] = {0};
    size_t exi_len = 0;

    EXPECT_EQ(cbv2g_encode(req_json, "urn:not:a:real:namespace", exi, sizeof(exi), &exi_len),
              CBV2G_ERROR_UNKNOWN_NAMESPACE);

    char decoded[64] = {0};
    EXPECT_EQ(
        cbv2g_decode(reinterpret_cast<const uint8_t*>("\x80"), 1, "urn:not:a:real:namespace", decoded, sizeof(decoded)),
        CBV2G_ERROR_UNKNOWN_NAMESPACE);
}

TEST(AppHandConverter, VersionStringNonEmpty) {
    const char* v = cbv2g_get_version();
    ASSERT_NE(v, nullptr);
    EXPECT_GT(std::strlen(v), 0u);
}
