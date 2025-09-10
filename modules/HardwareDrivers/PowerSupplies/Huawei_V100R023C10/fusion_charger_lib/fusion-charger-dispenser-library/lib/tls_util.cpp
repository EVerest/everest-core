// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include "tls_util.hpp"

#include <openssl/bio.h>
#include <openssl/ssl.h>
#include <unistd.h>

#include <chrono>
#include <stdexcept>
#include <thread>

using namespace tls_util;

std::tuple<SSL*, SSL_CTX*> tls_util::init_mutual_tls_client(int socket, MutualTlsClientConfig config) {
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();

    SSL_CTX* ctx = SSL_CTX_new(TLS_client_method());
    if (ctx == NULL) {
        throw std::runtime_error("SSL_CTX_new failed");
    }

    if (SSL_CTX_use_certificate_chain_file(ctx, config.ca_cert.c_str()) != 1) {
        throw std::runtime_error("Could not load CA certificate: " + config.ca_cert);
    }
    if (SSL_CTX_load_verify_locations(ctx, config.ca_cert.c_str(), NULL) != 1) {
        throw std::runtime_error("Could not load CA certificate");
    }

    printf("Client cert: %s\n", config.client_cert.c_str());
    if (SSL_CTX_use_certificate_file(ctx, config.client_cert.c_str(), SSL_FILETYPE_PEM) != 1) {
        throw std::runtime_error("Could not load client certificate");
    }

    printf("Client key: %s\n", config.client_key.c_str());
    if (SSL_CTX_use_PrivateKey_file(ctx, config.client_key.c_str(), SSL_FILETYPE_PEM) != 1) {
        throw std::runtime_error("Could not load client key");
    }

    if (!SSL_CTX_check_private_key(ctx)) {
        throw std::runtime_error("Private key invalid");
    }

    SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT, NULL);

    auto rc = BIO_socket_nbio(socket, 1);
    if (rc != 1) {
        throw std::runtime_error("BIO_socket_nbio failed");
    }

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, socket);

    bool has_worked = false;

    do {
        auto error = SSL_connect(ssl);

        if (error != 1) {
            auto error_code = SSL_get_error(ssl, error);
            // printf("SSL Error code: %d\n", error_code);

            if (error_code == SSL_ERROR_WANT_READ) {
                std::this_thread::yield();
                // std::this_thread::sleep_for(std::chrono::microseconds(10));
                continue;
            }

            if (error_code == SSL_ERROR_WANT_WRITE) {
                std::this_thread::yield();
                // std::this_thread::sleep_for(std::chrono::microseconds(10));
                continue;
            }

            SSL_free(ssl);
            SSL_CTX_free(ctx);

            throw std::runtime_error("Could not connect to server");
        }

        has_worked = true;

    } while (!has_worked);

    return {ssl, ctx};
}

std::tuple<SSL*, SSL_CTX*> tls_util::init_mutual_tls_server(int socket, MutualTlsServerConfig config) {
    SSL_load_error_strings();
    SSL_library_init();
    OpenSSL_add_all_algorithms();

    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());

    if (ctx == NULL) {
        throw std::runtime_error("SSL_CTX_new failed");
    }

    if (SSL_CTX_use_certificate_chain_file(ctx, config.client_ca.c_str()) != 1) {
        throw std::runtime_error("Could not load CA certificate: " + config.client_ca);
    }

    if (SSL_CTX_load_verify_locations(ctx, config.client_ca.c_str(), NULL) != 1) {
        throw std::runtime_error("Could not load CA certificate");
    }

    if (SSL_CTX_use_certificate_file(ctx, config.server_cert.c_str(), SSL_FILETYPE_PEM) != 1) {
        throw std::runtime_error("Could not load server certificate");
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, config.server_key.c_str(), SSL_FILETYPE_PEM) != 1) {
        throw std::runtime_error("Could not load server key");
    }

    if (!SSL_CTX_check_private_key(ctx)) {
        throw std::runtime_error("Private key invalid");
    }

    auto rc = BIO_socket_nbio(socket, 1);
    if (rc != 1) {
        throw std::runtime_error("BIO_socket_nbio failed");
    }

    SSL* ssl = SSL_new(ctx);
    SSL_set_fd(ssl, socket);

    for (;;) {
        int ret = SSL_accept(ssl);

        if (ret == 1) {
            break;
        }

        int err = SSL_get_error(ssl, ret);
        if (err != SSL_ERROR_WANT_READ && err != SSL_ERROR_WANT_WRITE) {
            printf("TLS Connection failed\n");
            SSL_free(ssl);
            SSL_CTX_free(ctx);
            close(socket);

            throw std::runtime_error("TLS Connection failed");
        }

        std::this_thread::yield();
    }

    return {ssl, ctx};
}

void tls_util::free_ssl(std::tuple<SSL*, SSL_CTX*> ssl) {
    SSL_free(std::get<0>(ssl));
    SSL_CTX_free(std::get<1>(ssl));
}
