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

#include <types.hpp>

namespace evse_security {

class CertificateLoadException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/// @brief Convenience wrapper around openssl X509
class X509Wrapper {

private:
    X509* x509;
    int valid_in; // seconds; if > 0 cert is not yet valid
    int valid_to; // seconds; if < 0 cert has expired
    std::string str;
    std::optional<std::filesystem::path> path;

    void load_certificate(const std::string& data, const EncodingFormat encoding);

public:
    // Constructors
    X509Wrapper(const std::string& certificate, const EncodingFormat encoding);
    X509Wrapper(const std::filesystem::path& path, const EncodingFormat encoding);
    X509Wrapper(X509* x509);
    X509Wrapper(const X509Wrapper& other);
    ~X509Wrapper();

    /// @brief Gets raw X509 pointer
    /// @return
    X509* get() const;

    /// @brief Resets raw X509 pointer to given \p x509
    /// @param x509
    void reset(X509* x509);

    /// @brief Splits the certificate (chain) into single certificates
    /// @return vector containing single certificates
    std::vector<X509Wrapper> split();

    /// @brief Gets valid_in
    /// @return seconds until certificate is valid; if > 0 cert is not yet valid
    int get_valid_in() const;

    /// @brief Gets valid_in
    /// @return seconds until certificate is expired; if < 0 cert has expired
    int get_valid_to() const;

    /// @brief Gets str
    /// @result raw certificate string
    std::string get_str() const;

    /// @brief Gets optional path of certificate
    /// @result
    std::optional<std::filesystem::path> get_path() const;

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
};

} // namespace evse_security

#endif // X509_WRAPPER_HPP
