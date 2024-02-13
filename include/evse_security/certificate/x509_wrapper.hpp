// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include <evse_security/crypto/interface/crypto_types.hpp>
#include <evse_security/evse_types.hpp>
#include <evse_security/utils/evse_filesystem_types.hpp>

namespace evse_security {

enum class X509CertificateSource {
    // Built from a certificate file
    FILE,
    // Built from a directory of certificates
    DIRECTORY,
    // Build from a raw string
    STRING
};

const fs::path PEM_EXTENSION = ".pem";
const fs::path DER_EXTENSION = ".der";
const fs::path KEY_EXTENSION = ".key";
const fs::path TPM_KEY_EXTENSION = ".tkey";

/// @brief Convenience wrapper around openssl X509 certificate
class X509Wrapper {
public:
    X509Wrapper(const fs::path& file, const EncodingFormat encoding);
    X509Wrapper(const std::string& data, const EncodingFormat encoding);

    explicit X509Wrapper(X509Handle_ptr&& x509);
    X509Wrapper(X509Handle_ptr&& x509, const fs::path& file);

    X509Wrapper(const X509Wrapper& other);
    X509Wrapper(X509Wrapper&& other) = default;

    ~X509Wrapper();

public:
    /// @brief Returns true if this certificate is the child of the provided parent
    bool is_child(const X509Wrapper& parent) const;

    /// @brief Returns true if this certificate is self-signed
    bool is_selfsigned() const;

public:
    /// @brief Gets x509 raw handle
    inline X509Handle* get() const {
        return x509.get();
    }

    /// @brief Gets valid_in
    /// @return seconds until certificate is valid; if > 0 cert is not yet valid
    int64_t get_valid_in() const;

    /// @brief Gets valid_in
    /// @return seconds until certificate is expired; if < 0 cert has expired
    int64_t get_valid_to() const;

    /// @brief Gets optional file of certificate
    /// @result
    std::optional<fs::path> get_file() const;

    void set_file(fs::path& path);

    /// @brief Gets the source of this certificate, if it is from a file it's 'FILE'
    /// but it can also be constructed from a string, or another certificate
    X509CertificateSource get_source() const;

    /// @brief Gets CN of certificate
    /// @result
    std::string get_common_name() const;

    /// @brief Gets issuer name hash of certificate
    /// @result
    std::string get_issuer_name_hash() const;

    /// @brief Gets issuer key hash of certificate. As per the specification
    /// this is not the hash of our public key, but the hash of the pubkey of
    /// the parent certificate. If it is a self-signed root, then the key hash
    /// and the issuer key hash are the same
    /// @result
    std::string get_issuer_key_hash() const;

    /// @brief Gets key hash of this certificate
    /// @result
    std::string get_key_hash() const;

    /// @brief Gets serial number of certificate
    /// @result
    std::string get_serial_number() const;

    /// @brief Gets certificate hash data of a self-signed certificate
    /// @return
    CertificateHashData get_certificate_hash_data() const;

    /// @brief Gets certificate hash data of certificate with an issuer
    /// @return
    CertificateHashData get_certificate_hash_data(const X509Wrapper& issuer) const;

    /// @brief Gets OCSP responder URL of certificate if present, else returns an empty string
    /// @return
    std::string get_responder_url() const;

    /// @brief Gets the export string representation for this certificate
    /// @return
    std::string get_export_string() const;

    /// @brief If the certificate is within the validity date. Can return false in 2 cases,
    /// if it is expired (current date > valid_to) or if (current data < valid_in), that is
    /// we are not in force yet
    bool is_valid() const;

    /// @brief If the certificate has expired
    bool is_expired() const;

public:
    X509Wrapper& operator=(X509Wrapper&& other) = default;

    /// @return true if the two certificates are the same
    bool operator==(const X509Wrapper& other) const;

    bool operator==(const CertificateHashData& other) const {
        return get_issuer_name_hash() == other.issuer_name_hash && get_issuer_key_hash() == other.issuer_key_hash &&
               get_serial_number() == other.serial_number;
    }

private:
    void update_validity();

private:
    X509Handle_ptr x509;   // X509 wrapper object
    std::int64_t valid_in; // seconds; if > 0 cert is not yet valid, negative value means past, positive is in future
    std::int64_t valid_to; // seconds; if < 0 cert has expired, negative value means past, positive is in future

    // Relevant file in which this certificate resides
    std::optional<fs::path> file;
};

} // namespace evse_security
