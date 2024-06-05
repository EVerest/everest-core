// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2023 chargebyte GmbH
// Copyright (C) 2023 Contributors to EVerest

#include <inttypes.h>
#include <mbedtls/base64.h>
#include <openv2g/appHandEXIDatatypesDecoder.h>
#include <openv2g/appHandEXIDatatypesEncoder.h>
#include <openv2g/dinEXIDatatypes.h>
#include <openv2g/dinEXIDatatypesDecoder.h>
#include <openv2g/dinEXIDatatypesEncoder.h>
#include <openv2g/iso1EXIDatatypes.h>
#include <openv2g/iso1EXIDatatypesDecoder.h>
#include <openv2g/iso1EXIDatatypesEncoder.h>
#include <openv2g/v2gtp.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "connection.hpp"
#include "din_server.hpp"
#include "iso_server.hpp"
#include "log.hpp"
#include "tools.hpp"
#include "v2g_server.hpp"

#define MAX_RES_TIME 98
#define UINT32_MAX   0xFFFFFFFF

static types::iso15118_charger::V2G_Message_ID get_V2G_Message_ID(enum V2gMsgTypeId v2g_msg,
                                                                  enum v2g_protocol selected_protocol, bool is_req) {
    switch (v2g_msg) {
    case V2G_SUPPORTED_APP_PROTOCOL_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::SupportedAppProtocolReq
                              : types::iso15118_charger::V2G_Message_ID::SupportedAppProtocolRes;
    case V2G_SESSION_SETUP_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::SessionSetupReq
                              : types::iso15118_charger::V2G_Message_ID::SessionSetupRes;
    case V2G_SERVICE_DISCOVERY_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::ServiceDiscoveryReq
                              : types::iso15118_charger::V2G_Message_ID::ServiceDiscoveryRes;
    case V2G_SERVICE_DETAIL_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::ServiceDetailReq
                              : types::iso15118_charger::V2G_Message_ID::ServiceDetailRes;
    case V2G_PAYMENT_SERVICE_SELECTION_MSG:
        return is_req == true ? selected_protocol == V2G_PROTO_DIN70121
                                    ? types::iso15118_charger::V2G_Message_ID::ServicePaymentSelectionReq
                                    : types::iso15118_charger::V2G_Message_ID::PaymentServiceSelectionReq
               : selected_protocol == V2G_PROTO_DIN70121
                   ? types::iso15118_charger::V2G_Message_ID::ServicePaymentSelectionRes
                   : types::iso15118_charger::V2G_Message_ID::PaymentServiceSelectionRes;
    case V2G_PAYMENT_DETAILS_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::PaymentDetailsReq
                              : types::iso15118_charger::V2G_Message_ID::PaymentDetailsRes;
    case V2G_AUTHORIZATION_MSG:
        return is_req == true ? selected_protocol == V2G_PROTO_DIN70121
                                    ? types::iso15118_charger::V2G_Message_ID::ContractAuthenticationReq
                                    : types::iso15118_charger::V2G_Message_ID::AuthorizationReq
               : selected_protocol == V2G_PROTO_DIN70121
                   ? types::iso15118_charger::V2G_Message_ID::ContractAuthenticationRes
                   : types::iso15118_charger::V2G_Message_ID::AuthorizationRes;
    case V2G_CHARGE_PARAMETER_DISCOVERY_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::ChargeParameterDiscoveryReq
                              : types::iso15118_charger::V2G_Message_ID::ChargeParameterDiscoveryRes;
    case V2G_METERING_RECEIPT_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::MeteringReceiptReq
                              : types::iso15118_charger::V2G_Message_ID::MeteringReceiptRes;
    case V2G_CERTIFICATE_UPDATE_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::CertificateUpdateReq
                              : types::iso15118_charger::V2G_Message_ID::CertificateUpdateRes;
    case V2G_CERTIFICATE_INSTALLATION_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::CertificateInstallationReq
                              : types::iso15118_charger::V2G_Message_ID::CertificateInstallationRes;
    case V2G_CHARGING_STATUS_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::ChargingStatusReq
                              : types::iso15118_charger::V2G_Message_ID::ChargingStatusRes;
    case V2G_CABLE_CHECK_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::CableCheckReq
                              : types::iso15118_charger::V2G_Message_ID::CableCheckRes;
    case V2G_PRE_CHARGE_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::PreChargeReq
                              : types::iso15118_charger::V2G_Message_ID::PreChargeRes;
    case V2G_POWER_DELIVERY_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::PowerDeliveryReq
                              : types::iso15118_charger::V2G_Message_ID::PowerDeliveryRes;
    case V2G_CURRENT_DEMAND_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::CurrentDemandReq
                              : types::iso15118_charger::V2G_Message_ID::CurrentDemandRes;
    case V2G_WELDING_DETECTION_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::WeldingDetectionReq
                              : types::iso15118_charger::V2G_Message_ID::WeldingDetectionRes;
    case V2G_SESSION_STOP_MSG:
        return is_req == true ? types::iso15118_charger::V2G_Message_ID::SessionStopReq
                              : types::iso15118_charger::V2G_Message_ID::SessionStopRes;
    }
    return types::iso15118_charger::V2G_Message_ID::UnknownMessage;
}

/*!
 * \brief publish_var_V2G_Message This function fills a V2G_Messages type with the V2G EXI message as HEX and Base64
 * \param conn hold the context of the V2G-connection.
 * \param is_req if it is a V2G request or response: 'true' if a request, and 'false' if a response
 */
static void publish_var_V2G_Message(v2g_connection* conn, bool is_req) {
    types::iso15118_charger::V2G_Messages v2gMessage;

    u_int8_t* tempbuff = conn->buffer;
    std::string msg_as_hex_string;
    for (int i = 0; ((tempbuff != NULL) && (i < conn->payload_len + V2GTP_HEADER_LENGTH)); i++) {
        char hex[4];
        snprintf(hex, 4, "%x", *tempbuff); // to hex
        if (std::string(hex).size() == 1)
            msg_as_hex_string += '0';
        msg_as_hex_string += hex;
        tempbuff++;
    }

    unsigned char* base64_buffer = NULL;
    size_t base64_buffer_len = 0;
    mbedtls_base64_encode(NULL, 0, &base64_buffer_len, conn->buffer, (size_t)conn->payload_len + V2GTP_HEADER_LENGTH);
    base64_buffer = static_cast<unsigned char*>(malloc(base64_buffer_len));
    if ((base64_buffer == NULL) || (mbedtls_base64_encode(base64_buffer, base64_buffer_len, &base64_buffer_len,
                                                          static_cast<unsigned char*>(conn->buffer),
                                                          (size_t)conn->payload_len + V2GTP_HEADER_LENGTH) != 0)) {
        dlog(DLOG_LEVEL_WARNING, "Unable to base64 encode EXI buffer");
    }

    v2gMessage.V2G_Message_EXI_Base64 = std::string(reinterpret_cast<char const*>(base64_buffer), base64_buffer_len);
    if (base64_buffer != NULL) {
        free(base64_buffer);
    }
    v2gMessage.V2G_Message_ID = get_V2G_Message_ID(conn->ctx->current_v2g_msg, conn->ctx->selected_protocol, is_req);
    v2gMessage.V2G_Message_EXI_Hex = msg_as_hex_string;
    conn->ctx->p_charger->publish_V2G_Messages(v2gMessage);
}

/*!
 * \brief v2g_incoming_v2gtp This function reads the V2G transport header
 * \param conn hold the context of the V2G-connection.
 * \return Returns 0 if the V2G-session was successfully stopped, otherwise -1.
 */
static int v2g_incoming_v2gtp(struct v2g_connection* conn) {
    int rv;

    /* read and process header */
    rv = connection_read(conn, conn->buffer, V2GTP_HEADER_LENGTH);
    if (rv < 0) {
        dlog(DLOG_LEVEL_ERROR, "connection_read(header) failed: %s",
             (rv == -1) ? strerror(errno) : "connection terminated");
        return -1;
    }
    /* peer closed connection */
    if (rv == 0)
        return 1;
    if (rv != V2GTP_HEADER_LENGTH) {
        dlog(DLOG_LEVEL_ERROR, "connection_read(header) too short: expected %d, got %d", V2GTP_HEADER_LENGTH, rv);
        return -1;
    }

    rv = read_v2gtpHeader(conn->buffer, &conn->payload_len);
    if (rv == -1) {
        dlog(DLOG_LEVEL_ERROR, "Invalid v2gtp header");
        return -1;
    }

    if (conn->payload_len >= UINT32_MAX - V2GTP_HEADER_LENGTH) {
        dlog(DLOG_LEVEL_ERROR, "Prevent integer overflow - payload too long: have %d, would need %u",
             DEFAULT_BUFFER_SIZE, conn->payload_len);
        return -1;
    }

    if (conn->payload_len + V2GTP_HEADER_LENGTH > DEFAULT_BUFFER_SIZE) {
        dlog(DLOG_LEVEL_ERROR, "payload too long: have %d, would need %u", DEFAULT_BUFFER_SIZE,
             conn->payload_len + V2GTP_HEADER_LENGTH);

        /* we have no way to flush/discard remaining unread data from the socket without reading it in chunks,
         * but this opens the chance to bind us in a "endless" read loop; so to protect us, simply close the connection
         */

        return -1;
    }
    /* read request */
    rv = connection_read(conn, &conn->buffer[V2GTP_HEADER_LENGTH], conn->payload_len);
    if (rv < 0) {
        dlog(DLOG_LEVEL_ERROR, "connection_read(payload) failed: %s",
             (rv == -1) ? strerror(errno) : "connection terminated");
        return -1;
    }
    if (rv != conn->payload_len) {
        dlog(DLOG_LEVEL_ERROR, "connection_read(payload) too short: expected %d, got %d", conn->payload_len, rv);
        return -1;
    }
    /* adjust buffer pos to decode request */
    conn->buffer_pos = V2GTP_HEADER_LENGTH;
    conn->stream.size = conn->payload_len + V2GTP_HEADER_LENGTH;

    return 0;
}

/*!
 * \brief v2g_outgoing_v2gtp This function creates the v2g transport header
 * \param conn hold the context of the v2g-connection.
 * \return Returns 0 if the v2g-session was successfully stopped, otherwise -1.
 */
int v2g_outgoing_v2gtp(struct v2g_connection* conn) {
    /* fixup/create header */
    if (write_v2gtpHeader(conn->buffer, conn->buffer_pos - V2GTP_HEADER_LENGTH, V2GTP_EXI_TYPE) != 0) {
        dlog(DLOG_LEVEL_ERROR, "write_v2gtpHeader() failed");
        return -1;
    }

    if (connection_write(conn, conn->buffer, conn->buffer_pos) == -1) {
        dlog(DLOG_LEVEL_ERROR, "connection_write(header) failed: %s", strerror(errno));
        return -1;
    }

    return 0;
}

/*!
 * \brief v2g_handle_apphandshake After receiving a supportedAppProtocolReq message,
 * the SECC shall process the received information. DIN [V2G-DC-436] ISO [V2G2-540]
 * \param conn hold the context of the v2g-connection.
 * \return Returns a v2g-event of type enum v2g_event.
 */
static enum v2g_event v2g_handle_apphandshake(struct v2g_connection* conn) {
    enum v2g_event next_event = V2G_EVENT_NO_EVENT;
    int i;
    uint8_t ev_app_priority = 20; // lowest priority

    /* validate handshake request and create response */
    init_appHandEXIDocument(&conn->handshake_resp);
    conn->handshake_resp.supportedAppProtocolRes_isUsed = 1;
    conn->handshake_resp.supportedAppProtocolRes.ResponseCode =
        appHandresponseCodeType_Failed_NoNegotiation; // [V2G2-172]

    dlog(DLOG_LEVEL_INFO, "Handling SupportedAppProtocolReq");
    conn->ctx->current_v2g_msg = V2G_SUPPORTED_APP_PROTOCOL_MSG;

    if (decode_appHandExiDocument(&conn->stream, &conn->handshake_req) != 0) {
        dlog(DLOG_LEVEL_ERROR, "decode_appHandExiDocument() failed");
        return V2G_EVENT_TERMINATE_CONNECTION; // If the mesage can't be decoded we have to terminate the tcp-connection
                                               // (e.g. after an unexpected message)
    }

    json appProtocolArray = json::array(); // to publish supported app protocol array

    for (i = 0; i < conn->handshake_req.supportedAppProtocolReq.AppProtocol.arrayLen; i++) {
        struct appHandAppProtocolType* app_proto = &conn->handshake_req.supportedAppProtocolReq.AppProtocol.array[i];
        char* proto_ns = strndup(static_cast<const char*>(app_proto->ProtocolNamespace.characters),
                                 app_proto->ProtocolNamespace.charactersLen);

        if (!proto_ns) {
            dlog(DLOG_LEVEL_ERROR, "out-of-memory condition");
            return V2G_EVENT_TERMINATE_CONNECTION;
        }

        dlog(DLOG_LEVEL_TRACE,
             "handshake_req: Namespace: %s, Version: %" PRIu32 ".%" PRIu32 ", SchemaID: %" PRIu8 ", Priority: %" PRIu8,
             proto_ns, app_proto->VersionNumberMajor, app_proto->VersionNumberMinor, app_proto->SchemaID,
             app_proto->Priority);

        if ((conn->ctx->supported_protocols & (1 << V2G_PROTO_DIN70121)) &&
            (strcmp(proto_ns, DIN_70121_MSG_DEF) == 0) && (app_proto->VersionNumberMajor == DIN_70121_MAJOR) &&
            (ev_app_priority >= app_proto->Priority)) {
            conn->handshake_resp.supportedAppProtocolRes.ResponseCode =
                appHandresponseCodeType_OK_SuccessfulNegotiation;
            ev_app_priority = app_proto->Priority;
            conn->handshake_resp.supportedAppProtocolRes.SchemaID = app_proto->SchemaID;
            conn->ctx->selected_protocol = V2G_PROTO_DIN70121;
        } else if ((conn->ctx->supported_protocols & (1 << V2G_PROTO_ISO15118_2013)) &&
                   (strcmp(proto_ns, ISO_15118_2013_MSG_DEF) == 0) &&
                   (app_proto->VersionNumberMajor == ISO_15118_2013_MAJOR) &&
                   (ev_app_priority >= app_proto->Priority)) {

            conn->handshake_resp.supportedAppProtocolRes.ResponseCode =
                appHandresponseCodeType_OK_SuccessfulNegotiation;
            ev_app_priority = app_proto->Priority;
            conn->handshake_resp.supportedAppProtocolRes.SchemaID = app_proto->SchemaID;
            conn->ctx->selected_protocol = V2G_PROTO_ISO15118_2013;
        }

        if (conn->ctx->debugMode == true) {
            /* Create json array for publishing */
            json appProtocolElement;
            appProtocolElement["ProtocolNamespace"] = std::string(proto_ns);
            appProtocolElement["VersionNumberMajor"] = app_proto->VersionNumberMajor;
            appProtocolElement["VersionNumberMinor"] = app_proto->VersionNumberMinor;
            appProtocolElement["SchemaID"] = app_proto->SchemaID;
            appProtocolElement["Priority"] = app_proto->Priority;
            appProtocolArray.push_back(appProtocolElement);
        }

        // TODO: ISO15118v2
        free(proto_ns);
    }

    if (conn->ctx->debugMode == true) {
        conn->ctx->p_charger->publish_EV_AppProtocol(appProtocolArray);
        /* form the content of V2G_Message type and publish the request*/
        publish_var_V2G_Message(conn, true);
    }

    std::string selected_protocol_str;
    if (conn->handshake_resp.supportedAppProtocolRes.ResponseCode == appHandresponseCodeType_OK_SuccessfulNegotiation) {
        conn->handshake_resp.supportedAppProtocolRes.SchemaID_isUsed = (unsigned int)1;
        if (V2G_PROTO_DIN70121 == conn->ctx->selected_protocol) {
            dlog(DLOG_LEVEL_INFO, "Protocol negotiation was successful. Selected protocol is DIN70121");
            selected_protocol_str = "DIN70121";
        } else if (V2G_PROTO_ISO15118_2013 == conn->ctx->selected_protocol) {
            dlog(DLOG_LEVEL_INFO, "Protocol negotiation was successful. Selected protocol is ISO15118");
            selected_protocol_str = "ISO15118-2-2013";
        } else if (V2G_PROTO_ISO15118_2010 == conn->ctx->selected_protocol) {
            dlog(DLOG_LEVEL_INFO, "Protocol negotiation was successful. Selected protocol is ISO15118-2010");
            selected_protocol_str = "ISO15118-2-2010";
        }
    } else {
        dlog(DLOG_LEVEL_ERROR, "No compatible protocol found");
        selected_protocol_str = "None";
        next_event = V2G_EVENT_SEND_AND_TERMINATE; // Send response and terminate tcp-connection
    }

    if (conn->ctx->debugMode == true) {
        conn->ctx->p_charger->publish_Selected_Protocol(selected_protocol_str);
    }

    if (conn->ctx->is_connection_terminated == true) {
        dlog(DLOG_LEVEL_ERROR, "Connection is terminated. Abort charging");
        return V2G_EVENT_TERMINATE_CONNECTION; // Abort charging without sending a response
    }

    /* Validate response code */
    if ((conn->ctx->intl_emergency_shutdown == true) || (conn->ctx->stop_hlc == true) ||
        (V2G_EVENT_SEND_AND_TERMINATE == next_event)) {
        conn->handshake_resp.supportedAppProtocolRes.ResponseCode = appHandresponseCodeType_Failed_NoNegotiation;
        dlog(DLOG_LEVEL_ERROR, "Abort charging session");

        if (conn->ctx->terminate_connection_on_failed_response == true) {
            next_event = V2G_EVENT_SEND_AND_TERMINATE; // send response and terminate the TCP-connection
        }
    }

    /* encode response at the right buffer location */
    *(conn->stream.pos) = V2GTP_HEADER_LENGTH;
    conn->stream.capacity = 8; // as it should be for send
    conn->stream.buffer = 0;

    if (encode_appHandExiDocument(&conn->stream, &conn->handshake_resp) != 0) {
        dlog(DLOG_LEVEL_ERROR, "Encoding of the protocol handshake message failed");
        next_event = V2G_EVENT_SEND_AND_TERMINATE;
    }

    return next_event;
}

int v2g_handle_connection(struct v2g_connection* conn) {
    int rv = -1;
    enum v2g_event rvAppHandshake = V2G_EVENT_NO_EVENT;
    bool stop_receiving_loop = false;
    int64_t start_time = 0; // in ms

    enum v2g_protocol selected_protocol = V2G_UNKNOWN_PROTOCOL;
    v2g_ctx_init_charging_state(conn->ctx, false);
    conn->buffer = static_cast<uint8_t*>(malloc(DEFAULT_BUFFER_SIZE));
    if (!conn->buffer)
        return -1;

    /* static setup */
    conn->stream.data = conn->buffer;
    conn->stream.pos = &conn->buffer_pos;

    /* Here is a good point to wait until the customer is ready for a resumed session,
     * because we are waiting for the incoming message of the ev */
    if (conn->dlink_action == MQTT_DLINK_ACTION_PAUSE) {
        // TODO: D_LINK pause
    }

    do {
        /* setup for receive */
        conn->stream.buffer = 0;
        conn->stream.capacity = 0; // Set to 8 for send and 0 for recv
        conn->buffer_pos = 0;
        conn->payload_len = 0;

        /* next call return -1 on error, 1 when peer closed connection, 0 on success */
        rv = v2g_incoming_v2gtp(conn);

        if (rv != 0) {
            dlog(DLOG_LEVEL_ERROR, "v2g_incoming_v2gtp() failed");
            goto error_out;
        }

        if (conn->ctx->is_connection_terminated == true) {
            rv = -1;
            goto error_out;
        }

        /* next call return -1 on non-recoverable errors, 1 on recoverable errors, 0 on success */
        rvAppHandshake = v2g_handle_apphandshake(conn);

        if (rvAppHandshake == V2G_EVENT_IGNORE_MSG) {
            dlog(DLOG_LEVEL_WARNING, "v2g_handle_apphandshake() failed, ignoring packet");
        }
    } while ((rv == 1) && (rvAppHandshake == V2G_EVENT_IGNORE_MSG));

    /* stream setup for sending is done within v2g_handle_apphandshake */
    /* send supportedAppRes message */
    if ((rvAppHandshake == V2G_EVENT_SEND_AND_TERMINATE) || (rvAppHandshake == V2G_EVENT_NO_EVENT)) {
        /* form the content of V2G_Message type and publish the response for debugging*/
        if (conn->ctx->debugMode == true) {
            publish_var_V2G_Message(conn, false);
        }

        rv = v2g_outgoing_v2gtp(conn);

        if (rv == -1) {
            dlog(DLOG_LEVEL_ERROR, "v2g_outgoing_v2gtp() failed");
            goto error_out;
        }
    }

    /* terminate connection, if supportedApp handshake has failed */
    if ((rvAppHandshake == V2G_EVENT_SEND_AND_TERMINATE) || (rvAppHandshake == V2G_EVENT_TERMINATE_CONNECTION)) {
        rv = -1;
        goto error_out;
    }

    /* Backup the selected protocol, because this value is shared and can be reseted while unplugging. */
    selected_protocol = conn->ctx->selected_protocol;

    /* allocate in/out documents dynamically */
    switch (selected_protocol) {
    case V2G_PROTO_DIN70121:
    case V2G_PROTO_ISO15118_2010:
        conn->exi_in.dinEXIDocument = static_cast<struct dinEXIDocument*>(calloc(1, sizeof(struct dinEXIDocument)));
        if (conn->exi_in.dinEXIDocument == NULL) {
            dlog(DLOG_LEVEL_ERROR, "out-of-memory");
            goto error_out;
        }
        conn->exi_out.dinEXIDocument = static_cast<struct dinEXIDocument*>(calloc(1, sizeof(struct dinEXIDocument)));
        if (conn->exi_out.dinEXIDocument == NULL) {
            dlog(DLOG_LEVEL_ERROR, "out-of-memory");
            goto error_out;
        }
        break;
    case V2G_PROTO_ISO15118_2013:
        conn->exi_in.iso1EXIDocument = static_cast<struct iso1EXIDocument*>(calloc(1, sizeof(struct iso1EXIDocument)));
        if (conn->exi_in.iso1EXIDocument == NULL) {
            dlog(DLOG_LEVEL_ERROR, "out-of-memory");
            goto error_out;
        }
        conn->exi_out.iso1EXIDocument = static_cast<struct iso1EXIDocument*>(calloc(1, sizeof(struct iso1EXIDocument)));
        if (conn->exi_out.iso1EXIDocument == NULL) {
            dlog(DLOG_LEVEL_ERROR, "out-of-memory");
            goto error_out;
        }
        break;
    default:
        goto error_out; //     if protocol is unknown
    }

    do {
        /* setup for receive */
        conn->stream.buffer = 0;
        conn->stream.capacity = 0; // Set to 8 for send and 0 for recv
        conn->buffer_pos = 0;
        conn->payload_len = 0;

        /* next call return -1 on error, 1 when peer closed connection, 0 on success */
        rv = v2g_incoming_v2gtp(conn);

        if (rv == 1) {
            dlog(DLOG_LEVEL_ERROR, "Timeout waiting for next request or peer closed connection");
            break;
        } else if (rv == -1) {
            dlog(DLOG_LEVEL_ERROR, "v2g_incoming_v2gtp() (previous message \"%s\") failed",
                 v2g_msg_type[conn->ctx->last_v2g_msg]);
            break;
        }

        start_time = getmonotonictime(); // To calc the duration of req msg configuration

        /* according to agreed protocol decode the stream */
        enum v2g_event v2gEvent = V2G_EVENT_NO_EVENT;
        switch (selected_protocol) {
        case V2G_PROTO_DIN70121:
        case V2G_PROTO_ISO15118_2010:
            memset(conn->exi_in.dinEXIDocument, 0, sizeof(struct dinEXIDocument));
            rv = decode_dinExiDocument(&conn->stream, conn->exi_in.dinEXIDocument);
            if (rv != 0) {
                dlog(DLOG_LEVEL_ERROR, "decode_dinExiDocument() (previous message \"%s\") failed: %d",
                     v2g_msg_type[conn->ctx->last_v2g_msg], rv);
                /* we must ignore packet which we cannot decode, so reset rv to zero to stay in loop */
                rv = 0;
                v2gEvent = V2G_EVENT_IGNORE_MSG;
                break;
            }

            memset(conn->exi_out.dinEXIDocument, 0, sizeof(struct dinEXIDocument));
            conn->exi_out.dinEXIDocument->V2G_Message_isUsed = 1;

            v2gEvent = din_handle_request(conn);
            break;

        case V2G_PROTO_ISO15118_2013:
            memset(conn->exi_in.iso1EXIDocument, 0, sizeof(struct iso1EXIDocument));
            rv = decode_iso1ExiDocument(&conn->stream, conn->exi_in.iso1EXIDocument);
            if (rv != 0) {
                dlog(DLOG_LEVEL_ERROR, "decode_iso1EXIDocument() (previous message \"%s\") failed: %d",
                     v2g_msg_type[conn->ctx->last_v2g_msg], rv);
                /* we must ignore packet which we cannot decode, so reset rv to zero to stay in loop */
                rv = 0;
                v2gEvent = V2G_EVENT_IGNORE_MSG;
                break;
            }
            conn->buffer_pos = 0; // Reset buffer pos for the case if exi msg will be configured over mqtt
            memset(conn->exi_out.iso1EXIDocument, 0, sizeof(struct iso1EXIDocument));
            conn->exi_out.iso1EXIDocument->V2G_Message_isUsed = 1;

            v2gEvent = iso_handle_request(conn);

            break;
        default:
            goto error_out; //     if protocol is unknown
        }

        /* form the content of V2G_Message type and publish the request*/
        if (conn->ctx->debugMode == true) {
            publish_var_V2G_Message(conn, true);
        }

        switch (v2gEvent) {
        case V2G_EVENT_SEND_AND_TERMINATE:
            stop_receiving_loop = true;
        case V2G_EVENT_NO_EVENT: { // fall-through intended
            /* Reset v2g-buffer */
            conn->stream.buffer = 0;
            conn->stream.capacity = 8; // Set to 8 for send and 0 for recv
            conn->buffer_pos = V2GTP_HEADER_LENGTH;
            conn->stream.size = DEFAULT_BUFFER_SIZE;

            /* Configure msg and send */
            switch (selected_protocol) {
            case V2G_PROTO_DIN70121:
            case V2G_PROTO_ISO15118_2010:
                if ((rv = encode_dinExiDocument(&conn->stream, conn->exi_out.dinEXIDocument)) != 0) {
                    dlog(DLOG_LEVEL_ERROR, "encode_dinExiDocument() (message \"%s\") failed: %d",
                         v2g_msg_type[conn->ctx->current_v2g_msg], rv);
                }
                break;
            case V2G_PROTO_ISO15118_2013:
                if ((rv = encode_iso1ExiDocument(&conn->stream, conn->exi_out.iso1EXIDocument)) != 0) {
                    dlog(DLOG_LEVEL_ERROR, "encode_iso1ExiDocument() (message \"%s\") failed: %d",
                         v2g_msg_type[conn->ctx->current_v2g_msg], rv);
                }
                break;
            default:
                goto error_out; //     if protocol is unknown
            }
            /* Wait max. res-time before sending the next response */
            int64_t time_to_conf_res = getmonotonictime() - start_time;

            if (time_to_conf_res < MAX_RES_TIME) {
                // dlog(DLOG_LEVEL_ERROR,"time_to_conf_res %llu", time_to_conf_res);
                std::this_thread::sleep_for(std::chrono::microseconds((MAX_RES_TIME - time_to_conf_res) * 1000));
            } else {
                dlog(DLOG_LEVEL_WARNING, "Response message (type %d) not configured within %d ms (took %" PRIi64 " ms)",
                     conn->ctx->current_v2g_msg, MAX_RES_TIME, time_to_conf_res);
            }
        }
        case V2G_EVENT_SEND_RECV_EXI_MSG: { // fall-through intended
            /* form the content of V2G_Message type and publish the response for debugging*/
            if (conn->ctx->debugMode == true) {
                publish_var_V2G_Message(conn, false);
            }

            /* Write header and send next res-msg */
            if ((rv != 0) || ((rv = v2g_outgoing_v2gtp(conn)) == -1)) {
                dlog(DLOG_LEVEL_ERROR, "v2g_outgoing_v2gtp() \"%s\" failed: %d",
                     v2g_msg_type[conn->ctx->current_v2g_msg], rv);
                break;
            }
            break;
        }
        case V2G_EVENT_IGNORE_MSG:
            dlog(DLOG_LEVEL_ERROR, "Ignoring V2G request message \"%s\". Waiting for next request",
                 v2g_msg_type[conn->ctx->current_v2g_msg]);
            break;
        case V2G_EVENT_TERMINATE_CONNECTION: // fall-through intended
        default:
            dlog(DLOG_LEVEL_ERROR, "Failed to handle V2G request message \"%s\"",
                 v2g_msg_type[conn->ctx->current_v2g_msg]);
            stop_receiving_loop = true;
            break;
        }
    } while ((rv == 0) && (stop_receiving_loop == false));

error_out:
    switch (selected_protocol) {
    case V2G_PROTO_DIN70121:
    case V2G_PROTO_ISO15118_2010:
        if (conn->exi_in.dinEXIDocument != NULL)
            free(conn->exi_in.dinEXIDocument);
        if (conn->exi_out.dinEXIDocument != NULL)
            free(conn->exi_out.dinEXIDocument);
        break;
    case V2G_PROTO_ISO15118_2013:
        if (conn->exi_in.iso1EXIDocument != NULL)
            free(conn->exi_in.iso1EXIDocument);
        if (conn->exi_out.iso1EXIDocument != NULL)
            free(conn->exi_out.iso1EXIDocument);
        break;
    default:
        break;
    }

    if (conn->buffer != NULL) {
        free(conn->buffer);
    }

    v2g_ctx_init_charging_state(conn->ctx, true);

    return rv ? -1 : 0;
}

uint64_t v2g_session_id_from_exi(bool is_iso, void* exi_in) {
    uint64_t session_id = 0;

    if (is_iso) {
        struct iso1EXIDocument* req = static_cast<struct iso1EXIDocument*>(exi_in);
        struct iso1MessageHeaderType* hdr = &req->V2G_Message.Header;

        /* the provided session id could be smaller (error) in case that the peer did not
         * send our full session id back to us; this is why we init the id with 0 above
         * and only copy the provided byte len
         */
        memcpy(&session_id, &hdr->SessionID.bytes, std::min((int)sizeof(session_id), (int)hdr->SessionID.bytesLen));
    } else {
        struct dinEXIDocument* req = static_cast<struct dinEXIDocument*>(exi_in);
        struct dinMessageHeaderType* hdr = &req->V2G_Message.Header;

        /* see comment above */
        memcpy(&session_id, &hdr->SessionID.bytes, std::min((int)sizeof(session_id), (int)hdr->SessionID.bytesLen));
    }

    return session_id;
}
