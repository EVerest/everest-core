// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#include <iso15118/io/connection_ssl.hpp>

#include <cassert>
#include <cinttypes>
#include <cstring>
#include <filesystem>

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/debug.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/net_sockets.h"
#include "mbedtls/ssl.h"

#include <iso15118/detail/helper.hpp>
#include <iso15118/detail/io/helper_mbedtls.hpp>
#include <iso15118/detail/io/socket_helper.hpp>

namespace std {
template <> class default_delete<mbedtls_ssl_context> {
public:
    void operator()(mbedtls_ssl_context* ptr) const {
        ::mbedtls_ssl_free(ptr);
    }
};
template <> class default_delete<mbedtls_net_context> {
public:
    void operator()(mbedtls_net_context* ptr) const {
        ::mbedtls_net_free(ptr);
    }
};
template <> class default_delete<mbedtls_ssl_config> {
public:
    void operator()(mbedtls_ssl_config* ptr) const {
        ::mbedtls_ssl_config_free(ptr);
    }
};
template <> class default_delete<mbedtls_x509_crt> {
public:
    void operator()(mbedtls_x509_crt* ptr) const {
        ::mbedtls_x509_crt_free(ptr);
    }
};
template <> class default_delete<mbedtls_ctr_drbg_context> {
public:
    void operator()(mbedtls_ctr_drbg_context* ptr) const {
        ::mbedtls_ctr_drbg_free(ptr);
    }
};
template <> class default_delete<mbedtls_entropy_context> {
public:
    void operator()(mbedtls_entropy_context* ptr) const {
        ::mbedtls_entropy_free(ptr);
    }
};
template <> class default_delete<mbedtls_pk_context> {
public:
    void operator()(mbedtls_pk_context* ptr) const {
        ::mbedtls_pk_free(ptr);
    }
};

} // namespace std

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
    std::string private_key_password = ssl_config.private_key_password.value_or(std::string());

    if (ssl_config.backend == config::CertificateBackend::JOSEPPA_LAYOUT) {
        const std::filesystem::path prefix(ssl_config.config_string);
        auto chain = &ssl.server_certificate;

        parse_crt_file(chain, prefix / "seccLeafCert.pem");
        parse_crt_file(chain, prefix / "cpoSubCA2Cert.pem");
        parse_crt_file(chain, prefix / "cpoSubCA1Cert.pem");
        parse_key_file(&ssl.pkey, prefix / "seccLeaf.key", private_key_password, &ssl.ctr_drbg);
    } else if (ssl_config.backend == config::CertificateBackend::EVEREST_LAYOUT) {
        auto chain = &ssl.server_certificate;
        // mbedtls knows how to parse multi-PEM files
        parse_crt_file(chain, ssl_config.path_certificate_chain);
        parse_key_file(&ssl.pkey, ssl_config.path_certificate_key, private_key_password, &ssl.ctr_drbg);
    }
}

ConnectionSSL::ConnectionSSL(PollManager& poll_manager_, const std::string& interface_name,
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

    end_point.port = 50000;
    memcpy(&end_point.address, &address.sin6_addr, sizeof(address.sin6_addr));

    const auto address_name = sockaddr_in6_to_name(address);

    if (not address_name) {
        const auto msg =
            "Failed to determine string representation of ipv6 socket address for interface " + interface_name;
        log_and_throw(msg.c_str());
    }

    logf_info("Start TLS server [%s]:%" PRIu16, address_name.get(), end_point.port);

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

ConnectionSSL::~ConnectionSSL() = default;

void ConnectionSSL::set_event_callback(const ConnectionEventCallback& callback) {
    this->event_callback = callback;
}

Ipv6EndPoint ConnectionSSL::get_public_endpoint() const {
    return end_point;
}

void ConnectionSSL::write(const uint8_t* buf, size_t len) {
    assert(handshake_complete);

    const auto ssl_write_result = mbedtls_ssl_write(&ssl->ssl, buf, len);

    if (ssl_write_result < 0) {
        log_and_raise_mbed_error("Failed to mbedtls_ssl_write()", ssl_write_result);
    } else if (ssl_write_result != len) {
        log_and_throw("Didn't complete to write");
    }
}

ReadResult ConnectionSSL::read(uint8_t* buf, size_t len) {
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

void ConnectionSSL::handle_connect() {
    const auto accept_result =
        mbedtls_net_accept(&ssl->accepting_net_ctx, &ssl->connection_net_ctx, nullptr, 0, nullptr);
    if (accept_result != 0) {
        log_and_raise_mbed_error("Failed to mbedtls_net_accept()", accept_result);
    }

    // setup callbacks for communcation
    mbedtls_ssl_set_bio(&ssl->ssl, &ssl->connection_net_ctx, mbedtls_net_send, mbedtls_net_recv, NULL);

    // FIXME (aw): is it okay (according to iso15118 and mbedtls) to close the accepting socket here?
    // NOTE (sl): Closed when the SSLContext object is deleted by the default_delete
    poll_manager.unregister_fd(ssl->accepting_net_ctx.fd);

    call_if_available(event_callback, ConnectionEvent::ACCEPTED);

    poll_manager.register_fd(ssl->connection_net_ctx.fd, [this]() { this->handle_data(); });
}

void ConnectionSSL::handle_data() {
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
            logf_info("Handshake complete!");

            handshake_complete = true;

            call_if_available(event_callback, ConnectionEvent::OPEN);

            return;
        }
    }

    call_if_available(event_callback, ConnectionEvent::NEW_DATA);
}

void ConnectionSSL::close() {

    /* tear down TLS connection gracefully */
    logf_info("Closing TLS connection");

    poll_manager.unregister_fd(ssl->connection_net_ctx.fd);

    const auto ssl_close_result = mbedtls_ssl_close_notify(&ssl->ssl);

    if (ssl_close_result != 0) {
        if ((ssl_close_result != MBEDTLS_ERR_SSL_WANT_READ) and (ssl_close_result != MBEDTLS_ERR_SSL_WANT_WRITE)) {
            log_and_raise_mbed_error("Failed to mbedtls_ssl_close_notify()", ssl_close_result);
        }
    } else {
        logf_info("TLS connection closed gracefully");
    }

    call_if_available(event_callback, ConnectionEvent::CLOSED);
}

} // namespace iso15118::io
