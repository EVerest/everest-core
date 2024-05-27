// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <chrono>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string>

namespace evse_security {

enum class CryptoKeyType {
    EC_prime256v1, // Default EC. P-256, ~equiv to rsa 3072
    EC_secp384r1,  // P-384, ~equiv to rsa 7680
    RSA_2048,
    RSA_TPM20 = RSA_2048, // Default TPM RSA, only option allowed for TPM (universal support), 2048 bits
    RSA_3072,             // Default RSA. Protection lifetime: ~2030
    RSA_7680,             // Protection lifetime: >2031. Very long generation time 8-40s on 16 core PC
};

enum class KeyValidationResult {
    Valid,
    KeyLoadFailure, // The key could not be loaded, might be an password or invalid string
    Invalid,        // The key is not linked to the specified certificate
    Unknown,        // Unknown error, not related to provider validation
};

enum class CertificateSignRequestResult {
    Valid,
    KeyGenerationError, // Error when generating the key, maybe invalid key type
    VersioningError,    // The version could not be set
    PubkeyError,        // The public key could not be attached
    ExtensionsError,    // The extensions could not be appended
    SigningError,       // The CSR could not be signed, maybe key or signing algo invalid
    Unknown,            // Any other error
};

struct KeyGenerationInfo {
    CryptoKeyType key_type;

    /// @brief If the key should be generated on the TPM, should check before if
    /// the provider supports the operation, or the operation will fail by default
    bool generate_on_tpm;

    /// @brief If we should export the public key to a file
    std::optional<std::string> public_key_file;

    /// @brief If we should export the private key to a file
    std::optional<std::string> private_key_file;
    /// @brief If we should have a pass for the private key file
    std::optional<std::string> private_key_pass;
};

struct CertificateSigningRequestInfo {
    // Minimal mandatory
    int n_version;
    std::string country;
    std::string organization;
    std::string commonName;

    /// @brief incude a subjectAlternativeName DNSName
    std::optional<std::string> dns_name;
    /// @brief incude a subjectAlternativeName IPAddress
    std::optional<std::string> ip_address;

    KeyGenerationInfo key_info;
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

using X509Handle_ptr = std::unique_ptr<X509Handle>;
using KeyHandle_ptr = std::unique_ptr<KeyHandle>;

// Transforms a duration of days into seconds
using days_to_seconds = std::chrono::duration<std::int64_t, std::ratio<86400>>;

namespace conversions {
std::string get_certificate_sign_request_result_to_string(CertificateSignRequestResult e);
}

} // namespace evse_security