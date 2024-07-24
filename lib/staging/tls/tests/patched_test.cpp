// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

/**
 * \file testing patched version of OpenSSL
 *
 * These tests will only pass on a patched version of OpenSSL.
 * (they should compile and run fine with some test failures)
 *
 * It is recommended to also run tests alongside Wireshark
 * e.g. `./patched_test --gtest_filter=OcspTest.TLS12`
 * to check that the Server Hello record is correctly formed:
 * - no status_request or status_request_v2 then no Certificate Status record
 * - status_request or status_request_v2 then there is a Certificate Status record
 * - never both status_request and status_request_v2
 */

#include <EnumFlags.hpp>
#include <csignal>
#include <gtest/gtest.h>
#include <openssl/ssl.h>
#include <thread>
#include <tls.hpp>
#include <unistd.h>

namespace {

// ----------------------------------------------------------------------------
// set up code

void log_handler(openssl::log_level_t level, const std::string& str) {
    switch (level) {
    case openssl::log_level_t::debug:
        std::cout << "DEBUG:   " << str << std::endl;
        break;
    case openssl::log_level_t::warning:
        std::cout << "WARN:    " << str << std::endl;
        break;
    case openssl::log_level_t::error:
        std::cerr << "ERROR:   " << str << std::endl;
        break;
    default:
        std::cerr << "Unknown: " << str << std::endl;
        break;
    }
}

struct ClientTest : public tls::Client {
    enum class flags_t {
        status_request_cb,
        status_request,
        status_request_v2,
        connected,
        last = connected,
    };
    util::AtomicEnumFlags<flags_t, std::uint8_t> flags;

    void reset() {
        flags.reset();
    }

    virtual int status_request_cb(tls::Ssl* ctx) {
        /*
         * This callback is called when status_request or status_request_v2 extensions
         * were present in the Client Hello. It doesn't mean that the extension is in
         * the Server Hello SSL_get_tlsext_status_ocsp_resp() returns -1 in that case
         */
        const unsigned char* response{nullptr};
        const auto total_length = SSL_get_tlsext_status_ocsp_resp(ctx, &response);
#if 0
        // could set a different flag to spot no extension
        if (total_length != -1) {
            // -1 is the extension isn't present
            flags.set(flags_t::status_request_cb);
        }
#else
        flags.set(flags_t::status_request_cb);
#endif
        if ((response != nullptr) && (total_length > 0)) {
            switch (response[0]) {
            case 0x30:
                flags.set(flags_t::status_request);
                break;
            case 0x00:
                flags.set(flags_t::status_request_v2);
                break;
            default:
                break;
            }
        }
        return 1;
        // return tls::Client::status_request_cb(ctx);
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

class OcspTest : public testing::Test {
protected:
    using flags_t = ClientTest::flags_t;

    tls::Server server;
    tls::Server::config_t server_config;
    std::thread server_thread;
    ClientTest client;
    tls::Client::config_t client_config;

    static void SetUpTestSuite() {
        openssl::set_log_handler(log_handler);
        struct sigaction action;
        std::memset(&action, 0, sizeof(action));
        action.sa_handler = SIG_IGN;
        sigaction(SIGPIPE, &action, nullptr);
        if (std::system("./pki.sh > /dev/null") != 0) {
            std::cerr << "Problem creating test certificates and keys" << std::endl;
            char buf[PATH_MAX];
            if (getcwd(&buf[0], sizeof(buf)) != nullptr) {
                std::cerr << "./pki.sh not found in " << buf << std::endl;
            }
            exit(1);
        }
    }

    void SetUp() override {
        server_config.cipher_list = "ECDHE-ECDSA-AES128-SHA256";
        // server_config.ciphersuites = "TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384";
        server_config.ciphersuites = "";
        server_config.certificate_chain_file = "server_chain.pem";
        server_config.private_key_file = "server_priv.pem";
        // server_config.verify_locations_file = "client_root_cert.pem";
        server_config.ocsp_response_files = {"ocsp_response.der", "ocsp_response.der"};
        server_config.host = "localhost";
        server_config.service = "8444";
        server_config.ipv6_only = false;
        server_config.verify_client = false;
        server_config.io_timeout_ms = 500;

        client_config.cipher_list = "ECDHE-ECDSA-AES128-SHA256";
        // client_config.ciphersuites = "TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384";
        // client_config.certificate_chain_file = "client_chain.pem";
        // client_config.private_key_file = "client_priv.pem";
        client_config.verify_locations_file = "server_root_cert.pem";
        client_config.io_timeout_ms = 100;
        client_config.verify_server = false;
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

    void connect() {
        client.init(client_config);
        client.reset();
        // localhost works in some cases but not in the CI pipeline for IPv6
        // use ip6-localhost
        auto connection = client.connect("localhost", "8444", false);
        if (connection) {
            if (connection->connect()) {
                set(ClientTest::flags_t::connected);
                connection->shutdown();
            }
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
};

bool ssl_init(tls::Server& server) {
    std::cout << "ssl_init" << std::endl;
    tls::Server::config_t server_config;
    server_config.cipher_list = "ECDHE-ECDSA-AES128-SHA256";
    server_config.ciphersuites = "";
    server_config.certificate_chain_file = "server_chain.pem";
    server_config.private_key_file = "server_priv.pem";
    server_config.ocsp_response_files = {"ocsp_response.der", "ocsp_response.der"};
    server_config.host = "localhost";
    server_config.service = "8444";
    server_config.ipv6_only = false;
    server_config.verify_client = false;
    server_config.io_timeout_ms = 100;
    const auto res = server.update(server_config);
    EXPECT_TRUE(res);
    return res;
}

// ----------------------------------------------------------------------------
// The tests

TEST_F(OcspTest, NonBlocking) {
    // test shouldn't hang
    start();
}

TEST_F(OcspTest, NonBlockingConnect) {
    // test shouldn't hang
    start();
    connect();
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_reset(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));
}

TEST_F(OcspTest, delayedConfig) {
    // partial config
    server_config.certificate_chain_file = nullptr;
    server_config.private_key_file = nullptr;
    server_config.ocsp_response_files.clear();

    start(ssl_init);
    connect();
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_reset(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));
}

TEST_F(OcspTest, TLS12) {
    // test using TLS 1.2
    start();
    connect();
    // no status requested
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_reset(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));

    client_config.status_request = true;
    connect();
    // status_request only
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_set(flags_t::status_request_cb));
    EXPECT_TRUE(is_set(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));

    client_config.status_request = false;
    client_config.status_request_v2 = true;
    connect();
    // status_request_v2 only
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_set(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_set(flags_t::status_request_v2));

    client_config.status_request = true;
    connect();
    // status_request and status_request_v2
    // status_request_v2 is preferred over status_request
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_set(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_set(flags_t::status_request_v2));
}

TEST_F(OcspTest, TLS13) {
    // test using TLS 1.3
    // there shouldn't be status_request_v2 responses
    // TLS 1.3 still supports status_request however it is handled differently
    // (which is handled within the OpenSSL API)
    server_config.ciphersuites = "TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384";
    start();
    connect();
    // no status requested
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_reset(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));

    client_config.status_request = true;
    connect();
    // status_request only
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_set(flags_t::status_request_cb));
    EXPECT_TRUE(is_set(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));

    client_config.status_request = false;
    client_config.status_request_v2 = true;
    connect();
    // status_request_v2 only - ignored by server
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_set(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));

    client_config.status_request = true;
    connect();
    // status_request and status_request_v2
    // status_request_v2 is ignored by server and status_request used
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_set(flags_t::status_request_cb));
    EXPECT_TRUE(is_set(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));
}

TEST_F(OcspTest, NoOcspFiles) {
    // test using TLS 1.2
    server_config.ocsp_response_files.clear();

    start();
    connect();
    // no status requested
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_reset(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));

    client_config.status_request = true;
    connect();
    // status_request only
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_set(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));

    client_config.status_request = false;
    client_config.status_request_v2 = true;
    connect();
    // status_request_v2 only
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_set(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));

    client_config.status_request = true;
    connect();
    // status_request and status_request_v2
    // status_request_v2 is preferred over status_request
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_set(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));
}

} // namespace
