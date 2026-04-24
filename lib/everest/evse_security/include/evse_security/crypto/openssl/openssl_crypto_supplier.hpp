// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#ifdef LIBEVSE_CRYPTO_SUPPLIER_OPENSSL

#include <evse_security/crypto/interface/crypto_supplier.hpp>

#include <openssl/types.h>

namespace evse_security {

/// X509 verify callback that suppresses X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION when every
/// critical extension on the current cert has a well-known RFC 5280 NID. Unknown/custom critical
/// OIDs still cause verification to fail (and are logged). Signature matches both
/// X509_STORE_CTX_set_verify_cb and SSL_CTX_set_verify.
int critical_extension_bypass_callback(int ok, X509_STORE_CTX* ctx);

class OpenSSLSupplier : public AbstractCryptoSupplier {
public:
    static const char* get_supplier_name();

    static bool supports_tpm();
    static bool supports_tpm_key_creation();
    static bool supports_custom_key_creation();

    static bool generate_key(const KeyGenerationInfo& key_info, KeyHandle_ptr& out_key);

    static std::vector<X509Handle_ptr> load_certificates(const std::string& data, const EncodingFormat encoding);

    static std::string x509_to_string(X509Handle* handle);
    static std::string x509_get_responder_url(X509Handle* handle);
    static std::string x509_get_key_hash(X509Handle* handle);
    static std::string x509_get_serial_number(X509Handle* handle);
    static std::string x509_get_issuer_name_hash(X509Handle* handle);
    static std::string x509_get_common_name(X509Handle* handle);
    static bool x509_get_validity(X509Handle* handle, std::int64_t& out_valid_in, std::int64_t& out_valid_to);
    static bool x509_is_selfsigned(X509Handle* handle);
    static bool x509_is_child(X509Handle* child, X509Handle* parent, bool ignore_unhandled_critical_extensions = false);
    static bool x509_is_equal(X509Handle* a, X509Handle* b);
    static X509Handle_ptr x509_duplicate_unique(X509Handle* handle);
    static CertificateValidationResult
    x509_verify_certificate_chain(X509Handle* target, const std::vector<X509Handle*>& parents,
                                  const std::vector<X509Handle*>& untrusted_subcas, bool allow_future_certificates,
                                  const std::optional<fs::path> dir_path, const std::optional<fs::path> file_path,
                                  bool ignore_unhandled_critical_extensions = false);
    static KeyValidationResult x509_check_private_key(X509Handle* handle, std::string private_key,
                                                      std::optional<std::string> password);
    static bool x509_verify_signature(X509Handle* handle, const std::vector<std::uint8_t>& signature,
                                      const std::vector<std::uint8_t>& data);

    static CertificateSignRequestResult x509_generate_csr(const CertificateSigningRequestInfo& csr_info,
                                                          std::string& out_csr);

    static bool digest_file_sha256(const fs::path& path, std::vector<std::uint8_t>& out_digest);

    static bool base64_decode_to_bytes(const std::string& base64_string, std::vector<std::uint8_t>& out_decoded);
    static bool base64_decode_to_string(const std::string& base64_string, std::string& out_decoded);

    static bool base64_encode_from_bytes(const std::vector<std::uint8_t>& bytes, std::string& out_encoded);
    static bool base64_encode_from_string(const std::string& string, std::string& out_encoded);
};

} // namespace evse_security

#endif