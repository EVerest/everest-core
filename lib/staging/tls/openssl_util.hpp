// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#ifndef OPENSSL_UTIL_HPP_
#define OPENSSL_UTIL_HPP_

#include "extensions/tls_types.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <map>
#include <memory>
#include <string>
#include <vector>

struct evp_pkey_st;
struct ssl_st;
struct x509_st;

namespace openssl {

/// X509 certificate verification result
enum class verify_result_t : std::uint8_t {
    Verified,
    CertChainError,
    CertificateExpired,
    CertificateRevoked,
    NoCertificateAvailable,
    OtherError,
};

constexpr std::size_t signature_size = 64;
constexpr std::size_t signature_n_size = 32;
constexpr std::size_t signature_der_size = 128;
constexpr std::size_t sha_1_digest_size = 20;
constexpr std::size_t sha_256_digest_size = 32;
constexpr std::size_t sha_384_digest_size = 48;
constexpr std::size_t sha_512_digest_size = 64;

enum class digest_alg_t : std::uint8_t {
    sha1,
    sha256,
    sha384,
    sha512,
};

using sha_1_digest_t = std::array<std::uint8_t, sha_1_digest_size>;
using sha_256_digest_t = std::array<std::uint8_t, sha_256_digest_size>;
using sha_384_digest_t = std::array<std::uint8_t, sha_384_digest_size>;
using sha_512_digest_t = std::array<std::uint8_t, sha_512_digest_size>;
using bn_t = std::array<std::uint8_t, signature_n_size>;
using bn_const_t = std::array<const std::uint8_t, signature_n_size>;

using certificate_ptr = std::unique_ptr<x509_st, void (*)(x509_st*)>;
using certificate_list = std::vector<certificate_ptr>;
using der_underlying_t = std::uint8_t;
using der_ptr = std::unique_ptr<der_underlying_t, void (*)(der_underlying_t*)>;
using pkey_ptr = std::unique_ptr<evp_pkey_st, void (*)(evp_pkey_st*)>;

/**
 * \brief represents DER encoded ASN1 data
 *
 * This class simplifies managing DER data ensuring that it is managed and
 * supporting equality tests.
 */
class DER {
private:
    der_ptr ptr{nullptr, nullptr}; //!< pointer to the data
    std::size_t len{0};            //!< length of the data

public:
    DER() = default;
    /**
     * \brief create space for DER data of specified size
     * \param[in] size to allocate (note data is zeroed)
     */
    explicit DER(std::size_t size);
    /**
     * \brief create a copy of the supplied DER data
     * \param[in] crc pointer to the DER data
     * \param[in] size of the DER data
     */
    DER(const der_underlying_t* src, std::size_t size);
    /**
     * \brief move a unique pointer and size to this object
     * \param[in] der a unique pointer to DER data
     * \param[in] size the size of the DER data
     */
    DER(der_ptr&& der, std::size_t size) : ptr(std::move(der)), len(size) {
    }

    DER(const DER& obj);
    DER& operator=(const DER& obj);

    DER(DER&& obj) noexcept;
    DER& operator=(DER&& obj) noexcept;

    bool operator==(const DER& rhs) const;
    inline bool operator!=(const DER& rhs) const {
        return !(*this == rhs);
    }
    bool operator==(const der_underlying_t* rhs) const;
    inline bool operator!=(const der_underlying_t* rhs) const {
        return !(*this == rhs);
    }
    explicit operator bool() const;

    [[nodiscard]] inline der_underlying_t* get() {
        return ptr.get();
    }

    /**
     * \brief release the pointer, must be freed using DER::free()
     * \return the pointer to DER data (or nullptr)
     */
    [[nodiscard]] inline der_underlying_t* release() {
        len = 0;
        return ptr.release();
    }

    [[nodiscard]] inline const der_underlying_t* get() const {
        return ptr.get();
    }

    [[nodiscard]] inline std::size_t size() const {
        return len;
    }

    /**
     * \brief create unmanaged copy which must be freed using DER::free()
     * \param[in] obj copy the memory contents from this object
     * \return a pointer to newly allocated heap memory
     */
    static der_underlying_t* dup(const DER& obj);

    /**
     * \brief free memory allocated by DER::dup()
     * \param[in] ptr pointer to the allocated memory (can be nullptr)
     */
    static void free(der_underlying_t* ptr);
};

/// contains filenames for the leaf, intermediate CAs and roots
struct chain_filenames_t {
    const char* leaf;
    const char* chain;
    const char* root;
};

using chain_filenames_list_t = std::vector<chain_filenames_t>;

/// contains the X509 certificates for leaf, intermediate CAs and roots
struct chain_info_t {
    certificate_ptr leaf;
    certificate_list chain;
    certificate_list trust_anchors;
};

using chain_info_list_t = std::vector<chain_info_t>;

/// X509 certificate chain and the private key for the leaf certificate
struct chain_t {
    chain_info_t chain{{nullptr, nullptr}, {}, {}};
    pkey_ptr private_key{nullptr, nullptr};
};

using chain_list = std::vector<chain_t>;

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
 * \brief calculate the SHA1 digest over an array of bytes
 * \param[in] data the start of the data
 * \param[in] len the length of the data
 * \param[out] the SHA1 digest
 * \return true on success
 */
bool sha_1(const void* data, std::size_t len, sha_1_digest_t& digest);

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
 * \brief load a private key from file
 * \param mem the structure to zero
 */
pkey_ptr load_private_key(const char* filename, const char* password);

/**
 * \brief convert R, S BIGNUM to DER signature
 * \param[in] r the BIGNUM R component of the signature
 * \param[in] s the BIGNUM S component of the signature
 * \return The DER signature and its length
 */
DER bn_to_signature(const bn_t& r, const bn_t& s);

/**
 * \brief convert R, S BIGNUM to DER signature
 * \param[in] r the BIGNUM R component of the signature (0-padded 32 bytes)
 * \param[in] s the BIGNUM S component of the signature (0-padded 32 bytes)
 * \return The DER signature and its length
 */
DER bn_to_signature(const std::uint8_t* r, const std::uint8_t* s);

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
certificate_list load_certificates(const char* filename);

/**
 * \brief load any PEM encoded certificates from list of files
 * \param[in] filenames
 * \return a list of 0 or more certificates
 */
certificate_list load_certificates(const std::vector<const char*>& filenames);

/**
 * \brief load a PKI chain from leaf to root
 * \param[in] leaf_file is the server certificate
 * \param[in] chain_file is the file of intermediate certificates
 * \param[in] root_file is the self signed trust anchor
 * \return the certificate chain. chain_info_t::leaf will be nullptr when a chain
 *         cannot be built
 * \note when leaf_file is null pointer the server certificate is the first certificate in the chain_file
 */
chain_info_t load_certificates(const char* leaf_file, const char* chain_file, const char* root_file);

/**
 * \brief load a PKI chain from leaf to root
 * \param[in] chain is the structure containing the three filenames
 * \return the certificate chain. chain_info_t::leaf will be nullptr when a chain
 *         cannot be built
 * \note when leaf_file is null pointer the server certificate is the first certificate in the chain_file
 */
static inline chain_info_t load_certificates(const chain_filenames_t& chain) {
    return load_certificates(chain.leaf, chain.chain, chain.root);
}

/**
 * \brief load a PKI chains from leaf to root from a list of chain filenames
 * \param[in] chains is a list of structures containing the three filenames
 * \return a list of valid certificate chains (can be an empty list)
 */
chain_info_list_t load_certificates(const chain_filenames_list_t& chains);

/**
 * \brief check that a private key is associated with a certificate
 * \param[in] cert is the certificate
 * \param[in] pkey is the private key
 * \return true when the key matches the certificate
 */
bool verify_certificate_key(const x509_st* cert, const evp_pkey_st* pkey);

/**
 * \brief verify a certificate chain from leaf to trust anchor(s)
 * \param[in] chain the structure containing the certificates
 * \return true when there is a valid chain
 */
bool verify_chain(const chain_info_t& chain);

/**
 * \brief verify a certificate chain from leaf to trust anchor(s) and that
 *        the private key is associated with the leaf certificate
 * \param[in] chain the structure containing the certificates and private key
 * \return true when there is a valid chain and the key matches
 */
bool verify_chain(const chain_t& chain);

/**
 * \brief apply the certificates and private key to the SSL session
 * \param[in] chain the structure containing the certificates and private key
 * \return true when successfully applied
 */
bool use_certificate_and_key(ssl_st* ssl, const chain_t& chain);

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
certificate_ptr der_to_certificate(const std::uint8_t* der, std::size_t len);

/**
 * \brief verify a certificate against a certificate chain and trust anchors
 * \param[in] cert the certificate to verify - when nullptr the certificate must
 *            be the first certificate in the untrusted list
 * \param[in] trust_anchors a list of trust anchors. Must not contain any
 *            intermediate CAs
 * \param[in] untrusted intermediate CAs needed to form a chain from the leaf
 *            certificate to one of the supplied trust anchors
 */
verify_result_t verify_certificate(const x509_st* cert, const certificate_list& trust_anchors,
                                   const certificate_list& untrusted);

/**
 * \brief extract the certificate subject as a dictionary of name/value pairs
 * \param cert the certificate
 * \return dictionary of the (short name, value) pairs
 * \note short name examples "CN" for CommonName "OU" for OrganizationalUnit
 *       "C" for Country ...
 */
std::map<std::string, std::string> certificate_subject(const x509_st* cert);

/**
 * \brief extract the certificate subject as DER encoded data
 * \param cert the certificate
 * \return the DER encoded subject or nullptr on error
 */
DER certificate_subject_der(const x509_st* cert);

/**
 * \brief extract the subject public key from the certificate
 * \param[in] cert the certificate
 * \return a unique_ptr holding the key or empty on error
 */
pkey_ptr certificate_public_key(x509_st* cert);

/**
 * \brief calculate SHA1 hash over the DER certificate
 * \param[out] digest the SHA1 digest of the certificate
 * \param[in] cert the certificate
 * \return true on success
 */
bool certificate_sha_1(openssl::sha_1_digest_t& digest, const x509_st* cert);

/**
 * \brief calculate SHA1 hash over the DER certificate's subject public key
 * \param[out] digest the SHA1 digest of the public key
 * \param[in] cert the certificate
 * \return true on success
 */
bool certificate_subject_public_key_sha_1(openssl::sha_1_digest_t& digest, const x509_st* cert);

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
