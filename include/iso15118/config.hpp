// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <string>

namespace iso15118::config {

enum class TlsNegotiationStrategy {
    ACCEPT_CLIENT_OFFER,
    ENFORCE_TLS,
    ENFORCE_NO_TLS,
};

enum class CertificateBackend {
    EVEREST_LAYOUT,
    JOSEPPA_LAYOUT,
};
struct SSLConfig {
    CertificateBackend backend;
    std::string config_string;
    std::string private_key_password;
    bool enable_ssl_logging{false};
    bool enable_tls_key_logging{false};
};

} // namespace iso15118::config
