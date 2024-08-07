// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#include "IsoMux.hpp"
#include "connection.hpp"
#include "log.hpp"
#include "sdp.hpp"

struct v2g_context* v2g_ctx = nullptr;

namespace module {

void IsoMux::init() {
    /* create v2g context */
    v2g_ctx = v2g_ctx_create(&(*r_security));

    if (v2g_ctx == nullptr)
        return;

    v2g_ctx->proxy_port_iso2 = config.proxy_port_iso2;
    v2g_ctx->proxy_port_iso20 = config.proxy_port_iso20;
    v2g_ctx->selected_iso20 = false;

    v2g_ctx->tls_server = &tls_server;

    invoke_init(*p_charger);
}

void IsoMux::ready() {
    int rv = 0;

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

    invoke_ready(*p_charger);

    rv = sdp_listen(v2g_ctx);

    if (rv == -1) {
        dlog(DLOG_LEVEL_ERROR, "sdp_listen() failed");
        goto err_out;
    }

    return;

err_out:
    v2g_ctx_free(v2g_ctx);
}

IsoMux::~IsoMux() {
    v2g_ctx_free(v2g_ctx);
}

bool IsoMux::selected_iso20() {
    return v2g_ctx->selected_iso20;
}

} // namespace module
