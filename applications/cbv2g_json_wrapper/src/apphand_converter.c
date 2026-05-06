/*
 * SPDX-License-Identifier: Apache-2.0
 * Copyright 2026 Pionix GmbH and Contributors to EVerest
 *
 * apphand_converter.c - App Handshake (SAP) JSON/EXI converter
 */

#include "converters.h"
#include "json_utils.h"
#include "cJSON.h"

#include <cbv2g/common/exi_bitstream.h>
#include <cbv2g/app_handshake/appHand_Datatypes.h>
#include <cbv2g/app_handshake/appHand_Encoder.h>
#include <cbv2g/app_handshake/appHand_Decoder.h>

#include <string.h>
#include <stdio.h>

/* Forward declarations */
static int json_to_apphand_req(cJSON* json, struct appHand_supportedAppProtocolReq* req);
static int json_to_apphand_res(cJSON* json, struct appHand_supportedAppProtocolRes* res);
static cJSON* apphand_req_to_json(const struct appHand_supportedAppProtocolReq* req);
static cJSON* apphand_res_to_json(const struct appHand_supportedAppProtocolRes* res);

/*
 * Encode App Handshake message from JSON to EXI
 */
int apphand_encode(const char* json_str, uint8_t* out, size_t out_size, size_t* out_len) {
    cJSON* root = cJSON_Parse(json_str);
    if (root == NULL) {
        set_error("Failed to parse JSON: %s", cJSON_GetErrorPtr());
        return CBV2G_ERROR_JSON_PARSE;
    }

    struct appHand_exiDocument doc;
    init_appHand_exiDocument(&doc);

    /* Determine message type and convert. The two variants are mutually
     * exclusive in valid SAP traffic; we check both keys explicitly so the
     * "unknown message" branch is reached on neither. */
    cJSON* req = cJSON_GetObjectItemCaseSensitive(root, "supportedAppProtocolReq");
    cJSON* res = cJSON_GetObjectItemCaseSensitive(root, "supportedAppProtocolRes");

    if (req != NULL) {
        doc.supportedAppProtocolReq_isUsed = 1;
        int rc = json_to_apphand_req(req, &doc.supportedAppProtocolReq);
        if (rc != CBV2G_SUCCESS) {
            cJSON_Delete(root);
            return rc;
        }
    } else if (res != NULL) {
        doc.supportedAppProtocolRes_isUsed = 1;
        int rc = json_to_apphand_res(res, &doc.supportedAppProtocolRes);
        if (rc != CBV2G_SUCCESS) {
            cJSON_Delete(root);
            return rc;
        }
    } else {
        set_error("Unknown App Handshake message type");
        cJSON_Delete(root);
        return CBV2G_ERROR_UNKNOWN_MESSAGE;
    }

    exi_bitstream_t stream;
    exi_bitstream_init(&stream, out, out_size, 0, NULL);

    int exi_result = encode_appHand_exiDocument(&stream, &doc);
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
 * Decode App Handshake message from EXI to JSON
 */
int apphand_decode(const uint8_t* exi, size_t exi_len, char* out, size_t out_size) {
    exi_bitstream_t stream;
    exi_bitstream_init(&stream, (uint8_t*)exi, exi_len, 0, NULL);

    struct appHand_exiDocument doc;
    init_appHand_exiDocument(&doc);

    int exi_result = decode_appHand_exiDocument(&stream, &doc);
    if (exi_result != 0) {
        set_error("EXI decoding failed with error code: %d", exi_result);
        return CBV2G_ERROR_DECODING_FAILED;
    }

    cJSON* json = cJSON_CreateObject();
    if (json == NULL) {
        set_error("Failed to create JSON object");
        return CBV2G_ERROR_JSON_GENERATE;
    }

    if (doc.supportedAppProtocolReq_isUsed) {
        cJSON* req_json = apphand_req_to_json(&doc.supportedAppProtocolReq);
        if (req_json == NULL) {
            set_error("Failed to convert supportedAppProtocolReq to JSON");
            cJSON_Delete(json);
            return CBV2G_ERROR_JSON_GENERATE;
        }
        cJSON_AddItemToObject(json, "supportedAppProtocolReq", req_json);
    } else if (doc.supportedAppProtocolRes_isUsed) {
        cJSON* res_json = apphand_res_to_json(&doc.supportedAppProtocolRes);
        if (res_json == NULL) {
            set_error("Failed to convert supportedAppProtocolRes to JSON");
            cJSON_Delete(json);
            return CBV2G_ERROR_JSON_GENERATE;
        }
        cJSON_AddItemToObject(json, "supportedAppProtocolRes", res_json);
    } else {
        set_error("No valid message found in decoded document");
        cJSON_Delete(json);
        return CBV2G_ERROR_DECODING_FAILED;
    }

    char* json_str = cJSON_PrintUnformatted(json);
    if (json_str == NULL) {
        set_error("Failed to serialize JSON");
        cJSON_Delete(json);
        return CBV2G_ERROR_JSON_GENERATE;
    }

    /* Bounded write to caller's buffer (CWE-120 / CWE-126). */
    int written = snprintf(out, out_size, "%s", json_str);
    cJSON_free(json_str);
    cJSON_Delete(json);

    if (written < 0 || (size_t)written >= out_size) {
        set_error("Output buffer too small: need %d, have %zu", written + 1, out_size);
        return CBV2G_ERROR_BUFFER_TOO_SMALL;
    }
    return CBV2G_SUCCESS;
}

/*
 * Convert JSON to supportedAppProtocolReq struct
 */
static int json_to_apphand_req(cJSON* json, struct appHand_supportedAppProtocolReq* req) {
    init_appHand_supportedAppProtocolReq(req);

    cJSON* app_protocol = cJSON_GetObjectItemCaseSensitive(json, "AppProtocol");
    if (app_protocol == NULL || !cJSON_IsArray(app_protocol)) {
        set_error("AppProtocol array not found or not an array");
        return CBV2G_ERROR_JSON_PARSE;
    }

    int count = cJSON_GetArraySize(app_protocol);
    if (count > appHand_AppProtocolType_5_ARRAY_SIZE) {
        count = appHand_AppProtocolType_5_ARRAY_SIZE;
    }
    req->AppProtocol.arrayLen = count;

    for (int i = 0; i < count; i++) {
        cJSON* item = cJSON_GetArrayItem(app_protocol, i);
        struct appHand_AppProtocolType* proto = &req->AppProtocol.array[i];
        init_appHand_AppProtocolType(proto);

        /* ProtocolNamespace - snprintf with %.*s is bounded and always
         * null-terminates (CWE-120 / CWE-126). */
        const char* ns = json_get_string(item, "ProtocolNamespace");
        size_t ns_len = strnlen(ns, appHand_ProtocolNamespace_CHARACTER_SIZE);
        int ns_written = snprintf(proto->ProtocolNamespace.characters,
                                  appHand_ProtocolNamespace_CHARACTER_SIZE,
                                  "%.*s", (int)ns_len, ns);
        if (ns_written < 0) {
            ns_written = 0;
        }
        if ((size_t)ns_written >= appHand_ProtocolNamespace_CHARACTER_SIZE) {
            ns_written = appHand_ProtocolNamespace_CHARACTER_SIZE - 1;
        }
        proto->ProtocolNamespace.charactersLen = (size_t)ns_written;

        /* Version numbers */
        proto->VersionNumberMajor = json_get_int(item, "VersionNumberMajor");
        proto->VersionNumberMinor = json_get_int(item, "VersionNumberMinor");

        /* SchemaID and Priority */
        proto->SchemaID = json_get_int(item, "SchemaID");
        proto->Priority = json_get_int(item, "Priority");
    }

    return CBV2G_SUCCESS;
}

/*
 * Convert JSON to supportedAppProtocolRes struct
 */
static int json_to_apphand_res(cJSON* json, struct appHand_supportedAppProtocolRes* res) {
    init_appHand_supportedAppProtocolRes(res);

    /* ResponseCode accepts either the appHand enum's symbolic name (the
     * canonical form Josev uses) or its numeric value. Anything else is
     * a hard error — falling through to a default would silently coerce
     * an unknown / misspelled code to OK_SuccessfulNegotiation (=0). */
    cJSON* rc_item = cJSON_GetObjectItemCaseSensitive(json, "ResponseCode");
    if (cJSON_IsString(rc_item) && rc_item->valuestring != NULL) {
        const char* response_code = rc_item->valuestring;
        /* Bounded compare on potentially non-null-terminated input (CWE-126). */
        if (strncmp(response_code, "OK_SuccessfulNegotiation",
                    sizeof("OK_SuccessfulNegotiation")) == 0) {
            res->ResponseCode = appHand_responseCodeType_OK_SuccessfulNegotiation;
        } else if (strncmp(response_code, "OK_SuccessfulNegotiationWithMinorDeviation",
                           sizeof("OK_SuccessfulNegotiationWithMinorDeviation")) == 0) {
            res->ResponseCode = appHand_responseCodeType_OK_SuccessfulNegotiationWithMinorDeviation;
        } else if (strncmp(response_code, "Failed_NoNegotiation",
                           sizeof("Failed_NoNegotiation")) == 0) {
            res->ResponseCode = appHand_responseCodeType_Failed_NoNegotiation;
        } else {
            set_error("Unknown ResponseCode string: '%s'", response_code);
            return CBV2G_ERROR_JSON_PARSE;
        }
    } else if (cJSON_IsNumber(rc_item)) {
        res->ResponseCode = rc_item->valueint;
    } else {
        set_error("ResponseCode missing or has invalid type");
        return CBV2G_ERROR_JSON_PARSE;
    }

    /* SchemaID (optional) */
    if (json_has_key(json, "SchemaID")) {
        res->SchemaID = json_get_int(json, "SchemaID");
        res->SchemaID_isUsed = 1;
    }

    return CBV2G_SUCCESS;
}

/*
 * Convert supportedAppProtocolReq struct to JSON
 */
static cJSON* apphand_req_to_json(const struct appHand_supportedAppProtocolReq* req) {
    cJSON* json = cJSON_CreateObject();
    if (json == NULL) {
        return NULL;
    }

    cJSON* app_protocol = cJSON_CreateArray();
    if (app_protocol == NULL) {
        cJSON_Delete(json);
        return NULL;
    }

    for (int i = 0; i < req->AppProtocol.arrayLen; i++) {
        const struct appHand_AppProtocolType* proto = &req->AppProtocol.array[i];

        cJSON* item = cJSON_CreateObject();
        if (item == NULL) {
            cJSON_Delete(app_protocol);
            cJSON_Delete(json);
            return NULL;
        }

        /* Create null-terminated string for ProtocolNamespace using
         * snprintf for a bounded copy (CWE-120). */
        char ns[appHand_ProtocolNamespace_CHARACTER_SIZE + 1];
        size_t ns_len = proto->ProtocolNamespace.charactersLen;
        if (ns_len > appHand_ProtocolNamespace_CHARACTER_SIZE) {
            ns_len = appHand_ProtocolNamespace_CHARACTER_SIZE;
        }
        snprintf(ns, sizeof(ns), "%.*s", (int)ns_len,
                 proto->ProtocolNamespace.characters);

        cJSON_AddStringToObject(item, "ProtocolNamespace", ns);
        cJSON_AddNumberToObject(item, "VersionNumberMajor", proto->VersionNumberMajor);
        cJSON_AddNumberToObject(item, "VersionNumberMinor", proto->VersionNumberMinor);
        cJSON_AddNumberToObject(item, "SchemaID", proto->SchemaID);
        cJSON_AddNumberToObject(item, "Priority", proto->Priority);

        cJSON_AddItemToArray(app_protocol, item);
    }

    cJSON_AddItemToObject(json, "AppProtocol", app_protocol);
    return json;
}

/*
 * Convert supportedAppProtocolRes struct to JSON
 */
static cJSON* apphand_res_to_json(const struct appHand_supportedAppProtocolRes* res) {
    cJSON* json = cJSON_CreateObject();
    if (json == NULL) {
        return NULL;
    }

    const char* response_code_str;
    switch (res->ResponseCode) {
        case appHand_responseCodeType_OK_SuccessfulNegotiation:
            response_code_str = "OK_SuccessfulNegotiation";
            break;
        case appHand_responseCodeType_OK_SuccessfulNegotiationWithMinorDeviation:
            response_code_str = "OK_SuccessfulNegotiationWithMinorDeviation";
            break;
        case appHand_responseCodeType_Failed_NoNegotiation:
            response_code_str = "Failed_NoNegotiation";
            break;
        default:
            /* An unknown ResponseCode value in a decoded document indicates
             * either a corrupt EXI stream or a schema we don't recognise;
             * surface it instead of emitting a placeholder string. */
            set_error("Unknown ResponseCode value in decoded document: %d",
                      res->ResponseCode);
            cJSON_Delete(json);
            return NULL;
    }
    cJSON_AddStringToObject(json, "ResponseCode", response_code_str);

    /* SchemaID (optional) */
    if (res->SchemaID_isUsed) {
        cJSON_AddNumberToObject(json, "SchemaID", res->SchemaID);
    }

    return json;
}
