// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include "tls_connection.hpp"
#include "connection.hpp"
#include "log.hpp"
#include "v2g.hpp"
#include "v2g_server.hpp"
#include <tls.hpp>

#include <cassert>
#include <cerrno>
#include <cstring>
#include <ctime>
#include <iostream>
#include <memory>
#include <sys/types.h>
#include <thread>

#ifdef EVEREST_MBED_TLS
namespace tls {
int connection_init(struct v2g_context* ctx) {
    return -1;
}
int connection_start_servers(struct v2g_context* ctx) {
    return -1;
}
ssize_t connection_read(struct v2g_connection* conn, unsigned char* buf, std::size_t count) {
    return -1;
}
ssize_t connection_write(struct v2g_connection* conn, unsigned char* buf, std::size_t count) {
    return -1;
}
} // namespace tls

#else // EVEREST_MBED_TLS

namespace {

void process_connection_thread(std::shared_ptr<tls::ServerConnection> con, struct v2g_context* ctx) {
    assert(con != nullptr);
    assert(ctx != nullptr);

    openssl::PKey_ptr contract_public_key{nullptr, nullptr};
    auto connection = std::make_unique<v2g_connection>();
    connection->ctx = ctx;
    connection->is_tls_connection = true;
    connection->read = &tls::connection_read;
    connection->write = &tls::connection_write;
    connection->tls_connection = con.get();
    connection->pubkey = &contract_public_key;

    dlog(DLOG_LEVEL_INFO, "Incoming TLS connection");

    if (con->accept()) {
        // TODO(james-ctc) v2g_ctx->tls_key_logging

        if (ctx->state == 0) {
            const auto rv = ::v2g_handle_connection(connection.get());
            dlog(DLOG_LEVEL_INFO, "v2g_dispatch_connection exited with %d", rv);
        } else {
            dlog(DLOG_LEVEL_INFO, "%s", "Closing tls-connection. v2g-session is already running");
        }

        con->shutdown();
    }

    ::connection_teardown(connection.get());
}

void handle_new_connection_cb(std::shared_ptr<tls::ServerConnection> con, struct v2g_context* ctx) {
    assert(con != nullptr);
    assert(ctx != nullptr);
    // create a thread to process this connection
    try {
        std::thread connection_loop(process_connection_thread, con, ctx);
        connection_loop.detach();
    } catch (const std::system_error&) {
        // unable to start thread
        dlog(DLOG_LEVEL_ERROR, "pthread_create() failed: %s", strerror(errno));
        con->shutdown();
    }
}

void server_loop_thread(struct v2g_context* ctx) {
    assert(ctx != nullptr);
    assert(ctx->tls_server != nullptr);
    const auto res = ctx->tls_server->serve([ctx](auto con) { handle_new_connection_cb(con, ctx); });
    if (res != tls::Server::state_t::stopped) {
        dlog(DLOG_LEVEL_ERROR, "tls::Server failed to serve");
    }
}

bool build_config(tls::Server::config_t& config, struct v2g_context* ctx) {
    assert(ctx != nullptr);
    assert(ctx->r_security != nullptr);

    using types::evse_security::CaCertificateType;
    using types::evse_security::EncodingFormat;
    using types::evse_security::GetCertificateInfoStatus;
    using types::evse_security::LeafCertificateType;

    /*
     * libevse-security checks for an optional password and when one
     * isn't set is uses an empty string as the password rather than nullptr.
     * hence private keys are always encrypted.
     */

    bool bResult{false};

    config.cipher_list = "ECDHE-ECDSA-AES128-SHA256:ECDH-ECDSA-AES128-SHA256";
    config.ciphersuites = "";     // disable TLS 1.3
    config.verify_client = false; // contract certificate managed in-band in 15118-2

    // use the existing configured socket
    // TODO(james-ctc): switch to server socket init code otherwise there
    //                  may be issues with reinitialisation
    config.socket = ctx->tls_socket.fd;
    config.io_timeout_ms = static_cast<std::int32_t>(ctx->network_read_timeout_tls);

    // information from libevse-security
    const auto cert_info =
        ctx->r_security->call_get_leaf_certificate_info(LeafCertificateType::V2G, EncodingFormat::PEM, false);
    if (cert_info.status != GetCertificateInfoStatus::Accepted) {
        dlog(DLOG_LEVEL_ERROR, "Failed to read cert_info! Not Accepted");
    } else {
        if (cert_info.info) {
            const auto& info = cert_info.info.value();
            const auto cert_path = info.certificate.value_or("");
            const auto key_path = info.key;

            // workaround (see above libevse-security comment)
            const auto key_password = info.password.value_or("");

            config.certificate_chain_file = cert_path.c_str();
            config.private_key_file = key_path.c_str();
            config.private_key_password = key_password.c_str();

            if (info.ocsp) {
                for (const auto& ocsp : info.ocsp.value()) {
                    const char* file{nullptr};
                    if (ocsp.ocsp_path) {
                        file = ocsp.ocsp_path.value().c_str();
                    }
                    config.ocsp_response_files.push_back(file);
                }
            }

            bResult = true;
        } else {
            dlog(DLOG_LEVEL_ERROR, "Failed to read cert_info! Empty response");
        }
    }

    return bResult;
}

bool configure_ssl(tls::Server& server, struct v2g_context* ctx) {
    tls::Server::config_t config;
    bool bResult{false};

    dlog(DLOG_LEVEL_WARNING, "configure_ssl");

    // The config of interest is from Evse Security, no point in updating
    // config when there is a problem
    if (build_config(config, ctx)) {
        bResult = server.update(config);
    }

    return bResult;
}

} // namespace

namespace tls {

int connection_init(struct v2g_context* ctx) {
    using state_t = tls::Server::state_t;

    assert(ctx != nullptr);
    assert(ctx->tls_server != nullptr);
    assert(ctx->r_security != nullptr);

    int res{-1};
    tls::Server::config_t config;

    // build_config can fail due to issues with Evse Security,
    // this can be retried later. Not treated as an error.
    (void)build_config(config, ctx);

    // apply config
    ctx->tls_server->stop();
    ctx->tls_server->wait_stopped();
    const auto result = ctx->tls_server->init(config, [ctx](auto& server) { return configure_ssl(server, ctx); });
    if ((result == state_t::init_complete) || (result == state_t::init_socket)) {
        res = 0;
    }

    return res;
}

int connection_start_server(struct v2g_context* ctx) {
    assert(ctx != nullptr);
    assert(ctx->tls_server != nullptr);

    // only starts the TLS server

    int res = 0;
    try {
        ctx->tls_server->stop();
        ctx->tls_server->wait_stopped();
        if (ctx->tls_server->state() == tls::Server::state_t::stopped) {
            // need to re-initialise
            tls::connection_init(ctx);
        }
        std::thread serve_loop(server_loop_thread, ctx);
        serve_loop.detach();
        ctx->tls_server->wait_running();
    } catch (const std::system_error&) {
        // unable to start thread (caller logs failure)
        res = -1;
    }
    return res;
}

ssize_t connection_read(struct v2g_connection* conn, unsigned char* buf, const std::size_t count) {
    assert(conn != nullptr);
    assert(conn->tls_connection != nullptr);

    ssize_t result{0};
    std::size_t bytes_read{0};
    timespec ts_start{};

    if (clock_gettime(CLOCK_MONOTONIC, &ts_start) == -1) {
        dlog(DLOG_LEVEL_ERROR, "clock_gettime(ts_start) failed: %s", strerror(errno));
        result = -1;
    }

    while ((bytes_read < count) && (result >= 0)) {
        const std::size_t remaining = count - bytes_read;
        std::size_t bytes_in{0};
        auto* ptr = reinterpret_cast<std::byte*>(&buf[bytes_read]);

        switch (conn->tls_connection->read(ptr, remaining, bytes_in)) {
        case tls::Connection::result_t::success:
            bytes_read += bytes_in;
            break;
        case tls::Connection::result_t::timeout:
            break;
        case tls::Connection::result_t::error:
        default:
            result = -1;
            break;
        }

        if (conn->ctx->is_connection_terminated) {
            dlog(DLOG_LEVEL_ERROR, "Reading from tcp-socket aborted");
            conn->tls_connection->shutdown();
            result = -2;
        }

        if (::is_sequence_timeout(ts_start, conn->ctx)) {
            break;
        }
    }

    return (result < 0) ? result : static_cast<ssize_t>(bytes_read);
}

ssize_t connection_write(struct v2g_connection* conn, unsigned char* buf, std::size_t count) {
    assert(conn != nullptr);
    assert(conn->tls_connection != nullptr);

    ssize_t result{0};
    std::size_t bytes_written{0};

    while ((bytes_written < count) && (result >= 0)) {
        const std::size_t remaining = count - bytes_written;
        std::size_t bytes_out{0};
        const auto* ptr = reinterpret_cast<std::byte*>(&buf[bytes_written]);

        switch (conn->tls_connection->write(ptr, remaining, bytes_out)) {
        case tls::Connection::result_t::success:
            bytes_written += bytes_out;
            break;
        case tls::Connection::result_t::timeout:
            break;
        case tls::Connection::result_t::error:
        default:
            result = -1;
            break;
        }
    }

    if ((result == -1) && (conn->tls_connection->state() == tls::Connection::state_t::closed)) {
        // if the connection has closed - return the number of bytes sent
        result = 0;
    }

    return (result < 0) ? result : static_cast<ssize_t>(bytes_written);
}

} // namespace tls

#endif // EVEREST_MBED_TLS
