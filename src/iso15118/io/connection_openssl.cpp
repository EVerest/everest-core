// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#include <iso15118/io/connection_ssl.hpp>

#include <cassert>
#include <cstring>
#include <filesystem>
#include <unistd.h>

#include <sys/socket.h>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include <iso15118/detail/helper.hpp>
#include <iso15118/detail/io/helper_openssl.hpp>
#include <iso15118/detail/io/socket_helper.hpp>
#include <iso15118/io/sdp_server.hpp>

namespace std {
template <> class default_delete<SSL> {
public:
    void operator()(SSL* ptr) const {
        ::SSL_free(ptr);
    }
};
template <> class default_delete<SSL_CTX> {
public:
    void operator()(SSL_CTX* ptr) const {
        ::SSL_CTX_free(ptr);
    }
};
} // namespace std

namespace iso15118::io {

static constexpr auto DEFAULT_SOCKET_BACKLOG = 4;

static int ex_data_idx;

struct SSLContext {
    std::unique_ptr<SSL_CTX> ssl_ctx;
    std::unique_ptr<SSL> ssl;
    int fd{-1};
    int accept_fd{-1};
    std::string interface_name;
    bool enable_key_logging{false};
    std::unique_ptr<io::TlsKeyLoggingServer> key_server;
};

static SSL_CTX* init_ssl(const config::SSLConfig& ssl_config) {

    // Note: openssl does not provide support for ECDH-ECDSA-AES128-SHA256 anymore
    static constexpr auto TLS1_2_CIPHERSUITES = "ECDHE-ECDSA-AES128-SHA256";
    static constexpr auto TLS1_3_CIPHERSUITES = "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256";

    const std::filesystem::path prefix(ssl_config.config_string);

    const SSL_METHOD* method = TLS_server_method();
    const auto ctx = SSL_CTX_new(method);

    if (ctx == nullptr) {
        log_and_raise_openssl_error("Failed in SSL_CTX_new()");
    }

    if (SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION) == 0) {
        log_and_raise_openssl_error("Failed in SSL_CTX_set_min_proto_version()");
    }

    if (SSL_CTX_set_cipher_list(ctx, TLS1_2_CIPHERSUITES) == 0) {
        log_and_raise_openssl_error("Failed in SSL_CTX_set_cipher_list()");
    }

    if (SSL_CTX_set_ciphersuites(ctx, TLS1_3_CIPHERSUITES) == 0) {
        log_and_raise_openssl_error("Failed in SSL_CTX_set_ciphersuites()");
    }

    // TODO(sl): Better cert path handling
    const std::filesystem::path certificate_chain_file_path = prefix / "client/cso/CPO_CERT_CHAIN.pem";

    if (SSL_CTX_use_certificate_chain_file(ctx, certificate_chain_file_path.c_str()) != 1) {
        log_and_raise_openssl_error("Failed in SSL_CTX_use_certificate_chain_file()");
    }

    // INFO: the password callback uses a non-const argument
    std::string pass_str = ssl_config.private_key_password;
    SSL_CTX_set_default_passwd_cb_userdata(ctx, pass_str.data());

    const std::filesystem::path private_key_file_path = prefix / "client/cso/SECC_LEAF.key";
    if (SSL_CTX_use_PrivateKey_file(ctx, private_key_file_path.c_str(), SSL_FILETYPE_PEM) != 1) {
        log_and_raise_openssl_error("Failed in SSL_CTX_use_PrivateKey_file()");
    }

    // TODO(sl): How switch verify mode to none if tls 1.2 is used?
    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);

    if (ssl_config.enable_tls_key_logging) {

        SSL_CTX_set_keylog_callback(ctx, [](const SSL* ssl, const char* line) {
            logf_info("TLS handshake keys: %s\n", line);

            auto& key_logging_server = *static_cast<io::TlsKeyLoggingServer*>(SSL_get_ex_data(ssl, ex_data_idx));

            if (key_logging_server.get_fd() == -1) {
                logf_warning("Error - key_logging_server fd is not available");
                return;
            }
            const auto result = key_logging_server.send(line);
            if (result != strlen(line)) {
                const auto error_msg = adding_err_msg("key_logging_server send() failed");
                logf_error(error_msg.c_str());
            }
        });
    }

    return ctx;
}

ConnectionSSL::ConnectionSSL(PollManager& poll_manager_, const std::string& interface_name_,
                             const config::SSLConfig& ssl_config) :
    poll_manager(poll_manager_), ssl(std::make_unique<SSLContext>()) {

    ssl->interface_name = interface_name_;
    ssl->enable_key_logging = ssl_config.enable_tls_key_logging;

    // Openssl stuff missing!
    const auto ssl_ctx = init_ssl(ssl_config);
    ssl->ssl_ctx = std::unique_ptr<SSL_CTX>(ssl_ctx);

    sockaddr_in6 address;
    if (not get_first_sockaddr_in6_for_interface(interface_name_, address)) {
        const auto msg = "Failed to get ipv6 socket address for interface " + interface_name_;
        log_and_throw(msg.c_str());
    }

    const auto address_name = sockaddr_in6_to_name(address);

    if (not address_name) {
        const auto msg =
            "Failed to determine string representation of ipv6 socket address for interface " + interface_name_;
        log_and_throw(msg.c_str());
    }

    // Todo(sl): Define constexpr -> 50000 is fixed?
    end_point.port = 50000;
    memcpy(&end_point.address, &address.sin6_addr, sizeof(address.sin6_addr));

    ssl->fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (ssl->fd == -1) {
        log_and_throw("Failed to create an ipv6 socket");
    }

    // before bind, set the port
    address.sin6_port = htobe16(end_point.port);

    int optval_tmp{1};
    const auto set_reuseaddr = setsockopt(ssl->fd, SOL_SOCKET, SO_REUSEADDR, &optval_tmp, sizeof(optval_tmp));
    if (set_reuseaddr == -1) {
        log_and_throw("setsockopt(SO_REUSEADDR) failed");
    }

    const auto set_reuseport = setsockopt(ssl->fd, SOL_SOCKET, SO_REUSEPORT, &optval_tmp, sizeof(optval_tmp));
    if (set_reuseport == -1) {
        log_and_throw("setsockopt(SO_REUSEPORT) failed");
    }

    const auto bind_result = bind(ssl->fd, reinterpret_cast<const struct sockaddr*>(&address), sizeof(address));
    if (bind_result == -1) {
        const auto error = "Failed to bind ipv6 socket to interface " + interface_name_;
        log_and_throw(error.c_str());
    }

    const auto listen_result = listen(ssl->fd, DEFAULT_SOCKET_BACKLOG);
    if (listen_result == -1) {
        log_and_throw("Listen on socket failed");
    }

    poll_manager.register_fd(ssl->fd, [this]() { this->handle_connect(); });
}

ConnectionSSL::~ConnectionSSL() = default;

void ConnectionSSL::set_event_callback(const ConnectionEventCallback& callback) {
    event_callback = callback;
}

Ipv6EndPoint ConnectionSSL::get_public_endpoint() const {
    return end_point;
}

void ConnectionSSL::write(const uint8_t* buf, size_t len) {
    assert(handshake_complete); // TODO(sl): Adding states?

    size_t writebytes = 0;
    const auto ssl_ptr = ssl->ssl.get();

    const auto ssl_write_result = SSL_write_ex(ssl_ptr, buf, len, &writebytes);

    if (ssl_write_result <= 0) {
        const auto ssl_err_raw = SSL_get_error(ssl_ptr, ssl_write_result);
        log_and_raise_openssl_error("Failed to SSL_write_ex(): " + std::to_string(ssl_err_raw));
    } else if (writebytes != len) {
        log_and_throw("Didn't complete to write");
    }
}

ReadResult ConnectionSSL::read(uint8_t* buf, size_t len) {
    assert(handshake_complete); // TODO(sl): Adding states?

    size_t readbytes = 0;
    const auto ssl_ptr = ssl->ssl.get();

    const auto ssl_read_result = SSL_read_ex(ssl_ptr, buf, len, &readbytes);

    if (ssl_read_result > 0) {
        const auto would_block = (readbytes < len);
        return {would_block, readbytes};
    }

    const auto ssl_error = SSL_get_error(ssl_ptr, ssl_read_result);

    if ((ssl_error == SSL_ERROR_WANT_READ) or (ssl_error == SSL_ERROR_WANT_WRITE)) {
        return {true, 0};
    }

    log_and_raise_openssl_error("Failed to SSL_read_ex(): " + std::to_string(ssl_error));

    return {false, 0};
}

void ConnectionSSL::handle_connect() {

    const auto peer = BIO_ADDR_new();
    ssl->accept_fd = BIO_accept_ex(ssl->fd, peer, BIO_SOCK_NONBLOCK);

    if (ssl->accept_fd < 0) {
        log_and_raise_openssl_error("Failed to BIO_accept_ex");
    }

    const auto ip = BIO_ADDR_hostname_string(peer, 1);
    const auto service = BIO_ADDR_service_string(peer, 1);

    logf_info("Incoming connection from [%s]:%s", ip, service);

    poll_manager.unregister_fd(ssl->fd);
    ::close(ssl->fd);

    call_if_available(event_callback, ConnectionEvent::ACCEPTED);

    ssl->ssl = std::unique_ptr<SSL>(SSL_new(ssl->ssl_ctx.get()));
    const auto socket_bio = BIO_new_socket(ssl->accept_fd, BIO_CLOSE);

    const auto ssl_ptr = ssl->ssl.get();

    SSL_set_bio(ssl_ptr, socket_bio, socket_bio);
    SSL_set_accept_state(ssl_ptr);
    SSL_set_app_data(ssl_ptr, this);

    if (ssl->enable_key_logging) {
        const auto port = std::stoul(service);
        ssl->key_server = std::make_unique<io::TlsKeyLoggingServer>(ssl->interface_name, port);

        // NOTE(sl): SSL_get_ex_new_index does not work with "ex data" because of const
        std::string tmp = "ex data";
        ex_data_idx = SSL_get_ex_new_index(0, tmp.data(), nullptr, nullptr, nullptr);

        SSL_set_ex_data(ssl_ptr, ex_data_idx, ssl->key_server.get());
    }

    poll_manager.register_fd(ssl->accept_fd, [this]() { this->handle_data(); });

    OPENSSL_free(ip);
    OPENSSL_free(service);

    BIO_ADDR_free(peer);
}

void ConnectionSSL::handle_data() {
    if (not handshake_complete) {
        const auto ssl_ptr = ssl->ssl.get();

        const auto ssl_handshake_result = SSL_accept(ssl_ptr);

        if (ssl_handshake_result <= 0) {

            const auto ssl_error = SSL_get_error(ssl_ptr, ssl_handshake_result);

            if ((ssl_error == SSL_ERROR_WANT_READ) or (ssl_error == SSL_ERROR_WANT_WRITE)) {
                return;
            }
            log_and_raise_openssl_error("Failed to SSL_accept(): " + std::to_string(ssl_error));
        } else {
            logf_info("Handshake complete!\n");

            handshake_complete = true;
            if (ssl->enable_key_logging) {
                ssl->key_server.reset();
            }

            call_if_available(event_callback, ConnectionEvent::OPEN);

            return;
        }
    }

    call_if_available(event_callback, ConnectionEvent::NEW_DATA);
}

void ConnectionSSL::close() {
    /* tear down TLS connection gracefully */
    logf_info("Closing TLS connection\n");

    const auto ssl_ptr = ssl->ssl.get();

    const auto ssl_close_result = SSL_shutdown(ssl_ptr); // TODO(sl): Correct shutdown handling

    if (ssl_close_result < 0) {
        const auto ssl_error = SSL_get_error(ssl_ptr, ssl_close_result);
        if ((ssl_error != SSL_ERROR_WANT_READ) and (ssl_error != SSL_ERROR_WANT_WRITE)) {
            log_and_raise_openssl_error("Failed to SSL_shutdown(): " + std::to_string(ssl_error));
        }
    }

    // TODO(sl): Test if correct

    ::close(ssl->accept_fd);
    poll_manager.unregister_fd(ssl->accept_fd);

    logf_info("TLS connection closed gracefully");

    call_if_available(event_callback, ConnectionEvent::CLOSED);
}

} // namespace iso15118::io
