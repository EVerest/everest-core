// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <openssl/ssl.h>

#include <string>

namespace tls_util {
struct MutualTlsClientConfig {
    // path to ca certificate of the server
    std::string ca_cert;

    // path to client certificate
    std::string client_cert;
    // path to client key
    std::string client_key;
};

std::tuple<SSL*, SSL_CTX*> init_mutual_tls_client(int socket, MutualTlsClientConfig config);

struct MutualTlsServerConfig {
    std::string client_ca;

    std::string server_cert;
    std::string server_key;
};

std::tuple<SSL*, SSL_CTX*> init_mutual_tls_server(int socket, MutualTlsServerConfig config);

void free_ssl(std::tuple<SSL*, SSL_CTX*> ssl);

} // namespace tls_util
