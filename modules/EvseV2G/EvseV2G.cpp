// SPDX-License-Identifier: Apache-2.0
// Copyright (C) 2022-2023 chargebyte GmbH
// Copyright (C) 2022-2023 Contributors to EVerest
#include "EvseV2G.hpp"
#include "connection/connection.hpp"
#include "connection/tls_connection.hpp"
#include "log.hpp"
#include "sdp.hpp"
#include <everest/logging.hpp>

#include <csignal>
#include <openssl_util.hpp>
namespace {
void log_handler(openssl::log_level_t level, const std::string& str) {
    switch (level) {
    case openssl::log_level_t::debug:
        // ignore debug logs
        break;
    case openssl::log_level_t::info:
        EVLOG_info << str;
        break;
    case openssl::log_level_t::warning:
        EVLOG_warning << str;
        break;
    case openssl::log_level_t::error:
    default:
        EVLOG_error << str;
        break;
    }
}
} // namespace

struct v2g_context* v2g_ctx = nullptr;

namespace module {

void EvseV2G::init() {
    /* create v2g context */
    v2g_ctx = v2g_ctx_create(&(*p_charger), &(*p_extensions), &(*r_security));

    if (v2g_ctx == nullptr)
        return;

    (void)openssl::set_log_handler(log_handler);
    tls::Server::configure_signal_handler(SIGUSR1);
    v2g_ctx->tls_server = &tls_server;

    this->r_security->subscribe_certificate_store_update(
        [this](const types::evse_security::CertificateStoreUpdate& update) {
            if (!update.leaf_certificate_type.has_value()) {
                return;
            }

            if (update.leaf_certificate_type.value() != types::evse_security::LeafCertificateType::V2G) {
                return;
            }

            dlog(DLOG_LEVEL_INFO, "Certificate store update received, reconfiguring TLS server");
            auto config = std::make_unique<tls::Server::config_t>();
            if (build_config(*config, v2g_ctx)) {
                dlog(DLOG_LEVEL_INFO, "Configuration of TLS server successful, updating it");
                v2g_ctx->tls_server->update(*config);
            } else {
                dlog(DLOG_LEVEL_INFO, "Configuration of TLS server failed, suspending it");
                v2g_ctx->tls_server->suspend();
            }
        });

    invoke_init(*p_charger);
    invoke_init(*p_extensions);
}

void EvseV2G::ready() {
    int rv = 0;

    dlog(DLOG_LEVEL_DEBUG, "Starting SDP responder");

    rv = connection_init(v2g_ctx);

    if (rv == -1) {
        dlog(DLOG_LEVEL_ERROR, "Failed to initialize connection");
        goto err_out;
    }

    if (config.enable_sdp_server) {
        rv = sdp_init(v2g_ctx);

        if (rv == -1) {
            dlog(DLOG_LEVEL_ERROR, "Failed to start SDP responder");
            goto err_out;
        }
    }

    dlog(DLOG_LEVEL_DEBUG, "starting socket server(s)");
    if (connection_start_servers(v2g_ctx)) {
        dlog(DLOG_LEVEL_ERROR, "start_connection_servers() failed");
        goto err_out;
    }

    invoke_ready(*p_charger);
    invoke_ready(*p_extensions);

    rv = sdp_listen(v2g_ctx);

    if (rv == -1) {
        dlog(DLOG_LEVEL_ERROR, "sdp_listen() failed");
        goto err_out;
    }

    return;

err_out:
    v2g_ctx_free(v2g_ctx);
}

EvseV2G::~EvseV2G() {
    v2g_ctx_free(v2g_ctx);
}

} // namespace module
