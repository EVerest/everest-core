/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2026 Pionix GmbH and Contributors to EVerest
 *
 * test_apphand.c - Round-trip tests for the App Handshake (SAP) converter.
 *
 * Self-contained: uses plain assert(), no external test framework.
 * Builds against the public cbv2g_json_wrapper.h API plus cJSON for
 * JSON-structural equality assertions.
 */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cJSON.h"
#include "cbv2g_json_wrapper.h"

static int g_failed = 0;
static int g_passed = 0;

#define TEST(name) static void name(void)

#define RUN(name) do { \
    printf("[ RUN  ] %s\n", #name); \
    name(); \
    printf("[  OK  ] %s\n", #name); \
    g_passed++; \
} while (0)

#define EXPECT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "  FAIL: %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        g_failed++; \
        return; \
    } \
} while (0)

/* Compare two JSON values structurally (cJSON_Compare is case-sensitive). */
static int json_eq(cJSON* a, cJSON* b) {
    return cJSON_Compare(a, b, 1 /* case_sensitive */);
}

TEST(test_apphand_req_round_trip) {
    const char* req_json =
        "{\"supportedAppProtocolReq\":{\"AppProtocol\":["
        "{\"ProtocolNamespace\":\"urn:din:70121:2012:MsgDef\","
        "\"VersionNumberMajor\":2,\"VersionNumberMinor\":0,"
        "\"SchemaID\":1,\"Priority\":1}"
        "]}}";

    uint8_t exi[256] = {0};
    size_t exi_len = 0;
    int rc = cbv2g_encode(req_json, NS_SAP, exi, sizeof(exi), &exi_len);
    EXPECT(rc == CBV2G_SUCCESS);
    EXPECT(exi_len > 0);

    char decoded[1024] = {0};
    rc = cbv2g_decode(exi, exi_len, NS_SAP, decoded, sizeof(decoded));
    EXPECT(rc == CBV2G_SUCCESS);
    EXPECT(strlen(decoded) > 0);

    cJSON* original = cJSON_Parse(req_json);
    cJSON* round = cJSON_Parse(decoded);
    EXPECT(original != NULL);
    EXPECT(round != NULL);
    EXPECT(json_eq(original, round));

    cJSON_Delete(original);
    cJSON_Delete(round);
}

TEST(test_apphand_res_round_trip) {
    const char* res_json =
        "{\"supportedAppProtocolRes\":{"
        "\"ResponseCode\":\"OK_SuccessfulNegotiation\","
        "\"SchemaID\":1"
        "}}";

    uint8_t exi[64] = {0};
    size_t exi_len = 0;
    int rc = cbv2g_encode(res_json, NS_SAP, exi, sizeof(exi), &exi_len);
    EXPECT(rc == CBV2G_SUCCESS);
    EXPECT(exi_len > 0);

    char decoded[512] = {0};
    rc = cbv2g_decode(exi, exi_len, NS_SAP, decoded, sizeof(decoded));
    EXPECT(rc == CBV2G_SUCCESS);

    cJSON* original = cJSON_Parse(res_json);
    cJSON* round = cJSON_Parse(decoded);
    EXPECT(original != NULL);
    EXPECT(round != NULL);
    EXPECT(json_eq(original, round));

    cJSON_Delete(original);
    cJSON_Delete(round);
}

TEST(test_apphand_res_failed_round_trip) {
    const char* res_json =
        "{\"supportedAppProtocolRes\":{"
        "\"ResponseCode\":\"Failed_NoNegotiation\""
        "}}";

    uint8_t exi[64] = {0};
    size_t exi_len = 0;
    int rc = cbv2g_encode(res_json, NS_SAP, exi, sizeof(exi), &exi_len);
    EXPECT(rc == CBV2G_SUCCESS);

    char decoded[512] = {0};
    rc = cbv2g_decode(exi, exi_len, NS_SAP, decoded, sizeof(decoded));
    EXPECT(rc == CBV2G_SUCCESS);

    cJSON* original = cJSON_Parse(res_json);
    cJSON* round = cJSON_Parse(decoded);
    EXPECT(json_eq(original, round));

    cJSON_Delete(original);
    cJSON_Delete(round);
}

TEST(test_null_inputs_return_invalid_param) {
    uint8_t exi[64] = {0};
    size_t exi_len = 0;
    char json[64] = {0};

    EXPECT(cbv2g_encode(NULL, NS_SAP, exi, sizeof(exi), &exi_len)
           == CBV2G_ERROR_INVALID_PARAM);
    EXPECT(cbv2g_encode("{}", NULL, exi, sizeof(exi), &exi_len)
           == CBV2G_ERROR_INVALID_PARAM);
    EXPECT(cbv2g_encode("{}", NS_SAP, NULL, sizeof(exi), &exi_len)
           == CBV2G_ERROR_INVALID_PARAM);
    EXPECT(cbv2g_encode("{}", NS_SAP, exi, sizeof(exi), NULL)
           == CBV2G_ERROR_INVALID_PARAM);
    EXPECT(cbv2g_encode("{}", NS_SAP, exi, 0, &exi_len)
           == CBV2G_ERROR_INVALID_PARAM);

    EXPECT(cbv2g_decode(NULL, 1, NS_SAP, json, sizeof(json))
           == CBV2G_ERROR_INVALID_PARAM);
    EXPECT(cbv2g_decode(exi, 1, NULL, json, sizeof(json))
           == CBV2G_ERROR_INVALID_PARAM);
    EXPECT(cbv2g_decode(exi, 1, NS_SAP, NULL, sizeof(json))
           == CBV2G_ERROR_INVALID_PARAM);
    EXPECT(cbv2g_decode(exi, 0, NS_SAP, json, sizeof(json))
           == CBV2G_ERROR_INVALID_PARAM);
    EXPECT(cbv2g_decode(exi, 1, NS_SAP, json, 0)
           == CBV2G_ERROR_INVALID_PARAM);
}

TEST(test_unknown_namespace_rejected) {
    const char* req_json =
        "{\"supportedAppProtocolReq\":{\"AppProtocol\":[]}}";
    uint8_t exi[64] = {0};
    size_t exi_len = 0;

    int rc = cbv2g_encode(req_json, "urn:not:a:real:namespace",
                          exi, sizeof(exi), &exi_len);
    EXPECT(rc == CBV2G_ERROR_UNKNOWN_NAMESPACE);

    char decoded[64] = {0};
    rc = cbv2g_decode((const uint8_t*)"\x80", 1, "urn:not:a:real:namespace",
                      decoded, sizeof(decoded));
    EXPECT(rc == CBV2G_ERROR_UNKNOWN_NAMESPACE);
}

TEST(test_version_string) {
    const char* v = cbv2g_get_version();
    EXPECT(v != NULL);
    EXPECT(strlen(v) > 0);
}

int main(void) {
    RUN(test_apphand_req_round_trip);
    RUN(test_apphand_res_round_trip);
    RUN(test_apphand_res_failed_round_trip);
    RUN(test_null_inputs_return_invalid_param);
    RUN(test_unknown_namespace_rejected);
    RUN(test_version_string);

    printf("\n%d passed, %d failed\n", g_passed, g_failed);
    return g_failed == 0 ? 0 : 1;
}
