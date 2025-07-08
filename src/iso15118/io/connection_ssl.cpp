// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#include <iso15118/io/connection_ssl.hpp>

#include <algorithm>
#include <array>
#include <cassert>
#include <cinttypes>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <vector>

#include <sys/socket.h>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>

#include <iso15118/detail/helper.hpp>
#include <iso15118/detail/io/helper_ssl.hpp>
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

struct SSLContext {
    std::unique_ptr<SSL_CTX> ssl_ctx;
    std::unique_ptr<SSL> ssl;
    int fd{-1};
    int accept_fd{-1};
    std::string interface_name;
    bool enable_key_logging{false};
    std::filesystem::path tls_key_log_file_path{};
    std::unique_ptr<io::TlsKeyLoggingServer> key_server;
    bool enforce_tls_1_3{false};
    std::optional<sha512_hash_t> vehicle_cert_hash{std::nullopt};
};

namespace {

constexpr auto DEFAULT_SOCKET_BACKLOG = 4;
constexpr auto TLS_PORT = 50000;
constexpr auto NAME_LENGTH = 256;

int ssl_keylog_server_index{-1};
int ssl_keylog_file_index{-1};

std::string convert_ssl_tls_versions_to_string(uint16_t version) {
    switch (version) {
    case SSL3_VERSION:
        return "SSL3";
    case TLS1_VERSION:
        return "TLS1";
    case TLS1_1_VERSION:
        return "TLS1_1";
    case TLS1_2_VERSION:
        return "TLS1_2";
    case TLS1_3_VERSION:
        return "TLS1_3";
    default:
        return "Unknown";
    }
}

// The use of X509_NAME_oneline() function is strongly discouraged and could be deprecated in a future release. This is
// the reason for this wrapper.
auto x509_name_oneline(const X509_NAME* a, char* buf, int size) {
    return X509_NAME_oneline(a, buf, size);
}

// I found no get or callback function for the supported_versions extension, so I wrote my own parser.
bool is_tls_1_3(const uint8_t* data, std::size_t remaining) {
    if (data == nullptr) {
        return false;
    }

    uint8_t length_supported_versions = *(data++);
    remaining -= 1;

    if (length_supported_versions != remaining) {
        logf_error("length_supported_versions is not remaining");
        return false;
    }

    if (length_supported_versions % 2 != 0) {
        logf_error("length_supported_versions is not divisible by 2");
        return false;
    }

    // First Byte: length
    // Byte 2+3 -> First Version (03, 04)
    // Byte 4+5 -> Second Version (03, 03)
    // ....

    bool result{false};

    for (auto i = 0; i < length_supported_versions; i += 2) {
        const uint8_t first_byte = *(data++);
        const uint8_t second_byte = *(data++);

        const auto tls_version = first_byte << 8 | second_byte;

        if (tls_version == TLS1_3_VERSION) {
            result = true;
        }

        logf_debug("Client supported tls version: %s", convert_ssl_tls_versions_to_string(tls_version).c_str());
    }

    return result;
}

int client_hello_cb(SSL* ssl, int* /* alert */, void* /* object */) {

    const unsigned char* data;
    std::size_t datalen{0};

    if (SSL_client_hello_get0_ext(ssl, TLSEXT_TYPE_supported_versions, &data, &datalen)) {
        const auto tls_1_3_found = is_tls_1_3(data, datalen);

        if (tls_1_3_found) {
            logf_info("Client supports TLS1.3: Change verify mode to SSL_VERIFY_PEER and "
                      "SSL_VERIFY_FAIL_IF_NO_PEER_CERT");
            int mode = SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
            SSL_set_verify(ssl, mode, nullptr);
        }
    }

    if (SSL_client_hello_get0_ext(ssl, TLSEXT_TYPE_certificate_authorities, &data, &datalen)) {
        logf_info("Extension certificate_authorities found!");
        // TODO(SL): Setting var for handle_certificate_cb
    }

    return SSL_CLIENT_HELLO_SUCCESS;
}

int handle_certificate_cb(SSL* ssl, void* /* arg */) {

    // TODO(sl): Check only after names if the extension is there
    const STACK_OF(X509_NAME)* names = SSL_get0_peer_CA_list(ssl);

    if (names == NULL || sk_X509_NAME_num(names) == 0) {
        logf_error("No certificate CA names sent");
    } else {
        logf_info("Found certificate CA names!");

        for (auto i = 0; i < sk_X509_NAME_num(names); i++) {
            char name[NAME_LENGTH]{};
            x509_name_oneline(sk_X509_NAME_value(names, i), name, sizeof(name));
            logf_info("Name: %s", name);
        }

        // TODO(sl): What to do with the CA Names? Check if the charger has the secc_leaf from this CA Name?
    }

    // TODO(sl): Change leaf and chain certificate based on ca names

    return 1;
}

void keylog_callback(const SSL* ssl, const char* line) {
    auto key_logging_server = static_cast<io::TlsKeyLoggingServer*>(SSL_get_ex_data(ssl, ssl_keylog_server_index));

    std::string key_log_msg = "TLS Handshake keys on port ";
    key_log_msg += std::to_string(key_logging_server->get_port()) + ": ";
    key_log_msg += std::string(line);

    logf_info(key_log_msg.c_str());

    if (key_logging_server->get_fd() != -1) {
        const auto result = key_logging_server->send(line);
        if (not cmp_equal(result, strlen(line))) {
            const auto error_msg = adding_err_msg("key_logging_server send() failed");
            logf_error(error_msg.c_str());
        }
    }

    const auto keylog_file_path =
        static_cast<std::filesystem::path*>(SSL_CTX_get_ex_data(SSL_get_SSL_CTX(ssl), ssl_keylog_file_index));

    if (not keylog_file_path->empty()) {
        std::ofstream ofs;
        ofs.open(keylog_file_path->string(), std::ofstream::out | std::ofstream::app);
        ofs << line << std::endl;
        ofs.close();
    }
}

int private_key_callback(char* buf, int size, [[maybe_unused]] int rwflag, void* userdata) {
    const auto* password = static_cast<const std::string*>(userdata);
    const std::size_t max_pass_len = (size - 1); // we exclude the endline
    const std::size_t max_copy_chars =
        std::min(max_pass_len, password->length()); // truncate if pass is too large and buffer too small

    std::memset(buf, 0, size);
    std::memcpy(buf, password->c_str(), max_copy_chars);

    return max_copy_chars;
}

SSL_CTX* init_ssl(const config::SSLConfig& ssl_config) {

    // Note: openssl does not provide support for ECDH-ECDSA-AES128-SHA256 anymore
    static constexpr auto TLS1_2_CIPHERSUITES = "ECDHE-ECDSA-AES128-SHA256";
    static constexpr auto TLS1_3_CIPHERSUITES = "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256";

    const SSL_METHOD* method = TLS_server_method();
    const auto ctx = SSL_CTX_new(method);

    if (ctx == nullptr) {
        log_and_raise_openssl_error("Failed in SSL_CTX_new()");
    }

    const int result_set_min_proto_version = (ssl_config.enforce_tls_1_3)
                                                 ? SSL_CTX_set_min_proto_version(ctx, TLS1_3_VERSION)
                                                 : SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION);

    if (ssl_config.enforce_tls_1_3) {
        SSL_CTX_clear_options(ctx, SSL_OP_ENABLE_MIDDLEBOX_COMPAT);
    }

    if (result_set_min_proto_version == 0) {
        log_and_raise_openssl_error("Failed in SSL_CTX_set_min_proto_version()");
    }

    if (not ssl_config.enforce_tls_1_3 and SSL_CTX_set_cipher_list(ctx, TLS1_2_CIPHERSUITES) == 0) {
        log_and_raise_openssl_error("Failed in SSL_CTX_set_cipher_list()");
    }

    if (SSL_CTX_set_ciphersuites(ctx, TLS1_3_CIPHERSUITES) == 0) {
        log_and_raise_openssl_error("Failed in SSL_CTX_set_ciphersuites()");
    }

    if (SSL_CTX_use_certificate_chain_file(ctx, ssl_config.path_certificate_chain.c_str()) != 1) {
        log_and_raise_openssl_error("Failed in SSL_CTX_use_certificate_chain_file()");
    }

    // INFO: the password callback uses a non-const argument
    if (ssl_config.private_key_password.has_value()) {
        // Lifetime of the password is important because using a callback we'll require a valid ref
        SSL_CTX_set_default_passwd_cb_userdata(
            ctx, &const_cast<config::SSLConfig&>(ssl_config).private_key_password.value());
        SSL_CTX_set_default_passwd_cb(ctx, private_key_callback);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, ssl_config.path_certificate_key.c_str(), SSL_FILETYPE_PEM) != 1) {
        log_and_raise_openssl_error("Failed in SSL_CTX_use_PrivateKey_file()");
    }

    // Loading root certificates to verify client (only for tls 1.3)
    if (SSL_CTX_load_verify_file(ctx, ssl_config.path_certificate_v2g_root.c_str()) == 0) {
        logf_error("Verify V2G root not found!");
    }

    if (SSL_CTX_load_verify_file(ctx, ssl_config.path_certificate_mo_root.c_str()) == 0) {
        logf_error("Verify OEM root not found!");
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_NONE, nullptr);

    SSL_CTX_set_client_hello_cb(ctx, &client_hello_cb, nullptr);

    // TODO(SL): Adding multi root support with certificate_authorities extension
    // SSL_CTX_set_cert_cb(ctx, &handle_certificate_cb, nullptr);

    if (ssl_config.enable_tls_key_logging) {
        ssl_keylog_file_index = SSL_CTX_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);
        ssl_keylog_server_index = SSL_get_ex_new_index(0, nullptr, nullptr, nullptr, nullptr);

        if (ssl_keylog_file_index == -1 or ssl_keylog_server_index == -1) {
            auto error_msg = std::string("_get_ex_new_index failed: ssl_keylog_file_index: ");
            error_msg += std::to_string(ssl_keylog_file_index);
            error_msg += ", ssl_keylog_server_index: " + std::to_string(ssl_keylog_server_index);
            logf_error(error_msg.c_str());
        } else {
            SSL_CTX_set_keylog_callback(ctx, keylog_callback);
        }
    }

    return ctx;
}
} // namespace

ConnectionSSL::ConnectionSSL(PollManager& poll_manager_, const std::string& interface_name_,
                             const config::SSLConfig& ssl_config) :
    poll_manager(poll_manager_), ssl(std::make_unique<SSLContext>()) {

    ssl->interface_name = interface_name_;
    ssl->enable_key_logging = ssl_config.enable_tls_key_logging;
    ssl->enforce_tls_1_3 = ssl_config.enforce_tls_1_3;

    // Openssl stuff missing!
    const auto ssl_ctx = init_ssl(ssl_config);
    ssl->ssl_ctx = std::unique_ptr<SSL_CTX>(ssl_ctx);

    if (ssl_keylog_file_index != -1) {
        ssl->tls_key_log_file_path = ssl_config.tls_key_logging_path / "tls_session_keys.log";
        SSL_CTX_set_ex_data(ssl->ssl_ctx.get(), ssl_keylog_file_index, &ssl->tls_key_log_file_path);
    }
    sockaddr_in6 address;
    if (not get_first_sockaddr_in6_for_interface(interface_name_, address)) {
        const auto msg = "Failed to get ipv6 socket address for interface " + interface_name_;
        log_and_throw(msg.c_str());
    }

    end_point.port = TLS_PORT;
    memcpy(&end_point.address, &address.sin6_addr, sizeof(address.sin6_addr));

    const auto address_name = sockaddr_in6_to_name(address);

    if (not address_name) {
        const auto msg =
            "Failed to determine string representation of ipv6 socket address for interface " + interface_name_;
        log_and_throw(msg.c_str());
    }

    logf_info("Start TLS server [%s]:%" PRIu16, address_name.get(), end_point.port);

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

std::optional<sha512_hash_t> ConnectionSSL::get_vehicle_cert_hash() const {
    return ssl->vehicle_cert_hash;
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
        SSL_set_ex_data(ssl_ptr, ssl_keylog_server_index, ssl->key_server.get());
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
            logf_info("Handshake complete!");

            const auto peer = SSL_get0_peer_certificate(ssl_ptr);

            if (SSL_get_verify_mode(ssl_ptr) != SSL_VERIFY_NONE and peer) {

                const auto verify_result = SSL_get_verify_result(ssl_ptr);
                if (verify_result == X509_V_OK) {
                    logf_info("Verify certificate result is okay");
                    char name[NAME_LENGTH]{};
                    x509_name_oneline(X509_get_subject_name(peer), name, sizeof(name));
                    logf_debug("Peer subject name: %s", name);
                    name[0] = '\0';
                    x509_name_oneline(X509_get_issuer_name(peer), name, sizeof(name));
                    logf_debug("Peer issuer name: %s", name);

                    unsigned int length = 0;
                    auto& vehicle_hash = ssl->vehicle_cert_hash.emplace();
                    const auto result_digest = X509_digest(peer, EVP_sha512(), vehicle_hash.data(), &length);

                    if (result_digest) {
                        std::stringstream ss;
                        ss << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
                           << static_cast<int>(vehicle_hash[0]);
                        for (unsigned int i = 1; i < length; ++i) {
                            ss << ":" << std::uppercase << std::hex << std::setw(2) << std::setfill('0')
                               << (int)static_cast<int>(vehicle_hash[i]);
                        }
                        logf_debug("sha512 fingerprint: %s", ss.str().c_str());
                        // openssl command: openssl x509 -in *.pem -noout -fingerprint -sha512
                    } else {
                        logf_error("X509_digest failed");
                    }
                } else {
                    logf_error("Verify certificate result is not okay");
                }
            }

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
    logf_info("Closing TLS connection");

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
