// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <tls.hpp>

#include <chrono>
#include <iostream>
#include <thread>
#include <unistd.h>

using namespace std::chrono_literals;

namespace {
const char* short_opts = "h123";
bool use_tls1_3{false};
bool use_status_request{false};
bool use_status_request_v2{false};

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
        case 'h':
        case '?':
            std::cout << "Usage: " << argv[0] << " [-1|-2|-3]" << std::endl;
            std::cout << "       -1 request status_request" << std::endl;
            std::cout << "       -2 request status_request_v2" << std::endl;
            std::cout << "       -3 use TLS 1.3 (TLS 1.2 otherwise)" << std::endl;
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

    tls::Client client;
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

    // localhost works in some cases but not in the CI pipeline
    auto connection = client.connect("ip6-localhost", "8444", true);
    if (connection) {
        if (connection->connect()) {
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
