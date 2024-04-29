// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <evse_security/crypto/openssl/openssl_tpm.hpp>

#include <openssl/opensslv.h>

#define USING_OPENSSL_3 (OPENSSL_VERSION_NUMBER >= 0x30000000L)

#if USING_OPENSSL_3 && defined(USING_TPM2)
// OpenSSL3 without TPM will use the default provider anyway
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/provider.h>

#include <everest/logging.hpp>
#define USING_OPENSSL_3_TPM
#else
// dummy structures for non-OpenSSL 3
struct ossl_provider_st {};
typedef struct ossl_provider_st OSSL_PROVIDER;
struct ossl_lib_ctx_st;
typedef struct ossl_lib_ctx_st OSSL_LIB_CTX;
#endif

namespace evse_security {

bool is_tpm_key_string(const std::string& private_key_pem) {
    return private_key_pem.find("-----BEGIN TSS2 PRIVATE KEY-----") != std::string::npos;
}

bool is_tpm_key_file(const fs::path& private_key_file_pem) {
    if (fs::is_regular_file(private_key_file_pem)) {
        std::ifstream key_file(private_key_file_pem);
        std::string line;
        std::getline(key_file, line);
        key_file.close();

        return line.find("-----BEGIN TSS2 PRIVATE KEY-----") != std::string::npos;
    }

    return false;
}

#ifdef USING_OPENSSL_3_TPM
// ----------------------------------------------------------------------------
// class OpenSSLProvider OpenSSL 3

static const char* mode_t_str[2] = {
    "default provider", // mode_t::default_provider
    "tpm2 provider"     // mode_t::tpm2_provider
};

static_assert(static_cast<int>(OpenSSLProvider::mode_t::default_provider) == 0);
static_assert(static_cast<int>(OpenSSLProvider::mode_t::tpm2_provider) == 1);

std::ostream& operator<<(std::ostream& out, OpenSSLProvider::mode_t mode) {
    const unsigned int idx = static_cast<unsigned int>(mode);
    if (idx <= static_cast<unsigned int>(OpenSSLProvider::mode_t::tpm2_provider)) {
        out << mode_t_str[idx];
    }
    return out;
}

static bool s_load_and_test_provider(OSSL_PROVIDER*& provider, OSSL_LIB_CTX* libctx, const char* provider_name) {
    bool bResult = true;
#ifdef DEBUG
    const char* modestr = (libctx == nullptr) ? "global" : "TLS";
    EVLOG_info << "Loading " << modestr << " provider: " << provider_name;
#endif
    if ((provider = OSSL_PROVIDER_load(libctx, provider_name)) == nullptr) {
        EVLOG_error << "Unable to load OSSL_PROVIDER: " << provider_name;
        ERR_print_errors_fp(stderr);
        bResult = false;
    } else {
#ifdef DEBUG
        EVLOG_info << "Testing " << modestr << " provider: " << provider_name;
#endif
        if (OSSL_PROVIDER_self_test(provider) == 0) {
            EVLOG_error << "Self-test failed: OSSL_PROVIDER: " << provider_name;
            ERR_print_errors_fp(stderr);
            OSSL_PROVIDER_unload(provider);
            provider = nullptr;
            bResult = false;
        }
    }
    return bResult;
}

std::mutex OpenSSLProvider::s_mux;
OpenSSLProvider::flags_underlying_t OpenSSLProvider::s_flags = 0;

OSSL_PROVIDER* OpenSSLProvider::s_global_prov_default_p = nullptr;
OSSL_PROVIDER* OpenSSLProvider::s_global_prov_tpm_p = nullptr;
OSSL_PROVIDER* OpenSSLProvider::s_tls_prov_default_p = nullptr;
OSSL_PROVIDER* OpenSSLProvider::s_tls_prov_tpm_p = nullptr;
OSSL_LIB_CTX* OpenSSLProvider::s_tls_libctx_p = nullptr;

// propquery strings (see CMakeLists.txt)
static const char* s_default_provider = PROPQUERY_DEFAULT;
static const char* s_tpm2_provider = PROPQUERY_TPM2;

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
        (void)load_global(mode_t::tpm2_provider);
        (void)set_propstr(nullptr, mode_t::default_provider);

        // load providers for tls context
        (void)load_tls(mode_t::default_provider);
        (void)load_tls(mode_t::tpm2_provider);
        (void)set_propstr(s_tls_libctx_p, mode_t::default_provider);
    }
}

OpenSSLProvider::~OpenSSLProvider() {
    s_mux.unlock();
}

bool OpenSSLProvider::load(OSSL_PROVIDER*& default_p, OSSL_PROVIDER*& tpm2_p, OSSL_LIB_CTX* libctx_p, mode_t mode) {
    bool bResult = true;
    switch (mode) {
    case mode_t::tpm2_provider:
        if (tpm2_p == nullptr) {
            bResult = s_load_and_test_provider(tpm2_p, libctx_p, "tpm2");
            update(flags_t::tpm2_available, bResult);
        }
        break;
    case mode_t::default_provider:
    default:
        if (default_p == nullptr) {
            bResult = s_load_and_test_provider(default_p, libctx_p, "default");
        }
        break;
    }
    return bResult;
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
    bool bResult;
    const flags_t f = (libctx == nullptr) ? flags_t::global_tpm2 : flags_t::tls_tpm2;

    const bool apply = update(f, mode == mode_t::tpm2_provider);
    if (apply) {
        bResult = set_propstr(libctx, mode);
    }

    return bResult;
}

const char* OpenSSLProvider::propquery(mode_t mode) const {
    const char* propquery_str = nullptr;

    switch (mode) {
    case mode_t::default_provider:
        propquery_str = s_default_provider;
        break;
    case mode_t::tpm2_provider:
        propquery_str = s_tpm2_provider;
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
    if (OSSL_PROVIDER_unload(s_tls_prov_tpm_p) == 0) {
        ERR_print_errors_fp(stderr);
    }
    if (OSSL_PROVIDER_unload(s_tls_prov_default_p) == 0) {
        ERR_print_errors_fp(stderr);
    }
    if (OSSL_PROVIDER_unload(s_global_prov_tpm_p) == 0) {
        ERR_print_errors_fp(stderr);
    }
    if (OSSL_PROVIDER_unload(s_global_prov_default_p) == 0) {
        ERR_print_errors_fp(stderr);
    }

    s_tls_prov_tpm_p = nullptr;
    s_tls_prov_default_p = nullptr;
    s_global_prov_tpm_p = nullptr;
    s_global_prov_default_p = nullptr;

    OSSL_LIB_CTX_free(s_tls_libctx_p);

    s_tls_libctx_p = nullptr;
    s_flags = 0;
}

#else // USING_OPENSSL_3_TPM
// ----------------------------------------------------------------------------
// class OpenSSLProvider dummy where OpenSSL 3 is not available

OpenSSLProvider::flags_underlying_t OpenSSLProvider::s_flags = 0;

OSSL_PROVIDER* OpenSSLProvider::s_global_prov_default_p = nullptr;
OSSL_PROVIDER* OpenSSLProvider::s_global_prov_tpm_p = nullptr;
OSSL_PROVIDER* OpenSSLProvider::s_tls_prov_default_p = nullptr;
OSSL_PROVIDER* OpenSSLProvider::s_tls_prov_tpm_p = nullptr;
OSSL_LIB_CTX* OpenSSLProvider::s_tls_libctx_p = nullptr;

OpenSSLProvider::OpenSSLProvider() {
}

OpenSSLProvider::~OpenSSLProvider() {
}

bool OpenSSLProvider::load(OSSL_PROVIDER*&, OSSL_PROVIDER*&, OSSL_LIB_CTX*, mode_t) {
    return false;
}

bool OpenSSLProvider::set_propstr(OSSL_LIB_CTX*, mode_t) {
    return false;
}

bool OpenSSLProvider::set_mode(OSSL_LIB_CTX*, mode_t) {
    return false;
}

const char* OpenSSLProvider::propquery(mode_t mode) const {
    return nullptr;
}

void OpenSSLProvider::cleanup() {
}

#endif // USING_OPENSSL_3_TPM

} // namespace evse_security
