// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef OPENSSL_UTIL_HPP_
#define OPENSSL_UTIL_HPP_

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

struct evp_pkey_st;
struct x509_st;

namespace openssl {

enum class verify_result_t : std::uint8_t {
    verified,
    CertChainError,
    CertificateExpired,
    CertificateRevoked,
    NoCertificateAvailable,
    OtherError,
};

constexpr std::size_t signature_size = 64;
constexpr std::size_t signature_n_size = 32;
constexpr std::size_t signature_der_size = 128;
constexpr std::size_t sha_256_digest_size = 32;
constexpr std::size_t sha_384_digest_size = 48;
constexpr std::size_t sha_512_digest_size = 64;

enum class digest_alg_t : std::uint8_t {
    sha256,
    sha384,
    sha512,
};

using sha_256_digest_t = std::array<std::uint8_t, sha_256_digest_size>;
using sha_384_digest_t = std::array<std::uint8_t, sha_384_digest_size>;
using sha_512_digest_t = std::array<std::uint8_t, sha_512_digest_size>;
using bn_t = std::array<std::uint8_t, signature_n_size>;
using bn_const_t = std::array<const std::uint8_t, signature_n_size>;

using Certificate_ptr = std::unique_ptr<x509_st, void (*)(x509_st*)>;
using CertificateList = std::vector<Certificate_ptr>;
using DER_Signature_ptr = std::unique_ptr<std::uint8_t, void (*)(std::uint8_t*)>;
using PKey_ptr = std::unique_ptr<evp_pkey_st, void (*)(evp_pkey_st*)>;

/**
 * \brief sign using ECDSA on curve secp256r1/prime256v1/P-256 of a SHA 256 digest
 * \param[in] pkey the private key
 * \param[out] r the R component of the signature as a BIGNUM
 * \param[out] s the S component of the signature as a BIGNUM
 * \param[out] digest the SHA256 digest to sign
 * \return true when successful
 */
bool sign(evp_pkey_st* pkey, bn_t& r, bn_t& s, const sha_256_digest_t& digest);

/**
 * \brief sign using ECDSA on curve secp256r1/prime256v1/P-256 of a SHA 256 digest
 * \param[in] pkey the private key
 * \param[out] sig the buffer where the DER encoded signature will be placed
 * \param[inout] siglen the size of the signature buffer, updated to be the size of the signature
 * \param[in] tbs a pointer to the SHA256 digest
 * \param[in] tbslen the size of the SHA256 digest
 * \return true when successful
 */
bool sign(evp_pkey_st* pkey, unsigned char* sig, std::size_t& siglen, const unsigned char* tbs, std::size_t tbslen);

/**
 * \brief verify a signature against a SHA 256 digest using ECDSA on curve secp256r1/prime256v1/P-256
 * \param[in] pkey the public key
 * \param[in] r the R component of the signature as a BIGNUM
 * \param[in] s the S component of the signature as a BIGNUM
 * \param[in] digest the SHA256 digest to sign
 * \return true when successful
 */
bool verify(evp_pkey_st* pkey, const bn_t& r, const bn_t& s, const sha_256_digest_t& digest);

/**
 * \brief verify a signature against a SHA 256 digest using ECDSA on curve secp256r1/prime256v1/P-256
 * \param[in] pkey the public key
 * \param[in] r the R component of the signature as a BIGNUM (0-padded 32 bytes)
 * \param[in] s the S component of the signature as a BIGNUM (0-padded 32 bytes)
 * \param[in] digest the SHA256 digest to sign
 * \return true when successful
 */
bool verify(evp_pkey_st* pkey, const std::uint8_t* r, const std::uint8_t* s, const sha_256_digest_t& digest);

/**
 * \brief verify a signature against a SHA 256 digest using ECDSA on curve secp256r1/prime256v1/P-256
 * \param[in] pkey the public key
 * \param[in] sig the DER encoded signature
 * \param[in] siglen the size of the DER encoded signature
 * \param[in] tbs a pointer to the SHA256 digest
 * \param[in] tbslen the size of the SHA256 digest
 * \return true when successful
 */
bool verify(evp_pkey_st* pkey, const unsigned char* sig, std::size_t siglen, const unsigned char* tbs,
            std::size_t tbslen);

/**
 * \brief calculate the SHA256 digest over an array of bytes
 * \param[in] data the start of the data
 * \param[in] len the length of the data
 * \param[out] the SHA256 digest
 * \return true on success
 */
bool sha_256(const void* data, std::size_t len, sha_256_digest_t& digest);

/**
 * \brief calculate the SHA384 digest over an array of bytes
 * \param[in] data the start of the data
 * \param[in] len the length of the data
 * \param[out] the SHA384 digest
 * \return true on success
 */
bool sha_384(const void* data, std::size_t len, sha_384_digest_t& digest);

/**
 * \brief calculate the SHA512 digest over an array of bytes
 * \param[in] data the start of the data
 * \param[in] len the length of the data
 * \param[out] the SHA512 digest
 * \return true on success
 */
bool sha_512(const void* data, std::size_t len, sha_512_digest_t& digest);

/**
 * \brief decode a base64 string into it's binary form
 * \param[in] text the base64 string (does not need to be \0 terminated)
 * \param[in] len the length of the string (excluding any terminating \0)
 * \return binary array or empty on error
 */
std::vector<std::uint8_t> base64_decode(const char* text, std::size_t len);

/**
 * \brief decode a base64 string into it's binary form
 * \param[in] text the base64 string (does not need to be \0 terminated)
 * \param[in] len the length of the string (excluding any terminating \0)
 * \param[out] out_data where to place the decoded data
 * \param[inout] out_len the size of out_data, updated to be the length of the decoded data
 * \return true on success
 */
bool base64_decode(const char* text, std::size_t len, std::uint8_t* out_data, std::size_t& out_len);

/**
 * \brief encode data into a base64 text string
 * \param[in] data the data to encode
 * \param[in] len the length of the data
 * \param[in] newLine when true add a \n to break the result into multiple lines
 * \return base64 string or empty on error
 */
std::string base64_encode(const std::uint8_t* data, std::size_t len, bool newLine);

/**
 * \brief encode data into a base64 text string
 * \param[in] data the data to encode
 * \param[in] len the length of the data
 * \return base64 string or empty on error
 * \note the return string doesn't include line breaks
 */
inline std::string base64_encode(const std::uint8_t* data, std::size_t len) {
    return base64_encode(data, len, false);
}

/**
 * \brief zero a structure
 * \param mem the structure to zero
 */
template <typename T> constexpr void zero(T& mem) {
    std::memset(mem.data(), 0, mem.size());
}

/**
 * \brief convert R, S BIGNUM to DER signature
 * \param[in] r the BIGNUM R component of the signature
 * \param[in] s the BIGNUM S component of the signature
 * \return The DER signature and it's length
 */
std::tuple<DER_Signature_ptr, std::size_t> bn_to_signature(const bn_t& r, const bn_t& s);

/**
 * \brief convert R, S BIGNUM to DER signature
 * \param[in] r the BIGNUM R component of the signature (0-padded 32 bytes)
 * \param[in] s the BIGNUM S component of the signature (0-padded 32 bytes)
 * \return The DER signature and it's length
 */
std::tuple<DER_Signature_ptr, std::size_t> bn_to_signature(const std::uint8_t* r, const std::uint8_t* s);

/**
 * \brief convert DER signature into BIGNUM R and S components
 * \param[out] r the BIGNUM R component of the signature
 * \param[out] s the BIGNUM S component of the signature
 * \param[in] sig_p a pointer to the DER encoded signature
 * \param[in] len the length of the DER encoded signature
 * \return true when successful
 */
bool signature_to_bn(openssl::bn_t& r, openssl::bn_t& s, const std::uint8_t* sig_p, std::size_t len);

/**
 * \brief load any PEM encoded certificates from a file
 * \param[in] filename
 * \return a list of 0 or more certificates
 */
CertificateList load_certificates(const char* filename);

/**
 * \brief convert a certificate to a PEM string
 * \param[in] cert the certificate
 * \return the PEM string or empty on error
 */
std::string certificate_to_pem(const x509_st* cert);

/**
 * \brief parse a DER (ASN.1) encoded certificate
 * \param[in] der a pointer to the DER encoded certificate
 * \param[in] len the length of the DER encoded certificate
 * \return the certificate or empty unique_ptr on error
 */
Certificate_ptr der_to_certificate(const std::uint8_t* der, std::size_t len);

/**
 * \brief verify a certificate against a certificate chain and trust anchors
 * \param[in] cert the certificate to verify - when nullptr the certificate must
 *            be the first certificate in the untrusted list
 * \param[in] trust_anchors a list of trust anchors. Must not contain any
 *            intermediate CAs
 * \param[in] untrusted intermediate CAs needed to form a chain from the leaf
 *            certificate to one of the supplied trust anchors
 */
verify_result_t verify_certificate(const x509_st* cert, const CertificateList& trust_anchors,
                                   const CertificateList& untrusted);

/**
 * \brief extract the certificate subject as a dictionary of name/value pairs
 * \param cert the certificate
 * \return dictionary of the (short name, value) pairs
 * \note short name examples "CN" for CommonName "OU" for OrganizationalUnit
 *       "C" for Country ...
 */
std::map<std::string, std::string> certificate_subject(const x509_st* cert);

/**
 * \brief extract the subject public key from the certificate
 * \param[in] cert the certificate
 * \return a unique_ptr holding the key or empty on error
 */
PKey_ptr certificate_public_key(x509_st* cert);

enum class log_level_t : std::uint8_t {
    debug,
    warning,
    error,
};

/**
 * \brief log an OpenSSL event
 * \param[in] level the event level
 * \param[in] str string to display
 * \note any OpenSSL error is displayed after the string
 */
void log(log_level_t level, const std::string& str);

static inline void log_error(const std::string& str) {
    log(log_level_t::error, str);
}

static inline void log_warning(const std::string& str) {
    log(log_level_t::warning, str);
}

static inline void log_debug(const std::string& str) {
    log(log_level_t::debug, str);
}

using log_handler_t = void (*)(log_level_t level, const std::string& err);

/**
 * \brief set log handler function
 * \param[in] handler a pointer to the function
 * \return the pointer to the previous handler or nullptr
 *         where there is no previous handler
 */
log_handler_t set_log_handler(log_handler_t handler);

} // namespace openssl

#endif // OPENSSL_UTIL_HPP_
