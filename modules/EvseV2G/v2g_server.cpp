// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2023 chargebyte GmbH
// Copyright (C) 2023 Contributors to EVerest
#include "v2g_server.hpp"

#include <cstdint>
#include <cstdlib>
#include <inttypes.h>
#include <string.h>
#include <unistd.h>

#include <cbv2g/app_handshake/appHand_Decoder.h>
#include <cbv2g/app_handshake/appHand_Encoder.h>
#include <cbv2g/common/exi_basetypes.h>
#include <cbv2g/din/din_msgDefDecoder.h>
#include <cbv2g/din/din_msgDefEncoder.h>
#include <cbv2g/exi_v2gtp.h>
#include <cbv2g/iso_2/iso2_msgDefDecoder.h>
#include <cbv2g/iso_2/iso2_msgDefEncoder.h>

#include "connection.hpp"
#include "din_server.hpp"
#include "iso_server.hpp"
#include "log.hpp"
#include "tools.hpp"

#define MAX_RES_TIME 98

static types::iso15118::V2gMessageId get_v2g_message_id(enum V2gMsgTypeId v2g_msg, enum v2g_protocol selected_protocol,
                                                        bool is_req) {
    switch (v2g_msg) {
    case V2G_SUPPORTED_APP_PROTOCOL_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::SupportedAppProtocolReq
                              : types::iso15118::V2gMessageId::SupportedAppProtocolRes;
    case V2G_SESSION_SETUP_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::SessionSetupReq
                              : types::iso15118::V2gMessageId::SessionSetupRes;
    case V2G_SERVICE_DISCOVERY_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::ServiceDiscoveryReq
                              : types::iso15118::V2gMessageId::ServiceDiscoveryRes;
    case V2G_SERVICE_DETAIL_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::ServiceDetailReq
                              : types::iso15118::V2gMessageId::ServiceDetailRes;
    case V2G_PAYMENT_SERVICE_SELECTION_MSG:
        return is_req == true                            ? selected_protocol == V2G_PROTO_DIN70121
                                                               ? types::iso15118::V2gMessageId::ServicePaymentSelectionReq
                                                               : types::iso15118::V2gMessageId::PaymentServiceSelectionReq
               : selected_protocol == V2G_PROTO_DIN70121 ? types::iso15118::V2gMessageId::ServicePaymentSelectionRes
                                                         : types::iso15118::V2gMessageId::PaymentServiceSelectionRes;
    case V2G_PAYMENT_DETAILS_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::PaymentDetailsReq
                              : types::iso15118::V2gMessageId::PaymentDetailsRes;
    case V2G_AUTHORIZATION_MSG:
        return is_req == true                            ? selected_protocol == V2G_PROTO_DIN70121
                                                               ? types::iso15118::V2gMessageId::ContractAuthenticationReq
                                                               : types::iso15118::V2gMessageId::AuthorizationReq
               : selected_protocol == V2G_PROTO_DIN70121 ? types::iso15118::V2gMessageId::ContractAuthenticationRes
                                                         : types::iso15118::V2gMessageId::AuthorizationRes;
    case V2G_CHARGE_PARAMETER_DISCOVERY_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::ChargeParameterDiscoveryReq
                              : types::iso15118::V2gMessageId::ChargeParameterDiscoveryRes;
    case V2G_METERING_RECEIPT_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::MeteringReceiptReq
                              : types::iso15118::V2gMessageId::MeteringReceiptRes;
    case V2G_CERTIFICATE_UPDATE_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::CertificateUpdateReq
                              : types::iso15118::V2gMessageId::CertificateUpdateRes;
    case V2G_CERTIFICATE_INSTALLATION_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::CertificateInstallationReq
                              : types::iso15118::V2gMessageId::CertificateInstallationRes;
    case V2G_CHARGING_STATUS_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::ChargingStatusReq
                              : types::iso15118::V2gMessageId::ChargingStatusRes;
    case V2G_CABLE_CHECK_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::CableCheckReq
                              : types::iso15118::V2gMessageId::CableCheckRes;
    case V2G_PRE_CHARGE_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::PreChargeReq
                              : types::iso15118::V2gMessageId::PreChargeRes;
    case V2G_POWER_DELIVERY_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::PowerDeliveryReq
                              : types::iso15118::V2gMessageId::PowerDeliveryRes;
    case V2G_CURRENT_DEMAND_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::CurrentDemandReq
                              : types::iso15118::V2gMessageId::CurrentDemandRes;
    case V2G_WELDING_DETECTION_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::WeldingDetectionReq
                              : types::iso15118::V2gMessageId::WeldingDetectionRes;
    case V2G_SESSION_STOP_MSG:
        return is_req == true ? types::iso15118::V2gMessageId::SessionStopReq
                              : types::iso15118::V2gMessageId::SessionStopRes;
    case V2G_UNKNOWN_MSG:
    default:
        return types::iso15118::V2gMessageId::UnknownMessage;
    }
}

/*!
 * \brief publish_var_V2G_Message This function fills a V2gMessages type with the V2G EXI message as HEX and Base64
 * \param conn hold the context of the V2G-connection.
 * \param is_req if it is a V2G request or response: 'true' if a request, and 'false' if a response
 */
static void publish_var_V2G_Message(v2g_connection* conn, bool is_req) {
    types::iso15118::V2gMessages v2g_message;

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

    std::string EXI_Base64;

    EXI_Base64 = openssl::base64_encode(conn->buffer, conn->payload_len + V2GTP_HEADER_LENGTH);
    if (EXI_Base64.size() == 0) {
        dlog(DLOG_LEVEL_WARNING, "Unable to base64 encode EXI buffer");
    }

    v2g_message.exi_base64 = EXI_Base64;
    v2g_message.id = get_v2g_message_id(conn->ctx->current_v2g_msg, conn->ctx->selected_protocol, is_req);
    v2g_message.exi = msg_as_hex_string;
    conn->ctx->p_charger->publish_v2g_messages(v2g_message);
}

/*!
 * \brief v2g_incoming_v2gtp This function reads the V2G transport header
 * \param conn hold the context of the V2G-connection.
 * \return Returns 0 if the V2G-session was successfully stopped, otherwise -1.
 */
static int v2g_incoming_v2gtp(struct v2g_connection* conn) {
    assert(conn != nullptr);
    assert(conn->read != nullptr);

    int rv;

    /* read and process header */
    rv = conn->read(conn, conn->buffer, V2GTP_HEADER_LENGTH);
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

    rv = V2GTP_ReadHeader(conn->buffer, &conn->payload_len);
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
    rv = conn->read(conn, &conn->buffer[V2GTP_HEADER_LENGTH], conn->payload_len);
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
    conn->stream.byte_pos = V2GTP_HEADER_LENGTH;
    conn->stream.data_size = conn->payload_len + V2GTP_HEADER_LENGTH;

    return 0;
}

/*!
 * \brief v2g_outgoing_v2gtp This function creates the v2g transport header
 * \param conn hold the context of the v2g-connection.
 * \return Returns 0 if the v2g-session was successfully stopped, otherwise -1.
 */
int v2g_outgoing_v2gtp(struct v2g_connection* conn) {
    assert(conn != nullptr);
    assert(conn->write != nullptr);

    /* fixup/create header */
    const auto len = exi_bitstream_get_length(&conn->stream);

    V2GTP_WriteHeader(conn->buffer, len - V2GTP_HEADER_LENGTH);

    if (conn->write(conn, conn->buffer, len) == -1) {
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
    init_appHand_exiDocument(&conn->handshake_resp);
    conn->handshake_resp.supportedAppProtocolRes_isUsed = 1;
    conn->handshake_resp.supportedAppProtocolRes.ResponseCode =
        appHand_responseCodeType_Failed_NoNegotiation; // [V2G2-172]

    dlog(DLOG_LEVEL_INFO, "Handling SupportedAppProtocolReq");
    conn->ctx->current_v2g_msg = V2G_SUPPORTED_APP_PROTOCOL_MSG;

    if (decode_appHand_exiDocument(&conn->stream, &conn->handshake_req) != 0) {
        dlog(DLOG_LEVEL_ERROR, "decode_appHandExiDocument() failed");
        return V2G_EVENT_TERMINATE_CONNECTION; // If the mesage can't be decoded we have to terminate the tcp-connection
                                               // (e.g. after an unexpected message)
    }

    types::iso15118::AppProtocols app_protocols; // to publish supported app protocol array

    for (i = 0; i < conn->handshake_req.supportedAppProtocolReq.AppProtocol.arrayLen; i++) {
        struct appHand_AppProtocolType* app_proto = &conn->handshake_req.supportedAppProtocolReq.AppProtocol.array[i];
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
                appHand_responseCodeType_OK_SuccessfulNegotiation;
            ev_app_priority = app_proto->Priority;
            conn->handshake_resp.supportedAppProtocolRes.SchemaID = app_proto->SchemaID;
            conn->ctx->selected_protocol = V2G_PROTO_DIN70121;
        } else if ((conn->ctx->supported_protocols & (1 << V2G_PROTO_ISO15118_2013)) &&
                   (strcmp(proto_ns, ISO_15118_2013_MSG_DEF) == 0) &&
                   (app_proto->VersionNumberMajor == ISO_15118_2013_MAJOR) &&
                   (ev_app_priority >= app_proto->Priority)) {

            conn->handshake_resp.supportedAppProtocolRes.ResponseCode =
                appHand_responseCodeType_OK_SuccessfulNegotiation;
            ev_app_priority = app_proto->Priority;
            conn->handshake_resp.supportedAppProtocolRes.SchemaID = app_proto->SchemaID;
            conn->ctx->selected_protocol = V2G_PROTO_ISO15118_2013;
        }

        if (conn->ctx->debugMode == true) {
            const types::iso15118::AppProtocol protocol = {
                std::string(proto_ns), static_cast<int32_t>(app_proto->VersionNumberMajor),
                static_cast<int32_t>(app_proto->VersionNumberMinor), static_cast<int32_t>(app_proto->SchemaID),
                static_cast<int32_t>(app_proto->Priority)};

            app_protocols.Protocols.push_back(protocol);
        }

        // TODO: ISO15118v2
        free(proto_ns);
    }

    if (conn->ctx->debugMode == true) {
        conn->ctx->p_charger->publish_ev_app_protocol(app_protocols);
        /* form the content of V2G_Message type and publish the request*/
        publish_var_V2G_Message(conn, true);
    }

    std::string selected_protocol_str;
    if (conn->handshake_resp.supportedAppProtocolRes.ResponseCode ==
        appHand_responseCodeType_OK_SuccessfulNegotiation) {
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
        conn->ctx->p_charger->publish_selected_protocol(selected_protocol_str);
    }

    if (conn->ctx->is_connection_terminated == true) {
        dlog(DLOG_LEVEL_ERROR, "Connection is terminated. Abort charging");
        return V2G_EVENT_TERMINATE_CONNECTION; // Abort charging without sending a response
    }

    /* Validate response code */
    if ((conn->ctx->intl_emergency_shutdown == true) || (conn->ctx->stop_hlc == true) ||
        (V2G_EVENT_SEND_AND_TERMINATE == next_event)) {
        conn->handshake_resp.supportedAppProtocolRes.ResponseCode = appHand_responseCodeType_Failed_NoNegotiation;
        dlog(DLOG_LEVEL_ERROR, "Abort charging session");

        if (conn->ctx->terminate_connection_on_failed_response == true) {
            next_event = V2G_EVENT_SEND_AND_TERMINATE; // send response and terminate the TCP-connection
        }
    }

    /* encode response at the right buffer location */
    conn->stream.byte_pos = V2GTP_HEADER_LENGTH;
    conn->stream.bit_count = 0;

    if (encode_appHand_exiDocument(&conn->stream, &conn->handshake_resp) != 0) {
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

    /* Here is a good point to wait until the customer is ready for a resumed session,
     * because we are waiting for the incoming message of the ev */
    if (conn->dlink_action == MQTT_DLINK_ACTION_PAUSE) {
        // TODO: D_LINK pause
    }

    do {
        /* setup for receive */
        conn->stream.data[0] = 0;
        conn->payload_len = 0;
        exi_bitstream_init(&conn->stream, conn->buffer, 0, 0, nullptr);

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
        conn->exi_in.dinEXIDocument = static_cast<struct din_exiDocument*>(calloc(1, sizeof(struct din_exiDocument)));
        if (conn->exi_in.dinEXIDocument == NULL) {
            dlog(DLOG_LEVEL_ERROR, "out-of-memory");
            goto error_out;
        }
        conn->exi_out.dinEXIDocument = static_cast<struct din_exiDocument*>(calloc(1, sizeof(struct din_exiDocument)));
        if (conn->exi_out.dinEXIDocument == NULL) {
            dlog(DLOG_LEVEL_ERROR, "out-of-memory");
            goto error_out;
        }
        break;
    case V2G_PROTO_ISO15118_2013:
        conn->exi_in.iso2EXIDocument =
            static_cast<struct iso2_exiDocument*>(calloc(1, sizeof(struct iso2_exiDocument)));
        if (conn->exi_in.iso2EXIDocument == NULL) {
            dlog(DLOG_LEVEL_ERROR, "out-of-memory");
            goto error_out;
        }
        conn->exi_out.iso2EXIDocument =
            static_cast<struct iso2_exiDocument*>(calloc(1, sizeof(struct iso2_exiDocument)));
        if (conn->exi_out.iso2EXIDocument == NULL) {
            dlog(DLOG_LEVEL_ERROR, "out-of-memory");
            goto error_out;
        }
        break;
    default:
        goto error_out; //     if protocol is unknown
    }

    do {
        /* setup for receive */
        conn->stream.data[0] = 0;
        conn->stream.bit_count = 0;
        conn->stream.byte_pos = 0;
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
            memset(conn->exi_in.dinEXIDocument, 0, sizeof(struct din_exiDocument));
            rv = decode_din_exiDocument(&conn->stream, conn->exi_in.dinEXIDocument);
            if (rv != 0) {
                dlog(DLOG_LEVEL_ERROR, "decode_dinExiDocument() (previous message \"%s\") failed: %d",
                     v2g_msg_type[conn->ctx->last_v2g_msg], rv);
                /* we must ignore packet which we cannot decode, so reset rv to zero to stay in loop */
                rv = 0;
                v2gEvent = V2G_EVENT_IGNORE_MSG;
                break;
            }

            memset(conn->exi_out.dinEXIDocument, 0, sizeof(struct din_exiDocument));

            v2gEvent = din_handle_request(conn);
            break;

        case V2G_PROTO_ISO15118_2013:
            memset(conn->exi_in.iso2EXIDocument, 0, sizeof(struct iso2_exiDocument));
            rv = decode_iso2_exiDocument(&conn->stream, conn->exi_in.iso2EXIDocument);
            if (rv != 0) {
                dlog(DLOG_LEVEL_ERROR, "decode_iso2_exiDocument() (previous message \"%s\") failed: %d",
                     v2g_msg_type[conn->ctx->last_v2g_msg], rv);
                /* we must ignore packet which we cannot decode, so reset rv to zero to stay in loop */
                rv = 0;
                v2gEvent = V2G_EVENT_IGNORE_MSG;
                break;
            }
            conn->stream.byte_pos = 0; // Reset pos for the case if exi msg will be configured over mqtt
            memset(conn->exi_out.iso2EXIDocument, 0, sizeof(struct iso2_exiDocument));

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
            conn->stream.data[0] = 0;
            conn->stream.bit_count = 0;
            conn->stream.byte_pos = V2GTP_HEADER_LENGTH;
            conn->stream.data_size = DEFAULT_BUFFER_SIZE;

            /* Configure msg and send */
            switch (selected_protocol) {
            case V2G_PROTO_DIN70121:
            case V2G_PROTO_ISO15118_2010:
                if ((rv = encode_din_exiDocument(&conn->stream, conn->exi_out.dinEXIDocument)) != 0) {
                    dlog(DLOG_LEVEL_ERROR, "encode_dinExiDocument() (message \"%s\") failed: %d",
                         v2g_msg_type[conn->ctx->current_v2g_msg], rv);
                }
                break;
            case V2G_PROTO_ISO15118_2013:
                if ((rv = encode_iso2_exiDocument(&conn->stream, conn->exi_out.iso2EXIDocument)) != 0) {
                    dlog(DLOG_LEVEL_ERROR, "encode_iso2_exiDocument() (message \"%s\") failed: %d",
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
        if (conn->exi_in.iso2EXIDocument != NULL)
            free(conn->exi_in.iso2EXIDocument);
        if (conn->exi_out.iso2EXIDocument != NULL)
            free(conn->exi_out.iso2EXIDocument);
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
        struct iso2_exiDocument* req = static_cast<struct iso2_exiDocument*>(exi_in);
        struct iso2_MessageHeaderType* hdr = &req->V2G_Message.Header;

        /* the provided session id could be smaller (error) in case that the peer did not
         * send our full session id back to us; this is why we init the id with 0 above
         * and only copy the provided byte len
         */
        memcpy(&session_id, &hdr->SessionID.bytes, std::min((int)sizeof(session_id), (int)hdr->SessionID.bytesLen));
    } else {
        struct din_exiDocument* req = static_cast<struct din_exiDocument*>(exi_in);
        struct din_MessageHeaderType* hdr = &req->V2G_Message.Header;

        /* see comment above */
        memcpy(&session_id, &hdr->SessionID.bytes, std::min((int)sizeof(session_id), (int)hdr->SessionID.bytesLen));
    }

    return session_id;
}
