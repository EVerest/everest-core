// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

/*
 * testing options
 * openssl s_client -connect localhost:8444 -verify 2 -CAfile server_root_cert.pem -cert client_cert.pem -cert_chain
 * client_chain.pem -key client_priv.pem -verify_return_error -verify_hostname evse.pionix.de -status
 */

#include <tls.hpp>

#include <array>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

using namespace std::chrono_literals;

void handle_connection(std::shared_ptr<tls::ServerConnection>& con) {
    std::cout << "Connection" << std::endl;
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
    std::cout << "Connection closed" << std::endl;
}

int main() {
    tls::Server server;
    tls::Server::config_t config;

#if 0
    config.cipher_list =
        "ECDHE-ECDSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-GCM-SHA384:AES128-GCM-SHA256:AES256-GCM-SHA384";
    config.ciphersuites = "TLS_AES_256_GCM_SHA384:TLS_AES_128_GCM_SHA256";
#else
    // config.cipher_list = "ECDHE-ECDSA-AES128-SHA256:ECDH-ECDSA-AES128-SHA256";
    config.cipher_list = "ECDHE-ECDSA-AES128-SHA256";
    config.ciphersuites = "TLS_AES_128_GCM_SHA256:TLS_AES_256_GCM_SHA384";
    // config.ciphersuites = "";
#endif
    config.certificate_chain_file = "server_chain.pem";
    config.private_key_file = "server_priv.pem";
    config.verify_locations_file = "client_root_cert.pem";
    // config.ocsp_response_files = {"ocsp_response.der", nullptr};
    config.ocsp_response_files = {"ocsp_response.der", "ocsp_response.der"};
    config.service = "8444";
    config.ipv6_only = false;
    config.verify_client = true;
    config.io_timeout_ms = 1000;

    std::thread stop([&server]() {
        std::this_thread::sleep_for(30s);
        server.stop();
    });

    server.init(config, nullptr);
    server.wait_stopped();

    // server.serve(&handle_connection);
    server.serve([](auto con) { handle_connection(con); });
    server.wait_stopped();

    stop.join();

    return 0;
}
