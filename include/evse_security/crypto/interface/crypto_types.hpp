// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <chrono>
#include <memory>
#include <optional>

namespace evse_security {

enum class CertificateValidationError {
    NoError,
    Expired,
    InvalidSignature,
    IssuerNotFound,
    InvalidLeafSignature,
    InvalidChain,
    Unknown,
};

class CertificateLoadException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

struct CryptoHandle {
    virtual ~CryptoHandle() {
    }
};

/// @brief Handle abstraction to crypto lib X509 certificate
struct X509Handle : public CryptoHandle {};

/// @brief Handle abstraction to crypto lib key
struct KeyHandle : public CryptoHandle {};

struct CertificateSigningRequestInfo {
    // Minimal mandatory
    int n_version;
    std::string country;
    std::string organization;
    std::string commonName;

    std::optional<std::string> private_key_file;
    std::optional<std::string> private_key_pass;
};

using X509Handle_ptr = std::unique_ptr<X509Handle>;
using KeyHandle_ptr = std::unique_ptr<KeyHandle>;

// Transforms a duration of days into seconds
using days_to_seconds = std::chrono::duration<std::int64_t, std::ratio<86400>>;

} // namespace evse_security