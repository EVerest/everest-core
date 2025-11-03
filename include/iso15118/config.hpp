// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <filesystem>
#include <optional>
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
    CertificateBackend backend{CertificateBackend::EVEREST_LAYOUT};
    // Used by the JOSEPPA_LAYOUT
    std::string config_string;
    // Used by the EVEREST_LAYOUT
    std::string path_certificate_chain;
    std::string path_certificate_key;
    std::optional<std::string> private_key_password{};
    std::string path_certificate_v2g_root;
    std::string path_certificate_mo_root;
    bool enable_ssl_logging{false};
    bool enable_tls_key_logging{false};
    bool enforce_tls_1_3{false};
    std::filesystem::path tls_key_logging_path{};
};

} // namespace iso15118::config
