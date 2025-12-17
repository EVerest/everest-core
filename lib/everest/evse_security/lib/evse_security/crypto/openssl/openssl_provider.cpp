// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <evse_security/crypto/openssl/openssl_provider.hpp>
#include <evse_security/evse_types.hpp>

#include <openssl/opensslv.h>

#if USING_CUSTOM_PROVIDER
// OpenSSL3 without TPM will use the default provider anyway
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/provider.h>

#include <everest/logging.hpp>
#else
// dummy structures for non-OpenSSL 3
struct ossl_provider_st {};
using OSSL_PROVIDER = struct ossl_provider_st;
struct ossl_lib_ctx_st;
using OSSL_LIB_CTX = struct ossl_lib_ctx_st;
#endif

namespace evse_security {

namespace {
const auto KEY_HEADER_DEFAULT = "-----BEGIN PRIVATE KEY-----";
const auto KEY_HEADER_DEFAULT_ENCRYPTED = "-----BEGIN ENCRYPTED PRIVATE KEY-----";

const auto KEY_HEADER_TPM2 = "-----BEGIN TSS2 PRIVATE KEY-----";
} // namespace

bool is_custom_private_key_string(const std::string& private_key_pem) {
    // If we can't find the standard header it means it's a custom key
    return private_key_pem.find(KEY_HEADER_DEFAULT) == std::string::npos &&
           private_key_pem.find(KEY_HEADER_DEFAULT_ENCRYPTED) == std::string::npos;
}

bool is_custom_private_key_file(const fs::path& private_key_file_pem) {
    if (fs::is_regular_file(private_key_file_pem)) {
        fsstd::ifstream key_file(private_key_file_pem);
        std::string line;
        std::getline(key_file, line);
        key_file.close();

        // Search for the standard header
        return is_custom_private_key_string(line);
    }

    return false;
}

#ifdef USING_CUSTOM_PROVIDER

constexpr bool is_custom_provider_tpm() {
    // custom provider string (see CMakeLists.txt)
    constexpr const std::string_view custom_provider(CUSTOM_PROVIDER_NAME);
    return (custom_provider == "tpm2");
}

// ----------------------------------------------------------------------------
// class OpenSSLProvider OpenSSL 3

static const char* mode_t_str[2] = {
    "default provider", // mode_t::default_provider
    "custom provider"   // mode_t::custom_provider
};

static_assert(static_cast<int>(OpenSSLProvider::mode_t::default_provider) == 0);
static_assert(static_cast<int>(OpenSSLProvider::mode_t::custom_provider) == 1);

std::ostream& operator<<(std::ostream& out, OpenSSLProvider::mode_t mode) {
    const unsigned int idx = static_cast<unsigned int>(mode);
    if (idx <= static_cast<unsigned int>(OpenSSLProvider::mode_t::custom_provider)) {
        out << mode_t_str[idx];
    }
    return out;
}

static bool s_load_and_test_provider(OSSL_PROVIDER*& provider, OSSL_LIB_CTX* libctx, const char* provider_name) {
    bool result = true;
#ifdef DEBUG
    const char* modestr = (libctx == nullptr) ? "global" : "TLS";
    EVLOG_info << "Loading " << modestr << " provider: " << provider_name;
#endif
    if ((provider = OSSL_PROVIDER_load(libctx, provider_name)) == nullptr) {
        EVLOG_error << "Unable to load OSSL_PROVIDER: " << provider_name;
        ERR_print_errors_fp(stderr);
        result = false;
    } else {
#ifdef DEBUG
        EVLOG_info << "Testing " << modestr << " provider: " << provider_name;
#endif
        if (OSSL_PROVIDER_self_test(provider) == 0) {
            EVLOG_error << "Self-test failed: OSSL_PROVIDER: " << provider_name;
            ERR_print_errors_fp(stderr);
            OSSL_PROVIDER_unload(provider);
            provider = nullptr;
            result = false;
        }
    }
    return result;
}

std::mutex OpenSSLProvider::s_mux;
OpenSSLProvider::flags_underlying_t OpenSSLProvider::s_flags = 0;

OSSL_PROVIDER* OpenSSLProvider::s_global_prov_default_p = nullptr;
OSSL_PROVIDER* OpenSSLProvider::s_global_prov_custom_p = nullptr;
OSSL_PROVIDER* OpenSSLProvider::s_tls_prov_default_p = nullptr;
OSSL_PROVIDER* OpenSSLProvider::s_tls_prov_custom_p = nullptr;
OSSL_LIB_CTX* OpenSSLProvider::s_tls_libctx_p = nullptr;

// propquery strings (see CMakeLists.txt)
static const char* s_default_provider = PROPQUERY_PROVIDER_DEFAULT;
static const char* s_custom_provider = PROPQUERY_PROVIDER_CUSTOM;

OpenSSLProvider::OpenSSLProvider() {
    s_mux.lock();

    if (is_reset(flags_t::initialised)) {
        set(flags_t::initialised);
        OPENSSL_atexit(&OpenSSLProvider::cleanup);

        if (s_tls_libctx_p == nullptr) {
            s_tls_libctx_p = OSSL_LIB_CTX_new();
            if (s_tls_libctx_p == nullptr) {
                EVLOG_error << "Unable to create OpenSSL library context";
                ERR_print_errors_fp(stderr);
            }
        }

        // load providers for global context
        (void)load_global(mode_t::default_provider);
        (void)load_global(mode_t::custom_provider);
        (void)set_propstr(nullptr, mode_t::default_provider);

        // load providers for tls context
        (void)load_tls(mode_t::default_provider);
        (void)load_tls(mode_t::custom_provider);
        (void)set_propstr(s_tls_libctx_p, mode_t::default_provider);
    }
}

OpenSSLProvider::~OpenSSLProvider() {
    set_global_mode(OpenSSLProvider::mode_t::default_provider);
    s_mux.unlock();
}

bool OpenSSLProvider::load(OSSL_PROVIDER*& default_p, OSSL_PROVIDER*& custom_p, OSSL_LIB_CTX* libctx_p, mode_t mode) {
    bool result = true;
    switch (mode) {
    case mode_t::custom_provider:
        if (custom_p == nullptr) {
            // custom provider string (see CMakeLists.txt)
            result = s_load_and_test_provider(custom_p, libctx_p, CUSTOM_PROVIDER_NAME);
            update(flags_t::custom_provider_available, result);
        }
        break;
    case mode_t::default_provider:
    default:
        if (default_p == nullptr) {
            result = s_load_and_test_provider(default_p, libctx_p, "default");
        }
        break;
    }
    return result;
}

bool OpenSSLProvider::set_propstr(OSSL_LIB_CTX* libctx, mode_t mode) {
    const char* propstr = propquery(mode);
#ifdef DEBUG
    EVLOG_info << "Setting " << ((libctx == nullptr) ? "global" : "tls") << " propquery: " << propstr;
#endif
    const bool result = EVP_set_default_properties(libctx, propstr) == 1;
    if (!result) {
        EVLOG_error << "Unable to set OpenSSL provider: " << mode;
        ERR_print_errors_fp(stderr);
    }
    return result;
}

bool OpenSSLProvider::set_mode(OSSL_LIB_CTX* libctx, mode_t mode) {
    bool result;
    const flags_t f = (libctx == nullptr) ? flags_t::global_custom_provider : flags_t::tls_custom_provider;

    const bool apply = update(f, mode == mode_t::custom_provider);
    if (apply) {
        result = set_propstr(libctx, mode);
    }

    return result;
}

const char* OpenSSLProvider::propquery(mode_t mode) const {
    const char* propquery_str = nullptr;

    switch (mode) {
    case mode_t::default_provider:
        propquery_str = s_default_provider;
        break;
    case mode_t::custom_provider:
        propquery_str = s_custom_provider;
        break;
    default:
        break;
    }

    return propquery_str;
}

void OpenSSLProvider::cleanup() {
    // at the point this is called logging may not be available
    // relying on OpenSSL errors
    std::lock_guard guard(s_mux);
    if (OSSL_PROVIDER_unload(s_tls_prov_custom_p) == 0) {
        ERR_print_errors_fp(stderr);
    }
    if (OSSL_PROVIDER_unload(s_tls_prov_default_p) == 0) {
        ERR_print_errors_fp(stderr);
    }
    if (OSSL_PROVIDER_unload(s_global_prov_custom_p) == 0) {
        ERR_print_errors_fp(stderr);
    }
    if (OSSL_PROVIDER_unload(s_global_prov_default_p) == 0) {
        ERR_print_errors_fp(stderr);
    }

    s_tls_prov_custom_p = nullptr;
    s_tls_prov_default_p = nullptr;
    s_global_prov_custom_p = nullptr;
    s_global_prov_default_p = nullptr;

    OSSL_LIB_CTX_free(s_tls_libctx_p);

    s_tls_libctx_p = nullptr;
    s_flags = 0;
}

bool OpenSSLProvider::supports_provider_tpm() {
    return is_set(flags_t::custom_provider_available) && is_custom_provider_tpm();
}

bool OpenSSLProvider::supports_provider_custom() {
    return is_set(flags_t::custom_provider_available);
}

#else // USING_OPENSSL_3_TPM
// ----------------------------------------------------------------------------
// class OpenSSLProvider dummy where OpenSSL 3 is not available

OpenSSLProvider::flags_underlying_t OpenSSLProvider::s_flags = 0;

OSSL_PROVIDER* OpenSSLProvider::s_global_prov_default_p = nullptr;
OSSL_PROVIDER* OpenSSLProvider::s_global_prov_custom_p = nullptr;
OSSL_PROVIDER* OpenSSLProvider::s_tls_prov_default_p = nullptr;
OSSL_PROVIDER* OpenSSLProvider::s_tls_prov_custom_p = nullptr;
OSSL_LIB_CTX* OpenSSLProvider::s_tls_libctx_p = nullptr;

OpenSSLProvider::OpenSSLProvider() = default;

OpenSSLProvider::~OpenSSLProvider() = default;

bool OpenSSLProvider::load(OSSL_PROVIDER*& /*unused*/, OSSL_PROVIDER*& /*unused*/, OSSL_LIB_CTX* /*unused*/,
                           mode_t /*unused*/) {
    return false;
}

bool OpenSSLProvider::set_propstr(OSSL_LIB_CTX* /*unused*/, mode_t /*unused*/) {
    return false;
}

bool OpenSSLProvider::set_mode(OSSL_LIB_CTX* /*unused*/, mode_t /*unused*/) {
    return false;
}

const char* OpenSSLProvider::propquery(mode_t /*mode*/) const {
    return nullptr;
}

void OpenSSLProvider::cleanup() {
}

bool OpenSSLProvider::supports_provider_tpm() {
    return false;
}

bool OpenSSLProvider::supports_provider_custom() {
    return false;
}

#endif // USING_OPENSSL_3_TPM

} // namespace evse_security
