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
#include "log.hpp"
#include "tools.hpp"

#define MAX_RES_TIME 98

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
    rv = conn->read(conn, conn->buffer, V2GTP_HEADER_LENGTH, true);
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
    rv = conn->read(conn, &conn->buffer[V2GTP_HEADER_LENGTH], conn->payload_len, true);
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
 * \brief v2g_handle_apphandshake After receiving a supportedAppProtocolReq message,
 * the SECC shall process the received information. DIN [V2G-DC-436] ISO [V2G2-540]
 * \param conn hold the context of the v2g-connection.
 * \return Returns a v2g-event of type enum v2g_event.
 */
static bool v2g_sniff_apphandshake(struct v2g_connection* conn, bool& iso20) {
    int i;
    iso20 = false;

    /* validate handshake request and create response */
    init_appHand_exiDocument(&conn->handshake_resp);
    conn->handshake_resp.supportedAppProtocolRes_isUsed = 1;
    conn->handshake_resp.supportedAppProtocolRes.ResponseCode =
        appHand_responseCodeType_Failed_NoNegotiation; // [V2G2-172]

    dlog(DLOG_LEVEL_INFO, "Handling SupportedAppProtocolReq");
    conn->ctx->current_v2g_msg = V2G_SUPPORTED_APP_PROTOCOL_MSG;

    if (decode_appHand_exiDocument(&conn->stream, &conn->handshake_req) != 0) {
        dlog(DLOG_LEVEL_ERROR, "decode_appHandExiDocument() failed");
        return false; // If the mesage can't be decoded we have to terminate the tcp-connection
                      // (e.g. after an unexpected message)
    }

    for (i = 0; i < conn->handshake_req.supportedAppProtocolReq.AppProtocol.arrayLen; i++) {
        struct appHand_AppProtocolType* app_proto = &conn->handshake_req.supportedAppProtocolReq.AppProtocol.array[i];
        char* proto_ns = strndup(static_cast<const char*>(app_proto->ProtocolNamespace.characters),
                                 app_proto->ProtocolNamespace.charactersLen);

        if (!proto_ns) {
            dlog(DLOG_LEVEL_ERROR, "out-of-memory condition");
            return V2G_EVENT_TERMINATE_CONNECTION;
        }

        dlog(DLOG_LEVEL_INFO,
             "handshake_req: Namespace: %s, Version: %" PRIu32 ".%" PRIu32 ", SchemaID: %" PRIu8 ", Priority: %" PRIu8,
             proto_ns, app_proto->VersionNumberMajor, app_proto->VersionNumberMinor, app_proto->SchemaID,
             app_proto->Priority);

        // Check if it supports ISO-20
        const char* iso20_urn = "urn:iso:std:iso:15118:-20";
        if (strncmp(iso20_urn, proto_ns, strlen(iso20_urn)) == 0) {
            iso20 = true;
            return true;
        }

        free(proto_ns);
    }
    return true;
}

bool v2g_detect_iso20_support(struct v2g_connection* conn) {
    int rv = -1;
    enum v2g_event rvAppHandshake = V2G_EVENT_NO_EVENT;
    enum v2g_protocol selected_protocol = V2G_UNKNOWN_PROTOCOL;

    /* static setup */
    conn->stream.data = conn->buffer;
    bool app_protocol_received = false;
    do {
        /* setup for receive */
        conn->stream.data[0] = 0;
        conn->payload_len = 0;
        exi_bitstream_init(&conn->stream, conn->buffer, 0, 0, nullptr);

        /* next call return -1 on error, 1 when peer closed connection, 0 on success */
        rv = v2g_incoming_v2gtp(conn);

        if (rv != 0) {
            dlog(DLOG_LEVEL_ERROR, "v2g_incoming_v2gtp() failed");
        }

        if (conn->ctx->is_connection_terminated == true) {
            rv = -1;
        }

        bool iso20 = false;
        app_protocol_received = v2g_sniff_apphandshake(conn, iso20);

        if (iso20) {
            return true;
        }

    } while ((rv == 1) && not app_protocol_received);
    return false;
}
