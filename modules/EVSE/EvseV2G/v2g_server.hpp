// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2023 chargebyte GmbH
// Copyright (C) 2023 Contributors to EVerest
#ifndef V2G_SERVER_H
#define V2G_SERVER_H

#include "v2g.hpp"

static const char* v2g_msg_type[] = {
    "Supported App Protocol",
    "Session Setup",
    "Service Discovery",
    "Service Detail",
    "Payment Service Selection",
    "Payment Details",
    "Authorization",
    "Charge Parameter Discovery",
    "Metering Receipt",
    "Certificate Update",
    "Certificate Installation",
    "Charging Status",
    "Cable Check",
    "Pre Charge",
    "Power Delivery",
    "Current Demand",
    "Welding Detection",
    "Session Stop",
    "Unknown",
};

/*!
 * \brief v2g_handle_connection This function handles a v2g-charging-session.
 * \param conn hold the context of the v2g-connection.
 * \return Returns 0 if the v2g-session was successfully stopped, otherwise -1.
 */
int v2g_handle_connection(struct v2g_connection* conn);

/*!
 * \brief v2g_session_id_from_exi This function extracts session ID from an EXI stream.
 * \param is_iso determines if ISO or DIN should be handled.
 * \param exi_in holds the input EXI stream.
 * \return Returns the extracted session ID.
 */
uint64_t v2g_session_id_from_exi(bool is_iso, void* exi_in);

#endif /* V2G_SERVER_H */
