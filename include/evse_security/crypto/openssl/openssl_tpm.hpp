// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef OPENSSL_TPM_HPP
#define OPENSSL_TPM_HPP

#include <cstdint>
#include <fstream>
#include <mutex>
#include <string>

// opaque types (from OpenSSL)
struct ossl_lib_ctx_st;  // OpenSSL OSSL_LIB_CTX;
struct ossl_provider_st; // OpenSSL OSSL_PROVIDER

namespace evse_security {

/// @brief determine if the PEM string is a TSS2 private key
/// @param private_key_pem string containing the PEM encoded key
/// @return true when "-----BEGIN TSS2 PRIVATE KEY-----" found
/// @note works irrespective of OpenSSL version
bool is_tpm_key_string(const std::string& private_key_pem);

/// @brief determine if the PEM file contains a TSS2 private key
/// @param private_key_file_pem filename of the PEM file
/// @return true when file starts "-----BEGIN TSS2 PRIVATE KEY-----"
/// @note works irrespective of OpenSSL version
bool is_tpm_key_filename(const std::string& private_key_file_pem);

/// @brief Manage the loading and configuring of OpenSSL providers
///
/// There are two providers considered:
/// - 'default'
/// - 'tpm2' for working with TSS2 keys (protected by a TPM)
///
/// There are two contexts:
/// - 'global' for general use
/// - 'tls' for TLS connections
///
/// The class also acts as a scoped mutex to prevent changes in the
/// provider configuration during crypto operations
///
/// @note OpenSSL SSL_CTX caches the propquery so updates via
///       this class may not be effective. See SSL_CTX_new_ex()
///
/// This code provides a null implementation when OpenSSL 3 or later isn't
/// used. The null implementation is also used when -DUSING_TPM2=OFF is
/// set with cmake.
///
/// @note the tpm2-abrmd daemon is needed to support openssl-tpm2 for TLS
class OpenSSLProvider {
public:
    /// @brief supported propquery strings
    enum class mode_t {
        default_provider,
        tpm2_provider,
    };

private:
    typedef std::uint8_t flags_underlying_t;
    enum class flags_t : flags_underlying_t {
        initialised,
        tpm2_available,
        global_tpm2,
        tls_tpm2,
    };

    static std::mutex s_mux;
    static flags_underlying_t s_flags;

    static struct ossl_provider_st* s_global_prov_default_p;
    static struct ossl_provider_st* s_global_prov_tpm_p;
    static struct ossl_provider_st* s_tls_prov_default_p;
    static struct ossl_provider_st* s_tls_prov_tpm_p;
    static struct ossl_lib_ctx_st* s_tls_libctx_p;

    static inline void reset(flags_t f) {
        s_flags &= ~(1 << static_cast<flags_underlying_t>(f));
    }

    static inline void set(flags_t f) {
        s_flags |= 1 << static_cast<flags_underlying_t>(f);
    }

    static inline bool is_set(flags_t f) {
        return (s_flags & (1 << static_cast<flags_underlying_t>(f))) != 0;
    }

    static inline bool is_reset(flags_t f) {
        return !is_set(f);
    }

    /// @brief uodate the flag
    /// @param f - flag to update
    /// @param val - whether to set or reset the flag
    /// @return true when the flag was changed
    static inline bool update(flags_t f, bool val) {
        bool bResult = val != is_set(f);
        if (val) {
            set(f);
        } else {
            reset(f);
        }
        return bResult;
    }

    bool load(struct ossl_provider_st*& default_p, struct ossl_provider_st*& tpm2_p, struct ossl_lib_ctx_st* libctx_p,
              mode_t mode);
    inline bool load_global(mode_t mode) {
        return load(s_global_prov_default_p, s_global_prov_tpm_p, nullptr, mode);
    }
    inline bool load_tls(mode_t mode) {
        return load(s_tls_prov_default_p, s_tls_prov_tpm_p, s_tls_libctx_p, mode);
    }

    bool set_propstr(struct ossl_lib_ctx_st* libctx, mode_t mode);
    bool set_mode(struct ossl_lib_ctx_st* libctx, mode_t mode);

public:
    OpenSSLProvider();
    ~OpenSSLProvider();

    inline void set_global_mode(mode_t mode) {
        set_mode(nullptr, mode);
    }

    inline void set_tls_mode(mode_t mode) {
        set_mode(s_tls_libctx_p, mode);
    }

    const char* propquery(mode_t mode) const;

    inline mode_t propquery_global() const {
        return (is_set(flags_t::global_tpm2)) ? mode_t::tpm2_provider : mode_t::default_provider;
    }
    inline mode_t propquery_tls() const {
        return (is_set(flags_t::tls_tpm2)) ? mode_t::tpm2_provider : mode_t::default_provider;
    }

    inline const char* propquery_global_str() const {
        return propquery(propquery_global());
    }
    inline const char* propquery_tls_str() const {
        return propquery(propquery_tls());
    }

    /// @brief return the TLS OSSL library context
    inline operator struct ossl_lib_ctx_st *() {
        return s_tls_libctx_p;
    }

    static inline bool supports_tpm() {
        return is_set(flags_t::tpm2_available);
    }

    static void cleanup();
};

} // namespace evse_security

#endif // OPENSSL_TPM_HPP