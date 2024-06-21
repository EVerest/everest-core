// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#ifndef TLS_CONNECTION_TEST_HPP_
#define TLS_CONNECTION_TEST_HPP_

#include <extensions/helpers.hpp>
#include <openssl_util.hpp>

#include <EnumFlags.hpp>
#include <array>
#include <chrono>
#include <csignal>
#include <gtest/gtest.h>
#include <memory>
#include <openssl/ssl.h>
#include <thread>
#include <tls.hpp>
#include <unistd.h>

using namespace std::chrono_literals;

namespace {
using tls::status_request::ClientStatusRequestV2;

// ----------------------------------------------------------------------------
// set up code

struct ClientStatusRequestV2Test : public ClientStatusRequestV2 {
    enum class flags_t : std::uint8_t {
        status_request_cb,
        status_request,
        status_request_v2,
        connected,
        last = connected,
    };

    util::AtomicEnumFlags<flags_t, std::uint8_t>& flags;

    ClientStatusRequestV2Test() = delete;
    explicit ClientStatusRequestV2Test(util::AtomicEnumFlags<flags_t, std::uint8_t>& flag_ref) : flags(flag_ref) {
    }

    int status_request_cb(tls::Ssl* ctx) override {
        /*
         * This callback is called when status_request or status_request_v2 extensions
         * were present in the Client Hello. It doesn't mean that the extension is in
         * the Server Hello SSL_get_tlsext_status_ocsp_resp() returns -1 in that case
         */

        int result{1};
        const unsigned char* response{nullptr};
        const auto total_length = SSL_get_tlsext_status_ocsp_resp(ctx, &response);
        flags.set(flags_t::status_request_cb);
        if ((response != nullptr) && (total_length > 0) && (total_length <= std::numeric_limits<std::int32_t>::max())) {
            switch (response[0]) {
            case 0x30: // a status_request response
                flags.set(flags_t::status_request);
                if (!print_ocsp_response(stdout, response, total_length)) {
                    result = 0;
                }
                break;
            case 0x00: // a status_request_v2 response
            {
                flags.set(flags_t::status_request_v2);

                // multiple responses
                auto remaining = static_cast<std::int32_t>(total_length);
                const unsigned char* ptr{response};

                while (remaining >= 3) {
                    const auto len = tls::uint24(ptr);
                    tls::update_position(ptr, remaining, 3);
                    // print_ocsp_response updates tmp_p
                    auto* tmp_p = ptr;
                    const auto res = print_ocsp_response(stdout, tmp_p, len);
                    tls::update_position(ptr, remaining, len);
                    if (!res || (ptr != tmp_p)) {
                        result = 0;
                        remaining = -1;
                    }
                }

                if (remaining != 0) {
                    result = 0;
                }
                break;
            }
            default:
                break;
            }
        }
        return result;
    }
};

struct ClientTest : public tls::Client {
    using flags_t = ClientStatusRequestV2Test::flags_t;
    util::AtomicEnumFlags<flags_t, std::uint8_t> flags;

    ClientTest() : tls::Client(std::unique_ptr<ClientStatusRequestV2>(new ClientStatusRequestV2Test(flags))) {
    }

    void reset() {
        flags.reset();
    }
};

void handler(std::shared_ptr<tls::ServerConnection>& con) {
    if (con->accept()) {
        std::uint32_t count{0};
        std::array<std::byte, 1024> buffer{};
        bool bExit = false;
        while (!bExit) {
            std::size_t readbytes = 0;
            std::size_t writebytes = 0;

            switch (con->read(buffer.data(), buffer.size(), readbytes)) {
            case tls::Connection::result_t::success:
                switch (con->write(buffer.data(), readbytes, writebytes)) {
                case tls::Connection::result_t::success:
                    break;
                case tls::Connection::result_t::timeout:
                case tls::Connection::result_t::error:
                default:
                    bExit = true;
                    break;
                }
                break;
            case tls::Connection::result_t::timeout:
                count++;
                if (count > 10) {
                    bExit = true;
                }
                break;
            case tls::Connection::result_t::error:
            default:
                bExit = true;
                break;
            }
        }
        con->shutdown();
    }
}

void run_server(tls::Server& server) {
    server.serve(&handler);
}

class TlsTest : public testing::Test {
protected:
    using flags_t = ClientTest::flags_t;

    tls::Server server;
    tls::Server::config_t server_config;
    std::thread server_thread;
    ClientTest client;
    tls::Client::config_t client_config;

    static void SetUpTestSuite() {
        struct sigaction action;
        std::memset(&action, 0, sizeof(action));
        action.sa_handler = SIG_IGN;
        sigaction(SIGPIPE, &action, nullptr);
        tls::Server::configure_signal_handler(SIGUSR1);
#if 0
        // see gtest_main.cpp
        if (std::system("./pki.sh > /dev/null") != 0) {
            std::cerr << "Problem creating test certificates and keys" << std::endl;
            char buf[PATH_MAX];
            if (getcwd(&buf[0], sizeof(buf)) != nullptr) {
                std::cerr << "./pki.sh not found in " << buf << std::endl;
            }
            exit(1);
        }
#endif
    }

    void SetUp() override {
        server_config.cipher_list = "ECDHE-ECDSA-AES128-SHA256";
        // server_config.ciphersuites = "TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384";
        server_config.ciphersuites = "";
        server_config.chains.emplace_back();
        server_config.chains[0].certificate_chain_file = "server_chain.pem";
        server_config.chains[0].private_key_file = "server_priv.pem";
        server_config.chains[0].trust_anchor_file = "server_root_cert.pem";
        server_config.chains[0].ocsp_response_files = {"ocsp_response.der", "ocsp_response.der"};
        server_config.chains.emplace_back();
        server_config.chains[1].certificate_chain_file = "alt_server_chain.pem";
        server_config.chains[1].private_key_file = "alt_server_priv.pem";
        server_config.chains[1].trust_anchor_file = "alt_server_root_cert.pem";
        server_config.chains[1].ocsp_response_files = {"ocsp_response.der", "ocsp_response.der"};
        // server_config.verify_locations_file = "client_root_cert.pem";
        server_config.host = "localhost";
        server_config.service = "8444";
        server_config.ipv6_only = false;
        server_config.verify_client = false;
        server_config.io_timeout_ms = 500; // no lower than 200ms

        client_config.cipher_list = "ECDHE-ECDSA-AES128-SHA256";
        // client_config.ciphersuites = "TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384";
        // client_config.certificate_chain_file = "client_chain.pem";
        // client_config.private_key_file = "client_priv.pem";
        client_config.verify_locations_file = "server_root_cert.pem";
        client_config.io_timeout_ms = 200;
        client_config.verify_server = true;
        client_config.status_request = false;
        client_config.status_request_v2 = false;
        client.reset();
    }

    void TearDown() override {
        server.stop();
        server.wait_stopped();
        if (server_thread.joinable()) {
            server_thread.join();
        }
    }

    void start(const std::function<bool(tls::Server& server)>& init_ssl = nullptr) {
        using state_t = tls::Server::state_t;
        const auto res = server.init(server_config, init_ssl);
        if ((res == state_t::init_complete) || (res == state_t::init_socket)) {
            server_thread = std::thread(&run_server, std::ref(server));
            server.wait_running();
        }
    }

    void start(const std::function<void(std::shared_ptr<tls::ServerConnection>& con)>& handler) {
        using state_t = tls::Server::state_t;
        const auto res = server.init(server_config, nullptr);
        if ((res == state_t::init_complete) || (res == state_t::init_socket)) {
            server_thread = std::thread([this, handler]() { this->server.serve(handler); });
            server.wait_running();
        }
    }

    void connect(const std::function<void(std::unique_ptr<tls::ClientConnection>& con)>& handler = nullptr) {
        client.init(client_config);
        client.reset();
        // localhost works in some cases but not in the CI pipeline for IPv6
        // use ip6-localhost
        auto connection = client.connect("localhost", "8444", false, 1000);
        if (handler == nullptr) {
            if (connection) {
                if (connection->connect()) {
                    set(ClientTest::flags_t::connected);
                    connection->shutdown();
                }
            }
        } else {
            handler(connection);
        }
    }

    void set(flags_t flag) {
        client.flags.set(flag);
    }

    [[nodiscard]] bool is_set(flags_t flag) const {
        return client.flags.is_set(flag);
    }

    [[nodiscard]] bool is_reset(flags_t flag) const {
        return client.flags.is_reset(flag);
    }

    void add_ta_cert_hash(const char* filename) {
        openssl::sha_1_digest_t digest;
        auto certs = openssl::load_certificates(filename);
        for (const auto& ta : certs) {
            if (openssl::certificate_sha_1(digest, ta.get())) {
                client_config.trusted_ca_keys_data.cert_sha1_hash.push_back(digest);
            }
        }
    }
    void add_ta_key_hash(const char* filename) {
        openssl::sha_1_digest_t digest;
        auto certs = openssl::load_certificates(filename);
        for (const auto& ta : certs) {
            if (openssl::certificate_subject_public_key_sha_1(digest, ta.get())) {
                client_config.trusted_ca_keys_data.key_sha1_hash.push_back(digest);
            }
        }
    }
    void add_ta_name(const char* filename) {
        auto certs = openssl::load_certificates(filename);
        for (const auto& ta : certs) {
            auto res = openssl::certificate_subject_der(ta.get());
            if (res) {
                client_config.trusted_ca_keys_data.x509_name.emplace_back(std::move(res));
            }
        }
    }
};

} // namespace
#endif // TLS_CONNECTION_TEST_HPP_
