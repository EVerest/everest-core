// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest
#include <iso15118/io/connection_ssl.hpp>

#include <cassert>
#include <cstring>
#include <filesystem>
#include <thread>
#include <tuple>
#include <unistd.h>

#include <arpa/inet.h>
#include <net/if.h>
#include <sys/socket.h>

#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

#include <iso15118/detail/helper.hpp>
#include <iso15118/detail/io/socket_helper.hpp>

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

static constexpr auto LINK_LOCAL_MULTICAST = "ff02::1";

static auto key_log_fd{-1};
static sockaddr_in6 key_log_address;

struct SSLContext {
    std::unique_ptr<SSL_CTX> ssl_ctx;
    std::unique_ptr<SSL> ssl;
};

static std::tuple<int, sockaddr_in6> create_udp_socket(const char* interface_name, uint16_t port) {
    int udp_socket = socket(AF_INET6, SOCK_DGRAM, 0);
    if (udp_socket < 0) {
        const auto error_msg = adding_err_msg("Could not create socket");
        logf(error_msg.c_str());
        return {udp_socket, {}};
    }

    // source setup

    // find port between 49152-65535
    auto could_bind = false;
    auto source_port = 49152;
    for (; source_port < 65535; source_port++) {
        sockaddr_in6 source_address = {AF_INET6, htons(source_port)};
        if (bind(udp_socket, reinterpret_cast<sockaddr*>(&source_address), sizeof(sockaddr_in6)) == 0) {
            could_bind = true;
            break;
        }
    }

    if (!could_bind) {
        const auto error_msg = adding_err_msg("Could not bind");
        logf(error_msg.c_str());
        return {-1, {}};
    }

    logf("UDP socket bound to source port: %u", source_port);

    const auto index = if_nametoindex(interface_name);
    auto mreq = ipv6_mreq{};
    mreq.ipv6mr_interface = index;
    if (inet_pton(AF_INET6, LINK_LOCAL_MULTICAST, &mreq.ipv6mr_multiaddr) <= 0) {
        const auto error_msg = adding_err_msg("Failed to setup multicast address");
        logf(error_msg.c_str());
        return {-1, {}};
    }
    if (setsockopt(udp_socket, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
        const auto error_msg = adding_err_msg("Could not add multicast group membership");
        logf(error_msg.c_str());
        return {-1, {}};
    }

    if (setsockopt(udp_socket, IPPROTO_IPV6, IPV6_MULTICAST_IF, &index, sizeof(index)) < 0) {
        const auto error_msg = adding_err_msg("Could not set interface name:" + std::string(interface_name));
        logf(error_msg.c_str());
    }

    sockaddr_in6 dest_address = {AF_INET6, htons(port)};
    if (inet_pton(AF_INET6, LINK_LOCAL_MULTICAST, &dest_address.sin6_addr) <= 0) {
        const auto error_msg = adding_err_msg("Failed to setup server address, reset key_log_fd");
        logf(error_msg.c_str());
        return {-1, {}};
    }

    return {udp_socket, dest_address};
}

ConnectionSSL::ConnectionSSL(PollManager& poll_manager_, const std::string& interface_name_,
                             const config::SSLConfig& ssl_config) :
    poll_manager(poll_manager_),
    interface_name(interface_name_),
    enable_ssl_logging(ssl_config.enable_ssl_logging),
    ssl(std::make_unique<SSLContext>()) {

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

    // Openssl stuff missing!
    init_ssl(ssl_config);

    fd = socket(AF_INET6, SOCK_STREAM, 0);
    if (fd == -1) {
        log_and_throw("Failed to create an ipv6 socket");
    }

    // before bind, set the port
    address.sin6_port = htobe16(end_point.port);

    const auto bind_result = bind(fd, reinterpret_cast<const struct sockaddr*>(&address), sizeof(address));
    if (bind_result == -1) {
        const auto error = "Failed to bind ipv6 socket to interface " + interface_name_;
        log_and_throw(error.c_str());
    }

    const auto listen_result = listen(fd, DEFAULT_SOCKET_BACKLOG);
    if (listen_result == -1) {
        log_and_throw("Listen on socket failed");
    }

    poll_manager.register_fd(fd, [this]() { this->handle_connect(); });
}

ConnectionSSL::~ConnectionSSL() = default;

void ConnectionSSL::set_event_callback(const ConnectionEventCallback& callback) {
    event_callback = callback;
}

Ipv6EndPoint ConnectionSSL::get_public_endpoint() const {
    return end_point;
}

void ConnectionSSL::init_ssl(const config::SSLConfig& ssl_config) {

    // Note: openssl does not provide support for ECDH-ECDSA-AES128-SHA256 anymore
    static constexpr auto TLS1_2_CIPHERSUITES = "ECDHE-ECDSA-AES128-SHA256";
    static constexpr auto TLS1_3_CIPHERSUITES = "TLS_AES_256_GCM_SHA384:TLS_CHACHA20_POLY1305_SHA256";

    const std::filesystem::path prefix(ssl_config.config_string);

    const SSL_METHOD* method = TLS_server_method();
    auto* ctx = SSL_CTX_new(method);

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

    if (ssl_config.enable_ssl_logging) {

        SSL_CTX_set_keylog_callback(ctx, [](const SSL* ssl, const char* line) {
            logf("TLS handshake keys: %s\n", line);

            if (key_log_fd == -1) {
                logf("Error - key_log_fd is not available");
                return;
            }

            const auto udp_send_result =
                sendto(key_log_fd, line, strlen(line), 0, reinterpret_cast<const sockaddr*>(&key_log_address),
                       sizeof(key_log_address));

            if (udp_send_result != strlen(line)) {
                const auto error_msg = adding_err_msg("UDP send() failed");
                logf(error_msg.c_str());
            }
        });
    }

    ssl->ssl_ctx = std::unique_ptr<SSL_CTX>(ctx);
}

void ConnectionSSL::write(const uint8_t* buf, size_t len) {
    assert(handshake_complete); // TODO(sl): Adding states?

    size_t writebytes = 0;

    const auto ssl_write_result = SSL_write_ex(ssl->ssl.get(), buf, len, &writebytes);

    if (ssl_write_result <= 0) {
        const auto ssl_err_raw = SSL_get_error(ssl->ssl.get(), ssl_write_result);
        log_and_raise_openssl_error("Failed to SSL_write_ex(): " + std::to_string(ssl_err_raw));
    } else if (writebytes != len) {
        log_and_throw("Didn't complete to write");
    }
}

ReadResult ConnectionSSL::read(uint8_t* buf, size_t len) {
    assert(handshake_complete); // TODO(sl): Adding states?

    size_t readbytes = 0;

    const auto ssl_read_result = SSL_read_ex(ssl->ssl.get(), buf, len, &readbytes);

    if (ssl_read_result > 0) {
        const auto would_block = (readbytes < len);
        return {would_block, readbytes};
    }

    const auto ssl_error = SSL_get_error(ssl->ssl.get(), ssl_read_result);

    if ((ssl_error == SSL_ERROR_WANT_READ) or (ssl_error == SSL_ERROR_WANT_WRITE)) {
        return {true, 0};
    }

    log_and_raise_openssl_error("Failed to SSL_read_ex(): " + std::to_string(ssl_error));

    return {false, 0};
}

void ConnectionSSL::handle_connect() {

    auto* peer = BIO_ADDR_new();
    accept_fd = BIO_accept_ex(fd, peer, BIO_SOCK_NONBLOCK);

    if (accept_fd < 0) {
        log_and_raise_openssl_error("Failed to BIO_accept_ex");
    }

    auto* ip = BIO_ADDR_hostname_string(peer, 1);
    auto* service = BIO_ADDR_service_string(peer, 1);

    logf("Incoming connection from [%s]:%s", ip, service);

    if (enable_ssl_logging) {
        const auto port = std::stoul(service);
        std::tie(key_log_fd, key_log_address) = create_udp_socket(interface_name.c_str(), port);
    }

    poll_manager.unregister_fd(fd);
    ::close(fd);

    call_if_available(event_callback, ConnectionEvent::ACCEPTED);

    ssl->ssl = std::unique_ptr<SSL>(SSL_new(ssl->ssl_ctx.get()));
    const auto socket_bio = BIO_new_socket(accept_fd, BIO_CLOSE);

    SSL_set_bio(ssl->ssl.get(), socket_bio, socket_bio);
    SSL_set_accept_state(ssl->ssl.get());
    SSL_set_app_data(ssl->ssl.get(), this);

    poll_manager.register_fd(accept_fd, [this]() { this->handle_data(); });

    OPENSSL_free(ip);
    OPENSSL_free(service);

    BIO_ADDR_free(peer);
}

void ConnectionSSL::handle_data() {
    if (not handshake_complete) {
        const auto ssl_handshake_result = SSL_accept(ssl->ssl.get());

        if (ssl_handshake_result <= 0) {

            const auto ssl_error = SSL_get_error(ssl->ssl.get(), ssl_handshake_result);

            if ((ssl_error == SSL_ERROR_WANT_READ) or (ssl_error == SSL_ERROR_WANT_WRITE)) {
                return;
            }
            log_and_raise_openssl_error("Failed to SSL_accept(): " + std::to_string(ssl_error));
        } else {
            logf("Handshake complete!\n");

            handshake_complete = true;
            if (enable_ssl_logging) {
                ::close(key_log_fd);
                key_log_fd = -1;
                key_log_address = {};
            }

            call_if_available(event_callback, ConnectionEvent::OPEN);

            return;
        }
    }

    call_if_available(event_callback, ConnectionEvent::NEW_DATA);
}

void ConnectionSSL::close() {
    /* tear down TLS connection gracefully */
    logf("Closing TLS connection\n");

    // Wait for 5 seconds [V2G20-1643]
    std::this_thread::sleep_for(std::chrono::seconds(5));

    const auto ssl_close_result = SSL_shutdown(ssl->ssl.get()); // TODO(sl): Correct shutdown handling

    if (ssl_close_result < 0) {
        const auto ssl_error = SSL_get_error(ssl->ssl.get(), ssl_close_result);
        if ((ssl_error != SSL_ERROR_WANT_READ) and (ssl_error != SSL_ERROR_WANT_WRITE)) {
            log_and_raise_openssl_error("Failed to SSL_shutdown(): " + std::to_string(ssl_error));
        }
    }

    // TODO(sl): Test if correct

    poll_manager.unregister_fd(accept_fd);

    logf("TLS connection closed gracefully");

    SSL_free(ssl->ssl.get());
    SSL_CTX_free(ssl->ssl_ctx.get());

    call_if_available(event_callback, ConnectionEvent::CLOSED);
}

} // namespace iso15118::io
