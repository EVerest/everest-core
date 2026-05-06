/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2026 Pionix GmbH and Contributors to EVerest
 *
 * iso2_converter.c - ISO 15118-2 JSON/EXI converter
 */

#include "converters.h"
#include "json_utils.h"
#include "cJSON.h"

#include <cbv2g/common/exi_bitstream.h>
#include <cbv2g/iso_2/iso2_msgDefDatatypes.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>
#include <cbv2g/iso_2/iso2_msgDefDecoder.h>

#include <string.h>
#include <stdio.h>

/* Forward decls for xmldsig SignedInfo fragment (used during PnC signature creation) */
static int json_to_iso2_signed_info(cJSON* json, struct iso2_SignedInfoType* msg);
static cJSON* iso2_signed_info_to_json(const struct iso2_SignedInfoType* msg);

/* Forward declarations for message converters */
static int json_to_iso2_header(cJSON* json, struct iso2_MessageHeaderType* header);
static cJSON* iso2_header_to_json(const struct iso2_MessageHeaderType* header);

/* Body message converters - encode */
static int json_to_iso2_session_setup_req(cJSON* json, struct iso2_SessionSetupReqType* msg);
static int json_to_iso2_session_setup_res(cJSON* json, struct iso2_SessionSetupResType* msg);
static int json_to_iso2_service_discovery_req(cJSON* json, struct iso2_ServiceDiscoveryReqType* msg);
static int json_to_iso2_service_discovery_res(cJSON* json, struct iso2_ServiceDiscoveryResType* msg);
static int json_to_iso2_payment_service_selection_req(cJSON* json, struct iso2_PaymentServiceSelectionReqType* msg);
static int json_to_iso2_payment_service_selection_res(cJSON* json, struct iso2_PaymentServiceSelectionResType* msg);
static int json_to_iso2_payment_details_req(cJSON* json, struct iso2_PaymentDetailsReqType* msg);
static int json_to_iso2_payment_details_res(cJSON* json, struct iso2_PaymentDetailsResType* msg);
static int json_to_iso2_authorization_req(cJSON* json, struct iso2_AuthorizationReqType* msg);
static int json_to_iso2_authorization_res(cJSON* json, struct iso2_AuthorizationResType* msg);
static int json_to_iso2_charge_parameter_discovery_req(cJSON* json, struct iso2_ChargeParameterDiscoveryReqType* msg);
static int json_to_iso2_charge_parameter_discovery_res(cJSON* json, struct iso2_ChargeParameterDiscoveryResType* msg);
static int json_to_iso2_cable_check_req(cJSON* json, struct iso2_CableCheckReqType* msg);
static int json_to_iso2_cable_check_res(cJSON* json, struct iso2_CableCheckResType* msg);
static int json_to_iso2_pre_charge_req(cJSON* json, struct iso2_PreChargeReqType* msg);
static int json_to_iso2_pre_charge_res(cJSON* json, struct iso2_PreChargeResType* msg);
static int json_to_iso2_power_delivery_req(cJSON* json, struct iso2_PowerDeliveryReqType* msg);
static int json_to_iso2_power_delivery_res(cJSON* json, struct iso2_PowerDeliveryResType* msg);
static int json_to_iso2_current_demand_req(cJSON* json, struct iso2_CurrentDemandReqType* msg);
static int json_to_iso2_current_demand_res(cJSON* json, struct iso2_CurrentDemandResType* msg);
static int json_to_iso2_charging_status_req(cJSON* json, struct iso2_ChargingStatusReqType* msg);
static int json_to_iso2_charging_status_res(cJSON* json, struct iso2_ChargingStatusResType* msg);
static int json_to_iso2_welding_detection_req(cJSON* json, struct iso2_WeldingDetectionReqType* msg);
static int json_to_iso2_welding_detection_res(cJSON* json, struct iso2_WeldingDetectionResType* msg);
static int json_to_iso2_session_stop_req(cJSON* json, struct iso2_SessionStopReqType* msg);
static int json_to_iso2_session_stop_res(cJSON* json, struct iso2_SessionStopResType* msg);

/* Body message converters - decode */
static cJSON* iso2_session_setup_req_to_json(const struct iso2_SessionSetupReqType* msg);
static cJSON* iso2_session_setup_res_to_json(const struct iso2_SessionSetupResType* msg);
static cJSON* iso2_service_discovery_req_to_json(const struct iso2_ServiceDiscoveryReqType* msg);
static cJSON* iso2_service_discovery_res_to_json(const struct iso2_ServiceDiscoveryResType* msg);
static cJSON* iso2_payment_service_selection_req_to_json(const struct iso2_PaymentServiceSelectionReqType* msg);
static cJSON* iso2_payment_service_selection_res_to_json(const struct iso2_PaymentServiceSelectionResType* msg);
static cJSON* iso2_payment_details_req_to_json(const struct iso2_PaymentDetailsReqType* msg);
static cJSON* iso2_payment_details_res_to_json(const struct iso2_PaymentDetailsResType* msg);
static cJSON* iso2_authorization_req_to_json(const struct iso2_AuthorizationReqType* msg);
static cJSON* iso2_authorization_res_to_json(const struct iso2_AuthorizationResType* msg);
static cJSON* iso2_charge_parameter_discovery_req_to_json(const struct iso2_ChargeParameterDiscoveryReqType* msg);
static cJSON* iso2_charge_parameter_discovery_res_to_json(const struct iso2_ChargeParameterDiscoveryResType* msg);
static cJSON* iso2_cable_check_req_to_json(const struct iso2_CableCheckReqType* msg);
static cJSON* iso2_cable_check_res_to_json(const struct iso2_CableCheckResType* msg);
static cJSON* iso2_pre_charge_req_to_json(const struct iso2_PreChargeReqType* msg);
static cJSON* iso2_pre_charge_res_to_json(const struct iso2_PreChargeResType* msg);
static cJSON* iso2_power_delivery_req_to_json(const struct iso2_PowerDeliveryReqType* msg);
static cJSON* iso2_power_delivery_res_to_json(const struct iso2_PowerDeliveryResType* msg);
static cJSON* iso2_current_demand_req_to_json(const struct iso2_CurrentDemandReqType* msg);
static cJSON* iso2_current_demand_res_to_json(const struct iso2_CurrentDemandResType* msg);
static cJSON* iso2_charging_status_req_to_json(const struct iso2_ChargingStatusReqType* msg);
static cJSON* iso2_charging_status_res_to_json(const struct iso2_ChargingStatusResType* msg);
static cJSON* iso2_welding_detection_req_to_json(const struct iso2_WeldingDetectionReqType* msg);
static cJSON* iso2_welding_detection_res_to_json(const struct iso2_WeldingDetectionResType* msg);
static cJSON* iso2_session_stop_req_to_json(const struct iso2_SessionStopReqType* msg);
static cJSON* iso2_session_stop_res_to_json(const struct iso2_SessionStopResType* msg);

/* Helper functions for common types */
static int json_to_iso2_physical_value(cJSON* json, struct iso2_PhysicalValueType* pv);
static cJSON* iso2_physical_value_to_json(const struct iso2_PhysicalValueType* pv);
static int json_to_iso2_dc_ev_status(cJSON* json, struct iso2_DC_EVStatusType* status);
static cJSON* iso2_dc_ev_status_to_json(const struct iso2_DC_EVStatusType* status);
static int json_to_iso2_dc_evse_status(cJSON* json, struct iso2_DC_EVSEStatusType* status);
static cJSON* iso2_dc_evse_status_to_json(const struct iso2_DC_EVSEStatusType* status);

/* Response code string conversion */
static const char* iso2_response_code_to_string(iso2_responseCodeType code);
static iso2_responseCodeType iso2_string_to_response_code(const char* str);

/*
 * Encode ISO 15118-2 message from JSON to EXI
 */
int iso2_encode(const char* json_str, uint8_t* out, size_t out_size, size_t* out_len) {
    int result = CBV2G_ERROR_INTERNAL;
    cJSON* root = NULL;

    /* Parse JSON */
    root = cJSON_Parse(json_str);
    if (root == NULL) {
        set_error("Failed to parse JSON: %s", cJSON_GetErrorPtr());
        return CBV2G_ERROR_JSON_PARSE;
    }

    /* Get V2G_Message object. If absent, fall through to fragment encoding —
     * Josev calls to_exi() with a bare body element (e.g. AuthorizationReq) when
     * generating XML signatures during PnC, so the JSON is not wrapped. */
    cJSON* v2g_msg = cJSON_GetObjectItemCaseSensitive(root, "V2G_Message");
    if (v2g_msg == NULL) {
        struct iso2_exiFragment frag;
        init_iso2_exiFragment(&frag);
        cJSON* frag_msg = NULL;

        #define HANDLE_FRAG_ENCODE(name, field, init_func, conv_func) \
            frag_msg = cJSON_GetObjectItemCaseSensitive(root, #name); \
            if (frag_msg != NULL) { \
                init_func(&frag.field); \
                frag.field##_isUsed = 1; \
                result = conv_func(frag_msg, &frag.field); \
                if (result != CBV2G_SUCCESS) { \
                    cJSON_Delete(root); \
                    return result; \
                } \
                exi_bitstream_t fstream; \
                exi_bitstream_init(&fstream, out, out_size, 0, NULL); \
                int frag_result = encode_iso2_exiFragment(&fstream, &frag); \
                if (frag_result != 0) { \
                    set_error("EXI fragment encoding failed with error code: %d", frag_result); \
                    cJSON_Delete(root); \
                    return CBV2G_ERROR_ENCODING_FAILED; \
                } \
                *out_len = exi_bitstream_get_length(&fstream); \
                cJSON_Delete(root); \
                return CBV2G_SUCCESS; \
            }

        HANDLE_FRAG_ENCODE(AuthorizationReq, AuthorizationReq, init_iso2_AuthorizationReqType, json_to_iso2_authorization_req)
        HANDLE_FRAG_ENCODE(SignedInfo, SignedInfo, init_iso2_SignedInfoType, json_to_iso2_signed_info)
        /* iso2_exiFragment also covers MeteringReceiptReq, CertificateInstallationReq,
         * CertificateUpdateReq, ContractSignatureCertChain, eMAID. Add HANDLE_FRAG_ENCODE
         * entries here as additional PnC flows are exercised. */

        #undef HANDLE_FRAG_ENCODE

        set_error("V2G_Message not found in JSON");
        cJSON_Delete(root);
        return CBV2G_ERROR_JSON_PARSE;
    }

    /* Initialize EXI document */
    struct iso2_exiDocument doc;
    init_iso2_exiDocument(&doc);

    /* Parse Header */
    cJSON* header = cJSON_GetObjectItemCaseSensitive(v2g_msg, "Header");
    if (header != NULL) {
        result = json_to_iso2_header(header, &doc.V2G_Message.Header);
        if (result != CBV2G_SUCCESS) {
            cJSON_Delete(root);
            return result;
        }
    }

    /* Parse Body */
    cJSON* body = cJSON_GetObjectItemCaseSensitive(v2g_msg, "Body");
    if (body == NULL) {
        set_error("Body not found in V2G_Message");
        cJSON_Delete(root);
        return CBV2G_ERROR_JSON_PARSE;
    }

    /* Initialize Body structure - CRITICAL: must be done before setting any message */
    init_iso2_BodyType(&doc.V2G_Message.Body);

    /* Determine message type and convert */
    cJSON* msg = NULL;

    #define HANDLE_MSG_ENCODE(name, field, init_func, conv_func) \
        msg = cJSON_GetObjectItemCaseSensitive(body, #name); \
        if (msg != NULL) { \
            init_func(&doc.V2G_Message.Body.field); \
            doc.V2G_Message.Body.field##_isUsed = 1; \
            result = conv_func(msg, &doc.V2G_Message.Body.field); \
            goto encode; \
        }

    HANDLE_MSG_ENCODE(SessionSetupReq, SessionSetupReq, init_iso2_SessionSetupReqType, json_to_iso2_session_setup_req)
    HANDLE_MSG_ENCODE(SessionSetupRes, SessionSetupRes, init_iso2_SessionSetupResType, json_to_iso2_session_setup_res)
    HANDLE_MSG_ENCODE(ServiceDiscoveryReq, ServiceDiscoveryReq, init_iso2_ServiceDiscoveryReqType, json_to_iso2_service_discovery_req)
    HANDLE_MSG_ENCODE(ServiceDiscoveryRes, ServiceDiscoveryRes, init_iso2_ServiceDiscoveryResType, json_to_iso2_service_discovery_res)
    HANDLE_MSG_ENCODE(PaymentServiceSelectionReq, PaymentServiceSelectionReq, init_iso2_PaymentServiceSelectionReqType, json_to_iso2_payment_service_selection_req)
    HANDLE_MSG_ENCODE(PaymentServiceSelectionRes, PaymentServiceSelectionRes, init_iso2_PaymentServiceSelectionResType, json_to_iso2_payment_service_selection_res)
    HANDLE_MSG_ENCODE(PaymentDetailsReq, PaymentDetailsReq, init_iso2_PaymentDetailsReqType, json_to_iso2_payment_details_req)
    HANDLE_MSG_ENCODE(PaymentDetailsRes, PaymentDetailsRes, init_iso2_PaymentDetailsResType, json_to_iso2_payment_details_res)
    HANDLE_MSG_ENCODE(AuthorizationReq, AuthorizationReq, init_iso2_AuthorizationReqType, json_to_iso2_authorization_req)
    HANDLE_MSG_ENCODE(AuthorizationRes, AuthorizationRes, init_iso2_AuthorizationResType, json_to_iso2_authorization_res)
    HANDLE_MSG_ENCODE(ChargeParameterDiscoveryReq, ChargeParameterDiscoveryReq, init_iso2_ChargeParameterDiscoveryReqType, json_to_iso2_charge_parameter_discovery_req)
    HANDLE_MSG_ENCODE(ChargeParameterDiscoveryRes, ChargeParameterDiscoveryRes, init_iso2_ChargeParameterDiscoveryResType, json_to_iso2_charge_parameter_discovery_res)
    HANDLE_MSG_ENCODE(CableCheckReq, CableCheckReq, init_iso2_CableCheckReqType, json_to_iso2_cable_check_req)
    HANDLE_MSG_ENCODE(CableCheckRes, CableCheckRes, init_iso2_CableCheckResType, json_to_iso2_cable_check_res)
    HANDLE_MSG_ENCODE(PreChargeReq, PreChargeReq, init_iso2_PreChargeReqType, json_to_iso2_pre_charge_req)
    HANDLE_MSG_ENCODE(PreChargeRes, PreChargeRes, init_iso2_PreChargeResType, json_to_iso2_pre_charge_res)
    HANDLE_MSG_ENCODE(PowerDeliveryReq, PowerDeliveryReq, init_iso2_PowerDeliveryReqType, json_to_iso2_power_delivery_req)
    HANDLE_MSG_ENCODE(PowerDeliveryRes, PowerDeliveryRes, init_iso2_PowerDeliveryResType, json_to_iso2_power_delivery_res)
    HANDLE_MSG_ENCODE(CurrentDemandReq, CurrentDemandReq, init_iso2_CurrentDemandReqType, json_to_iso2_current_demand_req)
    HANDLE_MSG_ENCODE(CurrentDemandRes, CurrentDemandRes, init_iso2_CurrentDemandResType, json_to_iso2_current_demand_res)
    HANDLE_MSG_ENCODE(ChargingStatusReq, ChargingStatusReq, init_iso2_ChargingStatusReqType, json_to_iso2_charging_status_req)
    HANDLE_MSG_ENCODE(ChargingStatusRes, ChargingStatusRes, init_iso2_ChargingStatusResType, json_to_iso2_charging_status_res)
    HANDLE_MSG_ENCODE(WeldingDetectionReq, WeldingDetectionReq, init_iso2_WeldingDetectionReqType, json_to_iso2_welding_detection_req)
    HANDLE_MSG_ENCODE(WeldingDetectionRes, WeldingDetectionRes, init_iso2_WeldingDetectionResType, json_to_iso2_welding_detection_res)
    HANDLE_MSG_ENCODE(SessionStopReq, SessionStopReq, init_iso2_SessionStopReqType, json_to_iso2_session_stop_req)
    HANDLE_MSG_ENCODE(SessionStopRes, SessionStopRes, init_iso2_SessionStopResType, json_to_iso2_session_stop_res)

    #undef HANDLE_MSG_ENCODE

    set_error("Unknown ISO 15118-2 message type in Body");
    cJSON_Delete(root);
    return CBV2G_ERROR_UNKNOWN_MESSAGE;

encode:
    if (result != CBV2G_SUCCESS) {
        cJSON_Delete(root);
        return result;
    }

    /* Initialize EXI bitstream and encode */
    exi_bitstream_t stream;
    exi_bitstream_init(&stream, out, out_size, 0, NULL);

    int exi_result = encode_iso2_exiDocument(&stream, &doc);
    if (exi_result != 0) {
        set_error("EXI encoding failed with error code: %d", exi_result);
        cJSON_Delete(root);
        return CBV2G_ERROR_ENCODING_FAILED;
    }

    *out_len = exi_bitstream_get_length(&stream);
    cJSON_Delete(root);
    return CBV2G_SUCCESS;
}

/*
 * Decode ISO 15118-2 message from EXI to JSON
 */
int iso2_decode(const uint8_t* exi, size_t exi_len, char* out, size_t out_size) {
    int result = CBV2G_ERROR_INTERNAL;
    cJSON* root = NULL;
    char* json_str = NULL;

    /* Initialize and decode EXI document */
    exi_bitstream_t stream;
    exi_bitstream_init(&stream, (uint8_t*)exi, exi_len, 0, NULL);

    struct iso2_exiDocument doc;
    init_iso2_exiDocument(&doc);

    int exi_result = decode_iso2_exiDocument(&stream, &doc);
    if (exi_result != 0) {
        set_error("EXI decoding failed with error code: %d", exi_result);
        return CBV2G_ERROR_DECODING_FAILED;
    }

    /* Create JSON structure */
    root = cJSON_CreateObject();
    cJSON* v2g_msg = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "V2G_Message", v2g_msg);

    /* Convert Header */
    cJSON* header_json = iso2_header_to_json(&doc.V2G_Message.Header);
    cJSON_AddItemToObject(v2g_msg, "Header", header_json);

    /* Convert Body */
    cJSON* body_json = cJSON_CreateObject();
    cJSON_AddItemToObject(v2g_msg, "Body", body_json);

    #define HANDLE_MSG_DECODE(name, field, conv_func) \
        if (doc.V2G_Message.Body.field##_isUsed) { \
            cJSON* msg_json = conv_func(&doc.V2G_Message.Body.field); \
            if (msg_json == NULL) { \
                set_error("Failed to convert " #name " to JSON"); \
                result = CBV2G_ERROR_JSON_GENERATE; \
                goto cleanup; \
            } \
            cJSON_AddItemToObject(body_json, #name, msg_json); \
        }

    HANDLE_MSG_DECODE(SessionSetupReq, SessionSetupReq, iso2_session_setup_req_to_json)
    HANDLE_MSG_DECODE(SessionSetupRes, SessionSetupRes, iso2_session_setup_res_to_json)
    HANDLE_MSG_DECODE(ServiceDiscoveryReq, ServiceDiscoveryReq, iso2_service_discovery_req_to_json)
    HANDLE_MSG_DECODE(ServiceDiscoveryRes, ServiceDiscoveryRes, iso2_service_discovery_res_to_json)
    HANDLE_MSG_DECODE(PaymentServiceSelectionReq, PaymentServiceSelectionReq, iso2_payment_service_selection_req_to_json)
    HANDLE_MSG_DECODE(PaymentServiceSelectionRes, PaymentServiceSelectionRes, iso2_payment_service_selection_res_to_json)
    HANDLE_MSG_DECODE(PaymentDetailsReq, PaymentDetailsReq, iso2_payment_details_req_to_json)
    HANDLE_MSG_DECODE(PaymentDetailsRes, PaymentDetailsRes, iso2_payment_details_res_to_json)
    HANDLE_MSG_DECODE(AuthorizationReq, AuthorizationReq, iso2_authorization_req_to_json)
    HANDLE_MSG_DECODE(AuthorizationRes, AuthorizationRes, iso2_authorization_res_to_json)
    HANDLE_MSG_DECODE(ChargeParameterDiscoveryReq, ChargeParameterDiscoveryReq, iso2_charge_parameter_discovery_req_to_json)
    HANDLE_MSG_DECODE(ChargeParameterDiscoveryRes, ChargeParameterDiscoveryRes, iso2_charge_parameter_discovery_res_to_json)
    HANDLE_MSG_DECODE(CableCheckReq, CableCheckReq, iso2_cable_check_req_to_json)
    HANDLE_MSG_DECODE(CableCheckRes, CableCheckRes, iso2_cable_check_res_to_json)
    HANDLE_MSG_DECODE(PreChargeReq, PreChargeReq, iso2_pre_charge_req_to_json)
    HANDLE_MSG_DECODE(PreChargeRes, PreChargeRes, iso2_pre_charge_res_to_json)
    HANDLE_MSG_DECODE(PowerDeliveryReq, PowerDeliveryReq, iso2_power_delivery_req_to_json)
    HANDLE_MSG_DECODE(PowerDeliveryRes, PowerDeliveryRes, iso2_power_delivery_res_to_json)
    HANDLE_MSG_DECODE(CurrentDemandReq, CurrentDemandReq, iso2_current_demand_req_to_json)
    HANDLE_MSG_DECODE(CurrentDemandRes, CurrentDemandRes, iso2_current_demand_res_to_json)
    HANDLE_MSG_DECODE(ChargingStatusReq, ChargingStatusReq, iso2_charging_status_req_to_json)
    HANDLE_MSG_DECODE(ChargingStatusRes, ChargingStatusRes, iso2_charging_status_res_to_json)
    HANDLE_MSG_DECODE(WeldingDetectionReq, WeldingDetectionReq, iso2_welding_detection_req_to_json)
    HANDLE_MSG_DECODE(WeldingDetectionRes, WeldingDetectionRes, iso2_welding_detection_res_to_json)
    HANDLE_MSG_DECODE(SessionStopReq, SessionStopReq, iso2_session_stop_req_to_json)
    HANDLE_MSG_DECODE(SessionStopRes, SessionStopRes, iso2_session_stop_res_to_json)

    #undef HANDLE_MSG_DECODE

    /* Serialize JSON to string */
    json_str = cJSON_PrintUnformatted(root);
    if (json_str == NULL) {
        set_error("Failed to serialize JSON");
        result = CBV2G_ERROR_JSON_GENERATE;
        goto cleanup;
    }

    /* Bounded write to the output buffer via snprintf
     * (CWE-120 / CWE-126). */
    int written = snprintf(out, out_size, "%s", json_str);
    if (written < 0 || (size_t)written >= out_size) {
        set_error("Output buffer too small: need %d, have %zu", written + 1, out_size);
        result = CBV2G_ERROR_BUFFER_TOO_SMALL;
        goto cleanup;
    }
    result = CBV2G_SUCCESS;

cleanup:
    if (root != NULL) cJSON_Delete(root);
    if (json_str != NULL) cJSON_free(json_str);
    return result;
}

/* ============== Helper Functions ============== */

static int json_to_iso2_header(cJSON* json, struct iso2_MessageHeaderType* header) {
    init_iso2_MessageHeaderType(header);

    /* SessionID - default to 8 zero bytes if not provided */
    header->SessionID.bytesLen = iso2_sessionIDType_BYTES_SIZE;
    memset(header->SessionID.bytes, 0, iso2_sessionIDType_BYTES_SIZE);

    cJSON* session_id = cJSON_GetObjectItemCaseSensitive(json, "SessionID");
    if (session_id && cJSON_IsString(session_id)) {
        /* SessionID is hexBinary in XSD - use hex decoding (CWE-126:
         * bound the read by 2 * max byte size). */
        size_t vs_len = strnlen(session_id->valuestring, iso2_sessionIDType_BYTES_SIZE * 2);
        size_t len = hex_decode(session_id->valuestring, vs_len,
                                header->SessionID.bytes, iso2_sessionIDType_BYTES_SIZE);
        if (len > 0) {
            header->SessionID.bytesLen = len;
        }
    }

    /* Signature block (optional) — required for PnC AuthorizationReq + MeteringReceiptReq.
     * Josev provides the JSON shape:
     *   "Signature": {
     *       "SignedInfo": { ... },
     *       "SignatureValue": { "value": "<base64>" }   (or just a base64 string)
     *   }
     */
    cJSON* sig = cJSON_GetObjectItemCaseSensitive(json, "Signature");
    if (sig != NULL) {
        struct iso2_SignatureType* s = &header->Signature;

        cJSON* signed_info = cJSON_GetObjectItemCaseSensitive(sig, "SignedInfo");
        if (signed_info != NULL) {
            json_to_iso2_signed_info(signed_info, &s->SignedInfo);
        }

        cJSON* sigval = cJSON_GetObjectItemCaseSensitive(sig, "SignatureValue");
        if (sigval != NULL) {
            const char* b64 = NULL;
            if (cJSON_IsString(sigval)) {
                b64 = sigval->valuestring;
            } else if (cJSON_IsObject(sigval)) {
                /* Josev wraps the value under a "value" key when emitting hexBinary
                 * elements; fall back to raw "Value" too. */
                cJSON* v = cJSON_GetObjectItemCaseSensitive(sigval, "value");
                if (v == NULL) v = cJSON_GetObjectItemCaseSensitive(sigval, "Value");
                if (v && cJSON_IsString(v)) b64 = v->valuestring;
            }
            if (b64 != NULL) {
                /* Bound base64 read (CWE-126). */
                size_t sv_max = ((iso2_SignatureValueType_BYTES_SIZE + 2) / 3) * 4;
                size_t b64_len = strnlen(b64, sv_max);
                size_t len = base64_decode(b64, b64_len,
                                           s->SignatureValue.CONTENT.bytes,
                                           iso2_SignatureValueType_BYTES_SIZE);
                s->SignatureValue.CONTENT.bytesLen = (uint16_t)len;
            }
            s->SignatureValue.Id_isUsed = 0;
        }

        s->Id_isUsed = 0;
        s->KeyInfo_isUsed = 0;
        s->Object_isUsed = 0;
        header->Signature_isUsed = 1;
    }

    return CBV2G_SUCCESS;
}

static cJSON* iso2_header_to_json(const struct iso2_MessageHeaderType* header) {
    cJSON* json = cJSON_CreateObject();

    /* SessionID is hexBinary in XSD - use hex encoding */
    char hex[17];  /* 8 bytes * 2 + null terminator */
    hex_encode(header->SessionID.bytes, header->SessionID.bytesLen, hex, sizeof(hex));
    cJSON_AddStringToObject(json, "SessionID", hex);

    return json;
}

static int json_to_iso2_physical_value(cJSON* json, struct iso2_PhysicalValueType* pv) {
    pv->Multiplier = json_get_int(json, "Multiplier");
    pv->Value = json_get_int(json, "Value");

    const char* unit = json_get_string(json, "Unit");
    if (strcmp(unit, "h") == 0) pv->Unit = iso2_unitSymbolType_h;
    else if (strcmp(unit, "m") == 0) pv->Unit = iso2_unitSymbolType_m;
    else if (strcmp(unit, "s") == 0) pv->Unit = iso2_unitSymbolType_s;
    else if (strcmp(unit, "A") == 0) pv->Unit = iso2_unitSymbolType_A;
    else if (strcmp(unit, "V") == 0) pv->Unit = iso2_unitSymbolType_V;
    else if (strcmp(unit, "W") == 0) pv->Unit = iso2_unitSymbolType_W;
    else if (strcmp(unit, "Wh") == 0) pv->Unit = iso2_unitSymbolType_Wh;
    else pv->Unit = iso2_unitSymbolType_W; /* Default */

    return CBV2G_SUCCESS;
}

static cJSON* iso2_physical_value_to_json(const struct iso2_PhysicalValueType* pv) {
    cJSON* json = cJSON_CreateObject();

    cJSON_AddNumberToObject(json, "Multiplier", pv->Multiplier);
    cJSON_AddNumberToObject(json, "Value", pv->Value);

    const char* unit;
    switch (pv->Unit) {
        case iso2_unitSymbolType_h: unit = "h"; break;
        case iso2_unitSymbolType_m: unit = "m"; break;
        case iso2_unitSymbolType_s: unit = "s"; break;
        case iso2_unitSymbolType_A: unit = "A"; break;
        case iso2_unitSymbolType_V: unit = "V"; break;
        case iso2_unitSymbolType_W: unit = "W"; break;
        case iso2_unitSymbolType_Wh: unit = "Wh"; break;
        default: unit = "W"; break;
    }
    cJSON_AddStringToObject(json, "Unit", unit);

    return json;
}

static int json_to_iso2_dc_ev_status(cJSON* json, struct iso2_DC_EVStatusType* status) {
    status->EVReady = json_get_bool(json, "EVReady");

    const char* error_code = json_get_string(json, "EVErrorCode");
    if (strcmp(error_code, "NO_ERROR") == 0) {
        status->EVErrorCode = iso2_DC_EVErrorCodeType_NO_ERROR;
    } else if (strcmp(error_code, "FAILED_RESSTemperatureInhibit") == 0) {
        status->EVErrorCode = iso2_DC_EVErrorCodeType_FAILED_RESSTemperatureInhibit;
    } else {
        status->EVErrorCode = iso2_DC_EVErrorCodeType_NO_ERROR;
    }

    status->EVRESSSOC = json_get_int(json, "EVRESSSOC");
    return CBV2G_SUCCESS;
}

static cJSON* iso2_dc_ev_status_to_json(const struct iso2_DC_EVStatusType* status) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddBoolToObject(json, "EVReady", status->EVReady);

    const char* error_str;
    switch (status->EVErrorCode) {
        case iso2_DC_EVErrorCodeType_NO_ERROR: error_str = "NO_ERROR"; break;
        case iso2_DC_EVErrorCodeType_FAILED_RESSTemperatureInhibit: error_str = "FAILED_RESSTemperatureInhibit"; break;
        case iso2_DC_EVErrorCodeType_FAILED_EVShiftPosition: error_str = "FAILED_EVShiftPosition"; break;
        case iso2_DC_EVErrorCodeType_FAILED_ChargerConnectorLockFault: error_str = "FAILED_ChargerConnectorLockFault"; break;
        case iso2_DC_EVErrorCodeType_FAILED_EVRESSMalfunction: error_str = "FAILED_EVRESSMalfunction"; break;
        default: error_str = "NO_ERROR"; break;
    }
    cJSON_AddStringToObject(json, "EVErrorCode", error_str);
    cJSON_AddNumberToObject(json, "EVRESSSOC", status->EVRESSSOC);
    return json;
}

static int json_to_iso2_dc_evse_status(cJSON* json, struct iso2_DC_EVSEStatusType* status) {
    status->EVSEIsolationStatus_isUsed = json_has_key(json, "EVSEIsolationStatus");
    if (status->EVSEIsolationStatus_isUsed) {
        status->EVSEIsolationStatus = json_get_int(json, "EVSEIsolationStatus");
    }
    status->EVSEStatusCode = json_get_int(json, "EVSEStatusCode");
    status->NotificationMaxDelay = json_get_int(json, "NotificationMaxDelay");
    status->EVSENotification = json_get_int(json, "EVSENotification");
    return CBV2G_SUCCESS;
}

static const char* iso2_isolation_level_to_string(iso2_isolationLevelType level) {
    switch (level) {
        case iso2_isolationLevelType_Invalid: return "Invalid";
        case iso2_isolationLevelType_Valid: return "Valid";
        case iso2_isolationLevelType_Warning: return "Warning";
        case iso2_isolationLevelType_Fault: return "Fault";
        case iso2_isolationLevelType_No_IMD: return "No_IMD";
        default: return "Invalid";
    }
}

static const char* iso2_dc_evse_status_code_to_string(iso2_DC_EVSEStatusCodeType code) {
    switch (code) {
        case iso2_DC_EVSEStatusCodeType_EVSE_NotReady: return "EVSE_NotReady";
        case iso2_DC_EVSEStatusCodeType_EVSE_Ready: return "EVSE_Ready";
        case iso2_DC_EVSEStatusCodeType_EVSE_Shutdown: return "EVSE_Shutdown";
        case iso2_DC_EVSEStatusCodeType_EVSE_UtilityInterruptEvent: return "EVSE_UtilityInterruptEvent";
        case iso2_DC_EVSEStatusCodeType_EVSE_IsolationMonitoringActive: return "EVSE_IsolationMonitoringActive";
        case iso2_DC_EVSEStatusCodeType_EVSE_EmergencyShutdown: return "EVSE_EmergencyShutdown";
        case iso2_DC_EVSEStatusCodeType_EVSE_Malfunction: return "EVSE_Malfunction";
        case iso2_DC_EVSEStatusCodeType_Reserved_8: return "Reserved_8";
        case iso2_DC_EVSEStatusCodeType_Reserved_9: return "Reserved_9";
        case iso2_DC_EVSEStatusCodeType_Reserved_A: return "Reserved_A";
        case iso2_DC_EVSEStatusCodeType_Reserved_B: return "Reserved_B";
        case iso2_DC_EVSEStatusCodeType_Reserved_C: return "Reserved_C";
        default: return "EVSE_NotReady";
    }
}

static const char* iso2_evse_notification_to_string(iso2_EVSENotificationType notification) {
    switch (notification) {
        case iso2_EVSENotificationType_None: return "None";
        case iso2_EVSENotificationType_StopCharging: return "StopCharging";
        case iso2_EVSENotificationType_ReNegotiation: return "ReNegotiation";
        default: return "None";
    }
}

static cJSON* iso2_dc_evse_status_to_json(const struct iso2_DC_EVSEStatusType* status) {
    cJSON* json = cJSON_CreateObject();
    if (status->EVSEIsolationStatus_isUsed) {
        cJSON_AddStringToObject(json, "EVSEIsolationStatus", iso2_isolation_level_to_string(status->EVSEIsolationStatus));
    }
    cJSON_AddStringToObject(json, "EVSEStatusCode", iso2_dc_evse_status_code_to_string(status->EVSEStatusCode));
    cJSON_AddNumberToObject(json, "NotificationMaxDelay", status->NotificationMaxDelay);
    cJSON_AddStringToObject(json, "EVSENotification", iso2_evse_notification_to_string(status->EVSENotification));
    return json;
}

static cJSON* iso2_sa_schedule_list_to_json(const struct iso2_SAScheduleListType* sa_list) {
    cJSON* json = cJSON_CreateObject();
    cJSON* schedule_tuple_array = cJSON_CreateArray();

    for (uint16_t i = 0; i < sa_list->SAScheduleTuple.arrayLen; i++) {
        const struct iso2_SAScheduleTupleType* tuple = &sa_list->SAScheduleTuple.array[i];
        cJSON* tuple_json = cJSON_CreateObject();

        cJSON_AddNumberToObject(tuple_json, "SAScheduleTupleID", tuple->SAScheduleTupleID);

        /* PMaxSchedule */
        cJSON* pmax_schedule = cJSON_CreateObject();
        cJSON* pmax_entry_array = cJSON_CreateArray();

        for (uint16_t j = 0; j < tuple->PMaxSchedule.PMaxScheduleEntry.arrayLen; j++) {
            const struct iso2_PMaxScheduleEntryType* entry = &tuple->PMaxSchedule.PMaxScheduleEntry.array[j];
            cJSON* entry_json = cJSON_CreateObject();

            if (entry->RelativeTimeInterval_isUsed) {
                cJSON* rel_time = cJSON_CreateObject();
                cJSON_AddNumberToObject(rel_time, "start", entry->RelativeTimeInterval.start);
                if (entry->RelativeTimeInterval.duration_isUsed) {
                    cJSON_AddNumberToObject(rel_time, "duration", entry->RelativeTimeInterval.duration);
                }
                cJSON_AddItemToObject(entry_json, "RelativeTimeInterval", rel_time);
            }

            cJSON_AddItemToObject(entry_json, "PMax", iso2_physical_value_to_json(&entry->PMax));
            cJSON_AddItemToArray(pmax_entry_array, entry_json);
        }

        cJSON_AddItemToObject(pmax_schedule, "PMaxScheduleEntry", pmax_entry_array);
        cJSON_AddItemToObject(tuple_json, "PMaxSchedule", pmax_schedule);

        cJSON_AddItemToArray(schedule_tuple_array, tuple_json);
    }

    cJSON_AddItemToObject(json, "SAScheduleTuple", schedule_tuple_array);
    return json;
}

static const char* iso2_response_code_to_string(iso2_responseCodeType code) {
    switch (code) {
        case iso2_responseCodeType_OK: return "OK";
        case iso2_responseCodeType_OK_NewSessionEstablished: return "OK_NewSessionEstablished";
        case iso2_responseCodeType_OK_OldSessionJoined: return "OK_OldSessionJoined";
        case iso2_responseCodeType_OK_CertificateExpiresSoon: return "OK_CertificateExpiresSoon";
        case iso2_responseCodeType_FAILED: return "FAILED";
        case iso2_responseCodeType_FAILED_SequenceError: return "FAILED_SequenceError";
        case iso2_responseCodeType_FAILED_ServiceIDInvalid: return "FAILED_ServiceIDInvalid";
        case iso2_responseCodeType_FAILED_UnknownSession: return "FAILED_UnknownSession";
        case iso2_responseCodeType_FAILED_ServiceSelectionInvalid: return "FAILED_ServiceSelectionInvalid";
        case iso2_responseCodeType_FAILED_PaymentSelectionInvalid: return "FAILED_PaymentSelectionInvalid";
        case iso2_responseCodeType_FAILED_CertificateExpired: return "FAILED_CertificateExpired";
        case iso2_responseCodeType_FAILED_SignatureError: return "FAILED_SignatureError";
        case iso2_responseCodeType_FAILED_NoCertificateAvailable: return "FAILED_NoCertificateAvailable";
        case iso2_responseCodeType_FAILED_CertChainError: return "FAILED_CertChainError";
        case iso2_responseCodeType_FAILED_ChallengeInvalid: return "FAILED_ChallengeInvalid";
        case iso2_responseCodeType_FAILED_ContractCanceled: return "FAILED_ContractCanceled";
        case iso2_responseCodeType_FAILED_WrongChargeParameter: return "FAILED_WrongChargeParameter";
        case iso2_responseCodeType_FAILED_PowerDeliveryNotApplied: return "FAILED_PowerDeliveryNotApplied";
        case iso2_responseCodeType_FAILED_TariffSelectionInvalid: return "FAILED_TariffSelectionInvalid";
        case iso2_responseCodeType_FAILED_ChargingProfileInvalid: return "FAILED_ChargingProfileInvalid";
        case iso2_responseCodeType_FAILED_MeteringSignatureNotValid: return "FAILED_MeteringSignatureNotValid";
        case iso2_responseCodeType_FAILED_NoChargeServiceSelected: return "FAILED_NoChargeServiceSelected";
        case iso2_responseCodeType_FAILED_WrongEnergyTransferMode: return "FAILED_WrongEnergyTransferMode";
        case iso2_responseCodeType_FAILED_ContactorError: return "FAILED_ContactorError";
        default: return "FAILED";
    }
}

static iso2_responseCodeType iso2_string_to_response_code(const char* str) {
    if (strcmp(str, "OK") == 0) return iso2_responseCodeType_OK;
    if (strcmp(str, "OK_NewSessionEstablished") == 0) return iso2_responseCodeType_OK_NewSessionEstablished;
    if (strcmp(str, "OK_OldSessionJoined") == 0) return iso2_responseCodeType_OK_OldSessionJoined;
    if (strcmp(str, "OK_CertificateExpiresSoon") == 0) return iso2_responseCodeType_OK_CertificateExpiresSoon;
    if (strcmp(str, "FAILED") == 0) return iso2_responseCodeType_FAILED;
    if (strcmp(str, "FAILED_SequenceError") == 0) return iso2_responseCodeType_FAILED_SequenceError;
    if (strcmp(str, "FAILED_UnknownSession") == 0) return iso2_responseCodeType_FAILED_UnknownSession;
    return iso2_responseCodeType_FAILED;
}

/* Energy transfer mode enum to/from string */
static const char* iso2_energy_transfer_mode_to_string(iso2_EnergyTransferModeType mode) {
    switch (mode) {
        case iso2_EnergyTransferModeType_AC_single_phase_core: return "AC_single_phase_core";
        case iso2_EnergyTransferModeType_AC_three_phase_core: return "AC_three_phase_core";
        case iso2_EnergyTransferModeType_DC_core: return "DC_core";
        case iso2_EnergyTransferModeType_DC_extended: return "DC_extended";
        case iso2_EnergyTransferModeType_DC_combo_core: return "DC_combo_core";
        case iso2_EnergyTransferModeType_DC_unique: return "DC_unique";
        default: return "DC_extended";
    }
}

static iso2_EnergyTransferModeType iso2_string_to_energy_transfer_mode(const char* str) {
    if (strcmp(str, "AC_single_phase_core") == 0) return iso2_EnergyTransferModeType_AC_single_phase_core;
    if (strcmp(str, "AC_three_phase_core") == 0) return iso2_EnergyTransferModeType_AC_three_phase_core;
    if (strcmp(str, "DC_core") == 0) return iso2_EnergyTransferModeType_DC_core;
    if (strcmp(str, "DC_extended") == 0) return iso2_EnergyTransferModeType_DC_extended;
    if (strcmp(str, "DC_combo_core") == 0) return iso2_EnergyTransferModeType_DC_combo_core;
    if (strcmp(str, "DC_unique") == 0) return iso2_EnergyTransferModeType_DC_unique;
    return iso2_EnergyTransferModeType_DC_extended;
}

/* ============== Message Converters ============== */

/* SessionSetupReq */
static int json_to_iso2_session_setup_req(cJSON* json, struct iso2_SessionSetupReqType* msg) {
    cJSON* evcc_id = cJSON_GetObjectItemCaseSensitive(json, "EVCCID");
    if (evcc_id && cJSON_IsString(evcc_id)) {
        /* EVCCID is hexBinary in XSD - use hex decoding (CWE-126). */
        size_t vs_len = strnlen(evcc_id->valuestring, iso2_evccIDType_BYTES_SIZE * 2);
        size_t len = hex_decode(evcc_id->valuestring, vs_len,
                                msg->EVCCID.bytes, iso2_evccIDType_BYTES_SIZE);
        msg->EVCCID.bytesLen = len;
    }
    return CBV2G_SUCCESS;
}

static cJSON* iso2_session_setup_req_to_json(const struct iso2_SessionSetupReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    /* EVCCID is hexBinary in XSD - use hex encoding */
    char hex[13];  /* 6 bytes * 2 + null terminator */
    hex_encode(msg->EVCCID.bytes, msg->EVCCID.bytesLen, hex, sizeof(hex));
    cJSON_AddStringToObject(json, "EVCCID", hex);
    return json;
}

/* SessionSetupRes */
static int json_to_iso2_session_setup_res(cJSON* json, struct iso2_SessionSetupResType* msg) {
    msg->ResponseCode = iso2_string_to_response_code(json_get_string(json, "ResponseCode"));

    cJSON* evse_id = cJSON_GetObjectItemCaseSensitive(json, "EVSEID");
    if (evse_id && cJSON_IsString(evse_id)) {
        /* Bounded copy with snprintf (CWE-120 / CWE-126). */
        size_t evse_len = strnlen(evse_id->valuestring, iso2_EVSEID_CHARACTER_SIZE);
        int written = snprintf(msg->EVSEID.characters,
                               iso2_EVSEID_CHARACTER_SIZE,
                               "%.*s", (int)evse_len, evse_id->valuestring);
        if (written < 0) written = 0;
        if ((size_t)written >= iso2_EVSEID_CHARACTER_SIZE)
            written = iso2_EVSEID_CHARACTER_SIZE - 1;
        msg->EVSEID.charactersLen = (size_t)written;
    }

    msg->EVSETimeStamp_isUsed = json_has_key(json, "EVSETimeStamp");
    if (msg->EVSETimeStamp_isUsed) {
        msg->EVSETimeStamp = json_get_int(json, "EVSETimeStamp");
    }
    return CBV2G_SUCCESS;
}

static cJSON* iso2_session_setup_res_to_json(const struct iso2_SessionSetupResType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ResponseCode", iso2_response_code_to_string(msg->ResponseCode));

    char evse_id[iso2_EVSEID_CHARACTER_SIZE + 1] = {0};
    memcpy(evse_id, msg->EVSEID.characters, msg->EVSEID.charactersLen);
    cJSON_AddStringToObject(json, "EVSEID", evse_id);

    if (msg->EVSETimeStamp_isUsed) {
        cJSON_AddNumberToObject(json, "EVSETimeStamp", msg->EVSETimeStamp);
    }
    return json;
}

/* ServiceDiscoveryReq */
static int json_to_iso2_service_discovery_req(cJSON* json, struct iso2_ServiceDiscoveryReqType* msg) {
    msg->ServiceScope_isUsed = json_has_key(json, "ServiceScope");
    if (msg->ServiceScope_isUsed) {
        /* Bounded copy with snprintf (CWE-120 / CWE-126). */
        const char* scope = json_get_string(json, "ServiceScope");
        size_t scope_len = strnlen(scope, iso2_ServiceScope_CHARACTER_SIZE);
        int written = snprintf(msg->ServiceScope.characters,
                               iso2_ServiceScope_CHARACTER_SIZE,
                               "%.*s", (int)scope_len, scope);
        if (written < 0) written = 0;
        if ((size_t)written >= iso2_ServiceScope_CHARACTER_SIZE)
            written = iso2_ServiceScope_CHARACTER_SIZE - 1;
        msg->ServiceScope.charactersLen = (size_t)written;
    }
    msg->ServiceCategory_isUsed = json_has_key(json, "ServiceCategory");
    if (msg->ServiceCategory_isUsed) {
        const char* cat = json_get_string(json, "ServiceCategory");
        if (strcmp(cat, "EVCharging") == 0) msg->ServiceCategory = iso2_serviceCategoryType_EVCharging;
        else if (strcmp(cat, "Internet") == 0) msg->ServiceCategory = iso2_serviceCategoryType_Internet;
        else if (strcmp(cat, "ContractCertificate") == 0) msg->ServiceCategory = iso2_serviceCategoryType_ContractCertificate;
        else msg->ServiceCategory = iso2_serviceCategoryType_OtherCustom;
    }
    return CBV2G_SUCCESS;
}

static cJSON* iso2_service_discovery_req_to_json(const struct iso2_ServiceDiscoveryReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    if (msg->ServiceScope_isUsed) {
        char scope[iso2_ServiceScope_CHARACTER_SIZE + 1] = {0};
        memcpy(scope, msg->ServiceScope.characters, msg->ServiceScope.charactersLen);
        cJSON_AddStringToObject(json, "ServiceScope", scope);
    }
    if (msg->ServiceCategory_isUsed) {
        const char* cat;
        switch (msg->ServiceCategory) {
            case iso2_serviceCategoryType_EVCharging: cat = "EVCharging"; break;
            case iso2_serviceCategoryType_Internet: cat = "Internet"; break;
            case iso2_serviceCategoryType_ContractCertificate: cat = "ContractCertificate"; break;
            default: cat = "OtherCustom"; break;
        }
        cJSON_AddStringToObject(json, "ServiceCategory", cat);
    }
    return json;
}

/* ServiceDiscoveryRes */
static int json_to_iso2_service_discovery_res(cJSON* json, struct iso2_ServiceDiscoveryResType* msg) {
    msg->ResponseCode = iso2_string_to_response_code(json_get_string(json, "ResponseCode"));

    /* PaymentOptionList */
    cJSON* payment_list = cJSON_GetObjectItemCaseSensitive(json, "PaymentOptionList");
    if (payment_list) {
        cJSON* po_array = cJSON_GetObjectItemCaseSensitive(payment_list, "PaymentOption");
        if (po_array && cJSON_IsArray(po_array)) {
            int count = cJSON_GetArraySize(po_array);
            if (count > iso2_paymentOptionType_2_ARRAY_SIZE) count = iso2_paymentOptionType_2_ARRAY_SIZE;
            msg->PaymentOptionList.PaymentOption.arrayLen = count;
            for (int i = 0; i < count; i++) {
                cJSON* item = cJSON_GetArrayItem(po_array, i);
                if (cJSON_IsString(item)) {
                    if (strcmp(item->valuestring, "Contract") == 0) {
                        msg->PaymentOptionList.PaymentOption.array[i] = iso2_paymentOptionType_Contract;
                    } else {
                        msg->PaymentOptionList.PaymentOption.array[i] = iso2_paymentOptionType_ExternalPayment;
                    }
                }
            }
        }
    }

    /* ChargeService */
    cJSON* charge_service = cJSON_GetObjectItemCaseSensitive(json, "ChargeService");
    if (charge_service) {
        msg->ChargeService.ServiceID = json_get_int(charge_service, "ServiceID");
        msg->ChargeService.FreeService = json_get_bool(charge_service, "FreeService");

        cJSON* modes = cJSON_GetObjectItemCaseSensitive(charge_service, "SupportedEnergyTransferMode");
        if (modes) {
            cJSON* mode_array = cJSON_GetObjectItemCaseSensitive(modes, "EnergyTransferMode");
            if (mode_array && cJSON_IsArray(mode_array)) {
                int count = cJSON_GetArraySize(mode_array);
                if (count > iso2_EnergyTransferModeType_6_ARRAY_SIZE) count = iso2_EnergyTransferModeType_6_ARRAY_SIZE;
                msg->ChargeService.SupportedEnergyTransferMode.EnergyTransferMode.arrayLen = count;
                for (int i = 0; i < count; i++) {
                    cJSON* item = cJSON_GetArrayItem(mode_array, i);
                    if (cJSON_IsString(item)) {
                        msg->ChargeService.SupportedEnergyTransferMode.EnergyTransferMode.array[i] =
                            iso2_string_to_energy_transfer_mode(item->valuestring);
                    }
                }
            }
        }
    }

    return CBV2G_SUCCESS;
}

static cJSON* iso2_service_discovery_res_to_json(const struct iso2_ServiceDiscoveryResType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ResponseCode", iso2_response_code_to_string(msg->ResponseCode));

    /* PaymentOptionList */
    cJSON* payment_list = cJSON_CreateObject();
    cJSON* po_array = cJSON_CreateArray();
    for (int i = 0; i < msg->PaymentOptionList.PaymentOption.arrayLen; i++) {
        const char* po_str = msg->PaymentOptionList.PaymentOption.array[i] == iso2_paymentOptionType_Contract ?
                             "Contract" : "ExternalPayment";
        cJSON_AddItemToArray(po_array, cJSON_CreateString(po_str));
    }
    cJSON_AddItemToObject(payment_list, "PaymentOption", po_array);
    cJSON_AddItemToObject(json, "PaymentOptionList", payment_list);

    /* ChargeService */
    cJSON* charge_service = cJSON_CreateObject();
    cJSON_AddNumberToObject(charge_service, "ServiceID", msg->ChargeService.ServiceID);

    /* ServiceCategory */
    const char* service_category;
    switch (msg->ChargeService.ServiceCategory) {
        case iso2_serviceCategoryType_EVCharging: service_category = "EVCharging"; break;
        case iso2_serviceCategoryType_Internet: service_category = "Internet"; break;
        case iso2_serviceCategoryType_ContractCertificate: service_category = "ContractCertificate"; break;
        case iso2_serviceCategoryType_OtherCustom: service_category = "OtherCustom"; break;
        default: service_category = "EVCharging"; break;
    }
    cJSON_AddStringToObject(charge_service, "ServiceCategory", service_category);

    cJSON_AddBoolToObject(charge_service, "FreeService", msg->ChargeService.FreeService);

    /* SupportedEnergyTransferMode */
    cJSON* supported_modes = cJSON_CreateObject();
    cJSON* mode_array = cJSON_CreateArray();
    for (int i = 0; i < msg->ChargeService.SupportedEnergyTransferMode.EnergyTransferMode.arrayLen; i++) {
        cJSON_AddItemToArray(mode_array, cJSON_CreateString(
            iso2_energy_transfer_mode_to_string(msg->ChargeService.SupportedEnergyTransferMode.EnergyTransferMode.array[i])));
    }
    cJSON_AddItemToObject(supported_modes, "EnergyTransferMode", mode_array);
    cJSON_AddItemToObject(charge_service, "SupportedEnergyTransferMode", supported_modes);

    cJSON_AddItemToObject(json, "ChargeService", charge_service);

    return json;
}

/* PaymentServiceSelectionReq */
static int json_to_iso2_payment_service_selection_req(cJSON* json, struct iso2_PaymentServiceSelectionReqType* msg) {
    const char* po = json_get_string(json, "SelectedPaymentOption");
    msg->SelectedPaymentOption = strcmp(po, "Contract") == 0 ?
                                 iso2_paymentOptionType_Contract : iso2_paymentOptionType_ExternalPayment;

    cJSON* selected_list = cJSON_GetObjectItemCaseSensitive(json, "SelectedServiceList");
    if (selected_list) {
        cJSON* services = cJSON_GetObjectItemCaseSensitive(selected_list, "SelectedService");
        if (services && cJSON_IsArray(services)) {
            int count = cJSON_GetArraySize(services);
            if (count > iso2_SelectedServiceType_16_ARRAY_SIZE) count = iso2_SelectedServiceType_16_ARRAY_SIZE;
            msg->SelectedServiceList.SelectedService.arrayLen = count;
            for (int i = 0; i < count; i++) {
                cJSON* item = cJSON_GetArrayItem(services, i);
                msg->SelectedServiceList.SelectedService.array[i].ServiceID = json_get_int(item, "ServiceID");
            }
        }
    }
    return CBV2G_SUCCESS;
}

static cJSON* iso2_payment_service_selection_req_to_json(const struct iso2_PaymentServiceSelectionReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "SelectedPaymentOption",
                            msg->SelectedPaymentOption == iso2_paymentOptionType_Contract ? "Contract" : "ExternalPayment");

    cJSON* selected_list = cJSON_CreateObject();
    cJSON* services = cJSON_CreateArray();
    for (int i = 0; i < msg->SelectedServiceList.SelectedService.arrayLen; i++) {
        cJSON* item = cJSON_CreateObject();
        cJSON_AddNumberToObject(item, "ServiceID", msg->SelectedServiceList.SelectedService.array[i].ServiceID);
        cJSON_AddItemToArray(services, item);
    }
    cJSON_AddItemToObject(selected_list, "SelectedService", services);
    cJSON_AddItemToObject(json, "SelectedServiceList", selected_list);
    return json;
}

/* PaymentServiceSelectionRes */
static int json_to_iso2_payment_service_selection_res(cJSON* json, struct iso2_PaymentServiceSelectionResType* msg) {
    msg->ResponseCode = iso2_string_to_response_code(json_get_string(json, "ResponseCode"));
    return CBV2G_SUCCESS;
}

static cJSON* iso2_payment_service_selection_res_to_json(const struct iso2_PaymentServiceSelectionResType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ResponseCode", iso2_response_code_to_string(msg->ResponseCode));
    return json;
}

/* AuthorizationReq */
static int json_to_iso2_authorization_req(cJSON* json, struct iso2_AuthorizationReqType* msg) {
    msg->Id_isUsed = json_has_key(json, "Id");
    if (msg->Id_isUsed) {
        /* Bounded copy with snprintf (CWE-120 / CWE-126). */
        const char* id = json_get_string(json, "Id");
        size_t id_len = strnlen(id, iso2_Id_CHARACTER_SIZE);
        int written = snprintf(msg->Id.characters, iso2_Id_CHARACTER_SIZE,
                               "%.*s", (int)id_len, id);
        if (written < 0) written = 0;
        if ((size_t)written >= iso2_Id_CHARACTER_SIZE)
            written = iso2_Id_CHARACTER_SIZE - 1;
        msg->Id.charactersLen = (size_t)written;
    }
    msg->GenChallenge_isUsed = json_has_key(json, "GenChallenge");
    if (msg->GenChallenge_isUsed) {
        cJSON* gc = cJSON_GetObjectItemCaseSensitive(json, "GenChallenge");
        if (gc && cJSON_IsString(gc)) {
            /* Bound base64 read (CWE-126). */
            size_t gc_max = ((iso2_genChallengeType_BYTES_SIZE + 2) / 3) * 4;
            size_t gc_str_len = strnlen(gc->valuestring, gc_max);
            size_t len = base64_decode(gc->valuestring, gc_str_len,
                                       msg->GenChallenge.bytes, iso2_genChallengeType_BYTES_SIZE);
            msg->GenChallenge.bytesLen = len;
        }
    }
    return CBV2G_SUCCESS;
}

static cJSON* iso2_authorization_req_to_json(const struct iso2_AuthorizationReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    if (msg->Id_isUsed) {
        char id[iso2_Id_CHARACTER_SIZE + 1] = {0};
        memcpy(id, msg->Id.characters, msg->Id.charactersLen);
        cJSON_AddStringToObject(json, "Id", id);
    }
    if (msg->GenChallenge_isUsed) {
        char b64[64];
        base64_encode(msg->GenChallenge.bytes, msg->GenChallenge.bytesLen, b64, sizeof(b64));
        cJSON_AddStringToObject(json, "GenChallenge", b64);
    }
    return json;
}

/* AuthorizationRes */
static int json_to_iso2_authorization_res(cJSON* json, struct iso2_AuthorizationResType* msg) {
    msg->ResponseCode = iso2_string_to_response_code(json_get_string(json, "ResponseCode"));

    const char* processing = json_get_string(json, "EVSEProcessing");
    if (strcmp(processing, "Finished") == 0) {
        msg->EVSEProcessing = iso2_EVSEProcessingType_Finished;
    } else if (strcmp(processing, "Ongoing") == 0) {
        msg->EVSEProcessing = iso2_EVSEProcessingType_Ongoing;
    } else {
        msg->EVSEProcessing = iso2_EVSEProcessingType_Ongoing_WaitingForCustomerInteraction;
    }
    return CBV2G_SUCCESS;
}

static cJSON* iso2_authorization_res_to_json(const struct iso2_AuthorizationResType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ResponseCode", iso2_response_code_to_string(msg->ResponseCode));

    const char* processing;
    switch (msg->EVSEProcessing) {
        case iso2_EVSEProcessingType_Finished: processing = "Finished"; break;
        case iso2_EVSEProcessingType_Ongoing: processing = "Ongoing"; break;
        default: processing = "Ongoing_WaitingForCustomerInteraction"; break;
    }
    cJSON_AddStringToObject(json, "EVSEProcessing", processing);
    return json;
}

/* PaymentDetailsReq */
/* JSON shape produced by the Josev EVCC:
 *   { "eMAID": "<string>",
 *     "ContractSignatureCertChain": {
 *         "Id": "<optional NCName>",
 *         "Certificate": "<base64 leaf>",
 *         "SubCertificates": { "Certificate": ["<base64 sub-CA2>", "<base64 sub-CA1>"] }
 *     } }
 * Both eMAID + Certificate are required; SubCertificates is optional per ISO 15118-2.
 */
static int json_to_iso2_payment_details_req(cJSON* json, struct iso2_PaymentDetailsReqType* msg) {
    /* Bounded copy with snprintf (CWE-120 / CWE-126). */
    const char* emaid = json_get_string(json, "eMAID");
    if (emaid != NULL) {
        size_t emaid_in = strnlen(emaid, iso2_eMAID_CHARACTER_SIZE);
        int emaid_w = snprintf(msg->eMAID.characters,
                               iso2_eMAID_CHARACTER_SIZE,
                               "%.*s", (int)emaid_in, emaid);
        if (emaid_w < 0) emaid_w = 0;
        if ((size_t)emaid_w >= iso2_eMAID_CHARACTER_SIZE)
            emaid_w = iso2_eMAID_CHARACTER_SIZE - 1;
        msg->eMAID.charactersLen = (uint16_t)emaid_w;
    }

    cJSON* chain = cJSON_GetObjectItemCaseSensitive(json, "ContractSignatureCertChain");
    if (chain != NULL) {
        struct iso2_CertificateChainType* cc = &msg->ContractSignatureCertChain;

        cc->Id_isUsed = json_has_key(chain, "Id");
        if (cc->Id_isUsed) {
            const char* id = json_get_string(chain, "Id");
            if (id != NULL) {
                /* Bounded copy with snprintf (CWE-120 / CWE-126). */
                size_t id_in = strnlen(id, iso2_Id_CHARACTER_SIZE);
                int id_w = snprintf(cc->Id.characters,
                                    iso2_Id_CHARACTER_SIZE,
                                    "%.*s", (int)id_in, id);
                if (id_w < 0) id_w = 0;
                if ((size_t)id_w >= iso2_Id_CHARACTER_SIZE)
                    id_w = iso2_Id_CHARACTER_SIZE - 1;
                cc->Id.charactersLen = (uint16_t)id_w;
            }
        }

        cJSON* leaf = cJSON_GetObjectItemCaseSensitive(chain, "Certificate");
        if (leaf != NULL && cJSON_IsString(leaf)) {
            /* Bound base64 read by 4*max-bytes (CWE-126). */
            size_t cert_max = ((iso2_certificateType_BYTES_SIZE + 2) / 3) * 4;
            size_t leaf_str_len = strnlen(leaf->valuestring, cert_max);
            size_t len = base64_decode(leaf->valuestring, leaf_str_len,
                                       cc->Certificate.bytes, iso2_certificateType_BYTES_SIZE);
            cc->Certificate.bytesLen = (uint16_t)len;
        }

        cc->SubCertificates_isUsed = 0;
        cJSON* sub = cJSON_GetObjectItemCaseSensitive(chain, "SubCertificates");
        if (sub != NULL) {
            cJSON* cert_array = cJSON_GetObjectItemCaseSensitive(sub, "Certificate");
            if (cert_array != NULL && cJSON_IsArray(cert_array)) {
                int count = cJSON_GetArraySize(cert_array);
                if (count > iso2_certificateType_4_ARRAY_SIZE) {
                    count = iso2_certificateType_4_ARRAY_SIZE;
                }
                cc->SubCertificates.Certificate.arrayLen = (uint16_t)count;
                for (int i = 0; i < count; i++) {
                    cJSON* item = cJSON_GetArrayItem(cert_array, i);
                    if (item != NULL && cJSON_IsString(item)) {
                        /* Bound base64 read by 4*max-bytes (CWE-126). */
                        size_t sub_max = ((iso2_certificateType_BYTES_SIZE + 2) / 3) * 4;
                        size_t item_str_len = strnlen(item->valuestring, sub_max);
                        size_t len = base64_decode(item->valuestring, item_str_len,
                                                   cc->SubCertificates.Certificate.array[i].bytes,
                                                   iso2_certificateType_BYTES_SIZE);
                        cc->SubCertificates.Certificate.array[i].bytesLen = (uint16_t)len;
                    }
                }
                if (count > 0) {
                    cc->SubCertificates_isUsed = 1;
                }
            }
        }
    }

    return CBV2G_SUCCESS;
}

static cJSON* iso2_payment_details_req_to_json(const struct iso2_PaymentDetailsReqType* msg) {
    cJSON* json = cJSON_CreateObject();

    char emaid[iso2_eMAID_CHARACTER_SIZE + 1] = {0};
    memcpy(emaid, msg->eMAID.characters, msg->eMAID.charactersLen);
    cJSON_AddStringToObject(json, "eMAID", emaid);

    const struct iso2_CertificateChainType* cc = &msg->ContractSignatureCertChain;
    cJSON* chain = cJSON_CreateObject();

    if (cc->Id_isUsed) {
        char id[iso2_Id_CHARACTER_SIZE + 1] = {0};
        memcpy(id, cc->Id.characters, cc->Id.charactersLen);
        cJSON_AddStringToObject(chain, "Id", id);
    }

    /* base64-encoded DER cert: worst case ~1.4KB output for 800-byte input */
    char b64_leaf[iso2_certificateType_BYTES_SIZE * 2];
    base64_encode(cc->Certificate.bytes, cc->Certificate.bytesLen, b64_leaf, sizeof(b64_leaf));
    cJSON_AddStringToObject(chain, "Certificate", b64_leaf);

    if (cc->SubCertificates_isUsed && cc->SubCertificates.Certificate.arrayLen > 0) {
        cJSON* sub = cJSON_CreateObject();
        cJSON* cert_array = cJSON_CreateArray();
        for (uint16_t i = 0; i < cc->SubCertificates.Certificate.arrayLen; i++) {
            char b64_sub[iso2_certificateType_BYTES_SIZE * 2];
            base64_encode(cc->SubCertificates.Certificate.array[i].bytes,
                          cc->SubCertificates.Certificate.array[i].bytesLen,
                          b64_sub, sizeof(b64_sub));
            cJSON_AddItemToArray(cert_array, cJSON_CreateString(b64_sub));
        }
        cJSON_AddItemToObject(sub, "Certificate", cert_array);
        cJSON_AddItemToObject(chain, "SubCertificates", sub);
    }

    cJSON_AddItemToObject(json, "ContractSignatureCertChain", chain);
    return json;
}

/* PaymentDetailsRes */
static int json_to_iso2_payment_details_res(cJSON* json, struct iso2_PaymentDetailsResType* msg) {
    msg->ResponseCode = iso2_string_to_response_code(json_get_string(json, "ResponseCode"));

    cJSON* gc = cJSON_GetObjectItemCaseSensitive(json, "GenChallenge");
    if (gc != NULL && cJSON_IsString(gc)) {
        /* Bound base64 read (CWE-126). */
        size_t gc_max = ((iso2_genChallengeType_BYTES_SIZE + 2) / 3) * 4;
        size_t gc_str_len = strnlen(gc->valuestring, gc_max);
        size_t len = base64_decode(gc->valuestring, gc_str_len,
                                   msg->GenChallenge.bytes, iso2_genChallengeType_BYTES_SIZE);
        msg->GenChallenge.bytesLen = (uint16_t)len;
    }

    msg->EVSETimeStamp = (int64_t)json_get_int(json, "EVSETimeStamp");
    return CBV2G_SUCCESS;
}

static cJSON* iso2_payment_details_res_to_json(const struct iso2_PaymentDetailsResType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ResponseCode", iso2_response_code_to_string(msg->ResponseCode));

    char b64[64];
    base64_encode(msg->GenChallenge.bytes, msg->GenChallenge.bytesLen, b64, sizeof(b64));
    cJSON_AddStringToObject(json, "GenChallenge", b64);

    cJSON_AddNumberToObject(json, "EVSETimeStamp", (double)msg->EVSETimeStamp);
    return json;
}

/* xmldsig SignedInfo (used as fragment EXI for PnC AuthorizationReq signature). */
static int json_to_iso2_signed_info(cJSON* json, struct iso2_SignedInfoType* msg) {
    msg->Id_isUsed = json_has_key(json, "Id");
    if (msg->Id_isUsed) {
        const char* id = json_get_string(json, "Id");
        if (id != NULL) {
            size_t id_len = strlen(id);
            if (id_len >= iso2_Id_CHARACTER_SIZE) id_len = iso2_Id_CHARACTER_SIZE - 1;
            memcpy(msg->Id.characters, id, id_len);
            msg->Id.charactersLen = (uint16_t)id_len;
        }
    }

    cJSON* cm = cJSON_GetObjectItemCaseSensitive(json, "CanonicalizationMethod");
    if (cm != NULL) {
        const char* alg = json_get_string(cm, "Algorithm");
        if (alg != NULL) {
            size_t alg_len = strlen(alg);
            if (alg_len >= iso2_Algorithm_CHARACTER_SIZE) alg_len = iso2_Algorithm_CHARACTER_SIZE - 1;
            memcpy(msg->CanonicalizationMethod.Algorithm.characters, alg, alg_len);
            msg->CanonicalizationMethod.Algorithm.charactersLen = (uint16_t)alg_len;
        }
        msg->CanonicalizationMethod.ANY_isUsed = 0;
    }

    cJSON* sm = cJSON_GetObjectItemCaseSensitive(json, "SignatureMethod");
    if (sm != NULL) {
        const char* alg = json_get_string(sm, "Algorithm");
        if (alg != NULL) {
            size_t alg_len = strlen(alg);
            if (alg_len >= iso2_Algorithm_CHARACTER_SIZE) alg_len = iso2_Algorithm_CHARACTER_SIZE - 1;
            memcpy(msg->SignatureMethod.Algorithm.characters, alg, alg_len);
            msg->SignatureMethod.Algorithm.charactersLen = (uint16_t)alg_len;
        }
        msg->SignatureMethod.HMACOutputLength_isUsed = 0;
        msg->SignatureMethod.ANY_isUsed = 0;
    }

    cJSON* ref_array = cJSON_GetObjectItemCaseSensitive(json, "Reference");
    if (ref_array != NULL && cJSON_IsArray(ref_array)) {
        int count = cJSON_GetArraySize(ref_array);
        if (count > iso2_ReferenceType_4_ARRAY_SIZE) count = iso2_ReferenceType_4_ARRAY_SIZE;
        msg->Reference.arrayLen = (uint16_t)count;
        for (int i = 0; i < count; i++) {
            cJSON* ref = cJSON_GetArrayItem(ref_array, i);
            if (ref == NULL) continue;
            struct iso2_ReferenceType* r = &msg->Reference.array[i];

            r->Id_isUsed = json_has_key(ref, "Id");
            if (r->Id_isUsed) {
                const char* id = json_get_string(ref, "Id");
                if (id != NULL) {
                    size_t l = strlen(id);
                    if (l >= iso2_Id_CHARACTER_SIZE) l = iso2_Id_CHARACTER_SIZE - 1;
                    memcpy(r->Id.characters, id, l);
                    r->Id.charactersLen = (uint16_t)l;
                }
            }

            r->URI_isUsed = json_has_key(ref, "URI");
            if (r->URI_isUsed) {
                const char* uri = json_get_string(ref, "URI");
                if (uri != NULL) {
                    size_t l = strlen(uri);
                    if (l >= iso2_URI_CHARACTER_SIZE) l = iso2_URI_CHARACTER_SIZE - 1;
                    memcpy(r->URI.characters, uri, l);
                    r->URI.charactersLen = (uint16_t)l;
                }
            }

            r->Type_isUsed = json_has_key(ref, "Type");
            if (r->Type_isUsed) {
                const char* type = json_get_string(ref, "Type");
                if (type != NULL) {
                    size_t l = strlen(type);
                    if (l >= iso2_Type_CHARACTER_SIZE) l = iso2_Type_CHARACTER_SIZE - 1;
                    memcpy(r->Type.characters, type, l);
                    r->Type.charactersLen = (uint16_t)l;
                }
            }

            r->Transforms_isUsed = 0;
            cJSON* transforms = cJSON_GetObjectItemCaseSensitive(ref, "Transforms");
            if (transforms != NULL) {
                cJSON* t_array = cJSON_GetObjectItemCaseSensitive(transforms, "Transform");
                cJSON* t = NULL;
                if (t_array != NULL && cJSON_IsArray(t_array) && cJSON_GetArraySize(t_array) > 0) {
                    t = cJSON_GetArrayItem(t_array, 0);
                } else if (t_array != NULL && cJSON_IsObject(t_array)) {
                    t = t_array;
                }
                if (t != NULL) {
                    const char* alg = json_get_string(t, "Algorithm");
                    if (alg != NULL) {
                        size_t l = strlen(alg);
                        if (l >= iso2_Algorithm_CHARACTER_SIZE) l = iso2_Algorithm_CHARACTER_SIZE - 1;
                        memcpy(r->Transforms.Transform.Algorithm.characters, alg, l);
                        r->Transforms.Transform.Algorithm.charactersLen = (uint16_t)l;
                    }
                    r->Transforms.Transform.ANY_isUsed = 0;
                    r->Transforms.Transform.XPath_isUsed = 0;
                    r->Transforms_isUsed = 1;
                }
            }

            cJSON* dm = cJSON_GetObjectItemCaseSensitive(ref, "DigestMethod");
            if (dm != NULL) {
                const char* alg = json_get_string(dm, "Algorithm");
                if (alg != NULL) {
                    size_t l = strlen(alg);
                    if (l >= iso2_Algorithm_CHARACTER_SIZE) l = iso2_Algorithm_CHARACTER_SIZE - 1;
                    memcpy(r->DigestMethod.Algorithm.characters, alg, l);
                    r->DigestMethod.Algorithm.charactersLen = (uint16_t)l;
                }
                r->DigestMethod.ANY_isUsed = 0;
            }

            cJSON* dv = cJSON_GetObjectItemCaseSensitive(ref, "DigestValue");
            if (dv != NULL && cJSON_IsString(dv)) {
                /* Bound base64 read (CWE-126). */
                size_t dv_max = ((iso2_DigestValueType_BYTES_SIZE + 2) / 3) * 4;
                size_t dv_str_len = strnlen(dv->valuestring, dv_max);
                size_t len = base64_decode(dv->valuestring, dv_str_len,
                                           r->DigestValue.bytes, iso2_DigestValueType_BYTES_SIZE);
                r->DigestValue.bytesLen = (uint16_t)len;
            }
        }
    }

    return CBV2G_SUCCESS;
}

static cJSON* iso2_signed_info_to_json(const struct iso2_SignedInfoType* msg) {
    /* Decode is not exercised in current PnC SIL flow; emit a minimal stub. */
    cJSON* json = cJSON_CreateObject();
    if (msg->Id_isUsed) {
        char id[iso2_Id_CHARACTER_SIZE + 1] = {0};
        memcpy(id, msg->Id.characters, msg->Id.charactersLen);
        cJSON_AddStringToObject(json, "Id", id);
    }
    return json;
}

/* ChargeParameterDiscoveryReq */
static int json_to_iso2_charge_parameter_discovery_req(cJSON* json, struct iso2_ChargeParameterDiscoveryReqType* msg) {
    msg->MaxEntriesSAScheduleTuple_isUsed = json_has_key(json, "MaxEntriesSAScheduleTuple");
    if (msg->MaxEntriesSAScheduleTuple_isUsed) {
        msg->MaxEntriesSAScheduleTuple = json_get_int(json, "MaxEntriesSAScheduleTuple");
    }

    const char* mode = json_get_string(json, "RequestedEnergyTransferMode");
    if (strcmp(mode, "DC_extended") == 0) {
        msg->RequestedEnergyTransferMode = iso2_EnergyTransferModeType_DC_extended;
    } else if (strcmp(mode, "DC_core") == 0) {
        msg->RequestedEnergyTransferMode = iso2_EnergyTransferModeType_DC_core;
    } else if (strcmp(mode, "AC_single_phase_core") == 0) {
        msg->RequestedEnergyTransferMode = iso2_EnergyTransferModeType_AC_single_phase_core;
    } else if (strcmp(mode, "AC_three_phase_core") == 0) {
        msg->RequestedEnergyTransferMode = iso2_EnergyTransferModeType_AC_three_phase_core;
    } else {
        msg->RequestedEnergyTransferMode = iso2_EnergyTransferModeType_DC_extended;
    }

    cJSON* dc_params = cJSON_GetObjectItemCaseSensitive(json, "DC_EVChargeParameter");
    if (dc_params) {
        msg->DC_EVChargeParameter_isUsed = 1;
        cJSON* status = cJSON_GetObjectItemCaseSensitive(dc_params, "DC_EVStatus");
        if (status) json_to_iso2_dc_ev_status(status, &msg->DC_EVChargeParameter.DC_EVStatus);

        cJSON* max_current = cJSON_GetObjectItemCaseSensitive(dc_params, "EVMaximumCurrentLimit");
        if (max_current) json_to_iso2_physical_value(max_current, &msg->DC_EVChargeParameter.EVMaximumCurrentLimit);

        cJSON* max_voltage = cJSON_GetObjectItemCaseSensitive(dc_params, "EVMaximumVoltageLimit");
        if (max_voltage) json_to_iso2_physical_value(max_voltage, &msg->DC_EVChargeParameter.EVMaximumVoltageLimit);

        msg->DC_EVChargeParameter.EVMaximumPowerLimit_isUsed = json_has_key(dc_params, "EVMaximumPowerLimit");
        if (msg->DC_EVChargeParameter.EVMaximumPowerLimit_isUsed) {
            cJSON* max_power = cJSON_GetObjectItemCaseSensitive(dc_params, "EVMaximumPowerLimit");
            json_to_iso2_physical_value(max_power, &msg->DC_EVChargeParameter.EVMaximumPowerLimit);
        }

        msg->DC_EVChargeParameter.EVEnergyCapacity_isUsed = json_has_key(dc_params, "EVEnergyCapacity");
        if (msg->DC_EVChargeParameter.EVEnergyCapacity_isUsed) {
            cJSON* energy_cap = cJSON_GetObjectItemCaseSensitive(dc_params, "EVEnergyCapacity");
            json_to_iso2_physical_value(energy_cap, &msg->DC_EVChargeParameter.EVEnergyCapacity);
        }

        msg->DC_EVChargeParameter.EVEnergyRequest_isUsed = json_has_key(dc_params, "EVEnergyRequest");
        if (msg->DC_EVChargeParameter.EVEnergyRequest_isUsed) {
            cJSON* energy_req = cJSON_GetObjectItemCaseSensitive(dc_params, "EVEnergyRequest");
            json_to_iso2_physical_value(energy_req, &msg->DC_EVChargeParameter.EVEnergyRequest);
        }

        msg->DC_EVChargeParameter.FullSOC_isUsed = json_has_key(dc_params, "FullSOC");
        if (msg->DC_EVChargeParameter.FullSOC_isUsed) {
            msg->DC_EVChargeParameter.FullSOC = json_get_int(dc_params, "FullSOC");
        }

        msg->DC_EVChargeParameter.BulkSOC_isUsed = json_has_key(dc_params, "BulkSOC");
        if (msg->DC_EVChargeParameter.BulkSOC_isUsed) {
            msg->DC_EVChargeParameter.BulkSOC = json_get_int(dc_params, "BulkSOC");
        }
    }
    return CBV2G_SUCCESS;
}

static cJSON* iso2_charge_parameter_discovery_req_to_json(const struct iso2_ChargeParameterDiscoveryReqType* msg) {
    cJSON* json = cJSON_CreateObject();

    if (msg->MaxEntriesSAScheduleTuple_isUsed) {
        cJSON_AddNumberToObject(json, "MaxEntriesSAScheduleTuple", msg->MaxEntriesSAScheduleTuple);
    }

    const char* mode;
    switch (msg->RequestedEnergyTransferMode) {
        case iso2_EnergyTransferModeType_DC_extended: mode = "DC_extended"; break;
        case iso2_EnergyTransferModeType_DC_core: mode = "DC_core"; break;
        case iso2_EnergyTransferModeType_AC_single_phase_core: mode = "AC_single_phase_core"; break;
        case iso2_EnergyTransferModeType_AC_three_phase_core: mode = "AC_three_phase_core"; break;
        default: mode = "DC_extended"; break;
    }
    cJSON_AddStringToObject(json, "RequestedEnergyTransferMode", mode);

    if (msg->DC_EVChargeParameter_isUsed) {
        cJSON* dc_params = cJSON_CreateObject();
        cJSON_AddItemToObject(dc_params, "DC_EVStatus", iso2_dc_ev_status_to_json(&msg->DC_EVChargeParameter.DC_EVStatus));
        cJSON_AddItemToObject(dc_params, "EVMaximumCurrentLimit", iso2_physical_value_to_json(&msg->DC_EVChargeParameter.EVMaximumCurrentLimit));
        cJSON_AddItemToObject(dc_params, "EVMaximumVoltageLimit", iso2_physical_value_to_json(&msg->DC_EVChargeParameter.EVMaximumVoltageLimit));

        if (msg->DC_EVChargeParameter.EVMaximumPowerLimit_isUsed) {
            cJSON_AddItemToObject(dc_params, "EVMaximumPowerLimit", iso2_physical_value_to_json(&msg->DC_EVChargeParameter.EVMaximumPowerLimit));
        }
        if (msg->DC_EVChargeParameter.EVEnergyCapacity_isUsed) {
            cJSON_AddItemToObject(dc_params, "EVEnergyCapacity", iso2_physical_value_to_json(&msg->DC_EVChargeParameter.EVEnergyCapacity));
        }
        if (msg->DC_EVChargeParameter.EVEnergyRequest_isUsed) {
            cJSON_AddItemToObject(dc_params, "EVEnergyRequest", iso2_physical_value_to_json(&msg->DC_EVChargeParameter.EVEnergyRequest));
        }
        if (msg->DC_EVChargeParameter.FullSOC_isUsed) {
            cJSON_AddNumberToObject(dc_params, "FullSOC", msg->DC_EVChargeParameter.FullSOC);
        }
        if (msg->DC_EVChargeParameter.BulkSOC_isUsed) {
            cJSON_AddNumberToObject(dc_params, "BulkSOC", msg->DC_EVChargeParameter.BulkSOC);
        }
        cJSON_AddItemToObject(json, "DC_EVChargeParameter", dc_params);
    }
    return json;
}

/* ChargeParameterDiscoveryRes */
static int json_to_iso2_charge_parameter_discovery_res(cJSON* json, struct iso2_ChargeParameterDiscoveryResType* msg) {
    msg->ResponseCode = iso2_string_to_response_code(json_get_string(json, "ResponseCode"));

    const char* processing = json_get_string(json, "EVSEProcessing");
    if (strcmp(processing, "Finished") == 0) {
        msg->EVSEProcessing = iso2_EVSEProcessingType_Finished;
    } else {
        msg->EVSEProcessing = iso2_EVSEProcessingType_Ongoing;
    }

    cJSON* dc_params = cJSON_GetObjectItemCaseSensitive(json, "DC_EVSEChargeParameter");
    if (dc_params) {
        msg->DC_EVSEChargeParameter_isUsed = 1;
        cJSON* status = cJSON_GetObjectItemCaseSensitive(dc_params, "DC_EVSEStatus");
        if (status) json_to_iso2_dc_evse_status(status, &msg->DC_EVSEChargeParameter.DC_EVSEStatus);

        cJSON* max_current = cJSON_GetObjectItemCaseSensitive(dc_params, "EVSEMaximumCurrentLimit");
        if (max_current) json_to_iso2_physical_value(max_current, &msg->DC_EVSEChargeParameter.EVSEMaximumCurrentLimit);

        cJSON* max_voltage = cJSON_GetObjectItemCaseSensitive(dc_params, "EVSEMaximumVoltageLimit");
        if (max_voltage) json_to_iso2_physical_value(max_voltage, &msg->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit);

        cJSON* max_power = cJSON_GetObjectItemCaseSensitive(dc_params, "EVSEMaximumPowerLimit");
        if (max_power) json_to_iso2_physical_value(max_power, &msg->DC_EVSEChargeParameter.EVSEMaximumPowerLimit);

        cJSON* min_current = cJSON_GetObjectItemCaseSensitive(dc_params, "EVSEMinimumCurrentLimit");
        if (min_current) json_to_iso2_physical_value(min_current, &msg->DC_EVSEChargeParameter.EVSEMinimumCurrentLimit);

        cJSON* min_voltage = cJSON_GetObjectItemCaseSensitive(dc_params, "EVSEMinimumVoltageLimit");
        if (min_voltage) json_to_iso2_physical_value(min_voltage, &msg->DC_EVSEChargeParameter.EVSEMinimumVoltageLimit);

        cJSON* peak_current = cJSON_GetObjectItemCaseSensitive(dc_params, "EVSEPeakCurrentRipple");
        if (peak_current) json_to_iso2_physical_value(peak_current, &msg->DC_EVSEChargeParameter.EVSEPeakCurrentRipple);

        msg->DC_EVSEChargeParameter.EVSECurrentRegulationTolerance_isUsed = json_has_key(dc_params, "EVSECurrentRegulationTolerance");
        if (msg->DC_EVSEChargeParameter.EVSECurrentRegulationTolerance_isUsed) {
            cJSON* tolerance = cJSON_GetObjectItemCaseSensitive(dc_params, "EVSECurrentRegulationTolerance");
            json_to_iso2_physical_value(tolerance, &msg->DC_EVSEChargeParameter.EVSECurrentRegulationTolerance);
        }

        msg->DC_EVSEChargeParameter.EVSEEnergyToBeDelivered_isUsed = json_has_key(dc_params, "EVSEEnergyToBeDelivered");
        if (msg->DC_EVSEChargeParameter.EVSEEnergyToBeDelivered_isUsed) {
            cJSON* energy = cJSON_GetObjectItemCaseSensitive(dc_params, "EVSEEnergyToBeDelivered");
            json_to_iso2_physical_value(energy, &msg->DC_EVSEChargeParameter.EVSEEnergyToBeDelivered);
        }
    }
    return CBV2G_SUCCESS;
}

static cJSON* iso2_charge_parameter_discovery_res_to_json(const struct iso2_ChargeParameterDiscoveryResType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ResponseCode", iso2_response_code_to_string(msg->ResponseCode));

    const char* processing = msg->EVSEProcessing == iso2_EVSEProcessingType_Finished ? "Finished" : "Ongoing";
    cJSON_AddStringToObject(json, "EVSEProcessing", processing);

    /* SAScheduleList - required for EV to select schedule */
    if (msg->SAScheduleList_isUsed) {
        cJSON_AddItemToObject(json, "SAScheduleList", iso2_sa_schedule_list_to_json(&msg->SAScheduleList));
    }

    if (msg->DC_EVSEChargeParameter_isUsed) {
        cJSON* dc_params = cJSON_CreateObject();
        cJSON_AddItemToObject(dc_params, "DC_EVSEStatus", iso2_dc_evse_status_to_json(&msg->DC_EVSEChargeParameter.DC_EVSEStatus));
        cJSON_AddItemToObject(dc_params, "EVSEMaximumCurrentLimit", iso2_physical_value_to_json(&msg->DC_EVSEChargeParameter.EVSEMaximumCurrentLimit));
        cJSON_AddItemToObject(dc_params, "EVSEMaximumVoltageLimit", iso2_physical_value_to_json(&msg->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit));
        cJSON_AddItemToObject(dc_params, "EVSEMaximumPowerLimit", iso2_physical_value_to_json(&msg->DC_EVSEChargeParameter.EVSEMaximumPowerLimit));
        cJSON_AddItemToObject(dc_params, "EVSEMinimumCurrentLimit", iso2_physical_value_to_json(&msg->DC_EVSEChargeParameter.EVSEMinimumCurrentLimit));
        cJSON_AddItemToObject(dc_params, "EVSEMinimumVoltageLimit", iso2_physical_value_to_json(&msg->DC_EVSEChargeParameter.EVSEMinimumVoltageLimit));
        cJSON_AddItemToObject(dc_params, "EVSEPeakCurrentRipple", iso2_physical_value_to_json(&msg->DC_EVSEChargeParameter.EVSEPeakCurrentRipple));

        if (msg->DC_EVSEChargeParameter.EVSECurrentRegulationTolerance_isUsed) {
            cJSON_AddItemToObject(dc_params, "EVSECurrentRegulationTolerance", iso2_physical_value_to_json(&msg->DC_EVSEChargeParameter.EVSECurrentRegulationTolerance));
        }
        if (msg->DC_EVSEChargeParameter.EVSEEnergyToBeDelivered_isUsed) {
            cJSON_AddItemToObject(dc_params, "EVSEEnergyToBeDelivered", iso2_physical_value_to_json(&msg->DC_EVSEChargeParameter.EVSEEnergyToBeDelivered));
        }
        cJSON_AddItemToObject(json, "DC_EVSEChargeParameter", dc_params);
    }
    return json;
}

/* CableCheckReq */
static int json_to_iso2_cable_check_req(cJSON* json, struct iso2_CableCheckReqType* msg) {
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVStatus");
    if (status) json_to_iso2_dc_ev_status(status, &msg->DC_EVStatus);
    return CBV2G_SUCCESS;
}

static cJSON* iso2_cable_check_req_to_json(const struct iso2_CableCheckReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "DC_EVStatus", iso2_dc_ev_status_to_json(&msg->DC_EVStatus));
    return json;
}

/* CableCheckRes */
static int json_to_iso2_cable_check_res(cJSON* json, struct iso2_CableCheckResType* msg) {
    msg->ResponseCode = iso2_string_to_response_code(json_get_string(json, "ResponseCode"));
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVSEStatus");
    if (status) json_to_iso2_dc_evse_status(status, &msg->DC_EVSEStatus);

    const char* processing = json_get_string(json, "EVSEProcessing");
    msg->EVSEProcessing = strcmp(processing, "Finished") == 0 ?
                          iso2_EVSEProcessingType_Finished : iso2_EVSEProcessingType_Ongoing;
    return CBV2G_SUCCESS;
}

static cJSON* iso2_cable_check_res_to_json(const struct iso2_CableCheckResType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ResponseCode", iso2_response_code_to_string(msg->ResponseCode));
    cJSON_AddItemToObject(json, "DC_EVSEStatus", iso2_dc_evse_status_to_json(&msg->DC_EVSEStatus));
    cJSON_AddStringToObject(json, "EVSEProcessing",
                            msg->EVSEProcessing == iso2_EVSEProcessingType_Finished ? "Finished" : "Ongoing");
    return json;
}

/* PreChargeReq */
static int json_to_iso2_pre_charge_req(cJSON* json, struct iso2_PreChargeReqType* msg) {
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVStatus");
    if (status) json_to_iso2_dc_ev_status(status, &msg->DC_EVStatus);

    cJSON* target_voltage = cJSON_GetObjectItemCaseSensitive(json, "EVTargetVoltage");
    if (target_voltage) json_to_iso2_physical_value(target_voltage, &msg->EVTargetVoltage);

    cJSON* target_current = cJSON_GetObjectItemCaseSensitive(json, "EVTargetCurrent");
    if (target_current) json_to_iso2_physical_value(target_current, &msg->EVTargetCurrent);
    return CBV2G_SUCCESS;
}

static cJSON* iso2_pre_charge_req_to_json(const struct iso2_PreChargeReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "DC_EVStatus", iso2_dc_ev_status_to_json(&msg->DC_EVStatus));
    cJSON_AddItemToObject(json, "EVTargetVoltage", iso2_physical_value_to_json(&msg->EVTargetVoltage));
    cJSON_AddItemToObject(json, "EVTargetCurrent", iso2_physical_value_to_json(&msg->EVTargetCurrent));
    return json;
}

/* PreChargeRes */
static int json_to_iso2_pre_charge_res(cJSON* json, struct iso2_PreChargeResType* msg) {
    msg->ResponseCode = iso2_string_to_response_code(json_get_string(json, "ResponseCode"));
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVSEStatus");
    if (status) json_to_iso2_dc_evse_status(status, &msg->DC_EVSEStatus);

    cJSON* present_voltage = cJSON_GetObjectItemCaseSensitive(json, "EVSEPresentVoltage");
    if (present_voltage) json_to_iso2_physical_value(present_voltage, &msg->EVSEPresentVoltage);
    return CBV2G_SUCCESS;
}

static cJSON* iso2_pre_charge_res_to_json(const struct iso2_PreChargeResType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ResponseCode", iso2_response_code_to_string(msg->ResponseCode));
    cJSON_AddItemToObject(json, "DC_EVSEStatus", iso2_dc_evse_status_to_json(&msg->DC_EVSEStatus));
    cJSON_AddItemToObject(json, "EVSEPresentVoltage", iso2_physical_value_to_json(&msg->EVSEPresentVoltage));
    return json;
}

/* PowerDeliveryReq */
static int json_to_iso2_power_delivery_req(cJSON* json, struct iso2_PowerDeliveryReqType* msg) {
    const char* progress = json_get_string(json, "ChargeProgress");
    if (strcmp(progress, "Start") == 0) msg->ChargeProgress = iso2_chargeProgressType_Start;
    else if (strcmp(progress, "Stop") == 0) msg->ChargeProgress = iso2_chargeProgressType_Stop;
    else msg->ChargeProgress = iso2_chargeProgressType_Renegotiate;

    msg->SAScheduleTupleID = json_get_int(json, "SAScheduleTupleID");

    cJSON* dc_params = cJSON_GetObjectItemCaseSensitive(json, "DC_EVPowerDeliveryParameter");
    if (dc_params) {
        msg->DC_EVPowerDeliveryParameter_isUsed = 1;
        cJSON* status = cJSON_GetObjectItemCaseSensitive(dc_params, "DC_EVStatus");
        if (status) json_to_iso2_dc_ev_status(status, &msg->DC_EVPowerDeliveryParameter.DC_EVStatus);
        msg->DC_EVPowerDeliveryParameter.BulkChargingComplete_isUsed = json_has_key(dc_params, "BulkChargingComplete");
        if (msg->DC_EVPowerDeliveryParameter.BulkChargingComplete_isUsed) {
            msg->DC_EVPowerDeliveryParameter.BulkChargingComplete = json_get_bool(dc_params, "BulkChargingComplete");
        }
        msg->DC_EVPowerDeliveryParameter.ChargingComplete = json_get_bool(dc_params, "ChargingComplete");
    }
    return CBV2G_SUCCESS;
}

static cJSON* iso2_power_delivery_req_to_json(const struct iso2_PowerDeliveryReqType* msg) {
    cJSON* json = cJSON_CreateObject();

    const char* progress;
    switch (msg->ChargeProgress) {
        case iso2_chargeProgressType_Start: progress = "Start"; break;
        case iso2_chargeProgressType_Stop: progress = "Stop"; break;
        default: progress = "Renegotiate"; break;
    }
    cJSON_AddStringToObject(json, "ChargeProgress", progress);
    cJSON_AddNumberToObject(json, "SAScheduleTupleID", msg->SAScheduleTupleID);

    if (msg->DC_EVPowerDeliveryParameter_isUsed) {
        cJSON* dc_params = cJSON_CreateObject();
        cJSON_AddItemToObject(dc_params, "DC_EVStatus", iso2_dc_ev_status_to_json(&msg->DC_EVPowerDeliveryParameter.DC_EVStatus));
        if (msg->DC_EVPowerDeliveryParameter.BulkChargingComplete_isUsed) {
            cJSON_AddBoolToObject(dc_params, "BulkChargingComplete", msg->DC_EVPowerDeliveryParameter.BulkChargingComplete);
        }
        cJSON_AddBoolToObject(dc_params, "ChargingComplete", msg->DC_EVPowerDeliveryParameter.ChargingComplete);
        cJSON_AddItemToObject(json, "DC_EVPowerDeliveryParameter", dc_params);
    }
    return json;
}

/* PowerDeliveryRes */
static int json_to_iso2_power_delivery_res(cJSON* json, struct iso2_PowerDeliveryResType* msg) {
    msg->ResponseCode = iso2_string_to_response_code(json_get_string(json, "ResponseCode"));
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVSEStatus");
    if (status) {
        msg->DC_EVSEStatus_isUsed = 1;
        json_to_iso2_dc_evse_status(status, &msg->DC_EVSEStatus);
    }
    return CBV2G_SUCCESS;
}

static cJSON* iso2_power_delivery_res_to_json(const struct iso2_PowerDeliveryResType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ResponseCode", iso2_response_code_to_string(msg->ResponseCode));
    if (msg->DC_EVSEStatus_isUsed) {
        cJSON_AddItemToObject(json, "DC_EVSEStatus", iso2_dc_evse_status_to_json(&msg->DC_EVSEStatus));
    }
    return json;
}

/* CurrentDemandReq */
static int json_to_iso2_current_demand_req(cJSON* json, struct iso2_CurrentDemandReqType* msg) {
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVStatus");
    if (status) json_to_iso2_dc_ev_status(status, &msg->DC_EVStatus);

    cJSON* target_current = cJSON_GetObjectItemCaseSensitive(json, "EVTargetCurrent");
    if (target_current) json_to_iso2_physical_value(target_current, &msg->EVTargetCurrent);

    msg->EVMaximumVoltageLimit_isUsed = json_has_key(json, "EVMaximumVoltageLimit");
    if (msg->EVMaximumVoltageLimit_isUsed) {
        cJSON* max_voltage = cJSON_GetObjectItemCaseSensitive(json, "EVMaximumVoltageLimit");
        json_to_iso2_physical_value(max_voltage, &msg->EVMaximumVoltageLimit);
    }

    msg->EVMaximumCurrentLimit_isUsed = json_has_key(json, "EVMaximumCurrentLimit");
    if (msg->EVMaximumCurrentLimit_isUsed) {
        cJSON* max_current = cJSON_GetObjectItemCaseSensitive(json, "EVMaximumCurrentLimit");
        json_to_iso2_physical_value(max_current, &msg->EVMaximumCurrentLimit);
    }

    msg->EVMaximumPowerLimit_isUsed = json_has_key(json, "EVMaximumPowerLimit");
    if (msg->EVMaximumPowerLimit_isUsed) {
        cJSON* max_power = cJSON_GetObjectItemCaseSensitive(json, "EVMaximumPowerLimit");
        json_to_iso2_physical_value(max_power, &msg->EVMaximumPowerLimit);
    }

    msg->BulkChargingComplete_isUsed = json_has_key(json, "BulkChargingComplete");
    if (msg->BulkChargingComplete_isUsed) {
        msg->BulkChargingComplete = json_get_bool(json, "BulkChargingComplete");
    }

    msg->ChargingComplete = json_get_bool(json, "ChargingComplete");

    msg->RemainingTimeToFullSoC_isUsed = json_has_key(json, "RemainingTimeToFullSoC");
    if (msg->RemainingTimeToFullSoC_isUsed) {
        cJSON* time = cJSON_GetObjectItemCaseSensitive(json, "RemainingTimeToFullSoC");
        json_to_iso2_physical_value(time, &msg->RemainingTimeToFullSoC);
    }

    msg->RemainingTimeToBulkSoC_isUsed = json_has_key(json, "RemainingTimeToBulkSoC");
    if (msg->RemainingTimeToBulkSoC_isUsed) {
        cJSON* time = cJSON_GetObjectItemCaseSensitive(json, "RemainingTimeToBulkSoC");
        json_to_iso2_physical_value(time, &msg->RemainingTimeToBulkSoC);
    }

    cJSON* target_voltage = cJSON_GetObjectItemCaseSensitive(json, "EVTargetVoltage");
    if (target_voltage) json_to_iso2_physical_value(target_voltage, &msg->EVTargetVoltage);

    return CBV2G_SUCCESS;
}

static cJSON* iso2_current_demand_req_to_json(const struct iso2_CurrentDemandReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "DC_EVStatus", iso2_dc_ev_status_to_json(&msg->DC_EVStatus));
    cJSON_AddItemToObject(json, "EVTargetCurrent", iso2_physical_value_to_json(&msg->EVTargetCurrent));

    if (msg->EVMaximumVoltageLimit_isUsed) {
        cJSON_AddItemToObject(json, "EVMaximumVoltageLimit", iso2_physical_value_to_json(&msg->EVMaximumVoltageLimit));
    }
    if (msg->EVMaximumCurrentLimit_isUsed) {
        cJSON_AddItemToObject(json, "EVMaximumCurrentLimit", iso2_physical_value_to_json(&msg->EVMaximumCurrentLimit));
    }
    if (msg->EVMaximumPowerLimit_isUsed) {
        cJSON_AddItemToObject(json, "EVMaximumPowerLimit", iso2_physical_value_to_json(&msg->EVMaximumPowerLimit));
    }
    if (msg->BulkChargingComplete_isUsed) {
        cJSON_AddBoolToObject(json, "BulkChargingComplete", msg->BulkChargingComplete);
    }
    cJSON_AddBoolToObject(json, "ChargingComplete", msg->ChargingComplete);
    if (msg->RemainingTimeToFullSoC_isUsed) {
        cJSON_AddItemToObject(json, "RemainingTimeToFullSoC", iso2_physical_value_to_json(&msg->RemainingTimeToFullSoC));
    }
    if (msg->RemainingTimeToBulkSoC_isUsed) {
        cJSON_AddItemToObject(json, "RemainingTimeToBulkSoC", iso2_physical_value_to_json(&msg->RemainingTimeToBulkSoC));
    }
    cJSON_AddItemToObject(json, "EVTargetVoltage", iso2_physical_value_to_json(&msg->EVTargetVoltage));
    return json;
}

/* CurrentDemandRes */
static int json_to_iso2_current_demand_res(cJSON* json, struct iso2_CurrentDemandResType* msg) {
    msg->ResponseCode = iso2_string_to_response_code(json_get_string(json, "ResponseCode"));

    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVSEStatus");
    if (status) json_to_iso2_dc_evse_status(status, &msg->DC_EVSEStatus);

    cJSON* present_voltage = cJSON_GetObjectItemCaseSensitive(json, "EVSEPresentVoltage");
    if (present_voltage) json_to_iso2_physical_value(present_voltage, &msg->EVSEPresentVoltage);

    cJSON* present_current = cJSON_GetObjectItemCaseSensitive(json, "EVSEPresentCurrent");
    if (present_current) json_to_iso2_physical_value(present_current, &msg->EVSEPresentCurrent);

    msg->EVSECurrentLimitAchieved = json_get_bool(json, "EVSECurrentLimitAchieved");
    msg->EVSEVoltageLimitAchieved = json_get_bool(json, "EVSEVoltageLimitAchieved");
    msg->EVSEPowerLimitAchieved = json_get_bool(json, "EVSEPowerLimitAchieved");

    msg->EVSEMaximumVoltageLimit_isUsed = json_has_key(json, "EVSEMaximumVoltageLimit");
    if (msg->EVSEMaximumVoltageLimit_isUsed) {
        cJSON* max_voltage = cJSON_GetObjectItemCaseSensitive(json, "EVSEMaximumVoltageLimit");
        json_to_iso2_physical_value(max_voltage, &msg->EVSEMaximumVoltageLimit);
    }

    msg->EVSEMaximumCurrentLimit_isUsed = json_has_key(json, "EVSEMaximumCurrentLimit");
    if (msg->EVSEMaximumCurrentLimit_isUsed) {
        cJSON* max_current = cJSON_GetObjectItemCaseSensitive(json, "EVSEMaximumCurrentLimit");
        json_to_iso2_physical_value(max_current, &msg->EVSEMaximumCurrentLimit);
    }

    msg->EVSEMaximumPowerLimit_isUsed = json_has_key(json, "EVSEMaximumPowerLimit");
    if (msg->EVSEMaximumPowerLimit_isUsed) {
        cJSON* max_power = cJSON_GetObjectItemCaseSensitive(json, "EVSEMaximumPowerLimit");
        json_to_iso2_physical_value(max_power, &msg->EVSEMaximumPowerLimit);
    }

    msg->SAScheduleTupleID = json_get_int(json, "SAScheduleTupleID");

    return CBV2G_SUCCESS;
}

static cJSON* iso2_current_demand_res_to_json(const struct iso2_CurrentDemandResType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ResponseCode", iso2_response_code_to_string(msg->ResponseCode));
    cJSON_AddItemToObject(json, "DC_EVSEStatus", iso2_dc_evse_status_to_json(&msg->DC_EVSEStatus));
    cJSON_AddItemToObject(json, "EVSEPresentVoltage", iso2_physical_value_to_json(&msg->EVSEPresentVoltage));
    cJSON_AddItemToObject(json, "EVSEPresentCurrent", iso2_physical_value_to_json(&msg->EVSEPresentCurrent));
    cJSON_AddBoolToObject(json, "EVSECurrentLimitAchieved", msg->EVSECurrentLimitAchieved);
    cJSON_AddBoolToObject(json, "EVSEVoltageLimitAchieved", msg->EVSEVoltageLimitAchieved);
    cJSON_AddBoolToObject(json, "EVSEPowerLimitAchieved", msg->EVSEPowerLimitAchieved);

    if (msg->EVSEMaximumVoltageLimit_isUsed) {
        cJSON_AddItemToObject(json, "EVSEMaximumVoltageLimit", iso2_physical_value_to_json(&msg->EVSEMaximumVoltageLimit));
    }
    if (msg->EVSEMaximumCurrentLimit_isUsed) {
        cJSON_AddItemToObject(json, "EVSEMaximumCurrentLimit", iso2_physical_value_to_json(&msg->EVSEMaximumCurrentLimit));
    }
    if (msg->EVSEMaximumPowerLimit_isUsed) {
        cJSON_AddItemToObject(json, "EVSEMaximumPowerLimit", iso2_physical_value_to_json(&msg->EVSEMaximumPowerLimit));
    }

    /* EVSEID - null-terminate the string */
    char evseid[iso2_EVSEID_CHARACTER_SIZE + 1];
    memcpy(evseid, msg->EVSEID.characters, msg->EVSEID.charactersLen);
    evseid[msg->EVSEID.charactersLen] = '\0';
    cJSON_AddStringToObject(json, "EVSEID", evseid);

    cJSON_AddNumberToObject(json, "SAScheduleTupleID", msg->SAScheduleTupleID);

    if (msg->MeterInfo_isUsed) {
        cJSON* meter = cJSON_CreateObject();
        char meter_id[iso2_MeterID_CHARACTER_SIZE + 1];
        memcpy(meter_id, msg->MeterInfo.MeterID.characters, msg->MeterInfo.MeterID.charactersLen);
        meter_id[msg->MeterInfo.MeterID.charactersLen] = '\0';
        cJSON_AddStringToObject(meter, "MeterID", meter_id);
        if (msg->MeterInfo.MeterReading_isUsed) {
            cJSON_AddNumberToObject(meter, "MeterReading", msg->MeterInfo.MeterReading);
        }
        cJSON_AddItemToObject(json, "MeterInfo", meter);
    }

    if (msg->ReceiptRequired_isUsed) {
        cJSON_AddBoolToObject(json, "ReceiptRequired", msg->ReceiptRequired);
    }

    return json;
}

/* ChargingStatusReq (AC) */
static int json_to_iso2_charging_status_req(cJSON* json, struct iso2_ChargingStatusReqType* msg) {
    (void)json;
    (void)msg;
    /* ChargingStatusReq has no required fields */
    return CBV2G_SUCCESS;
}

static cJSON* iso2_charging_status_req_to_json(const struct iso2_ChargingStatusReqType* msg) {
    (void)msg;
    return cJSON_CreateObject();
}

/* ChargingStatusRes (AC) */
static int json_to_iso2_charging_status_res(cJSON* json, struct iso2_ChargingStatusResType* msg) {
    msg->ResponseCode = iso2_string_to_response_code(json_get_string(json, "ResponseCode"));

    cJSON* evse_id = cJSON_GetObjectItemCaseSensitive(json, "EVSEID");
    if (evse_id && cJSON_IsString(evse_id)) {
        /* Bounded copy with snprintf (CWE-120 / CWE-126). */
        size_t evse_len = strnlen(evse_id->valuestring, iso2_EVSEID_CHARACTER_SIZE);
        int written = snprintf(msg->EVSEID.characters,
                               iso2_EVSEID_CHARACTER_SIZE,
                               "%.*s", (int)evse_len, evse_id->valuestring);
        if (written < 0) written = 0;
        if ((size_t)written >= iso2_EVSEID_CHARACTER_SIZE)
            written = iso2_EVSEID_CHARACTER_SIZE - 1;
        msg->EVSEID.charactersLen = (size_t)written;
    }

    msg->SAScheduleTupleID = json_get_int(json, "SAScheduleTupleID");
    msg->ReceiptRequired_isUsed = json_has_key(json, "ReceiptRequired");
    if (msg->ReceiptRequired_isUsed) {
        msg->ReceiptRequired = json_get_bool(json, "ReceiptRequired");
    }
    return CBV2G_SUCCESS;
}

static cJSON* iso2_charging_status_res_to_json(const struct iso2_ChargingStatusResType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ResponseCode", iso2_response_code_to_string(msg->ResponseCode));

    char evse_id[iso2_EVSEID_CHARACTER_SIZE + 1] = {0};
    memcpy(evse_id, msg->EVSEID.characters, msg->EVSEID.charactersLen);
    cJSON_AddStringToObject(json, "EVSEID", evse_id);

    cJSON_AddNumberToObject(json, "SAScheduleTupleID", msg->SAScheduleTupleID);
    if (msg->ReceiptRequired_isUsed) {
        cJSON_AddBoolToObject(json, "ReceiptRequired", msg->ReceiptRequired);
    }
    return json;
}

/* WeldingDetectionReq */
static int json_to_iso2_welding_detection_req(cJSON* json, struct iso2_WeldingDetectionReqType* msg) {
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVStatus");
    if (status) json_to_iso2_dc_ev_status(status, &msg->DC_EVStatus);
    return CBV2G_SUCCESS;
}

static cJSON* iso2_welding_detection_req_to_json(const struct iso2_WeldingDetectionReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "DC_EVStatus", iso2_dc_ev_status_to_json(&msg->DC_EVStatus));
    return json;
}

/* WeldingDetectionRes */
static int json_to_iso2_welding_detection_res(cJSON* json, struct iso2_WeldingDetectionResType* msg) {
    msg->ResponseCode = iso2_string_to_response_code(json_get_string(json, "ResponseCode"));
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVSEStatus");
    if (status) json_to_iso2_dc_evse_status(status, &msg->DC_EVSEStatus);

    cJSON* present_voltage = cJSON_GetObjectItemCaseSensitive(json, "EVSEPresentVoltage");
    if (present_voltage) json_to_iso2_physical_value(present_voltage, &msg->EVSEPresentVoltage);
    return CBV2G_SUCCESS;
}

static cJSON* iso2_welding_detection_res_to_json(const struct iso2_WeldingDetectionResType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ResponseCode", iso2_response_code_to_string(msg->ResponseCode));
    cJSON_AddItemToObject(json, "DC_EVSEStatus", iso2_dc_evse_status_to_json(&msg->DC_EVSEStatus));
    cJSON_AddItemToObject(json, "EVSEPresentVoltage", iso2_physical_value_to_json(&msg->EVSEPresentVoltage));
    return json;
}

/* SessionStopReq */
static int json_to_iso2_session_stop_req(cJSON* json, struct iso2_SessionStopReqType* msg) {
    const char* action = json_get_string(json, "ChargingSession");
    if (strcmp(action, "Terminate") == 0) {
        msg->ChargingSession = iso2_chargingSessionType_Terminate;
    } else {
        msg->ChargingSession = iso2_chargingSessionType_Pause;
    }
    return CBV2G_SUCCESS;
}

static cJSON* iso2_session_stop_req_to_json(const struct iso2_SessionStopReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ChargingSession",
                            msg->ChargingSession == iso2_chargingSessionType_Terminate ? "Terminate" : "Pause");
    return json;
}

/* SessionStopRes */
static int json_to_iso2_session_stop_res(cJSON* json, struct iso2_SessionStopResType* msg) {
    msg->ResponseCode = iso2_string_to_response_code(json_get_string(json, "ResponseCode"));
    return CBV2G_SUCCESS;
}

static cJSON* iso2_session_stop_res_to_json(const struct iso2_SessionStopResType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ResponseCode", iso2_response_code_to_string(msg->ResponseCode));
    return json;
}
