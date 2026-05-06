/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2026 Pionix GmbH and Contributors to EVerest
 *
 * din_converter.c - DIN 70121 JSON/EXI converter
 */

#include "cJSON.h"
#include "converters.h"
#include "json_utils.h"

#include <cbv2g/common/exi_bitstream.h>
#include <cbv2g/din/din_msgDefDatatypes.h>
#include <cbv2g/din/din_msgDefDecoder.h>
#include <cbv2g/din/din_msgDefEncoder.h>

#include <stdio.h>
#include <string.h>

/* Forward declarations for message converters */
static int json_to_din_header(cJSON* json, struct din_MessageHeaderType* header);
static cJSON* din_header_to_json(const struct din_MessageHeaderType* header);

/* Body message converters - encode */
static int json_to_din_session_setup_req(cJSON* json, struct din_SessionSetupReqType* msg);
static int json_to_din_session_setup_res(cJSON* json, struct din_SessionSetupResType* msg);
static int json_to_din_service_discovery_req(cJSON* json, struct din_ServiceDiscoveryReqType* msg);
static int json_to_din_service_discovery_res(cJSON* json, struct din_ServiceDiscoveryResType* msg);
static int json_to_din_service_payment_selection_req(cJSON* json, struct din_ServicePaymentSelectionReqType* msg);
static int json_to_din_service_payment_selection_res(cJSON* json, struct din_ServicePaymentSelectionResType* msg);
static int json_to_din_contract_authentication_req(cJSON* json, struct din_ContractAuthenticationReqType* msg);
static int json_to_din_contract_authentication_res(cJSON* json, struct din_ContractAuthenticationResType* msg);
static int json_to_din_charge_parameter_discovery_req(cJSON* json, struct din_ChargeParameterDiscoveryReqType* msg);
static int json_to_din_charge_parameter_discovery_res(cJSON* json, struct din_ChargeParameterDiscoveryResType* msg);
static int json_to_din_cable_check_req(cJSON* json, struct din_CableCheckReqType* msg);
static int json_to_din_cable_check_res(cJSON* json, struct din_CableCheckResType* msg);
static int json_to_din_pre_charge_req(cJSON* json, struct din_PreChargeReqType* msg);
static int json_to_din_pre_charge_res(cJSON* json, struct din_PreChargeResType* msg);
static int json_to_din_power_delivery_req(cJSON* json, struct din_PowerDeliveryReqType* msg);
static int json_to_din_power_delivery_res(cJSON* json, struct din_PowerDeliveryResType* msg);
static int json_to_din_current_demand_req(cJSON* json, struct din_CurrentDemandReqType* msg);
static int json_to_din_current_demand_res(cJSON* json, struct din_CurrentDemandResType* msg);
static int json_to_din_welding_detection_req(cJSON* json, struct din_WeldingDetectionReqType* msg);
static int json_to_din_welding_detection_res(cJSON* json, struct din_WeldingDetectionResType* msg);
static int json_to_din_session_stop_req(cJSON* json, struct din_SessionStopType* msg);
static int json_to_din_session_stop_res(cJSON* json, struct din_SessionStopResType* msg);

/* Body message converters - decode */
static cJSON* din_session_setup_req_to_json(const struct din_SessionSetupReqType* msg);
static cJSON* din_session_setup_res_to_json(const struct din_SessionSetupResType* msg);
static cJSON* din_service_discovery_req_to_json(const struct din_ServiceDiscoveryReqType* msg);
static cJSON* din_service_discovery_res_to_json(const struct din_ServiceDiscoveryResType* msg);
static cJSON* din_service_payment_selection_req_to_json(const struct din_ServicePaymentSelectionReqType* msg);
static cJSON* din_service_payment_selection_res_to_json(const struct din_ServicePaymentSelectionResType* msg);
static cJSON* din_contract_authentication_req_to_json(const struct din_ContractAuthenticationReqType* msg);
static cJSON* din_contract_authentication_res_to_json(const struct din_ContractAuthenticationResType* msg);
static cJSON* din_charge_parameter_discovery_req_to_json(const struct din_ChargeParameterDiscoveryReqType* msg);
static cJSON* din_charge_parameter_discovery_res_to_json(const struct din_ChargeParameterDiscoveryResType* msg);
static cJSON* din_cable_check_req_to_json(const struct din_CableCheckReqType* msg);
static cJSON* din_cable_check_res_to_json(const struct din_CableCheckResType* msg);
static cJSON* din_pre_charge_req_to_json(const struct din_PreChargeReqType* msg);
static cJSON* din_pre_charge_res_to_json(const struct din_PreChargeResType* msg);
static cJSON* din_power_delivery_req_to_json(const struct din_PowerDeliveryReqType* msg);
static cJSON* din_power_delivery_res_to_json(const struct din_PowerDeliveryResType* msg);
static cJSON* din_current_demand_req_to_json(const struct din_CurrentDemandReqType* msg);
static cJSON* din_current_demand_res_to_json(const struct din_CurrentDemandResType* msg);
static cJSON* din_welding_detection_req_to_json(const struct din_WeldingDetectionReqType* msg);
static cJSON* din_welding_detection_res_to_json(const struct din_WeldingDetectionResType* msg);
static cJSON* din_session_stop_req_to_json(const struct din_SessionStopType* msg);
static cJSON* din_session_stop_res_to_json(const struct din_SessionStopResType* msg);

/* Helper functions for common types */
static int json_to_din_physical_value(cJSON* json, struct din_PhysicalValueType* pv);
static cJSON* din_physical_value_to_json(const struct din_PhysicalValueType* pv);
static int json_to_din_dc_ev_status(cJSON* json, struct din_DC_EVStatusType* status);
static cJSON* din_dc_ev_status_to_json(const struct din_DC_EVStatusType* status);
static int json_to_din_dc_evse_status(cJSON* json, struct din_DC_EVSEStatusType* status);
static cJSON* din_dc_evse_status_to_json(const struct din_DC_EVSEStatusType* status);

/* Response code string conversion */
static const char* din_response_code_to_string(din_responseCodeType code);
static din_responseCodeType din_string_to_response_code(const char* str);

/*
 * Dispatch a parsed Body JSON object to the correct per-message converter.
 *
 * Returns CBV2G_SUCCESS once a recognised message has been parsed (the
 * matching field on `body_out` is initialised and `_isUsed` is set), the
 * converter's error code if a recognised message fails to parse, or
 * CBV2G_ERROR_UNKNOWN_MESSAGE if no recognised message tag is present.
 */
static int json_to_din_body(cJSON* body_json, struct din_BodyType* body_out) {
    cJSON* msg;

    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "SessionSetupReq")) != NULL) {
        init_din_SessionSetupReqType(&body_out->SessionSetupReq);
        body_out->SessionSetupReq_isUsed = 1;
        return json_to_din_session_setup_req(msg, &body_out->SessionSetupReq);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "SessionSetupRes")) != NULL) {
        init_din_SessionSetupResType(&body_out->SessionSetupRes);
        body_out->SessionSetupRes_isUsed = 1;
        return json_to_din_session_setup_res(msg, &body_out->SessionSetupRes);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "ServiceDiscoveryReq")) != NULL) {
        init_din_ServiceDiscoveryReqType(&body_out->ServiceDiscoveryReq);
        body_out->ServiceDiscoveryReq_isUsed = 1;
        return json_to_din_service_discovery_req(msg, &body_out->ServiceDiscoveryReq);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "ServiceDiscoveryRes")) != NULL) {
        init_din_ServiceDiscoveryResType(&body_out->ServiceDiscoveryRes);
        body_out->ServiceDiscoveryRes_isUsed = 1;
        return json_to_din_service_discovery_res(msg, &body_out->ServiceDiscoveryRes);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "ServicePaymentSelectionReq")) != NULL) {
        init_din_ServicePaymentSelectionReqType(&body_out->ServicePaymentSelectionReq);
        body_out->ServicePaymentSelectionReq_isUsed = 1;
        return json_to_din_service_payment_selection_req(msg, &body_out->ServicePaymentSelectionReq);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "ServicePaymentSelectionRes")) != NULL) {
        init_din_ServicePaymentSelectionResType(&body_out->ServicePaymentSelectionRes);
        body_out->ServicePaymentSelectionRes_isUsed = 1;
        return json_to_din_service_payment_selection_res(msg, &body_out->ServicePaymentSelectionRes);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "ContractAuthenticationReq")) != NULL) {
        init_din_ContractAuthenticationReqType(&body_out->ContractAuthenticationReq);
        body_out->ContractAuthenticationReq_isUsed = 1;
        return json_to_din_contract_authentication_req(msg, &body_out->ContractAuthenticationReq);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "ContractAuthenticationRes")) != NULL) {
        init_din_ContractAuthenticationResType(&body_out->ContractAuthenticationRes);
        body_out->ContractAuthenticationRes_isUsed = 1;
        return json_to_din_contract_authentication_res(msg, &body_out->ContractAuthenticationRes);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "ChargeParameterDiscoveryReq")) != NULL) {
        init_din_ChargeParameterDiscoveryReqType(&body_out->ChargeParameterDiscoveryReq);
        body_out->ChargeParameterDiscoveryReq_isUsed = 1;
        return json_to_din_charge_parameter_discovery_req(msg, &body_out->ChargeParameterDiscoveryReq);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "ChargeParameterDiscoveryRes")) != NULL) {
        init_din_ChargeParameterDiscoveryResType(&body_out->ChargeParameterDiscoveryRes);
        body_out->ChargeParameterDiscoveryRes_isUsed = 1;
        return json_to_din_charge_parameter_discovery_res(msg, &body_out->ChargeParameterDiscoveryRes);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "CableCheckReq")) != NULL) {
        init_din_CableCheckReqType(&body_out->CableCheckReq);
        body_out->CableCheckReq_isUsed = 1;
        return json_to_din_cable_check_req(msg, &body_out->CableCheckReq);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "CableCheckRes")) != NULL) {
        init_din_CableCheckResType(&body_out->CableCheckRes);
        body_out->CableCheckRes_isUsed = 1;
        return json_to_din_cable_check_res(msg, &body_out->CableCheckRes);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "PreChargeReq")) != NULL) {
        init_din_PreChargeReqType(&body_out->PreChargeReq);
        body_out->PreChargeReq_isUsed = 1;
        return json_to_din_pre_charge_req(msg, &body_out->PreChargeReq);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "PreChargeRes")) != NULL) {
        init_din_PreChargeResType(&body_out->PreChargeRes);
        body_out->PreChargeRes_isUsed = 1;
        return json_to_din_pre_charge_res(msg, &body_out->PreChargeRes);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "PowerDeliveryReq")) != NULL) {
        init_din_PowerDeliveryReqType(&body_out->PowerDeliveryReq);
        body_out->PowerDeliveryReq_isUsed = 1;
        return json_to_din_power_delivery_req(msg, &body_out->PowerDeliveryReq);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "PowerDeliveryRes")) != NULL) {
        init_din_PowerDeliveryResType(&body_out->PowerDeliveryRes);
        body_out->PowerDeliveryRes_isUsed = 1;
        return json_to_din_power_delivery_res(msg, &body_out->PowerDeliveryRes);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "CurrentDemandReq")) != NULL) {
        init_din_CurrentDemandReqType(&body_out->CurrentDemandReq);
        body_out->CurrentDemandReq_isUsed = 1;
        return json_to_din_current_demand_req(msg, &body_out->CurrentDemandReq);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "CurrentDemandRes")) != NULL) {
        init_din_CurrentDemandResType(&body_out->CurrentDemandRes);
        body_out->CurrentDemandRes_isUsed = 1;
        return json_to_din_current_demand_res(msg, &body_out->CurrentDemandRes);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "WeldingDetectionReq")) != NULL) {
        init_din_WeldingDetectionReqType(&body_out->WeldingDetectionReq);
        body_out->WeldingDetectionReq_isUsed = 1;
        return json_to_din_welding_detection_req(msg, &body_out->WeldingDetectionReq);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "WeldingDetectionRes")) != NULL) {
        init_din_WeldingDetectionResType(&body_out->WeldingDetectionRes);
        body_out->WeldingDetectionRes_isUsed = 1;
        return json_to_din_welding_detection_res(msg, &body_out->WeldingDetectionRes);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "SessionStopReq")) != NULL) {
        init_din_SessionStopType(&body_out->SessionStopReq);
        body_out->SessionStopReq_isUsed = 1;
        return json_to_din_session_stop_req(msg, &body_out->SessionStopReq);
    }
    if ((msg = cJSON_GetObjectItemCaseSensitive(body_json, "SessionStopRes")) != NULL) {
        init_din_SessionStopResType(&body_out->SessionStopRes);
        body_out->SessionStopRes_isUsed = 1;
        return json_to_din_session_stop_res(msg, &body_out->SessionStopRes);
    }

    set_error("Unknown DIN message type in Body");
    return CBV2G_ERROR_UNKNOWN_MESSAGE;
}

/*
 * Encode DIN message from JSON to EXI
 */
int din_encode(const char* json_str, uint8_t* out, size_t out_size, size_t* out_len) {
    cJSON* root = cJSON_Parse(json_str);
    if (root == NULL) {
        set_error("Failed to parse JSON: %s", cJSON_GetErrorPtr());
        return CBV2G_ERROR_JSON_PARSE;
    }

    cJSON* v2g_msg = cJSON_GetObjectItemCaseSensitive(root, "V2G_Message");
    if (v2g_msg == NULL) {
        set_error("V2G_Message not found in JSON");
        cJSON_Delete(root);
        return CBV2G_ERROR_JSON_PARSE;
    }

    struct din_exiDocument doc;
    init_din_exiDocument(&doc);

    cJSON* header = cJSON_GetObjectItemCaseSensitive(v2g_msg, "Header");
    if (header != NULL) {
        int rc = json_to_din_header(header, &doc.V2G_Message.Header);
        if (rc != CBV2G_SUCCESS) {
            cJSON_Delete(root);
            return rc;
        }
    }

    cJSON* body = cJSON_GetObjectItemCaseSensitive(v2g_msg, "Body");
    if (body == NULL) {
        set_error("Body not found in V2G_Message");
        cJSON_Delete(root);
        return CBV2G_ERROR_JSON_PARSE;
    }

    /* Init Body before any per-message init runs against it. */
    init_din_BodyType(&doc.V2G_Message.Body);

    int rc = json_to_din_body(body, &doc.V2G_Message.Body);
    if (rc != CBV2G_SUCCESS) {
        cJSON_Delete(root);
        return rc;
    }

    exi_bitstream_t stream;
    exi_bitstream_init(&stream, out, out_size, 0, NULL);

    int exi_result = encode_din_exiDocument(&stream, &doc);
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
 * Small per-message helper used by din_body_to_json: if the message is
 * present in the decoded document, run the converter and attach the
 * resulting cJSON to `body_json_out` under `name`.
 */
static int append_decoded_msg(cJSON* body_json_out, const char* name, cJSON* msg_json) {
    if (msg_json == NULL) {
        set_error("Failed to convert %s to JSON", name);
        return CBV2G_ERROR_JSON_GENERATE;
    }
    cJSON_AddItemToObject(body_json_out, name, msg_json);
    return CBV2G_SUCCESS;
}

/*
 * Add the single populated Body message in `body_in` to `body_json_out`.
 *
 * At most one of the `_isUsed` flags is set on a valid decoded document,
 * but we walk all variants for robustness.
 */
static int din_body_to_json(const struct din_BodyType* body_in, cJSON* body_json_out) {
    int rc;
    if (body_in->SessionSetupReq_isUsed) {
        rc = append_decoded_msg(body_json_out, "SessionSetupReq",
                                din_session_setup_req_to_json(&body_in->SessionSetupReq));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->SessionSetupRes_isUsed) {
        rc = append_decoded_msg(body_json_out, "SessionSetupRes",
                                din_session_setup_res_to_json(&body_in->SessionSetupRes));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->ServiceDiscoveryReq_isUsed) {
        rc = append_decoded_msg(body_json_out, "ServiceDiscoveryReq",
                                din_service_discovery_req_to_json(&body_in->ServiceDiscoveryReq));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->ServiceDiscoveryRes_isUsed) {
        rc = append_decoded_msg(body_json_out, "ServiceDiscoveryRes",
                                din_service_discovery_res_to_json(&body_in->ServiceDiscoveryRes));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->ServicePaymentSelectionReq_isUsed) {
        rc = append_decoded_msg(body_json_out, "ServicePaymentSelectionReq",
                                din_service_payment_selection_req_to_json(&body_in->ServicePaymentSelectionReq));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->ServicePaymentSelectionRes_isUsed) {
        rc = append_decoded_msg(body_json_out, "ServicePaymentSelectionRes",
                                din_service_payment_selection_res_to_json(&body_in->ServicePaymentSelectionRes));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->ContractAuthenticationReq_isUsed) {
        rc = append_decoded_msg(body_json_out, "ContractAuthenticationReq",
                                din_contract_authentication_req_to_json(&body_in->ContractAuthenticationReq));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->ContractAuthenticationRes_isUsed) {
        rc = append_decoded_msg(body_json_out, "ContractAuthenticationRes",
                                din_contract_authentication_res_to_json(&body_in->ContractAuthenticationRes));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->ChargeParameterDiscoveryReq_isUsed) {
        rc = append_decoded_msg(body_json_out, "ChargeParameterDiscoveryReq",
                                din_charge_parameter_discovery_req_to_json(&body_in->ChargeParameterDiscoveryReq));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->ChargeParameterDiscoveryRes_isUsed) {
        rc = append_decoded_msg(body_json_out, "ChargeParameterDiscoveryRes",
                                din_charge_parameter_discovery_res_to_json(&body_in->ChargeParameterDiscoveryRes));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->CableCheckReq_isUsed) {
        rc = append_decoded_msg(body_json_out, "CableCheckReq", din_cable_check_req_to_json(&body_in->CableCheckReq));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->CableCheckRes_isUsed) {
        rc = append_decoded_msg(body_json_out, "CableCheckRes", din_cable_check_res_to_json(&body_in->CableCheckRes));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->PreChargeReq_isUsed) {
        rc = append_decoded_msg(body_json_out, "PreChargeReq", din_pre_charge_req_to_json(&body_in->PreChargeReq));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->PreChargeRes_isUsed) {
        rc = append_decoded_msg(body_json_out, "PreChargeRes", din_pre_charge_res_to_json(&body_in->PreChargeRes));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->PowerDeliveryReq_isUsed) {
        rc = append_decoded_msg(body_json_out, "PowerDeliveryReq",
                                din_power_delivery_req_to_json(&body_in->PowerDeliveryReq));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->PowerDeliveryRes_isUsed) {
        rc = append_decoded_msg(body_json_out, "PowerDeliveryRes",
                                din_power_delivery_res_to_json(&body_in->PowerDeliveryRes));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->CurrentDemandReq_isUsed) {
        rc = append_decoded_msg(body_json_out, "CurrentDemandReq",
                                din_current_demand_req_to_json(&body_in->CurrentDemandReq));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->CurrentDemandRes_isUsed) {
        rc = append_decoded_msg(body_json_out, "CurrentDemandRes",
                                din_current_demand_res_to_json(&body_in->CurrentDemandRes));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->WeldingDetectionReq_isUsed) {
        rc = append_decoded_msg(body_json_out, "WeldingDetectionReq",
                                din_welding_detection_req_to_json(&body_in->WeldingDetectionReq));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->WeldingDetectionRes_isUsed) {
        rc = append_decoded_msg(body_json_out, "WeldingDetectionRes",
                                din_welding_detection_res_to_json(&body_in->WeldingDetectionRes));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->SessionStopReq_isUsed) {
        rc =
            append_decoded_msg(body_json_out, "SessionStopReq", din_session_stop_req_to_json(&body_in->SessionStopReq));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    if (body_in->SessionStopRes_isUsed) {
        rc =
            append_decoded_msg(body_json_out, "SessionStopRes", din_session_stop_res_to_json(&body_in->SessionStopRes));
        if (rc != CBV2G_SUCCESS) {
            return rc;
        }
    }
    return CBV2G_SUCCESS;
}

/*
 * Decode DIN message from EXI to JSON
 */
int din_decode(const uint8_t* exi, size_t exi_len, char* out, size_t out_size) {
    exi_bitstream_t stream;
    exi_bitstream_init(&stream, (uint8_t*)exi, exi_len, 0, NULL);

    struct din_exiDocument doc;
    init_din_exiDocument(&doc);

    int exi_result = decode_din_exiDocument(&stream, &doc);
    if (exi_result != 0) {
        set_error("EXI decoding failed with error code: %d", exi_result);
        return CBV2G_ERROR_DECODING_FAILED;
    }

    cJSON* root = cJSON_CreateObject();
    if (root == NULL) {
        set_error("Failed to create JSON object");
        return CBV2G_ERROR_JSON_GENERATE;
    }
    cJSON* v2g_msg = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "V2G_Message", v2g_msg);

    cJSON* header_json = din_header_to_json(&doc.V2G_Message.Header);
    cJSON_AddItemToObject(v2g_msg, "Header", header_json);

    cJSON* body_json = cJSON_CreateObject();
    cJSON_AddItemToObject(v2g_msg, "Body", body_json);

    int body_rc = din_body_to_json(&doc.V2G_Message.Body, body_json);
    if (body_rc != CBV2G_SUCCESS) {
        cJSON_Delete(root);
        return body_rc;
    }

    char* json_str = cJSON_PrintUnformatted(root);
    if (json_str == NULL) {
        set_error("Failed to serialize JSON");
        cJSON_Delete(root);
        return CBV2G_ERROR_JSON_GENERATE;
    }

    /* Bounded write to caller's buffer (CWE-120 / CWE-126). */
    int written = snprintf(out, out_size, "%s", json_str);
    cJSON_free(json_str);
    cJSON_Delete(root);

    if (written < 0 || (size_t)written >= out_size) {
        set_error("Output buffer too small: need %d, have %zu", written + 1, out_size);
        return CBV2G_ERROR_BUFFER_TOO_SMALL;
    }
    return CBV2G_SUCCESS;
}

/* ============== Helper Functions ============== */

static int json_to_din_header(cJSON* json, struct din_MessageHeaderType* header) {
    init_din_MessageHeaderType(header);

    /* SessionID - default to 8 zero bytes if not provided */
    header->SessionID.bytesLen = din_sessionIDType_BYTES_SIZE;
    memset(header->SessionID.bytes, 0, din_sessionIDType_BYTES_SIZE);

    cJSON* session_id = cJSON_GetObjectItemCaseSensitive(json, "SessionID");
    if (session_id && cJSON_IsString(session_id)) {
        /* SessionID is hexBinary in XSD - use hex decoding. Bound the
         * read length by 2*max-bytes (hex doubles the byte count) to
         * avoid an unbounded strlen on non-null-terminated input
         * (CWE-126). */
        size_t vs_len = strnlen(session_id->valuestring, din_sessionIDType_BYTES_SIZE * 2);
        size_t len = hex_decode(session_id->valuestring, vs_len, header->SessionID.bytes, din_sessionIDType_BYTES_SIZE);
        if (len > 0) {
            header->SessionID.bytesLen = len;
        }
    }

    return CBV2G_SUCCESS;
}

static cJSON* din_header_to_json(const struct din_MessageHeaderType* header) {
    cJSON* json = cJSON_CreateObject();

    /* SessionID is hexBinary in XSD - use hex encoding */
    char hex[17]; /* 8 bytes * 2 + null terminator */
    hex_encode(header->SessionID.bytes, header->SessionID.bytesLen, hex, sizeof(hex));
    cJSON_AddStringToObject(json, "SessionID", hex);

    return json;
}

static int json_to_din_physical_value(cJSON* json, struct din_PhysicalValueType* pv) {
    pv->Multiplier = json_get_int(json, "Multiplier");
    pv->Value = json_get_int(json, "Value");

    const char* unit = json_get_string(json, "Unit");
    if (strcmp(unit, "h") == 0) {
        pv->Unit = din_unitSymbolType_h;
    } else if (strcmp(unit, "m") == 0) {
        pv->Unit = din_unitSymbolType_m;
    } else if (strcmp(unit, "s") == 0) {
        pv->Unit = din_unitSymbolType_s;
    } else if (strcmp(unit, "A") == 0) {
        pv->Unit = din_unitSymbolType_A;
    } else if (strcmp(unit, "V") == 0) {
        pv->Unit = din_unitSymbolType_V;
    } else if (strcmp(unit, "W") == 0) {
        pv->Unit = din_unitSymbolType_W;
    } else if (strcmp(unit, "Wh") == 0) {
        pv->Unit = din_unitSymbolType_Wh;
    } else {
        pv->Unit = din_unitSymbolType_W; /* Default */
    }

    return CBV2G_SUCCESS;
}

static cJSON* din_physical_value_to_json(const struct din_PhysicalValueType* pv) {
    cJSON* json = cJSON_CreateObject();

    cJSON_AddNumberToObject(json, "Multiplier", pv->Multiplier);
    cJSON_AddNumberToObject(json, "Value", pv->Value);

    const char* unit;
    switch (pv->Unit) {
    case din_unitSymbolType_h:
        unit = "h";
        break;
    case din_unitSymbolType_m:
        unit = "m";
        break;
    case din_unitSymbolType_s:
        unit = "s";
        break;
    case din_unitSymbolType_A:
        unit = "A";
        break;
    case din_unitSymbolType_V:
        unit = "V";
        break;
    case din_unitSymbolType_W:
        unit = "W";
        break;
    case din_unitSymbolType_Wh:
        unit = "Wh";
        break;
    default:
        unit = "W";
        break;
    }
    cJSON_AddStringToObject(json, "Unit", unit);

    return json;
}

static int json_to_din_dc_ev_status(cJSON* json, struct din_DC_EVStatusType* status) {
    status->EVReady = json_get_bool(json, "EVReady");
    status->EVCabinConditioning_isUsed = json_has_key(json, "EVCabinConditioning");
    if (status->EVCabinConditioning_isUsed) {
        status->EVCabinConditioning = json_get_bool(json, "EVCabinConditioning");
    }
    status->EVRESSConditioning_isUsed = json_has_key(json, "EVRESSConditioning");
    if (status->EVRESSConditioning_isUsed) {
        status->EVRESSConditioning = json_get_bool(json, "EVRESSConditioning");
    }
    status->EVErrorCode = json_get_int(json, "EVErrorCode");
    status->EVRESSSOC = json_get_int(json, "EVRESSSOC");
    return CBV2G_SUCCESS;
}

static cJSON* din_dc_ev_status_to_json(const struct din_DC_EVStatusType* status) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddBoolToObject(json, "EVReady", status->EVReady);
    if (status->EVCabinConditioning_isUsed) {
        cJSON_AddBoolToObject(json, "EVCabinConditioning", status->EVCabinConditioning);
    }
    if (status->EVRESSConditioning_isUsed) {
        cJSON_AddBoolToObject(json, "EVRESSConditioning", status->EVRESSConditioning);
    }
    cJSON_AddNumberToObject(json, "EVErrorCode", status->EVErrorCode);
    cJSON_AddNumberToObject(json, "EVRESSSOC", status->EVRESSSOC);
    return json;
}

static int json_to_din_dc_evse_status(cJSON* json, struct din_DC_EVSEStatusType* status) {
    status->EVSEIsolationStatus_isUsed = json_has_key(json, "EVSEIsolationStatus");
    if (status->EVSEIsolationStatus_isUsed) {
        status->EVSEIsolationStatus = json_get_int(json, "EVSEIsolationStatus");
    }
    status->EVSEStatusCode = json_get_int(json, "EVSEStatusCode");
    status->NotificationMaxDelay = json_get_int(json, "NotificationMaxDelay");
    status->EVSENotification = json_get_int(json, "EVSENotification");
    return CBV2G_SUCCESS;
}

static const char* din_isolation_level_to_string(din_isolationLevelType level) {
    switch (level) {
    case din_isolationLevelType_Invalid:
        return "Invalid";
    case din_isolationLevelType_Valid:
        return "Valid";
    case din_isolationLevelType_Warning:
        return "Warning";
    case din_isolationLevelType_Fault:
        return "Fault";
    default:
        return "Invalid";
    }
}

static const char* din_dc_evse_status_code_to_string(din_DC_EVSEStatusCodeType code) {
    switch (code) {
    case din_DC_EVSEStatusCodeType_EVSE_NotReady:
        return "EVSE_NotReady";
    case din_DC_EVSEStatusCodeType_EVSE_Ready:
        return "EVSE_Ready";
    case din_DC_EVSEStatusCodeType_EVSE_Shutdown:
        return "EVSE_Shutdown";
    case din_DC_EVSEStatusCodeType_EVSE_UtilityInterruptEvent:
        return "EVSE_UtilityInterruptEvent";
    case din_DC_EVSEStatusCodeType_EVSE_IsolationMonitoringActive:
        return "EVSE_IsolationMonitoringActive";
    case din_DC_EVSEStatusCodeType_EVSE_EmergencyShutdown:
        return "EVSE_EmergencyShutdown";
    case din_DC_EVSEStatusCodeType_EVSE_Malfunction:
        return "EVSE_Malfunction";
    case din_DC_EVSEStatusCodeType_Reserved_8:
        return "Reserved_8";
    case din_DC_EVSEStatusCodeType_Reserved_9:
        return "Reserved_9";
    case din_DC_EVSEStatusCodeType_Reserved_A:
        return "Reserved_A";
    case din_DC_EVSEStatusCodeType_Reserved_B:
        return "Reserved_B";
    case din_DC_EVSEStatusCodeType_Reserved_C:
        return "Reserved_C";
    default:
        return "EVSE_NotReady";
    }
}

static const char* din_evse_notification_to_string(din_EVSENotificationType notification) {
    switch (notification) {
    case din_EVSENotificationType_None:
        return "None";
    case din_EVSENotificationType_StopCharging:
        return "StopCharging";
    case din_EVSENotificationType_ReNegotiation:
        return "ReNegotiation";
    default:
        return "None";
    }
}

static cJSON* din_dc_evse_status_to_json(const struct din_DC_EVSEStatusType* status) {
    cJSON* json = cJSON_CreateObject();
    if (status->EVSEIsolationStatus_isUsed) {
        cJSON_AddStringToObject(json, "EVSEIsolationStatus",
                                din_isolation_level_to_string(status->EVSEIsolationStatus));
    }
    cJSON_AddStringToObject(json, "EVSEStatusCode", din_dc_evse_status_code_to_string(status->EVSEStatusCode));
    cJSON_AddNumberToObject(json, "NotificationMaxDelay", status->NotificationMaxDelay);
    cJSON_AddStringToObject(json, "EVSENotification", din_evse_notification_to_string(status->EVSENotification));
    return json;
}

static cJSON* din_sa_schedule_list_to_json(const struct din_SAScheduleListType* sa_list) {
    cJSON* json = cJSON_CreateObject();
    cJSON* schedule_tuple_array = cJSON_CreateArray();

    for (uint16_t i = 0; i < sa_list->SAScheduleTuple.arrayLen; i++) {
        const struct din_SAScheduleTupleType* tuple = &sa_list->SAScheduleTuple.array[i];
        cJSON* tuple_json = cJSON_CreateObject();

        cJSON_AddNumberToObject(tuple_json, "SAScheduleTupleID", tuple->SAScheduleTupleID);

        /* PMaxSchedule */
        cJSON* pmax_schedule = cJSON_CreateObject();
        cJSON_AddNumberToObject(pmax_schedule, "PMaxScheduleID", tuple->PMaxSchedule.PMaxScheduleID);

        cJSON* pmax_entry_array = cJSON_CreateArray();
        for (uint16_t j = 0; j < tuple->PMaxSchedule.PMaxScheduleEntry.arrayLen; j++) {
            const struct din_PMaxScheduleEntryType* entry = &tuple->PMaxSchedule.PMaxScheduleEntry.array[j];
            cJSON* entry_json = cJSON_CreateObject();

            if (entry->RelativeTimeInterval_isUsed) {
                cJSON* rel_time = cJSON_CreateObject();
                cJSON_AddNumberToObject(rel_time, "start", entry->RelativeTimeInterval.start);
                if (entry->RelativeTimeInterval.duration_isUsed) {
                    cJSON_AddNumberToObject(rel_time, "duration", entry->RelativeTimeInterval.duration);
                }
                cJSON_AddItemToObject(entry_json, "RelativeTimeInterval", rel_time);
            }

            /* DIN uses PMax as int16_t, not PhysicalValueType */
            cJSON_AddNumberToObject(entry_json, "PMax", entry->PMax);
            cJSON_AddItemToArray(pmax_entry_array, entry_json);
        }

        cJSON_AddItemToObject(pmax_schedule, "PMaxScheduleEntry", pmax_entry_array);
        cJSON_AddItemToObject(tuple_json, "PMaxSchedule", pmax_schedule);

        cJSON_AddItemToArray(schedule_tuple_array, tuple_json);
    }

    cJSON_AddItemToObject(json, "SAScheduleTuple", schedule_tuple_array);
    return json;
}

/*
 * Translate a din_responseCodeType enum value into its canonical string.
 * Returns NULL with set_error() on an unrecognised code so that callers
 * can surface the failure instead of emitting a placeholder.
 */
static const char* din_response_code_to_string(din_responseCodeType code) {
    switch (code) {
    case din_responseCodeType_OK:
        return "OK";
    case din_responseCodeType_OK_NewSessionEstablished:
        return "OK_NewSessionEstablished";
    case din_responseCodeType_OK_OldSessionJoined:
        return "OK_OldSessionJoined";
    case din_responseCodeType_OK_CertificateExpiresSoon:
        return "OK_CertificateExpiresSoon";
    case din_responseCodeType_FAILED:
        return "FAILED";
    case din_responseCodeType_FAILED_SequenceError:
        return "FAILED_SequenceError";
    case din_responseCodeType_FAILED_ServiceIDInvalid:
        return "FAILED_ServiceIDInvalid";
    case din_responseCodeType_FAILED_UnknownSession:
        return "FAILED_UnknownSession";
    case din_responseCodeType_FAILED_ServiceSelectionInvalid:
        return "FAILED_ServiceSelectionInvalid";
    case din_responseCodeType_FAILED_PaymentSelectionInvalid:
        return "FAILED_PaymentSelectionInvalid";
    case din_responseCodeType_FAILED_CertificateExpired:
        return "FAILED_CertificateExpired";
    case din_responseCodeType_FAILED_SignatureError:
        return "FAILED_SignatureError";
    case din_responseCodeType_FAILED_NoCertificateAvailable:
        return "FAILED_NoCertificateAvailable";
    case din_responseCodeType_FAILED_CertChainError:
        return "FAILED_CertChainError";
    case din_responseCodeType_FAILED_ChallengeInvalid:
        return "FAILED_ChallengeInvalid";
    case din_responseCodeType_FAILED_ContractCanceled:
        return "FAILED_ContractCanceled";
    case din_responseCodeType_FAILED_WrongChargeParameter:
        return "FAILED_WrongChargeParameter";
    case din_responseCodeType_FAILED_PowerDeliveryNotApplied:
        return "FAILED_PowerDeliveryNotApplied";
    case din_responseCodeType_FAILED_TariffSelectionInvalid:
        return "FAILED_TariffSelectionInvalid";
    case din_responseCodeType_FAILED_ChargingProfileInvalid:
        return "FAILED_ChargingProfileInvalid";
    case din_responseCodeType_FAILED_EVSEPresentVoltageToLow:
        return "FAILED_EVSEPresentVoltageToLow";
    case din_responseCodeType_FAILED_MeteringSignatureNotValid:
        return "FAILED_MeteringSignatureNotValid";
    case din_responseCodeType_FAILED_WrongEnergyTransferType:
        return "FAILED_WrongEnergyTransferType";
    default:
        set_error("Unknown DIN ResponseCode value: %d", (int)code);
        return NULL;
    }
}

/*
 * Add ResponseCode as a string to `json`. Wrapper over the enum→string
 * helper that propagates "unknown code" as an error instead of leaving
 * the field unset or filled with a placeholder. Returns 0 on success,
 * -1 on unknown code (set_error has already been called).
 */
static int add_response_code_string(cJSON* json, din_responseCodeType code) {
    const char* s = din_response_code_to_string(code);
    if (s == NULL) {
        return -1;
    }
    cJSON_AddStringToObject(json, "ResponseCode", s);
    return 0;
}

/*
 * Translate a ResponseCode string into its enum value. Covers every name
 * emitted by din_response_code_to_string; an unrecognised string falls
 * through to FAILED, which mirrors the wire-level "something is wrong"
 * default that DIN itself uses.
 */
static din_responseCodeType din_string_to_response_code(const char* str) {
    if (strcmp(str, "OK") == 0) {
        return din_responseCodeType_OK;
    }
    if (strcmp(str, "OK_NewSessionEstablished") == 0) {
        return din_responseCodeType_OK_NewSessionEstablished;
    }
    if (strcmp(str, "OK_OldSessionJoined") == 0) {
        return din_responseCodeType_OK_OldSessionJoined;
    }
    if (strcmp(str, "OK_CertificateExpiresSoon") == 0) {
        return din_responseCodeType_OK_CertificateExpiresSoon;
    }
    if (strcmp(str, "FAILED") == 0) {
        return din_responseCodeType_FAILED;
    }
    if (strcmp(str, "FAILED_SequenceError") == 0) {
        return din_responseCodeType_FAILED_SequenceError;
    }
    if (strcmp(str, "FAILED_ServiceIDInvalid") == 0) {
        return din_responseCodeType_FAILED_ServiceIDInvalid;
    }
    if (strcmp(str, "FAILED_UnknownSession") == 0) {
        return din_responseCodeType_FAILED_UnknownSession;
    }
    if (strcmp(str, "FAILED_ServiceSelectionInvalid") == 0) {
        return din_responseCodeType_FAILED_ServiceSelectionInvalid;
    }
    if (strcmp(str, "FAILED_PaymentSelectionInvalid") == 0) {
        return din_responseCodeType_FAILED_PaymentSelectionInvalid;
    }
    if (strcmp(str, "FAILED_CertificateExpired") == 0) {
        return din_responseCodeType_FAILED_CertificateExpired;
    }
    if (strcmp(str, "FAILED_SignatureError") == 0) {
        return din_responseCodeType_FAILED_SignatureError;
    }
    if (strcmp(str, "FAILED_NoCertificateAvailable") == 0) {
        return din_responseCodeType_FAILED_NoCertificateAvailable;
    }
    if (strcmp(str, "FAILED_CertChainError") == 0) {
        return din_responseCodeType_FAILED_CertChainError;
    }
    if (strcmp(str, "FAILED_ChallengeInvalid") == 0) {
        return din_responseCodeType_FAILED_ChallengeInvalid;
    }
    if (strcmp(str, "FAILED_ContractCanceled") == 0) {
        return din_responseCodeType_FAILED_ContractCanceled;
    }
    if (strcmp(str, "FAILED_WrongChargeParameter") == 0) {
        return din_responseCodeType_FAILED_WrongChargeParameter;
    }
    if (strcmp(str, "FAILED_PowerDeliveryNotApplied") == 0) {
        return din_responseCodeType_FAILED_PowerDeliveryNotApplied;
    }
    if (strcmp(str, "FAILED_TariffSelectionInvalid") == 0) {
        return din_responseCodeType_FAILED_TariffSelectionInvalid;
    }
    if (strcmp(str, "FAILED_ChargingProfileInvalid") == 0) {
        return din_responseCodeType_FAILED_ChargingProfileInvalid;
    }
    if (strcmp(str, "FAILED_EVSEPresentVoltageToLow") == 0) {
        return din_responseCodeType_FAILED_EVSEPresentVoltageToLow;
    }
    if (strcmp(str, "FAILED_MeteringSignatureNotValid") == 0) {
        return din_responseCodeType_FAILED_MeteringSignatureNotValid;
    }
    if (strcmp(str, "FAILED_WrongEnergyTransferType") == 0) {
        return din_responseCodeType_FAILED_WrongEnergyTransferType;
    }
    return din_responseCodeType_FAILED;
}

/* Energy transfer type enum to/from string */
static const char* din_energy_transfer_type_to_string(din_EVSESupportedEnergyTransferType type) {
    switch (type) {
    case din_EVSESupportedEnergyTransferType_AC_single_phase_core:
        return "AC_single_phase_core";
    case din_EVSESupportedEnergyTransferType_AC_three_phase_core:
        return "AC_three_phase_core";
    case din_EVSESupportedEnergyTransferType_DC_core:
        return "DC_core";
    case din_EVSESupportedEnergyTransferType_DC_extended:
        return "DC_extended";
    case din_EVSESupportedEnergyTransferType_DC_combo_core:
        return "DC_combo_core";
    case din_EVSESupportedEnergyTransferType_DC_dual:
        return "DC_unique";
    case din_EVSESupportedEnergyTransferType_AC_core1p_DC_extended:
        return "AC_core1p_DC_extended";
    case din_EVSESupportedEnergyTransferType_AC_single_DC_core:
        return "AC_single_DC_core";
    case din_EVSESupportedEnergyTransferType_AC_single_phase_three_phase_core_DC_extended:
        return "AC_single_phase_three_phase_core_DC_extended";
    case din_EVSESupportedEnergyTransferType_AC_core3p_DC_extended:
        return "AC_core3p_DC_extended";
    default:
        return "DC_extended";
    }
}

static din_EVSESupportedEnergyTransferType din_string_to_energy_transfer_type(const char* str) {
    if (strcmp(str, "AC_single_phase_core") == 0) {
        return din_EVSESupportedEnergyTransferType_AC_single_phase_core;
    }
    if (strcmp(str, "AC_three_phase_core") == 0) {
        return din_EVSESupportedEnergyTransferType_AC_three_phase_core;
    }
    if (strcmp(str, "DC_core") == 0) {
        return din_EVSESupportedEnergyTransferType_DC_core;
    }
    if (strcmp(str, "DC_extended") == 0) {
        return din_EVSESupportedEnergyTransferType_DC_extended;
    }
    if (strcmp(str, "DC_combo_core") == 0) {
        return din_EVSESupportedEnergyTransferType_DC_combo_core;
    }
    if (strcmp(str, "DC_unique") == 0) {
        return din_EVSESupportedEnergyTransferType_DC_dual;
    }
    return din_EVSESupportedEnergyTransferType_DC_extended;
}

/* ============== Message Converters ============== */

/* SessionSetupReq */
static int json_to_din_session_setup_req(cJSON* json, struct din_SessionSetupReqType* msg) {
    cJSON* evcc_id = cJSON_GetObjectItemCaseSensitive(json, "EVCCID");
    if (evcc_id && cJSON_IsString(evcc_id)) {
        /* EVCCID is hexBinary in XSD - use hex decoding (CWE-126: bounded
         * strnlen). */
        size_t vs_len = strnlen(evcc_id->valuestring, din_evccIDType_BYTES_SIZE * 2);
        size_t len = hex_decode(evcc_id->valuestring, vs_len, msg->EVCCID.bytes, din_evccIDType_BYTES_SIZE);
        msg->EVCCID.bytesLen = len;
    }
    return CBV2G_SUCCESS;
}

static cJSON* din_session_setup_req_to_json(const struct din_SessionSetupReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    /* EVCCID is hexBinary in XSD - use hex encoding */
    char hex[13]; /* 6 bytes * 2 + null terminator */
    hex_encode(msg->EVCCID.bytes, msg->EVCCID.bytesLen, hex, sizeof(hex));
    cJSON_AddStringToObject(json, "EVCCID", hex);
    return json;
}

/* SessionSetupRes */
static int json_to_din_session_setup_res(cJSON* json, struct din_SessionSetupResType* msg) {
    msg->ResponseCode = din_string_to_response_code(json_get_string(json, "ResponseCode"));

    cJSON* evse_id = cJSON_GetObjectItemCaseSensitive(json, "EVSEID");
    if (evse_id && cJSON_IsString(evse_id)) {
        /* EVSEID is hexBinary in XSD - use hex decoding (CWE-126). */
        size_t vs_len = strnlen(evse_id->valuestring, din_evseIDType_BYTES_SIZE * 2);
        size_t len = hex_decode(evse_id->valuestring, vs_len, msg->EVSEID.bytes, din_evseIDType_BYTES_SIZE);
        msg->EVSEID.bytesLen = len;
    }

    msg->DateTimeNow_isUsed = json_has_key(json, "DateTimeNow");
    if (msg->DateTimeNow_isUsed) {
        msg->DateTimeNow = json_get_int(json, "DateTimeNow");
    }
    return CBV2G_SUCCESS;
}

static cJSON* din_session_setup_res_to_json(const struct din_SessionSetupResType* msg) {
    cJSON* json = cJSON_CreateObject();
    if (add_response_code_string(json, msg->ResponseCode) != 0) {
        cJSON_Delete(json);
        return NULL;
    }

    /* EVSEID is hexBinary in XSD - use hex encoding */
    char hex[65]; /* 32 bytes * 2 + null terminator */
    hex_encode(msg->EVSEID.bytes, msg->EVSEID.bytesLen, hex, sizeof(hex));
    cJSON_AddStringToObject(json, "EVSEID", hex);

    if (msg->DateTimeNow_isUsed) {
        cJSON_AddNumberToObject(json, "DateTimeNow", msg->DateTimeNow);
    }
    return json;
}

/* ServiceDiscoveryReq */
static int json_to_din_service_discovery_req(cJSON* json, struct din_ServiceDiscoveryReqType* msg) {
    msg->ServiceScope_isUsed = json_has_key(json, "ServiceScope");
    if (msg->ServiceScope_isUsed) {
        /* Bounded copy with snprintf (CWE-120 / CWE-126). */
        const char* scope = json_get_string(json, "ServiceScope");
        size_t scope_len = strnlen(scope, din_ServiceScope_CHARACTER_SIZE);
        int written =
            snprintf(msg->ServiceScope.characters, din_ServiceScope_CHARACTER_SIZE, "%.*s", (int)scope_len, scope);
        if (written < 0) {
            written = 0;
        }
        if ((size_t)written >= din_ServiceScope_CHARACTER_SIZE) {
            written = din_ServiceScope_CHARACTER_SIZE - 1;
        }
        msg->ServiceScope.charactersLen = (size_t)written;
    }
    msg->ServiceCategory_isUsed = json_has_key(json, "ServiceCategory");
    if (msg->ServiceCategory_isUsed) {
        msg->ServiceCategory = json_get_int(json, "ServiceCategory");
    }
    return CBV2G_SUCCESS;
}

static cJSON* din_service_discovery_req_to_json(const struct din_ServiceDiscoveryReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    if (msg->ServiceScope_isUsed) {
        /* Bounded copy with snprintf (CWE-120). */
        char scope[din_ServiceScope_CHARACTER_SIZE + 1] = {0};
        snprintf(scope, sizeof(scope), "%.*s", (int)msg->ServiceScope.charactersLen, msg->ServiceScope.characters);
        cJSON_AddStringToObject(json, "ServiceScope", scope);
    }
    if (msg->ServiceCategory_isUsed) {
        cJSON_AddNumberToObject(json, "ServiceCategory", msg->ServiceCategory);
    }
    return json;
}

/* ServiceDiscoveryRes */
static int json_to_din_service_discovery_res(cJSON* json, struct din_ServiceDiscoveryResType* msg) {
    msg->ResponseCode = din_string_to_response_code(json_get_string(json, "ResponseCode"));

    /* PaymentOptions */
    cJSON* payment_options = cJSON_GetObjectItemCaseSensitive(json, "PaymentOptions");
    if (payment_options) {
        cJSON* po_array = cJSON_GetObjectItemCaseSensitive(payment_options, "PaymentOption");
        if (po_array && cJSON_IsArray(po_array)) {
            int count = cJSON_GetArraySize(po_array);
            if (count > din_paymentOptionType_2_ARRAY_SIZE) {
                count = din_paymentOptionType_2_ARRAY_SIZE;
            }
            msg->PaymentOptions.PaymentOption.arrayLen = count;
            for (int i = 0; i < count; i++) {
                cJSON* item = cJSON_GetArrayItem(po_array, i);
                if (cJSON_IsString(item)) {
                    if (strcmp(item->valuestring, "Contract") == 0) {
                        msg->PaymentOptions.PaymentOption.array[i] = din_paymentOptionType_Contract;
                    } else {
                        msg->PaymentOptions.PaymentOption.array[i] = din_paymentOptionType_ExternalPayment;
                    }
                }
            }
        }
    }

    /* ChargeService */
    cJSON* charge_service = cJSON_GetObjectItemCaseSensitive(json, "ChargeService");
    if (charge_service) {
        msg->ChargeService.ServiceTag.ServiceID =
            json_get_int(cJSON_GetObjectItemCaseSensitive(charge_service, "ServiceTag"), "ServiceID");
        msg->ChargeService.FreeService = json_get_bool(charge_service, "FreeService");
        msg->ChargeService.EnergyTransferType =
            din_string_to_energy_transfer_type(json_get_string(charge_service, "EnergyTransferType"));
    }

    return CBV2G_SUCCESS;
}

static cJSON* din_service_discovery_res_to_json(const struct din_ServiceDiscoveryResType* msg) {
    cJSON* json = cJSON_CreateObject();
    if (add_response_code_string(json, msg->ResponseCode) != 0) {
        cJSON_Delete(json);
        return NULL;
    }

    /* PaymentOptions */
    cJSON* payment_options = cJSON_CreateObject();
    cJSON* po_array = cJSON_CreateArray();
    for (int i = 0; i < msg->PaymentOptions.PaymentOption.arrayLen; i++) {
        const char* po_str = msg->PaymentOptions.PaymentOption.array[i] == din_paymentOptionType_Contract
                                 ? "Contract"
                                 : "ExternalPayment";
        cJSON_AddItemToArray(po_array, cJSON_CreateString(po_str));
    }
    cJSON_AddItemToObject(payment_options, "PaymentOption", po_array);
    cJSON_AddItemToObject(json, "PaymentOptions", payment_options);

    /* ChargeService */
    cJSON* charge_service = cJSON_CreateObject();
    cJSON* service_tag = cJSON_CreateObject();
    cJSON_AddNumberToObject(service_tag, "ServiceID", msg->ChargeService.ServiceTag.ServiceID);
    /* ServiceCategory: 0=EVCharging, 1=Internet, 2=ContractCertificate, 3=OtherCustom */
    const char* service_category;
    switch (msg->ChargeService.ServiceTag.ServiceCategory) {
    case din_serviceCategoryType_EVCharging:
        service_category = "EVCharging";
        break;
    case din_serviceCategoryType_Internet:
        service_category = "Internet";
        break;
    case din_serviceCategoryType_ContractCertificate:
        service_category = "ContractCertificate";
        break;
    case din_serviceCategoryType_OtherCustom:
        service_category = "OtherCustom";
        break;
    default:
        service_category = "EVCharging";
        break;
    }
    cJSON_AddStringToObject(service_tag, "ServiceCategory", service_category);
    cJSON_AddItemToObject(charge_service, "ServiceTag", service_tag);
    cJSON_AddBoolToObject(charge_service, "FreeService", msg->ChargeService.FreeService);
    cJSON_AddStringToObject(charge_service, "EnergyTransferType",
                            din_energy_transfer_type_to_string(msg->ChargeService.EnergyTransferType));
    cJSON_AddItemToObject(json, "ChargeService", charge_service);

    return json;
}

/* ServicePaymentSelectionReq */
static int json_to_din_service_payment_selection_req(cJSON* json, struct din_ServicePaymentSelectionReqType* msg) {
    const char* po = json_get_string(json, "SelectedPaymentOption");
    msg->SelectedPaymentOption =
        strcmp(po, "Contract") == 0 ? din_paymentOptionType_Contract : din_paymentOptionType_ExternalPayment;

    cJSON* selected_list = cJSON_GetObjectItemCaseSensitive(json, "SelectedServiceList");
    if (selected_list) {
        cJSON* services = cJSON_GetObjectItemCaseSensitive(selected_list, "SelectedService");
        if (services && cJSON_IsArray(services)) {
            int count = cJSON_GetArraySize(services);
            if (count > din_SelectedServiceType_16_ARRAY_SIZE) {
                count = din_SelectedServiceType_16_ARRAY_SIZE;
            }
            msg->SelectedServiceList.SelectedService.arrayLen = count;
            for (int i = 0; i < count; i++) {
                cJSON* item = cJSON_GetArrayItem(services, i);
                msg->SelectedServiceList.SelectedService.array[i].ServiceID = json_get_int(item, "ServiceID");
            }
        }
    }
    return CBV2G_SUCCESS;
}

static cJSON* din_service_payment_selection_req_to_json(const struct din_ServicePaymentSelectionReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "SelectedPaymentOption",
                            msg->SelectedPaymentOption == din_paymentOptionType_Contract ? "Contract"
                                                                                         : "ExternalPayment");

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

/* ServicePaymentSelectionRes */
static int json_to_din_service_payment_selection_res(cJSON* json, struct din_ServicePaymentSelectionResType* msg) {
    msg->ResponseCode = din_string_to_response_code(json_get_string(json, "ResponseCode"));
    return CBV2G_SUCCESS;
}

static cJSON* din_service_payment_selection_res_to_json(const struct din_ServicePaymentSelectionResType* msg) {
    cJSON* json = cJSON_CreateObject();
    if (add_response_code_string(json, msg->ResponseCode) != 0) {
        cJSON_Delete(json);
        return NULL;
    }
    return json;
}

/* ContractAuthenticationReq - has optional Id field but typically empty */
static int json_to_din_contract_authentication_req(cJSON* json, struct din_ContractAuthenticationReqType* msg) {
    (void)json;
    (void)msg;
    /* ContractAuthenticationReq has optional Id and GenChallenge fields, typically empty */
    msg->Id_isUsed = 0;
    msg->GenChallenge_isUsed = 0;
    return CBV2G_SUCCESS;
}

static cJSON* din_contract_authentication_req_to_json(const struct din_ContractAuthenticationReqType* msg) {
    (void)msg;
    return cJSON_CreateObject();
}

/* ContractAuthenticationRes */
static int json_to_din_contract_authentication_res(cJSON* json, struct din_ContractAuthenticationResType* msg) {
    msg->ResponseCode = din_string_to_response_code(json_get_string(json, "ResponseCode"));
    const char* processing = json_get_string(json, "EVSEProcessing");
    if (strcmp(processing, "Finished") == 0) {
        msg->EVSEProcessing = din_EVSEProcessingType_Finished;
    } else {
        msg->EVSEProcessing = din_EVSEProcessingType_Ongoing;
    }
    return CBV2G_SUCCESS;
}

static cJSON* din_contract_authentication_res_to_json(const struct din_ContractAuthenticationResType* msg) {
    cJSON* json = cJSON_CreateObject();
    if (add_response_code_string(json, msg->ResponseCode) != 0) {
        cJSON_Delete(json);
        return NULL;
    }
    cJSON_AddStringToObject(json, "EVSEProcessing",
                            msg->EVSEProcessing == din_EVSEProcessingType_Finished ? "Finished" : "Ongoing");
    return json;
}

/* ChargeParameterDiscoveryReq */
static int json_to_din_charge_parameter_discovery_req(cJSON* json, struct din_ChargeParameterDiscoveryReqType* msg) {
    msg->EVRequestedEnergyTransferType =
        din_string_to_energy_transfer_type(json_get_string(json, "EVRequestedEnergyTransferType"));

    cJSON* dc_params = cJSON_GetObjectItemCaseSensitive(json, "DC_EVChargeParameter");
    if (dc_params) {
        msg->DC_EVChargeParameter_isUsed = 1;
        cJSON* status = cJSON_GetObjectItemCaseSensitive(dc_params, "DC_EVStatus");
        if (status) {
            json_to_din_dc_ev_status(status, &msg->DC_EVChargeParameter.DC_EVStatus);
        }

        cJSON* max_current = cJSON_GetObjectItemCaseSensitive(dc_params, "EVMaximumCurrentLimit");
        if (max_current) {
            json_to_din_physical_value(max_current, &msg->DC_EVChargeParameter.EVMaximumCurrentLimit);
        }

        cJSON* max_voltage = cJSON_GetObjectItemCaseSensitive(dc_params, "EVMaximumVoltageLimit");
        if (max_voltage) {
            json_to_din_physical_value(max_voltage, &msg->DC_EVChargeParameter.EVMaximumVoltageLimit);
        }

        msg->DC_EVChargeParameter.EVMaximumPowerLimit_isUsed = json_has_key(dc_params, "EVMaximumPowerLimit");
        if (msg->DC_EVChargeParameter.EVMaximumPowerLimit_isUsed) {
            cJSON* max_power = cJSON_GetObjectItemCaseSensitive(dc_params, "EVMaximumPowerLimit");
            json_to_din_physical_value(max_power, &msg->DC_EVChargeParameter.EVMaximumPowerLimit);
        }

        cJSON* energy_cap = cJSON_GetObjectItemCaseSensitive(dc_params, "EVEnergyCapacity");
        if (energy_cap) {
            msg->DC_EVChargeParameter.EVEnergyCapacity_isUsed = 1;
            json_to_din_physical_value(energy_cap, &msg->DC_EVChargeParameter.EVEnergyCapacity);
        }

        cJSON* energy_req = cJSON_GetObjectItemCaseSensitive(dc_params, "EVEnergyRequest");
        if (energy_req) {
            msg->DC_EVChargeParameter.EVEnergyRequest_isUsed = 1;
            json_to_din_physical_value(energy_req, &msg->DC_EVChargeParameter.EVEnergyRequest);
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

static cJSON* din_charge_parameter_discovery_req_to_json(const struct din_ChargeParameterDiscoveryReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "EVRequestedEnergyTransferType",
                            din_energy_transfer_type_to_string(msg->EVRequestedEnergyTransferType));

    if (msg->DC_EVChargeParameter_isUsed) {
        cJSON* dc_params = cJSON_CreateObject();
        cJSON_AddItemToObject(dc_params, "DC_EVStatus",
                              din_dc_ev_status_to_json(&msg->DC_EVChargeParameter.DC_EVStatus));
        cJSON_AddItemToObject(dc_params, "EVMaximumCurrentLimit",
                              din_physical_value_to_json(&msg->DC_EVChargeParameter.EVMaximumCurrentLimit));
        cJSON_AddItemToObject(dc_params, "EVMaximumVoltageLimit",
                              din_physical_value_to_json(&msg->DC_EVChargeParameter.EVMaximumVoltageLimit));

        if (msg->DC_EVChargeParameter.EVMaximumPowerLimit_isUsed) {
            cJSON_AddItemToObject(dc_params, "EVMaximumPowerLimit",
                                  din_physical_value_to_json(&msg->DC_EVChargeParameter.EVMaximumPowerLimit));
        }
        if (msg->DC_EVChargeParameter.EVEnergyCapacity_isUsed) {
            cJSON_AddItemToObject(dc_params, "EVEnergyCapacity",
                                  din_physical_value_to_json(&msg->DC_EVChargeParameter.EVEnergyCapacity));
        }
        if (msg->DC_EVChargeParameter.EVEnergyRequest_isUsed) {
            cJSON_AddItemToObject(dc_params, "EVEnergyRequest",
                                  din_physical_value_to_json(&msg->DC_EVChargeParameter.EVEnergyRequest));
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

/* ChargeParameterDiscoveryRes - simplified */
static int json_to_din_charge_parameter_discovery_res(cJSON* json, struct din_ChargeParameterDiscoveryResType* msg) {
    msg->ResponseCode = din_string_to_response_code(json_get_string(json, "ResponseCode"));
    const char* processing = json_get_string(json, "EVSEProcessing");
    msg->EVSEProcessing =
        (strcmp(processing, "Finished") == 0) ? din_EVSEProcessingType_Finished : din_EVSEProcessingType_Ongoing;

    cJSON* dc_params = cJSON_GetObjectItemCaseSensitive(json, "DC_EVSEChargeParameter");
    if (dc_params) {
        msg->DC_EVSEChargeParameter_isUsed = 1;
        cJSON* status = cJSON_GetObjectItemCaseSensitive(dc_params, "DC_EVSEStatus");
        if (status) {
            json_to_din_dc_evse_status(status, &msg->DC_EVSEChargeParameter.DC_EVSEStatus);
        }

        cJSON* max_current = cJSON_GetObjectItemCaseSensitive(dc_params, "EVSEMaximumCurrentLimit");
        if (max_current) {
            json_to_din_physical_value(max_current, &msg->DC_EVSEChargeParameter.EVSEMaximumCurrentLimit);
        }

        cJSON* max_voltage = cJSON_GetObjectItemCaseSensitive(dc_params, "EVSEMaximumVoltageLimit");
        if (max_voltage) {
            json_to_din_physical_value(max_voltage, &msg->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit);
        }

        cJSON* max_power = cJSON_GetObjectItemCaseSensitive(dc_params, "EVSEMaximumPowerLimit");
        if (max_power) {
            msg->DC_EVSEChargeParameter.EVSEMaximumPowerLimit_isUsed = 1;
            json_to_din_physical_value(max_power, &msg->DC_EVSEChargeParameter.EVSEMaximumPowerLimit);
        }

        cJSON* min_current = cJSON_GetObjectItemCaseSensitive(dc_params, "EVSEMinimumCurrentLimit");
        if (min_current) {
            json_to_din_physical_value(min_current, &msg->DC_EVSEChargeParameter.EVSEMinimumCurrentLimit);
        }

        cJSON* min_voltage = cJSON_GetObjectItemCaseSensitive(dc_params, "EVSEMinimumVoltageLimit");
        if (min_voltage) {
            json_to_din_physical_value(min_voltage, &msg->DC_EVSEChargeParameter.EVSEMinimumVoltageLimit);
        }

        cJSON* peak_current = cJSON_GetObjectItemCaseSensitive(dc_params, "EVSEPeakCurrentRipple");
        if (peak_current) {
            json_to_din_physical_value(peak_current, &msg->DC_EVSEChargeParameter.EVSEPeakCurrentRipple);
        }
    }
    return CBV2G_SUCCESS;
}

static cJSON* din_charge_parameter_discovery_res_to_json(const struct din_ChargeParameterDiscoveryResType* msg) {
    cJSON* json = cJSON_CreateObject();
    if (add_response_code_string(json, msg->ResponseCode) != 0) {
        cJSON_Delete(json);
        return NULL;
    }
    cJSON_AddStringToObject(json, "EVSEProcessing",
                            msg->EVSEProcessing == din_EVSEProcessingType_Finished ? "Finished" : "Ongoing");

    /* SAScheduleList - required for EV to select schedule */
    if (msg->SAScheduleList_isUsed) {
        cJSON_AddItemToObject(json, "SAScheduleList", din_sa_schedule_list_to_json(&msg->SAScheduleList));
    }

    if (msg->DC_EVSEChargeParameter_isUsed) {
        cJSON* dc_params = cJSON_CreateObject();
        cJSON_AddItemToObject(dc_params, "DC_EVSEStatus",
                              din_dc_evse_status_to_json(&msg->DC_EVSEChargeParameter.DC_EVSEStatus));
        cJSON_AddItemToObject(dc_params, "EVSEMaximumCurrentLimit",
                              din_physical_value_to_json(&msg->DC_EVSEChargeParameter.EVSEMaximumCurrentLimit));
        cJSON_AddItemToObject(dc_params, "EVSEMaximumVoltageLimit",
                              din_physical_value_to_json(&msg->DC_EVSEChargeParameter.EVSEMaximumVoltageLimit));
        if (msg->DC_EVSEChargeParameter.EVSEMaximumPowerLimit_isUsed) {
            cJSON_AddItemToObject(dc_params, "EVSEMaximumPowerLimit",
                                  din_physical_value_to_json(&msg->DC_EVSEChargeParameter.EVSEMaximumPowerLimit));
        }
        cJSON_AddItemToObject(dc_params, "EVSEMinimumCurrentLimit",
                              din_physical_value_to_json(&msg->DC_EVSEChargeParameter.EVSEMinimumCurrentLimit));
        cJSON_AddItemToObject(dc_params, "EVSEMinimumVoltageLimit",
                              din_physical_value_to_json(&msg->DC_EVSEChargeParameter.EVSEMinimumVoltageLimit));
        cJSON_AddItemToObject(dc_params, "EVSEPeakCurrentRipple",
                              din_physical_value_to_json(&msg->DC_EVSEChargeParameter.EVSEPeakCurrentRipple));
        cJSON_AddItemToObject(json, "DC_EVSEChargeParameter", dc_params);
    }
    return json;
}

/* CableCheckReq */
static int json_to_din_cable_check_req(cJSON* json, struct din_CableCheckReqType* msg) {
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVStatus");
    if (status) {
        json_to_din_dc_ev_status(status, &msg->DC_EVStatus);
    }
    return CBV2G_SUCCESS;
}

static cJSON* din_cable_check_req_to_json(const struct din_CableCheckReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "DC_EVStatus", din_dc_ev_status_to_json(&msg->DC_EVStatus));
    return json;
}

/* CableCheckRes */
static int json_to_din_cable_check_res(cJSON* json, struct din_CableCheckResType* msg) {
    msg->ResponseCode = din_string_to_response_code(json_get_string(json, "ResponseCode"));
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVSEStatus");
    if (status) {
        json_to_din_dc_evse_status(status, &msg->DC_EVSEStatus);
    }
    const char* processing = json_get_string(json, "EVSEProcessing");
    msg->EVSEProcessing =
        (strcmp(processing, "Finished") == 0) ? din_EVSEProcessingType_Finished : din_EVSEProcessingType_Ongoing;
    return CBV2G_SUCCESS;
}

static cJSON* din_cable_check_res_to_json(const struct din_CableCheckResType* msg) {
    cJSON* json = cJSON_CreateObject();
    if (add_response_code_string(json, msg->ResponseCode) != 0) {
        cJSON_Delete(json);
        return NULL;
    }
    cJSON_AddItemToObject(json, "DC_EVSEStatus", din_dc_evse_status_to_json(&msg->DC_EVSEStatus));
    cJSON_AddStringToObject(json, "EVSEProcessing",
                            msg->EVSEProcessing == din_EVSEProcessingType_Finished ? "Finished" : "Ongoing");
    return json;
}

/* PreChargeReq */
static int json_to_din_pre_charge_req(cJSON* json, struct din_PreChargeReqType* msg) {
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVStatus");
    if (status) {
        json_to_din_dc_ev_status(status, &msg->DC_EVStatus);
    }

    cJSON* target_voltage = cJSON_GetObjectItemCaseSensitive(json, "EVTargetVoltage");
    if (target_voltage) {
        json_to_din_physical_value(target_voltage, &msg->EVTargetVoltage);
    }

    cJSON* target_current = cJSON_GetObjectItemCaseSensitive(json, "EVTargetCurrent");
    if (target_current) {
        json_to_din_physical_value(target_current, &msg->EVTargetCurrent);
    }
    return CBV2G_SUCCESS;
}

static cJSON* din_pre_charge_req_to_json(const struct din_PreChargeReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "DC_EVStatus", din_dc_ev_status_to_json(&msg->DC_EVStatus));
    cJSON_AddItemToObject(json, "EVTargetVoltage", din_physical_value_to_json(&msg->EVTargetVoltage));
    cJSON_AddItemToObject(json, "EVTargetCurrent", din_physical_value_to_json(&msg->EVTargetCurrent));
    return json;
}

/* PreChargeRes */
static int json_to_din_pre_charge_res(cJSON* json, struct din_PreChargeResType* msg) {
    msg->ResponseCode = din_string_to_response_code(json_get_string(json, "ResponseCode"));
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVSEStatus");
    if (status) {
        json_to_din_dc_evse_status(status, &msg->DC_EVSEStatus);
    }

    cJSON* present_voltage = cJSON_GetObjectItemCaseSensitive(json, "EVSEPresentVoltage");
    if (present_voltage) {
        json_to_din_physical_value(present_voltage, &msg->EVSEPresentVoltage);
    }
    return CBV2G_SUCCESS;
}

static cJSON* din_pre_charge_res_to_json(const struct din_PreChargeResType* msg) {
    cJSON* json = cJSON_CreateObject();
    if (add_response_code_string(json, msg->ResponseCode) != 0) {
        cJSON_Delete(json);
        return NULL;
    }
    cJSON_AddItemToObject(json, "DC_EVSEStatus", din_dc_evse_status_to_json(&msg->DC_EVSEStatus));
    cJSON_AddItemToObject(json, "EVSEPresentVoltage", din_physical_value_to_json(&msg->EVSEPresentVoltage));
    return json;
}

/* PowerDeliveryReq */
static int json_to_din_power_delivery_req(cJSON* json, struct din_PowerDeliveryReqType* msg) {
    msg->ReadyToChargeState = json_get_bool(json, "ReadyToChargeState");

    msg->ChargingProfile_isUsed = json_has_key(json, "ChargingProfile");
    /* ChargingProfile parsing would go here if needed */

    cJSON* dc_params = cJSON_GetObjectItemCaseSensitive(json, "DC_EVPowerDeliveryParameter");
    if (dc_params) {
        msg->DC_EVPowerDeliveryParameter_isUsed = 1;
        cJSON* status = cJSON_GetObjectItemCaseSensitive(dc_params, "DC_EVStatus");
        if (status) {
            json_to_din_dc_ev_status(status, &msg->DC_EVPowerDeliveryParameter.DC_EVStatus);
        }
        msg->DC_EVPowerDeliveryParameter.BulkChargingComplete_isUsed = json_has_key(dc_params, "BulkChargingComplete");
        if (msg->DC_EVPowerDeliveryParameter.BulkChargingComplete_isUsed) {
            msg->DC_EVPowerDeliveryParameter.BulkChargingComplete = json_get_bool(dc_params, "BulkChargingComplete");
        }
        msg->DC_EVPowerDeliveryParameter.ChargingComplete = json_get_bool(dc_params, "ChargingComplete");
    }
    return CBV2G_SUCCESS;
}

static cJSON* din_power_delivery_req_to_json(const struct din_PowerDeliveryReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddBoolToObject(json, "ReadyToChargeState", msg->ReadyToChargeState);

    if (msg->DC_EVPowerDeliveryParameter_isUsed) {
        cJSON* dc_params = cJSON_CreateObject();
        cJSON_AddItemToObject(dc_params, "DC_EVStatus",
                              din_dc_ev_status_to_json(&msg->DC_EVPowerDeliveryParameter.DC_EVStatus));
        if (msg->DC_EVPowerDeliveryParameter.BulkChargingComplete_isUsed) {
            cJSON_AddBoolToObject(dc_params, "BulkChargingComplete",
                                  msg->DC_EVPowerDeliveryParameter.BulkChargingComplete);
        }
        cJSON_AddBoolToObject(dc_params, "ChargingComplete", msg->DC_EVPowerDeliveryParameter.ChargingComplete);
        cJSON_AddItemToObject(json, "DC_EVPowerDeliveryParameter", dc_params);
    }
    return json;
}

/* PowerDeliveryRes */
static int json_to_din_power_delivery_res(cJSON* json, struct din_PowerDeliveryResType* msg) {
    msg->ResponseCode = din_string_to_response_code(json_get_string(json, "ResponseCode"));
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVSEStatus");
    if (status) {
        msg->DC_EVSEStatus_isUsed = 1;
        json_to_din_dc_evse_status(status, &msg->DC_EVSEStatus);
    }
    return CBV2G_SUCCESS;
}

static cJSON* din_power_delivery_res_to_json(const struct din_PowerDeliveryResType* msg) {
    cJSON* json = cJSON_CreateObject();
    if (add_response_code_string(json, msg->ResponseCode) != 0) {
        cJSON_Delete(json);
        return NULL;
    }
    if (msg->DC_EVSEStatus_isUsed) {
        cJSON_AddItemToObject(json, "DC_EVSEStatus", din_dc_evse_status_to_json(&msg->DC_EVSEStatus));
    }
    return json;
}

/* CurrentDemandReq */
static int json_to_din_current_demand_req(cJSON* json, struct din_CurrentDemandReqType* msg) {
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVStatus");
    if (status) {
        json_to_din_dc_ev_status(status, &msg->DC_EVStatus);
    }

    cJSON* target_current = cJSON_GetObjectItemCaseSensitive(json, "EVTargetCurrent");
    if (target_current) {
        json_to_din_physical_value(target_current, &msg->EVTargetCurrent);
    }

    msg->EVMaximumVoltageLimit_isUsed = json_has_key(json, "EVMaximumVoltageLimit");
    if (msg->EVMaximumVoltageLimit_isUsed) {
        cJSON* max_voltage = cJSON_GetObjectItemCaseSensitive(json, "EVMaximumVoltageLimit");
        json_to_din_physical_value(max_voltage, &msg->EVMaximumVoltageLimit);
    }

    msg->EVMaximumCurrentLimit_isUsed = json_has_key(json, "EVMaximumCurrentLimit");
    if (msg->EVMaximumCurrentLimit_isUsed) {
        cJSON* max_current = cJSON_GetObjectItemCaseSensitive(json, "EVMaximumCurrentLimit");
        json_to_din_physical_value(max_current, &msg->EVMaximumCurrentLimit);
    }

    msg->EVMaximumPowerLimit_isUsed = json_has_key(json, "EVMaximumPowerLimit");
    if (msg->EVMaximumPowerLimit_isUsed) {
        cJSON* max_power = cJSON_GetObjectItemCaseSensitive(json, "EVMaximumPowerLimit");
        json_to_din_physical_value(max_power, &msg->EVMaximumPowerLimit);
    }

    msg->BulkChargingComplete_isUsed = json_has_key(json, "BulkChargingComplete");
    if (msg->BulkChargingComplete_isUsed) {
        msg->BulkChargingComplete = json_get_bool(json, "BulkChargingComplete");
    }

    msg->ChargingComplete = json_get_bool(json, "ChargingComplete");

    msg->RemainingTimeToFullSoC_isUsed = json_has_key(json, "RemainingTimeToFullSoC");
    if (msg->RemainingTimeToFullSoC_isUsed) {
        cJSON* time = cJSON_GetObjectItemCaseSensitive(json, "RemainingTimeToFullSoC");
        json_to_din_physical_value(time, &msg->RemainingTimeToFullSoC);
    }

    msg->RemainingTimeToBulkSoC_isUsed = json_has_key(json, "RemainingTimeToBulkSoC");
    if (msg->RemainingTimeToBulkSoC_isUsed) {
        cJSON* time = cJSON_GetObjectItemCaseSensitive(json, "RemainingTimeToBulkSoC");
        json_to_din_physical_value(time, &msg->RemainingTimeToBulkSoC);
    }

    cJSON* target_voltage = cJSON_GetObjectItemCaseSensitive(json, "EVTargetVoltage");
    if (target_voltage) {
        json_to_din_physical_value(target_voltage, &msg->EVTargetVoltage);
    }

    return CBV2G_SUCCESS;
}

static cJSON* din_current_demand_req_to_json(const struct din_CurrentDemandReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "DC_EVStatus", din_dc_ev_status_to_json(&msg->DC_EVStatus));
    cJSON_AddItemToObject(json, "EVTargetCurrent", din_physical_value_to_json(&msg->EVTargetCurrent));

    if (msg->EVMaximumVoltageLimit_isUsed) {
        cJSON_AddItemToObject(json, "EVMaximumVoltageLimit", din_physical_value_to_json(&msg->EVMaximumVoltageLimit));
    }
    if (msg->EVMaximumCurrentLimit_isUsed) {
        cJSON_AddItemToObject(json, "EVMaximumCurrentLimit", din_physical_value_to_json(&msg->EVMaximumCurrentLimit));
    }
    if (msg->EVMaximumPowerLimit_isUsed) {
        cJSON_AddItemToObject(json, "EVMaximumPowerLimit", din_physical_value_to_json(&msg->EVMaximumPowerLimit));
    }
    if (msg->BulkChargingComplete_isUsed) {
        cJSON_AddBoolToObject(json, "BulkChargingComplete", msg->BulkChargingComplete);
    }
    cJSON_AddBoolToObject(json, "ChargingComplete", msg->ChargingComplete);
    if (msg->RemainingTimeToFullSoC_isUsed) {
        cJSON_AddItemToObject(json, "RemainingTimeToFullSoC", din_physical_value_to_json(&msg->RemainingTimeToFullSoC));
    }
    if (msg->RemainingTimeToBulkSoC_isUsed) {
        cJSON_AddItemToObject(json, "RemainingTimeToBulkSoC", din_physical_value_to_json(&msg->RemainingTimeToBulkSoC));
    }
    cJSON_AddItemToObject(json, "EVTargetVoltage", din_physical_value_to_json(&msg->EVTargetVoltage));
    return json;
}

/* CurrentDemandRes */
static int json_to_din_current_demand_res(cJSON* json, struct din_CurrentDemandResType* msg) {
    msg->ResponseCode = din_string_to_response_code(json_get_string(json, "ResponseCode"));

    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVSEStatus");
    if (status) {
        json_to_din_dc_evse_status(status, &msg->DC_EVSEStatus);
    }

    cJSON* present_voltage = cJSON_GetObjectItemCaseSensitive(json, "EVSEPresentVoltage");
    if (present_voltage) {
        json_to_din_physical_value(present_voltage, &msg->EVSEPresentVoltage);
    }

    cJSON* present_current = cJSON_GetObjectItemCaseSensitive(json, "EVSEPresentCurrent");
    if (present_current) {
        json_to_din_physical_value(present_current, &msg->EVSEPresentCurrent);
    }

    msg->EVSECurrentLimitAchieved = json_get_bool(json, "EVSECurrentLimitAchieved");
    msg->EVSEVoltageLimitAchieved = json_get_bool(json, "EVSEVoltageLimitAchieved");
    msg->EVSEPowerLimitAchieved = json_get_bool(json, "EVSEPowerLimitAchieved");

    msg->EVSEMaximumVoltageLimit_isUsed = json_has_key(json, "EVSEMaximumVoltageLimit");
    if (msg->EVSEMaximumVoltageLimit_isUsed) {
        cJSON* max_voltage = cJSON_GetObjectItemCaseSensitive(json, "EVSEMaximumVoltageLimit");
        json_to_din_physical_value(max_voltage, &msg->EVSEMaximumVoltageLimit);
    }

    msg->EVSEMaximumCurrentLimit_isUsed = json_has_key(json, "EVSEMaximumCurrentLimit");
    if (msg->EVSEMaximumCurrentLimit_isUsed) {
        cJSON* max_current = cJSON_GetObjectItemCaseSensitive(json, "EVSEMaximumCurrentLimit");
        json_to_din_physical_value(max_current, &msg->EVSEMaximumCurrentLimit);
    }

    msg->EVSEMaximumPowerLimit_isUsed = json_has_key(json, "EVSEMaximumPowerLimit");
    if (msg->EVSEMaximumPowerLimit_isUsed) {
        cJSON* max_power = cJSON_GetObjectItemCaseSensitive(json, "EVSEMaximumPowerLimit");
        json_to_din_physical_value(max_power, &msg->EVSEMaximumPowerLimit);
    }

    return CBV2G_SUCCESS;
}

static cJSON* din_current_demand_res_to_json(const struct din_CurrentDemandResType* msg) {
    cJSON* json = cJSON_CreateObject();
    if (add_response_code_string(json, msg->ResponseCode) != 0) {
        cJSON_Delete(json);
        return NULL;
    }
    cJSON_AddItemToObject(json, "DC_EVSEStatus", din_dc_evse_status_to_json(&msg->DC_EVSEStatus));
    cJSON_AddItemToObject(json, "EVSEPresentVoltage", din_physical_value_to_json(&msg->EVSEPresentVoltage));
    cJSON_AddItemToObject(json, "EVSEPresentCurrent", din_physical_value_to_json(&msg->EVSEPresentCurrent));
    cJSON_AddBoolToObject(json, "EVSECurrentLimitAchieved", msg->EVSECurrentLimitAchieved);
    cJSON_AddBoolToObject(json, "EVSEVoltageLimitAchieved", msg->EVSEVoltageLimitAchieved);
    cJSON_AddBoolToObject(json, "EVSEPowerLimitAchieved", msg->EVSEPowerLimitAchieved);

    if (msg->EVSEMaximumVoltageLimit_isUsed) {
        cJSON_AddItemToObject(json, "EVSEMaximumVoltageLimit",
                              din_physical_value_to_json(&msg->EVSEMaximumVoltageLimit));
    }
    if (msg->EVSEMaximumCurrentLimit_isUsed) {
        cJSON_AddItemToObject(json, "EVSEMaximumCurrentLimit",
                              din_physical_value_to_json(&msg->EVSEMaximumCurrentLimit));
    }
    if (msg->EVSEMaximumPowerLimit_isUsed) {
        cJSON_AddItemToObject(json, "EVSEMaximumPowerLimit", din_physical_value_to_json(&msg->EVSEMaximumPowerLimit));
    }
    return json;
}

/* WeldingDetectionReq */
static int json_to_din_welding_detection_req(cJSON* json, struct din_WeldingDetectionReqType* msg) {
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVStatus");
    if (status) {
        json_to_din_dc_ev_status(status, &msg->DC_EVStatus);
    }
    return CBV2G_SUCCESS;
}

static cJSON* din_welding_detection_req_to_json(const struct din_WeldingDetectionReqType* msg) {
    cJSON* json = cJSON_CreateObject();
    cJSON_AddItemToObject(json, "DC_EVStatus", din_dc_ev_status_to_json(&msg->DC_EVStatus));
    return json;
}

/* WeldingDetectionRes */
static int json_to_din_welding_detection_res(cJSON* json, struct din_WeldingDetectionResType* msg) {
    msg->ResponseCode = din_string_to_response_code(json_get_string(json, "ResponseCode"));
    cJSON* status = cJSON_GetObjectItemCaseSensitive(json, "DC_EVSEStatus");
    if (status) {
        json_to_din_dc_evse_status(status, &msg->DC_EVSEStatus);
    }

    cJSON* present_voltage = cJSON_GetObjectItemCaseSensitive(json, "EVSEPresentVoltage");
    if (present_voltage) {
        json_to_din_physical_value(present_voltage, &msg->EVSEPresentVoltage);
    }
    return CBV2G_SUCCESS;
}

static cJSON* din_welding_detection_res_to_json(const struct din_WeldingDetectionResType* msg) {
    cJSON* json = cJSON_CreateObject();
    if (add_response_code_string(json, msg->ResponseCode) != 0) {
        cJSON_Delete(json);
        return NULL;
    }
    cJSON_AddItemToObject(json, "DC_EVSEStatus", din_dc_evse_status_to_json(&msg->DC_EVSEStatus));
    cJSON_AddItemToObject(json, "EVSEPresentVoltage", din_physical_value_to_json(&msg->EVSEPresentVoltage));
    return json;
}

/* SessionStopReq */
static int json_to_din_session_stop_req(cJSON* json, struct din_SessionStopType* msg) {
    (void)json;
    (void)msg;
    /* SessionStopReq has no fields */
    return CBV2G_SUCCESS;
}

static cJSON* din_session_stop_req_to_json(const struct din_SessionStopType* msg) {
    (void)msg;
    return cJSON_CreateObject();
}

/* SessionStopRes */
static int json_to_din_session_stop_res(cJSON* json, struct din_SessionStopResType* msg) {
    msg->ResponseCode = din_string_to_response_code(json_get_string(json, "ResponseCode"));
    return CBV2G_SUCCESS;
}

static cJSON* din_session_stop_res_to_json(const struct din_SessionStopResType* msg) {
    cJSON* json = cJSON_CreateObject();
    if (add_response_code_string(json, msg->ResponseCode) != 0) {
        cJSON_Delete(json);
        return NULL;
    }
    return json;
}
