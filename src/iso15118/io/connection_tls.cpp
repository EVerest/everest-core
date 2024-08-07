// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#include <iso15118/io/connection_tls.hpp>

#include <cassert>
#include <cstring>
#include <filesystem>
#include <thread>

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"

#include <iso15118/detail/helper.hpp>
#include <iso15118/detail/io/socket_helper.hpp>

namespace iso15118::io {

struct SSLContext {
    SSLContext();

    mbedtls_net_context accepting_net_ctx;
    mbedtls_net_context connection_net_ctx;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_x509_crt server_certificate;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_entropy_context entropy;
    mbedtls_pk_context pkey;
};

SSLContext::SSLContext() {
    mbedtls_net_init(&accepting_net_ctx);
    mbedtls_net_init(&connection_net_ctx);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_x509_crt_init(&server_certificate);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_entropy_init(&entropy);
    mbedtls_pk_init(&pkey);
};

static void parse_crt_file(mbedtls_x509_crt* chain, const std::filesystem::path& path) {
    const auto x509_crt_parse_result = mbedtls_x509_crt_parse_file(chain, path.c_str());
    if (x509_crt_parse_result != 0) {
        const std::string error = "Failed in mbedtls_x509_crt_parse_file() with file " + path.string();
        log_and_raise_mbed_error(error.c_str(), x509_crt_parse_result);
    }
}

static void parse_key_file(mbedtls_pk_context* pk_context, const std::filesystem::path& path,
                           const std::string& password, mbedtls_ctr_drbg_context* drbg_context) {
    const auto password_ptr = (password.length() == 0) ? nullptr : password.c_str();
    const auto parse_key_result =
#if MBEDTLS_VERSION_MAJOR == 3
        mbedtls_pk_parse_keyfile(pk_context, path.c_str(), password_ptr, mbedtls_ctr_drbg_random, drbg_context);
#else
        mbedtls_pk_parse_keyfile(pk_context, path.c_str(), password_ptr);
#endif

    if (parse_key_result != 0) {
        const std::string error = "Failed in mbedtls_pk_parse_keyfile() with file " + path.string();
        log_and_raise_mbed_error(error.c_str(), parse_key_result);
    }
}

static void load_certificates(SSLContext& ssl, const config::SSLConfig& ssl_config) {
    if (ssl_config.backend == config::CertificateBackend::JOSEPPA_LAYOUT) {
        const std::filesystem::path prefix(ssl_config.config_string);
        auto chain = &ssl.server_certificate;

        parse_crt_file(chain, prefix / "seccLeafCert.pem");
        parse_crt_file(chain, prefix / "cpoSubCA2Cert.pem");
        parse_crt_file(chain, prefix / "cpoSubCA1Cert.pem");
        parse_key_file(&ssl.pkey, prefix / "seccLeaf.key", ssl_config.private_key_password, &ssl.ctr_drbg);
    } else if (ssl_config.backend == config::CertificateBackend::EVEREST_LAYOUT) {
        const std::filesystem::path prefix(ssl_config.config_string);

        auto chain = &ssl.server_certificate;
        parse_crt_file(chain, prefix / "client/cso/SECC_LEAF.pem");
        parse_crt_file(chain, prefix / "ca/cso/CPO_SUB_CA2.pem");
        parse_crt_file(chain, prefix / "ca/cso/CPO_SUB_CA1.pem");
        parse_key_file(&ssl.pkey, prefix / "client/cso/SECC_LEAF.key", ssl_config.private_key_password, &ssl.ctr_drbg);
    }
}

ConnectionTLS::ConnectionTLS(PollManager& poll_manager_, const std::string& interface_name,
                             const config::SSLConfig& ssl_config) :
    poll_manager(poll_manager_), ssl(std::make_unique<SSLContext>()) {

#if MBEDTLS_VERSION_MAJOR == 3
    // FIXME (aw): psa stuff?
    psa_crypto_init();
#endif

    sockaddr_in6 address;
    if (not get_first_sockaddr_in6_for_interface(interface_name, address)) {
        const auto msg = "Failed to get ipv6 socket address for interface " + interface_name;
        log_and_throw(msg.c_str());
    }

    const auto address_name = sockaddr_in6_to_name(address);

    if (not address_name) {
        const auto msg =
            "Failed to determine string representation of ipv6 socket address for interface " + interface_name;
        log_and_throw(msg.c_str());
    }

    end_point.port = 50000;
    memcpy(&end_point.address, &address.sin6_addr, sizeof(address.sin6_addr));

    //
    // mbedtls specifica
    //

    // initialize pseudo random number generator
    // FIXME (aw): what is the custom argument about?
    const auto ctr_drbg_seed_result =
        mbedtls_ctr_drbg_seed(&ssl->ctr_drbg, mbedtls_entropy_func, &ssl->entropy, nullptr, 0);
    if (ctr_drbg_seed_result != 0) {
        log_and_raise_mbed_error("Failed in mbedtls_ctr_drbg_seed()", ctr_drbg_seed_result);
    }

    // load certificate
    load_certificates(*ssl, ssl_config);

    const auto bind_result = mbedtls_net_bind(&ssl->accepting_net_ctx, address_name.get(),
                                              std::to_string(end_point.port).c_str(), MBEDTLS_NET_PROTO_TCP);
    if (bind_result != 0) {
        log_and_raise_mbed_error("Failed to mbedtls_net_bind()", bind_result);
    }

    // setup ssl context configuration
    const auto ssl_config_result = mbedtls_ssl_config_defaults(
        &ssl->conf, MBEDTLS_SSL_IS_SERVER, MBEDTLS_SSL_TRANSPORT_STREAM, MBEDTLS_SSL_PRESET_DEFAULT);
    if (ssl_config_result != 0) {
        log_and_raise_mbed_error("Failed to mbedtls_ssl_config_defaults()", ssl_config_result);
    }

    mbedtls_ssl_conf_rng(&ssl->conf, mbedtls_ctr_drbg_random, &ssl->ctr_drbg);

    mbedtls_ssl_conf_dbg(
        &ssl->conf,
        [](void* callback_context, int debug_level, const char* file_name, int line_number, const char* message) {
            logf_debug("mbedtls debug (level: %d) - %s\n", debug_level, message);
        },
        stdout);

    if (ssl_config.enable_ssl_logging) {
        mbedtls_debug_set_threshold(3);
    }

    mbedtls_ssl_conf_ca_chain(&ssl->conf, ssl->server_certificate.next, nullptr);
    const auto own_cert_result = mbedtls_ssl_conf_own_cert(&ssl->conf, &ssl->server_certificate, &ssl->pkey);
    if (own_cert_result != 0) {
        log_and_raise_mbed_error("Failed to mbedtls_ssl_conf_own_cert()", own_cert_result);
    }

    const auto ssl_setup_result = mbedtls_ssl_setup(&ssl->ssl, &ssl->conf);
    if (ssl_setup_result != 0) {
        log_and_raise_mbed_error("Failed to mbedtls_ssl_setup()", ssl_setup_result);
    }

    poll_manager.register_fd(ssl->accepting_net_ctx.fd, [this]() { this->handle_connect(); });
}

ConnectionTLS::~ConnectionTLS() = default;

void ConnectionTLS::set_event_callback(const ConnectionEventCallback& callback) {
    this->event_callback = callback;
}

Ipv6EndPoint ConnectionTLS::get_public_endpoint() const {
    return end_point;
}

void ConnectionTLS::write(const uint8_t* buf, size_t len) {
    assert(handshake_complete);

    const auto ssl_write_result = mbedtls_ssl_write(&ssl->ssl, buf, len);

    if (ssl_write_result < 0) {
        log_and_raise_mbed_error("Failed to mbedtls_ssl_write()", ssl_write_result);
    } else if (ssl_write_result != len) {
        log_and_throw("Didn't complete to write");
    }
}

ReadResult ConnectionTLS::read(uint8_t* buf, size_t len) {
    // FIXME (aw): any assert best practices?
    assert(handshake_complete);

    const auto ssl_read_result = mbedtls_ssl_read(&ssl->ssl, buf, len);

    if (ssl_read_result > 0) {
        size_t bytes_read = ssl_read_result;
        const auto would_block = (bytes_read < len);
        return {would_block, bytes_read};
    }

    if ((ssl_read_result == MBEDTLS_ERR_SSL_WANT_READ) or (ssl_read_result == MBEDTLS_ERR_SSL_WANT_WRITE)) {
        return {true, 0};
    }

    // FIXME (aw): error handling, when connection closed, i.e. ssl_read_result == 0
    log_and_raise_mbed_error("Failed to mbedtls_ssl_read()", ssl_read_result);
    return {false, 0};
}

void ConnectionTLS::handle_connect() {
    const auto accept_result =
        mbedtls_net_accept(&ssl->accepting_net_ctx, &ssl->connection_net_ctx, nullptr, 0, nullptr);
    if (accept_result != 0) {
        log_and_raise_mbed_error("Failed to mbedtls_net_accept()", accept_result);
    }

    // setup callbacks for communcation
    mbedtls_ssl_set_bio(&ssl->ssl, &ssl->connection_net_ctx, mbedtls_net_send, mbedtls_net_recv, NULL);

    // FIXME (aw): is it okay (according to iso15118 and mbedtls) to close the accepting socket here?
    poll_manager.unregister_fd(ssl->accepting_net_ctx.fd);
    mbedtls_net_free(&ssl->accepting_net_ctx);

    publish_event(ConnectionEvent::ACCEPTED);

    poll_manager.register_fd(ssl->connection_net_ctx.fd, [this]() { this->handle_data(); });
}

void ConnectionTLS::handle_data() {
    if (not handshake_complete) {
        // FIXME (aw): proper handshake handling (howto?)
        const auto ssl_handshake_result = mbedtls_ssl_handshake(&ssl->ssl);

        if (ssl_handshake_result != 0) {
            if ((ssl_handshake_result == MBEDTLS_ERR_SSL_WANT_READ) or
                (ssl_handshake_result == MBEDTLS_ERR_SSL_WANT_WRITE)) {
                // assuming we need more data, so just return
                return;
            }

            log_and_raise_mbed_error("Failed to mbedtls_ssl_handshake()", ssl_handshake_result);
        } else {
            // handshake complete!
            logf_info("Handshake complete!\n");

            handshake_complete = true;

            publish_event(ConnectionEvent::OPEN);

            return;
        }
    }

    publish_event(ConnectionEvent::NEW_DATA);
}

void ConnectionTLS::close() {

    /* tear down TLS connection gracefully */
    logf_info("Closing TLS connection\n");

    // Wait for 5 seconds [V2G20-1643]
    std::this_thread::sleep_for(std::chrono::seconds(5));

    poll_manager.unregister_fd(ssl->connection_net_ctx.fd);

    const auto ssl_close_result = mbedtls_ssl_close_notify(&ssl->ssl);

    if (ssl_close_result != 0) {
        if ((ssl_close_result != MBEDTLS_ERR_SSL_WANT_READ) and (ssl_close_result != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            log_and_raise_mbed_error("Failed to mbedtls_ssl_close_notify()", ssl_close_result);
        }
    } else {
        logf_info("TLS connection closed gracefully\n");
    }

    publish_event(ConnectionEvent::CLOSED);

    mbedtls_net_free(&ssl->connection_net_ctx);

    mbedtls_ssl_free(&ssl->ssl);
}

} // namespace iso15118::io
