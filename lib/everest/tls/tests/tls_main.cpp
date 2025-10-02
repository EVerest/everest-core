// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

/*
 * testing options
 * openssl s_client -connect localhost:8444 -verify 2 -CAfile server_root_cert.pem -cert client_cert.pem -cert_chain
 * client_chain.pem -key client_priv.pem -verify_return_error -verify_hostname evse.pionix.de -status
 */

#include <everest/tls/tls.hpp>

#include <array>
#include <chrono>
#include <csignal>
#include <iostream>
#include <memory>
#include <thread>
#include <unistd.h>

using namespace std::chrono_literals;

namespace {

const char* short_opts = "ch36t";
bool disable_tls1_3{false};
bool enable_tpm{false};
bool ipv6_only{false};
bool verify_client{false};

void parse_options(int argc, char** argv) {
    int c;

    while ((c = getopt(argc, argv, short_opts)) != -1) {
        switch (c) {
            break;
        case 'c':
            verify_client = true;
            break;
        case '3':
            disable_tls1_3 = true;
            break;
        case '6':
            ipv6_only = true;
            break;
        case 't':
            enable_tpm = true;
            break;
        case 'h':
        case '?':
            std::cout << "Usage: " << argv[0] << " [-c|-3|-6]" << std::endl;
            std::cout << "       -c verify client" << std::endl;
            std::cout << "       -3 disable TLS 1.3" << std::endl;
            std::cout << "       -6 IPv6 only" << std::endl;
            std::cout << "       -t enable TPM" << std::endl;
            exit(1);
            break;
        default:
            exit(2);
        }
    }
}

void handle_connection(tls::Server::ConnectionPtr&& con) {
    std::cout << "Connection" << std::endl;
    if (con->accept() == tls::Connection::result_t::success) {
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
                case tls::Connection::result_t::closed:
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
            case tls::Connection::result_t::closed:
            default:
                bExit = true;
                break;
            }
        }

        con->shutdown();
    }
    std::cout << "Connection closed" << std::endl;
}

} // namespace

int main(int argc, char** argv) {
    parse_options(argc, argv);

    tls::Server::configure_signal_handler(SIGUSR1);
    tls::Server server;
    tls::Server::config_t config;

    // 15118 required suites, ECDH-ECDSA-AES128-SHA256 not supported by OpenSSL
    // config.cipher_list = "ECDHE-ECDSA-AES128-SHA256:ECDH-ECDSA-AES128-SHA256";

    config.cipher_list = "ECDHE-ECDSA-AES128-SHA256";
    if (disable_tls1_3) {
        config.ciphersuites = "";
    } else {
        config.ciphersuites = "TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384";
    }

    auto& ref0 = config.chains.emplace_back();
    if (enable_tpm) {
        ref0.certificate_chain_file = "tpm_pki/server_chain.pem";
        ref0.private_key_file = "tpm_pki/server_priv.pem";
        ref0.trust_anchor_file = "tpm_pki/server_root_cert.pem";
    } else {
        ref0.certificate_chain_file = "server_chain.pem";
        ref0.private_key_file = "server_priv.pem";
        ref0.trust_anchor_file = "server_root_cert.pem";
    }
    ref0.ocsp_response_files = {"ocsp_response.der", "ocsp_response.der"};
    auto& ref1 = config.chains.emplace_back();
    ref1.certificate_chain_file = "alt_server_chain.pem";
    ref1.private_key_file = "alt_server_priv.pem";
    ref1.trust_anchor_file = "alt_server_root_cert.pem";
    config.verify_locations_file = "client_root_cert.pem";

    config.service = "8444";
    config.ipv6_only = ipv6_only;
    config.verify_client = verify_client;
    config.io_timeout_ms = 10000;

    std::thread stop([&server]() {
        std::this_thread::sleep_for(30s);
        std::cout << "stopping ..." << std::endl;
        server.stop();
    });

    server.init(config, nullptr);
    server.wait_stopped();

    server.serve(&handle_connection);
    server.wait_stopped();

    stop.join();

    return 0;
}
