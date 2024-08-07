// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest

#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <errno.h>
#include <math.h>
#include <unistd.h> // sleep

#include "log.hpp"
#include "v2g_ctx.hpp"

#include <cbv2g/iso_2/iso2_msgDefDatatypes.h>

struct v2g_context* v2g_ctx_create(evse_securityIntf* r_security) {
    struct v2g_context* ctx;

    // TODO There are c++ objects within v2g_context and calloc doesn't call initialisers.
    //      free() will not call destructors
    ctx = static_cast<v2g_context*>(calloc(1, sizeof(*ctx)));
    if (!ctx)
        return NULL;

    ctx->r_security = r_security;
    ctx->tls_security = TLS_SECURITY_PROHIBIT; // default

    ctx->local_tcp_addr = NULL;
    ctx->local_tls_addr = NULL;

    /* interface from config file or options */
    ctx->if_name = "eth1";

    ctx->network_read_timeout = 1000;
    ctx->network_read_timeout_tls = 5000;

    ctx->sdp_socket = -1;
    ctx->tcp_socket = -1;
    ctx->tls_socket.fd = -1;

    ctx->com_setup_timeout = NULL;

    return ctx;

free_out:
    free(ctx->local_tls_addr);
    free(ctx->local_tcp_addr);
    free(ctx);
    return NULL;
}

void v2g_ctx_free(struct v2g_context* ctx) {
    free(ctx->local_tls_addr);
    ctx->local_tls_addr = NULL;
    free(ctx->local_tcp_addr);
    ctx->local_tcp_addr = NULL;
    free(ctx);
}
