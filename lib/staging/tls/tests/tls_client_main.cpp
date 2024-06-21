// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#include "tls_connection_test.hpp"
#include <openssl_util.hpp>
#include <tls.hpp>

#include <chrono>
#include <iostream>
#include <thread>
#include <unistd.h>

using namespace std::chrono_literals;

namespace {
const char* short_opts = "h123r:";
bool use_tls1_3{false};
bool use_status_request{false};
bool use_status_request_v2{false};
const char* trust_anchor{nullptr};

void parse_options(int argc, char** argv) {
    int c;

    while ((c = getopt(argc, argv, short_opts)) != -1) {
        switch (c) {
            break;
        case '1':
            use_status_request = true;
            break;
        case '2':
            use_status_request_v2 = true;
            break;
        case '3':
            use_tls1_3 = true;
            break;
        case 'r':
            trust_anchor = optarg;
            break;
        case 'h':
        case '?':
            std::cout << "Usage: " << argv[0] << " [-1|-2|-3] [-r server_root_cert.pem]" << std::endl;
            std::cout << "       -1 request status_request" << std::endl;
            std::cout << "       -2 request status_request_v2" << std::endl;
            std::cout << "       -3 use TLS 1.3 (TLS 1.2 otherwise)" << std::endl;
            std::cout << "       -r root certificate / trust anchor" << std::endl;
            exit(1);
            break;
        default:
            exit(2);
        }
    }
}
} // namespace

int main(int argc, char** argv) {
    parse_options(argc, argv);

    // used test client for extra output
    ClientTest client;
    tls::Client::config_t config;

    if (use_tls1_3) {
        config.cipher_list = "ECDHE-ECDSA-AES128-SHA256";
        config.ciphersuites = "TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384";
        std::cout << "use_tls1_3            true" << std::endl;
    } else {
        config.cipher_list = "ECDHE-ECDSA-AES128-SHA256";
        config.ciphersuites = ""; // No TLS1.3
        std::cout << "use_tls1_3            false" << std::endl;
    }

    config.certificate_chain_file = "client_chain.pem";
    config.private_key_file = "client_priv.pem";
    config.verify_locations_file = "server_root_cert.pem";
    config.io_timeout_ms = 500;
    config.verify_server = false;

    if (trust_anchor != nullptr) {
        openssl::sha_1_digest_t digest;
        auto certs = openssl::load_certificates(trust_anchor);
        for (const auto& ta : certs) {
            if (openssl::certificate_sha_1(digest, ta.get())) {
                config.trusted_ca_keys_data.cert_sha1_hash.push_back(digest);
            }
        }
        config.verify_locations_file = trust_anchor;
        config.trusted_ca_keys = true;
        config.trusted_ca_keys_data.pre_agreed = true;
    }

    if (use_status_request) {
        config.status_request = true;
        std::cout << "use_status_request    true" << std::endl;
    } else {
        config.status_request = false;
        std::cout << "use_status_request    false" << std::endl;
    }

    if (use_status_request_v2) {
        config.status_request_v2 = true;
        std::cout << "use_status_request_v2 true" << std::endl;
    } else {
        config.status_request_v2 = false;
        std::cout << "use_status_request_v2 false" << std::endl;
    }

    client.init(config);

    // localhost works in some cases but not in the CI pipeline ip6-localhost is an option
    auto connection = client.connect("localhost", "8444", false, 1000);
    if (connection) {
        if (connection->connect()) {
            const auto* cert = connection->peer_certificate();
            if (cert != nullptr) {
                const auto subject = openssl::certificate_subject(cert);
                if (!subject.empty()) {
                    std::cout << "subject:";
                    for (const auto& itt : subject) {
                        std::cout << " " << itt.first << ":" << itt.second;
                    }
                    std::cout << std::endl;
                }
            }
            std::array<std::byte, 1024> buffer{};
            std::size_t readbytes = 0;
            std::cout << "about to read" << std::endl;
            const auto res = connection->read(buffer.data(), buffer.size(), readbytes);
            std::cout << (int)res << std::endl;
            std::this_thread::sleep_for(1s);
            connection->shutdown();
        }
    }

    return 0;
}
