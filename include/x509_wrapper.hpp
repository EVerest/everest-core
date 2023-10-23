// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef X509_WRAPPER_HPP
#define X509_WRAPPER_HPP

#include <filesystem>
#include <memory>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/x509.h>
#include <stdexcept>
#include <string>

#include <sec_types.hpp>
#include <types.hpp>

namespace evse_security {

class CertificateLoadException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

enum class X509CertificateSource {
    // Built from a certificate file
    FILE,
    // Built from a directory of certificates
    DIRECTORY,
    // Build from a raw string
    STRING
};

const std::filesystem::path PEM_EXTENSION = ".pem";
const std::filesystem::path DER_EXTENSION = ".der";
const std::filesystem::path KEY_EXTENSION = ".key";

using ossl_days_to_seconds = std::chrono::duration<std::int64_t, std::ratio<86400>>;

/// @brief Convenience wrapper around openssl X509 certificate
class X509Wrapper {
public:
    X509Wrapper(const std::filesystem::path& file, const EncodingFormat encoding);
    X509Wrapper(const std::string& data, const EncodingFormat encoding);

    /// @brief Since it implies ownership full transfer, must be very careful with this that's why it's explicit
    /// If another object owns the x509 will destroy it and if another one tries to use the dead reference will crash
    /// the program
    explicit X509Wrapper(X509* x509);
    explicit X509Wrapper(X509_ptr&& x509);

    X509Wrapper(X509* x509, const std::filesystem::path& file);
    X509Wrapper(X509_ptr&& x509, const std::filesystem::path& file);

    X509Wrapper(const X509Wrapper& other);
    X509Wrapper(X509Wrapper&& other) = default;

    ~X509Wrapper();

    /// @brief Gets raw X509 pointer
    /// @return
    inline X509* get() const {
        return x509.get();
    }

    /// @brief Resets raw X509 pointer to given \p x509
    /// @param x509
    void reset(X509* x509);

    /// @brief Gets valid_in
    /// @return seconds until certificate is valid; if > 0 cert is not yet valid
    int get_valid_in() const;

    /// @brief Gets valid_in
    /// @return seconds until certificate is expired; if < 0 cert has expired
    int get_valid_to() const;

    /// @brief Gets optional file of certificate
    /// @result
    std::optional<std::filesystem::path> get_file() const;

    /// @brief Gets CN of certificate
    /// @result
    std::string get_common_name() const;

    /// @brief Gets issuer name hash of certificate
    /// @result
    std::string get_issuer_name_hash() const;

    /// @brief Gets serial number of certificate
    /// @result
    std::string get_serial_number() const;

    /// @brief Gets issuer key hash of certificate
    /// @result
    std::string get_issuer_key_hash() const;

    /// @brief Gets certificate hash data of certificate
    /// @return
    CertificateHashData get_certificate_hash_data() const;

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
    bool operator==(const X509Wrapper& other) const {
        return get_issuer_name_hash() == other.get_issuer_name_hash() &&
               get_issuer_key_hash() == other.get_issuer_key_hash() && get_serial_number() == other.get_serial_number();
    }

    bool operator==(const CertificateHashData& other) const {
        return get_issuer_name_hash() == other.issuer_name_hash && get_issuer_key_hash() == other.issuer_key_hash &&
               get_serial_number() == other.serial_number;
    }

private:
    void update_validity();

private:
    X509_ptr x509;         // X509 wrapper object
    std::int64_t valid_in; // seconds; if > 0 cert is not yet valid
    std::int64_t valid_to; // seconds; if < 0 cert has expired

    // Relevant file in which this certificate resides
    std::optional<std::filesystem::path> file;
};

} // namespace evse_security

#endif // X509_WRAPPER_HPP
