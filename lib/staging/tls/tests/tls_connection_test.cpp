// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

/**
 * \file testing patched version of OpenSSL
 *
 * These tests will only pass on a patched version of OpenSSL.
 * (they should compile and run fine with some test failures)
 *
 * It is recommended to also run tests alongside Wireshark
 * e.g. `./patched_test --gtest_filter=TlsTest.TLS12`
 * to check that the Server Hello record is correctly formed:
 * - no status_request or status_request_v2 then no Certificate Status record
 * - status_request or status_request_v2 then there is a Certificate Status record
 * - never both status_request and status_request_v2
 */

#include "tls_connection_test.hpp"

#include <mutex>

using namespace std::chrono_literals;

namespace {
using tls::status_request::ClientStatusRequestV2;

constexpr auto server_root_CN = "00000000";
constexpr auto alt_server_root_CN = "11111111";

bool ssl_init(tls::Server& server) {
    std::cout << "ssl_init" << std::endl;
    tls::Server::config_t server_config;
    server_config.cipher_list = "ECDHE-ECDSA-AES128-SHA256";
    server_config.ciphersuites = "";
    server_config.chains.emplace_back();
    server_config.chains[0].certificate_chain_file = "server_chain.pem";
    server_config.chains[0].private_key_file = "server_priv.pem";
    server_config.chains[0].trust_anchor_file = "server_root_cert.pem";
    server_config.chains[0].ocsp_response_files = {"ocsp_response.der", "ocsp_response.der"};
    server_config.host = "localhost";
    server_config.service = "8444";
    server_config.ipv6_only = false;
    server_config.verify_client = false;
    server_config.io_timeout_ms = 500;
    const auto res = server.update(server_config);
    EXPECT_TRUE(res);
    return res;
}

// ----------------------------------------------------------------------------
// The tests

TEST_F(TlsTest, NonBlocking) {
    // test shouldn't hang
    start();

    // check TearDown on stopped server is okay
    server.stop();
    server.wait_stopped();
    if (server_thread.joinable()) {
        server_thread.join();
    }
}

TEST_F(TlsTest, NonBlockingConnect) {
    // test shouldn't hang
    start();
    connect();
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_reset(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));
}

TEST_F(TlsTest, ClientReadTimeout) {
    // test shouldn't hang
    client_config.io_timeout_ms = 50;

    auto client_handler_fn = [this](std::unique_ptr<tls::ClientConnection>& connection) {
        if (connection) {
            if (connection->connect()) {
                this->set(ClientTest::flags_t::connected);
                std::byte buffer{};
                std::size_t readbytes{0};
                auto res = connection->read(&buffer, sizeof(buffer), readbytes);
                EXPECT_EQ(readbytes, 0);
                EXPECT_EQ(res, tls::Connection::result_t::timeout);
                if (res != tls::Connection::result_t::error) {
                    connection->shutdown();
                }
                connection->shutdown();
            }
        }
    };

    start();
    connect(client_handler_fn);
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_reset(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));
}

TEST_F(TlsTest, ClientWriteTimeout) {
    // test shouldn't hang
    client_config.io_timeout_ms = 50;

    bool did_timeout{false};
    std::size_t count{0};
    std::mutex mux;
    std::lock_guard lock(mux);

    constexpr std::size_t max_bytes = 1024 * 1024 * 1024;

    auto server_handler_fn = [&mux](std::shared_ptr<tls::ServerConnection>& con) {
        if (con->accept()) {
            mux.lock();
            con->shutdown();
        }
    };

    auto client_handler_fn = [this, &mux, &did_timeout, &count](std::unique_ptr<tls::ClientConnection>& connection) {
        if (connection) {
            if (connection->connect()) {
                this->set(ClientTest::flags_t::connected);
                std::array<std::byte, 1024> buffer{};
                std::size_t writebytes{0};

                bool exit{false};

                while (!exit) {
                    switch (connection->write(buffer.data(), buffer.size(), writebytes)) {
                    case tls::Connection::result_t::success:
                        count += writebytes;
                        exit = count > max_bytes;
                        break;
                    case tls::Connection::result_t::timeout:
                        // std::cout << "timeout: " << count << " bytes" << std::endl;
                        did_timeout = true;
                        exit = true;
                        break;
                    case tls::Connection::result_t::error:
                    default:
                        exit = true;
                        break;
                    }
                }
                mux.unlock();
                std::size_t readbytes = 0;
                auto res = connection->read(buffer.data(), buffer.size(), readbytes);
                if (res != tls::Connection::result_t::error) {
                    connection->shutdown();
                }
            }
        }
    };

    start(server_handler_fn);
    connect(client_handler_fn);
    EXPECT_TRUE(did_timeout);
    EXPECT_LE(count, max_bytes);
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_reset(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));
}

TEST_F(TlsTest, ServerReadTimeout) {
    // test shouldn't hang
    bool did_timeout{false};
    std::mutex mux;
    std::lock_guard lock(mux);

    auto server_handler_fn = [&mux, &did_timeout](std::shared_ptr<tls::ServerConnection>& con) {
        if (con->accept()) {
            std::array<std::byte, 1024> buffer{};
            std::size_t readbytes = 0;
            auto res = con->read(buffer.data(), buffer.size(), readbytes);
            did_timeout = res == tls::Connection::result_t::timeout;
            mux.unlock();
            con->shutdown();
        }
    };

    auto client_handler_fn = [this, &mux](std::unique_ptr<tls::ClientConnection>& connection) {
        if (connection) {
            if (connection->connect()) {
                this->set(ClientTest::flags_t::connected);
                mux.lock();
                connection->shutdown();
            }
        }
    };

    start(server_handler_fn);
    connect(client_handler_fn);
    EXPECT_TRUE(did_timeout);
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_reset(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));
}

TEST_F(TlsTest, ServerWriteTimeout) {
    // test shouldn't hang
    bool did_timeout{false};
    std::size_t count{0};
    std::mutex mux;
    std::lock_guard lock(mux);

    constexpr std::size_t max_bytes = 1024 * 1024 * 1024;

    auto server_handler_fn = [&mux, &did_timeout, &count](std::shared_ptr<tls::ServerConnection>& con) {
        if (con->accept()) {
            std::array<std::byte, 1024> buffer{};
            std::size_t writebytes{0};

            bool exit{false};

            while (!exit) {
                switch (con->write(buffer.data(), buffer.size(), writebytes)) {
                case tls::Connection::result_t::success:
                    count += writebytes;
                    exit = count > max_bytes;
                    break;
                case tls::Connection::result_t::timeout:
                    // std::cout << "timeout: " << count << " bytes" << std::endl;
                    did_timeout = true;
                    exit = true;
                    break;
                case tls::Connection::result_t::error:
                default:
                    exit = true;
                    break;
                }
            }

            mux.unlock();
            std::size_t readbytes = 0;
            auto res = con->read(buffer.data(), buffer.size(), readbytes);
            if (res != tls::Connection::result_t::error) {
                con->shutdown();
            }
        }
    };

    auto client_handler_fn = [this, &mux](std::unique_ptr<tls::ClientConnection>& connection) {
        if (connection) {
            if (connection->connect()) {
                this->set(ClientTest::flags_t::connected);
            }
            mux.lock();
            connection->shutdown();
        }
    };

    start(server_handler_fn);
    connect(client_handler_fn);
    EXPECT_TRUE(did_timeout);
    EXPECT_LE(count, max_bytes);
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_reset(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));
}

TEST_F(TlsTest, delayedConfig) {
    // partial config
    server_config.chains.clear();

    start(ssl_init);
    connect();
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_reset(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));
}

TEST_F(TlsTest, partialConfig) {
    // partial config - no support for trusted_ca_keys
    for (auto& i : server_config.chains) {
        i.trust_anchor_file = nullptr;
    }

    start();
    connect();
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_TRUE(is_reset(flags_t::status_request_cb));
    EXPECT_TRUE(is_reset(flags_t::status_request));
    EXPECT_TRUE(is_reset(flags_t::status_request_v2));
}

TEST_F(TlsTest, TLS13) {
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

TEST_F(TlsTest, NoOcspFiles) {
    // test using TLS 1.2
    for (auto& chain : server_config.chains) {
        chain.ocsp_response_files.clear();
    }

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

TEST_F(TlsTest, CertVerify) {
    client_config.verify_locations_file = "alt_server_root_cert.pem";
    start();
    connect();
    EXPECT_FALSE(is_set(flags_t::connected));
}

TEST_F(TlsTest, TCKeysNone) {
    // trusted_ca_keys - none match - default certificate should be used
    std::map<std::string, std::string> subject;

    client_config.trusted_ca_keys = true;
    client_config.trusted_ca_keys_data.pre_agreed = true;
    add_ta_cert_hash("client_root_cert.pem");
    add_ta_key_hash("client_root_cert.pem");
    add_ta_name("client_root_cert.pem");

    auto client_handler_fn = [this, &subject](std::unique_ptr<tls::ClientConnection>& connection) {
        if (connection) {
            if (connection->connect()) {
                this->set(ClientTest::flags_t::connected);
                subject = openssl::certificate_subject(connection->peer_certificate());
                connection->shutdown();
            }
        }
    };

    start();
    connect(client_handler_fn);
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_EQ(subject["CN"], server_root_CN);
}

TEST_F(TlsTest, TCKeysCert) {
    // trusted_ca_keys - cert hash matches
    std::map<std::string, std::string> subject;

    client_config.trusted_ca_keys = true;
    client_config.verify_locations_file = "alt_server_root_cert.pem";
    add_ta_cert_hash("alt_server_root_cert.pem");

    auto client_handler_fn = [this, &subject](std::unique_ptr<tls::ClientConnection>& connection) {
        if (connection) {
            if (connection->connect()) {
                this->set(ClientTest::flags_t::connected);
                subject = openssl::certificate_subject(connection->peer_certificate());
                connection->shutdown();
            }
        }
    };

    start();
    connect(client_handler_fn);
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_EQ(subject["CN"], alt_server_root_CN);

    client_config.trusted_ca_keys_data.x509_name.clear();
    add_ta_cert_hash("client_root_cert.pem");
    add_ta_cert_hash("alt_server_root_cert.pem");

    connect(client_handler_fn);
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_EQ(subject["CN"], alt_server_root_CN);
}

TEST_F(TlsTest, TCKeysKey) {
    // trusted_ca_keys - key hash matches
    std::map<std::string, std::string> subject;

    client_config.trusted_ca_keys = true;
    client_config.verify_locations_file = "alt_server_root_cert.pem";
    add_ta_key_hash("alt_server_root_cert.pem");

    auto client_handler_fn = [this, &subject](std::unique_ptr<tls::ClientConnection>& connection) {
        if (connection) {
            if (connection->connect()) {
                this->set(ClientTest::flags_t::connected);
                subject = openssl::certificate_subject(connection->peer_certificate());
                connection->shutdown();
            }
        }
    };

    start();
    connect(client_handler_fn);
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_EQ(subject["CN"], alt_server_root_CN);

    client_config.trusted_ca_keys_data.x509_name.clear();
    add_ta_key_hash("client_root_cert.pem");
    add_ta_key_hash("alt_server_root_cert.pem");

    connect(client_handler_fn);
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_EQ(subject["CN"], alt_server_root_CN);
}

TEST_F(TlsTest, TCKeysName) {
    // trusted_ca_keys - subject name matches
    std::map<std::string, std::string> subject;

    client_config.trusted_ca_keys = true;
    client_config.verify_locations_file = "alt_server_root_cert.pem";
    add_ta_name("alt_server_root_cert.pem");

    auto client_handler_fn = [this, &subject](std::unique_ptr<tls::ClientConnection>& connection) {
        if (connection) {
            if (connection->connect()) {
                this->set(ClientTest::flags_t::connected);
                subject = openssl::certificate_subject(connection->peer_certificate());
                connection->shutdown();
            }
        }
    };

    start();
    connect(client_handler_fn);
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_EQ(subject["CN"], alt_server_root_CN);

    client_config.trusted_ca_keys_data.x509_name.clear();
    add_ta_name("client_root_cert.pem");
    add_ta_name("alt_server_root_cert.pem");

    connect(client_handler_fn);
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_EQ(subject["CN"], alt_server_root_CN);
}

// based on an example seen in a WireShark log
// (invalid missing the size of trusted_authorities_list)
// 01 identifier_type key_sha1_hash 4cd7290bf592d2c1ba90f56e08946d4c8e99dc38 SHA1Hash
// 01 identifier_type key_sha1_hash 00fae3900795c888a4d4d7bd9fdffa60418ac19f SHA1Hash
int trusted_ca_keys_add_bad(SSL* ctx, unsigned int ext_type, unsigned int context, const unsigned char** out,
                            std::size_t* outlen, X509* cert, std::size_t chainidx, int* alert, void* object) {
    // std::cout << "trusted_ca_keys_add_bad" << std::endl;
    int result{0};
    if ((context == SSL_EXT_CLIENT_HELLO) && (object != nullptr)) {
        constexpr std::uint8_t value[] = {
            0x01, 0x4c, 0xd7, 0x29, 0x0b, 0xf5, 0x92, 0xd2, 0xc1, 0xba, 0x90, 0xf5, 0x6e, 0x08,
            0x94, 0x6d, 0x4c, 0x8e, 0x99, 0xdc, 0x38, 0x01, 0x00, 0xfa, 0xe3, 0x90, 0x07, 0x95,
            0xc8, 0x88, 0xa4, 0xd4, 0xd7, 0xbd, 0x9f, 0xdf, 0xfa, 0x60, 0x41, 0x8a, 0xc1, 0x9f,
        };
        openssl::DER der(&value[0], sizeof(value));
        const auto len = der.size();
        auto* ptr = openssl::DER::dup(der);
        if (ptr != nullptr) {
            *out = ptr;
            *outlen = len;
            result = 1;
        }
    }
    return result;
}

TEST_F(TlsTest, TCKeysInvalid) {
    // trusted_ca_keys - incorrectly formatted extension, connect using defaults
    std::map<std::string, std::string> subject;

    client_config.trusted_ca_keys = true;
    client_config.verify_locations_file = "server_root_cert.pem";

    auto override = tls::Client::default_overrides();
    override.trusted_ca_keys_add = &trusted_ca_keys_add_bad;

    start();
    client.init(client_config, override);
    client.reset();
    // localhost works in some cases but not in the CI pipeline for IPv6
    // use ip6-localhost
    auto connection = client.connect("localhost", "8444", false, 1000);
    if (connection) {
        if (connection->connect()) {
            set(ClientTest::flags_t::connected);
            subject = openssl::certificate_subject(connection->peer_certificate());
            connection->shutdown();
        }
    }
    EXPECT_TRUE(is_set(flags_t::connected));
    EXPECT_EQ(subject["CN"], server_root_CN);
}

} // namespace
