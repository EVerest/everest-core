// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

/*
 * testing options
 * openssl s_client -connect [fe80::ae91:a1ff:fec9:a947%3]:64109 -verify 2 -CAfile server_root_cert.pem -cert
 * client_cert.pem -cert_chain client_chain.pem -key client_priv.pem -verify_return_error -verify_hostname
 * evse.pionix.de -status
 */

#include <array>
#include <cassert>
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <thread>
#include <unistd.h>

#include "ISO15118_chargerImplStub.hpp"
#include "ModuleAdapterStub.hpp"
#include "evse_securityIntfStub.hpp"
#include "iso15118_extensionsImplStub.hpp"

#include <connection.hpp>
#include <tls.hpp>
#include <v2g_ctx.hpp>

using namespace std::chrono_literals;

// needs to be in the global namespace
int v2g_handle_connection(struct v2g_connection* conn) {
    assert(conn != nullptr);
    assert(conn->read != nullptr);
    assert(conn->write != nullptr);

    std::array<unsigned char, 1024> buffer{};
    bool bExit = false;
    while (!bExit) {
        const ssize_t readbytes = conn->read(conn, buffer.data(), buffer.size());
        if (readbytes > 0) {
            const ssize_t writebytes = conn->write(conn, buffer.data(), readbytes);
            if (writebytes <= 0) {
                bExit = true;
            }
        } else if (readbytes < 0) {
            bExit = true;
        }
    }
    return 0;
}

namespace {

const char* interface;

void parse_options(int argc, char** argv) {
    interface = nullptr;
    int c;

    while ((c = getopt(argc, argv, "hi:")) != -1) {
        switch (c) {
        case 'i':
            interface = optarg;
            break;
        case 'h':
        case '?':
            std::cout << "Usage: " << argv[0] << " -i <interface name>" << std::endl;
            exit(1);
            break;
        default:
            exit(2);
        }
    }

    if (interface == nullptr) {
        std::cerr << "Error: " << argv[0] << " requires -i <interface name>" << std::endl;
        exit(3);
    }
}

// EvseSecurity "implementation"
struct EvseSecurity : public module::stub::evse_securityIntfStub {
    EvseSecurity(module::stub::ModuleAdapterStub& adapter) : module::stub::evse_securityIntfStub(&adapter) {
    }

    Result get_verify_file(const Requirement& req, const Parameters& args) override {
        return "client_root_cert.pem";
    }

    virtual Result get_leaf_certificate_info(const Requirement& req, const Parameters& args) override {
        // using types::evse_security::CertificateHashDataType;
        using types::evse_security::CertificateInfo;
        using types::evse_security::CertificateOCSP;
        using types::evse_security::GetCertificateInfoResult;
        using types::evse_security::GetCertificateInfoStatus;
        using types::evse_security::HashAlgorithm;

        CertificateInfo cert_info;
        cert_info.key = "server_priv.pem";
        cert_info.certificate = "server_chain.pem";
        cert_info.certificate_count = 2;
        cert_info.ocsp = {{
                              {HashAlgorithm::SHA256},
                              {"ocsp_response.der"},
                          },
                          {
                              {HashAlgorithm::SHA256},
                              {"ocsp_response.der"},
                          }};

        const GetCertificateInfoResult res = {
            GetCertificateInfoStatus::Accepted,
            cert_info,
        };
        json jres = res;
        return jres;
    }
};

} // namespace

int main(int argc, char** argv) {
    parse_options(argc, argv);

    tls::Server tls_server;
    module::stub::ModuleAdapterStub adapter;
    module::stub::ISO15118_chargerImplStub charger(adapter);
    EvseSecurity security(adapter);
    module::stub::iso15118_extensionsImplStub extensions;

    auto* ctx = v2g_ctx_create(&charger, &extensions, &security);

    if (ctx == nullptr) {
        std::cerr << "failed to create context" << std::endl;
    } else {
        ctx->tls_server = &tls_server;
        ctx->if_name = interface;
        ctx->tls_security = TLS_SECURITY_FORCE;
        ctx->is_connection_terminated = false;

        std::thread stop([ctx]() {
            // there is a 60 second read timeout in connection.cpp
            std::this_thread::sleep_for(75s);
            std::cout << "shutdown" << std::endl;
            ctx->is_connection_terminated = true;
            ctx->shutdown = true;
        });

        std::cout << "connection_init" << std::endl;
        if (::connection_init(ctx) != 0) {
            std::cerr << "connection_init failed" << std::endl;
        } else {
            std::cout << "connection_init started" << std::endl;
        }

        std::cout << "connection_start_servers " << std::endl;
        if (::connection_start_servers(ctx) != 0) {
            std::cerr << "connection_start_servers failed" << std::endl;
        } else {
            std::cout << "connection_start_servers started" << std::endl;
        }

        stop.join();
        tls::ServerConnection::wait_all_closed();

        // wait for v2g_ctx_start_events thread to stop
        std::this_thread::sleep_for(2s);
        v2g_ctx_free(ctx);
    }

    return 0;
}
