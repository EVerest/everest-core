// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include <everest/logging.hpp>
#include <openssl/provider.h>

namespace evse_security {

static constexpr const char* PROVIDER_NULL = "null";
static constexpr const char* PROVIDER_DEFAULT = "default";
static constexpr const char* PROVIDER_TPM = "tpm2";

enum class Provider {
    SSL_NULL,
    DEFAULT,
    TPM2,
    CUSTOM,
};

template <Provider provider> struct ProviderResolver {};

template <> struct ProviderResolver<Provider::SSL_NULL> {
    static constexpr const char* name() {
        return PROVIDER_NULL;
    }
};

template <> struct ProviderResolver<Provider::DEFAULT> {
    static constexpr const char* name() {
        return PROVIDER_DEFAULT;
    }
};

template <> struct ProviderResolver<Provider::TPM2> {
    static constexpr const char* name() {
        return PROVIDER_TPM;
    }
};

template <typename F> static void foreach_provider(F func) {
    auto iter = [](OSSL_PROVIDER* provider, void* cbdata) {
        F* fc = (F*)cbdata;
        (*fc)(provider);

        return 1;
    };

    OSSL_PROVIDER_do_all(nullptr, iter, &func);
}

static std::vector<std::string> get_current_provider_names() {
    std::vector<std::string> providers;
    foreach_provider(
        [&providers](OSSL_PROVIDER* provider) { providers.emplace_back(OSSL_PROVIDER_get0_name(provider)); });

    return providers;
}

static std::vector<OSSL_PROVIDER*> get_current_providers() {
    std::vector<OSSL_PROVIDER*> providers;
    foreach_provider([&providers](OSSL_PROVIDER* provider) { providers.emplace_back(provider); });

    return providers;
}

class ProviderLoadException : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

template <typename type, bool exists> struct OptionalMember {
    type data;
};

template <typename type> struct OptionalMember<type, false> {};

template <bool use_fallback> struct OSSLProvider {
    std::string get_required_provider() {
        return provider_required;
    }

    OSSL_PROVIDER* get_scope_provider_raw() {
        return provider_loaded;
    }

protected:
    OSSLProvider(const std::string& provider) {
        provider_required = provider;
    }

    virtual ~OSSLProvider() noexcept(false) {
    }

public: // Utilities for loading/unloading
    void load_provider() {
        persist_providers<use_fallback>();

        if constexpr (!use_fallback)
            provider_loaded = OSSL_PROVIDER_load(nullptr, provider_required.c_str());
        else
            provider_loaded = OSSL_PROVIDER_try_load(nullptr, provider_required.c_str(), 1);

        if (!provider_loaded) {
            std::string error = "Failed to load provider: [" + provider_required + "] all operations reverting!";
            throw new ProviderLoadException(error);
        }

        EVLOG_debug << "[PROV] Provider loaded: " << provider_required;
    }

    void unload_provider() {
        if (OSSL_PROVIDER_unload(provider_loaded) != 1) {
            std::string error = "Failed to unload provider: [" + provider_required + "]";
            throw new ProviderLoadException(error);
        }

        // Invalidate
        provider_loaded = nullptr;
        restore_providers<use_fallback>();

        EVLOG_debug << "[PROV] Provider unloaded: " << provider_required;
    }

private:
    template <bool T> typename std::enable_if<!T>::type persist_providers() {
        old_providers.data = get_current_provider_names();

        // No internal OSSL iteration while unloading
        std::vector<OSSL_PROVIDER*> providers = get_current_providers();
        for (auto& provider : providers) {
            if (!OSSL_PROVIDER_unload(provider)) {
                std::string error = "Failed to unload old provider!";
                throw new ProviderLoadException(error);
            }
        }
    }

    template <bool T> typename std::enable_if<!T>::type restore_providers() {
        for (auto& provider : old_providers.data) {
            if (!OSSL_PROVIDER_load(nullptr, provider.c_str())) {
                std::string error = "Failed to load old provider: " + provider;
                throw new ProviderLoadException(error);
            }
        }
    }

    template <bool T> typename std::enable_if<T>::type persist_providers() {
    }
    template <bool T> typename std::enable_if<T>::type restore_providers() {
    }

private:
    std::string provider_required;
    OSSL_PROVIDER* provider_loaded;

    OptionalMember<std::vector<std::string>, !use_fallback> old_providers;
};

template <bool use_fallback> struct OSSLScopedProvider : public OSSLProvider<use_fallback> {
protected:
    OSSLScopedProvider(const std::string& provider) : OSSLProvider<use_fallback>(provider) {
        this->load_provider();
    }

    virtual ~OSSLScopedProvider() noexcept(false) {
        this->unload_provider();
    }
};

template <Provider provider, bool use_fallback>
struct OSSLAutoresolvedScopedProvider : OSSLScopedProvider<use_fallback> {
    OSSLAutoresolvedScopedProvider() : OSSLScopedProvider<use_fallback>(ProviderResolver<provider>::name()) {
    }
};

template <Provider provider, bool use_fallback> struct OSSLAutoresolvedProvider : OSSLProvider<use_fallback> {
    OSSLAutoresolvedProvider() : OSSLProvider<use_fallback>(ProviderResolver<provider>::name()) {
    }
};

// NULL provider to test certain operations
typedef OSSLAutoresolvedScopedProvider<Provider::SSL_NULL, false> NullScopedProvider;

/// @brief Scoped TPM provider. In order to use the TPM for a certain operation simply
/// declare an object of this type within a scope. Example usage:
/// {
///     TPMScopedProvider TPM();
///     ... do TPM operations here normally ...
/// }
typedef OSSLAutoresolvedScopedProvider<Provider::TPM2, false> TPMScopedProvider;

/// @brief Same as above, but with manual locking and unlocking
typedef OSSLAutoresolvedProvider<Provider::TPM2, false> TPMProvider;

/// @brief Same as the above but with the currently loaded provider as fallback
typedef OSSLAutoresolvedScopedProvider<Provider::TPM2, true> TPMScopedProviderFallback;

} // namespace evse_security