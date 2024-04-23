// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>
#include <evse_security/crypto/interface/crypto_supplier.hpp>

#define default_crypto_supplier_usage_error()                                                                          \
    EVLOG_error << "Invoked default unimplemented crypto function: [" << __func__ << "]";

namespace evse_security {

const char* AbstractCryptoSupplier::get_supplier_name() {
    return "AbstractCryptoSupplier";
}

bool AbstractCryptoSupplier::supports_tpm() {
    default_crypto_supplier_usage_error() return false;
}

bool AbstractCryptoSupplier::supports_tpm_key_creation() {
    default_crypto_supplier_usage_error() return false;
}

bool AbstractCryptoSupplier::generate_key(const KeyGenerationInfo& key_info, KeyHandle_ptr& out_key) {
    default_crypto_supplier_usage_error() return false;
}

/// @brief Loads all certificates from the string data that can contain multiple cetifs
std::vector<X509Handle_ptr> AbstractCryptoSupplier::load_certificates(const std::string& data,
                                                                      const EncodingFormat encoding) {
    default_crypto_supplier_usage_error() return {};
}

std::string AbstractCryptoSupplier::x509_to_string(X509Handle* handle) {
    default_crypto_supplier_usage_error() return {};
}

std::string AbstractCryptoSupplier::x509_get_responder_url(X509Handle* handle) {
    default_crypto_supplier_usage_error() return {};
}

std::string AbstractCryptoSupplier::x509_get_key_hash(X509Handle* handle) {
    default_crypto_supplier_usage_error() return {};
}

std::string AbstractCryptoSupplier::x509_get_serial_number(X509Handle* handle) {
    default_crypto_supplier_usage_error() return {};
}

std::string AbstractCryptoSupplier::x509_get_issuer_name_hash(X509Handle* handle) {
    default_crypto_supplier_usage_error() return {};
}

std::string AbstractCryptoSupplier::x509_get_common_name(X509Handle* handle) {
    default_crypto_supplier_usage_error() return {};
}

void AbstractCryptoSupplier::x509_get_validity(X509Handle* handle, std::int64_t& out_valid_in,
                                               std::int64_t& out_valid_to) {
    default_crypto_supplier_usage_error()
}

bool AbstractCryptoSupplier::x509_is_selfsigned(X509Handle* handle) {
    default_crypto_supplier_usage_error() return false;
}

bool AbstractCryptoSupplier::x509_is_child(X509Handle* child, X509Handle* parent) {
    default_crypto_supplier_usage_error() return false;
}

bool AbstractCryptoSupplier::x509_is_equal(X509Handle* a, X509Handle* b) {
    default_crypto_supplier_usage_error() return (a == b);
}

static X509Handle_ptr x509_duplicate_unique(X509Handle* handle) {
    default_crypto_supplier_usage_error() return {};
}

CertificateValidationResult AbstractCryptoSupplier::x509_verify_certificate_chain(
    X509Handle* target, const std::vector<X509Handle*>& parents, bool allow_future_certificates,
    const std::optional<fs::path> dir_path, const std::optional<fs::path> file_path) {
    default_crypto_supplier_usage_error() return CertificateValidationResult::Unknown;
}

KeyValidationResult AbstractCryptoSupplier::x509_check_private_key(X509Handle* handle, std::string private_key,
                                                                   std::optional<std::string> password) {
    default_crypto_supplier_usage_error() return KeyValidationResult::Unknown;
}

bool AbstractCryptoSupplier::x509_verify_signature(X509Handle* handle, const std::vector<std::byte>& signature,
                                                   const std::vector<std::byte>& data) {
    default_crypto_supplier_usage_error() return false;
}

bool AbstractCryptoSupplier::x509_generate_csr(const CertificateSigningRequestInfo& csr_info, std::string& out_csr) {
    default_crypto_supplier_usage_error() return false;
}

bool AbstractCryptoSupplier::digest_file_sha256(const fs::path& path, std::vector<std::byte>& out_digest) {
    default_crypto_supplier_usage_error() return false;
}

bool AbstractCryptoSupplier::decode_base64_signature(const std::string& signature,
                                                     std::vector<std::byte>& out_decoded) {
    default_crypto_supplier_usage_error() return false;
}

} // namespace evse_security