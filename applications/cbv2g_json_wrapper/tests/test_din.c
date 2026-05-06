/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2026 Pionix GmbH and Contributors to EVerest
 *
 * test_din.c - Round-trip test for the DIN 70121 converter.
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

static int json_eq(cJSON* a, cJSON* b) {
    return cJSON_Compare(a, b, 1);
}

TEST(test_din_session_setup_req_round_trip) {
    /* DIN SessionSetupReq carries a hexBinary EVCCID. The wrapper expects a
     * V2G_Message envelope with Header (SessionID hex) and Body. */
    const char* json =
        "{\"V2G_Message\":{"
        "\"Header\":{\"SessionID\":\"0000000000000000\"},"
        "\"Body\":{\"SessionSetupReq\":{\"EVCCID\":\"010203040506\"}}"
        "}}";

    uint8_t exi[256] = {0};
    size_t exi_len = 0;
    int rc = cbv2g_encode(json, NS_DIN_MSG_DEF, exi, sizeof(exi), &exi_len);
    EXPECT(rc == CBV2G_SUCCESS);
    EXPECT(exi_len > 0);

    char decoded[1024] = {0};
    rc = cbv2g_decode(exi, exi_len, NS_DIN_MSG_DEF, decoded, sizeof(decoded));
    EXPECT(rc == CBV2G_SUCCESS);

    cJSON* original = cJSON_Parse(json);
    cJSON* round = cJSON_Parse(decoded);
    EXPECT(original != NULL);
    EXPECT(round != NULL);

    /* Compare Body.SessionSetupReq.EVCCID and Header.SessionID. */
    cJSON* orig_body = cJSON_GetObjectItemCaseSensitive(
        cJSON_GetObjectItemCaseSensitive(original, "V2G_Message"), "Body");
    cJSON* round_body = cJSON_GetObjectItemCaseSensitive(
        cJSON_GetObjectItemCaseSensitive(round, "V2G_Message"), "Body");
    EXPECT(orig_body != NULL && round_body != NULL);
    EXPECT(json_eq(orig_body, round_body));

    cJSON_Delete(original);
    cJSON_Delete(round);
}

TEST(test_din_session_setup_res_round_trip) {
    const char* json =
        "{\"V2G_Message\":{"
        "\"Header\":{\"SessionID\":\"1122334455667788\"},"
        "\"Body\":{\"SessionSetupRes\":{"
        "\"ResponseCode\":\"OK_NewSessionEstablished\","
        "\"EVSEID\":\"AABBCCDDEE\""
        "}}"
        "}}";

    uint8_t exi[256] = {0};
    size_t exi_len = 0;
    int rc = cbv2g_encode(json, NS_DIN_MSG_DEF, exi, sizeof(exi), &exi_len);
    EXPECT(rc == CBV2G_SUCCESS);

    char decoded[1024] = {0};
    rc = cbv2g_decode(exi, exi_len, NS_DIN_MSG_DEF, decoded, sizeof(decoded));
    EXPECT(rc == CBV2G_SUCCESS);

    cJSON* original = cJSON_Parse(json);
    cJSON* round = cJSON_Parse(decoded);
    EXPECT(original != NULL);
    EXPECT(round != NULL);

    cJSON* orig_res = cJSON_GetObjectItemCaseSensitive(
        cJSON_GetObjectItemCaseSensitive(
            cJSON_GetObjectItemCaseSensitive(original, "V2G_Message"), "Body"),
        "SessionSetupRes");
    cJSON* round_res = cJSON_GetObjectItemCaseSensitive(
        cJSON_GetObjectItemCaseSensitive(
            cJSON_GetObjectItemCaseSensitive(round, "V2G_Message"), "Body"),
        "SessionSetupRes");
    EXPECT(orig_res != NULL && round_res != NULL);

    /* ResponseCode should match exactly. */
    cJSON* rc_orig = cJSON_GetObjectItemCaseSensitive(orig_res, "ResponseCode");
    cJSON* rc_round = cJSON_GetObjectItemCaseSensitive(round_res, "ResponseCode");
    EXPECT(rc_orig != NULL && rc_round != NULL);
    EXPECT(strcmp(rc_orig->valuestring, rc_round->valuestring) == 0);

    cJSON_Delete(original);
    cJSON_Delete(round);
}

/* ServiceDiscovery — covers service negotiation. The Req has no payload,
 * the Res carries a ResponseCode and a ChargeService.ServiceTag.ServiceID. */
TEST(test_din_service_discovery_round_trip) {
    const char* req_json =
        "{\"V2G_Message\":{"
        "\"Header\":{\"SessionID\":\"0011223344556677\"},"
        "\"Body\":{\"ServiceDiscoveryReq\":{}}"
        "}}";

    uint8_t exi[256] = {0};
    size_t exi_len = 0;
    int rc = cbv2g_encode(req_json, NS_DIN_MSG_DEF, exi, sizeof(exi), &exi_len);
    EXPECT(rc == CBV2G_SUCCESS);

    char decoded[1024] = {0};
    rc = cbv2g_decode(exi, exi_len, NS_DIN_MSG_DEF, decoded, sizeof(decoded));
    EXPECT(rc == CBV2G_SUCCESS);

    /* Strict round-trip on the Body sub-tree. */
    cJSON* original = cJSON_Parse(req_json);
    cJSON* round = cJSON_Parse(decoded);
    EXPECT(original != NULL && round != NULL);
    cJSON* orig_body = cJSON_GetObjectItemCaseSensitive(
        cJSON_GetObjectItemCaseSensitive(original, "V2G_Message"), "Body");
    cJSON* round_body = cJSON_GetObjectItemCaseSensitive(
        cJSON_GetObjectItemCaseSensitive(round, "V2G_Message"), "Body");
    EXPECT(orig_body != NULL && round_body != NULL);
    EXPECT(json_eq(orig_body, round_body));

    cJSON_Delete(original);
    cJSON_Delete(round);

    /* Now the Res half. */
    const char* res_json =
        "{\"V2G_Message\":{"
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

    uint8_t exi2[512] = {0};
    size_t exi2_len = 0;
    rc = cbv2g_encode(res_json, NS_DIN_MSG_DEF, exi2, sizeof(exi2), &exi2_len);
    EXPECT(rc == CBV2G_SUCCESS);

    char decoded2[2048] = {0};
    rc = cbv2g_decode(exi2, exi2_len, NS_DIN_MSG_DEF, decoded2, sizeof(decoded2));
    EXPECT(rc == CBV2G_SUCCESS);

    cJSON* round2 = cJSON_Parse(decoded2);
    EXPECT(round2 != NULL);
    cJSON* res = cJSON_GetObjectItemCaseSensitive(
        cJSON_GetObjectItemCaseSensitive(
            cJSON_GetObjectItemCaseSensitive(round2, "V2G_Message"), "Body"),
        "ServiceDiscoveryRes");
    EXPECT(res != NULL);
    cJSON* rcode = cJSON_GetObjectItemCaseSensitive(res, "ResponseCode");
    EXPECT(rcode != NULL && cJSON_IsString(rcode));
    EXPECT(strcmp(rcode->valuestring, "OK") == 0);
    cJSON* svc = cJSON_GetObjectItemCaseSensitive(res, "ChargeService");
    EXPECT(svc != NULL);
    cJSON* etype = cJSON_GetObjectItemCaseSensitive(svc, "EnergyTransferType");
    EXPECT(etype != NULL && cJSON_IsString(etype));
    EXPECT(strcmp(etype->valuestring, "DC_extended") == 0);

    cJSON_Delete(round2);
}

/* ChargeParameterDiscovery — covers DC parameter negotiation. */
TEST(test_din_charge_parameter_discovery_round_trip) {
    const char* json =
        "{\"V2G_Message\":{"
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
    int rc = cbv2g_encode(json, NS_DIN_MSG_DEF, exi, sizeof(exi), &exi_len);
    EXPECT(rc == CBV2G_SUCCESS);

    char decoded[2048] = {0};
    rc = cbv2g_decode(exi, exi_len, NS_DIN_MSG_DEF, decoded, sizeof(decoded));
    EXPECT(rc == CBV2G_SUCCESS);

    cJSON* round = cJSON_Parse(decoded);
    EXPECT(round != NULL);
    cJSON* msg = cJSON_GetObjectItemCaseSensitive(
        cJSON_GetObjectItemCaseSensitive(
            cJSON_GetObjectItemCaseSensitive(round, "V2G_Message"), "Body"),
        "ChargeParameterDiscoveryReq");
    EXPECT(msg != NULL);
    cJSON* etype = cJSON_GetObjectItemCaseSensitive(msg, "EVRequestedEnergyTransferType");
    EXPECT(etype != NULL && cJSON_IsString(etype));
    EXPECT(strcmp(etype->valuestring, "DC_extended") == 0);

    cJSON* dcp = cJSON_GetObjectItemCaseSensitive(msg, "DC_EVChargeParameter");
    EXPECT(dcp != NULL);
    cJSON* status = cJSON_GetObjectItemCaseSensitive(dcp, "DC_EVStatus");
    EXPECT(status != NULL);
    cJSON* soc = cJSON_GetObjectItemCaseSensitive(status, "EVRESSSOC");
    EXPECT(soc != NULL && cJSON_IsNumber(soc));
    EXPECT(soc->valueint == 42);

    cJSON_Delete(round);
}

/* CurrentDemand — RFC-cited 250ms-latency hotspot. Strict round-trip
 * on the key body fields. Note: DIN's CurrentDemandReq does NOT have
 * EVTargetEnergyRequest (that is an ISO 15118-20 field). */
TEST(test_din_current_demand_round_trip) {
    const char* json =
        "{\"V2G_Message\":{"
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
    int rc = cbv2g_encode(json, NS_DIN_MSG_DEF, exi, sizeof(exi), &exi_len);
    EXPECT(rc == CBV2G_SUCCESS);

    char decoded[2048] = {0};
    rc = cbv2g_decode(exi, exi_len, NS_DIN_MSG_DEF, decoded, sizeof(decoded));
    EXPECT(rc == CBV2G_SUCCESS);

    cJSON* original = cJSON_Parse(json);
    cJSON* round = cJSON_Parse(decoded);
    EXPECT(original != NULL && round != NULL);

    cJSON* orig_msg = cJSON_GetObjectItemCaseSensitive(
        cJSON_GetObjectItemCaseSensitive(
            cJSON_GetObjectItemCaseSensitive(original, "V2G_Message"), "Body"),
        "CurrentDemandReq");
    cJSON* round_msg = cJSON_GetObjectItemCaseSensitive(
        cJSON_GetObjectItemCaseSensitive(
            cJSON_GetObjectItemCaseSensitive(round, "V2G_Message"), "Body"),
        "CurrentDemandReq");
    EXPECT(orig_msg != NULL && round_msg != NULL);
    /* Strict structural compare on the message body. */
    EXPECT(json_eq(orig_msg, round_msg));

    cJSON_Delete(original);
    cJSON_Delete(round);
}

int main(void) {
    RUN(test_din_session_setup_req_round_trip);
    RUN(test_din_session_setup_res_round_trip);
    RUN(test_din_service_discovery_round_trip);
    RUN(test_din_charge_parameter_discovery_round_trip);
    RUN(test_din_current_demand_round_trip);

    printf("\n%d passed, %d failed\n", g_passed, g_failed);
    return g_failed == 0 ? 0 : 1;
}
