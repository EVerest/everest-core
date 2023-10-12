// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#include "EvseV2G.hpp"
#include "connection.hpp"
#include "log.hpp"
#include "sdp.hpp"

struct v2g_context* v2g_ctx = NULL;

namespace module {

void EvseV2G::init() {
    int rv = 0;
    /* create v2g context */
    v2g_ctx = v2g_ctx_create(&(*p_charger), &(*r_security));

    if (v2g_ctx == NULL)
        return;

    invoke_init(*p_charger);

    dlog(DLOG_LEVEL_DEBUG, "Starting SDP responder");

    rv = connection_init(v2g_ctx);

    if (rv == -1) {
        dlog(DLOG_LEVEL_ERROR, "Failed to initialize connection");
        goto err_out;
    }

    rv = sdp_init(v2g_ctx);

    if (rv == -1) {
        dlog(DLOG_LEVEL_ERROR, "Failed to start SDP responder");
        goto err_out;
    }

    dlog(DLOG_LEVEL_DEBUG, "starting socket server(s)");
    if (connection_start_servers(v2g_ctx)) {
        dlog(DLOG_LEVEL_ERROR, "start_connection_servers() failed");
        goto err_out;
    }

    return;
err_out:
    v2g_ctx_free(v2g_ctx);
}

void EvseV2G::ready() {
    int rv = 0;

    invoke_ready(*p_charger);

    rv = sdp_listen(v2g_ctx);

    if (rv == -1) {
        dlog(DLOG_LEVEL_ERROR, "sdp_listen() failed");
        goto err_out;
    }

err_out:
    v2g_ctx_free(v2g_ctx);
}

EvseV2G::~EvseV2G() {
    v2g_ctx_free(v2g_ctx);
}

} // namespace module
