// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#ifndef EXTENSIONS_TLS_TYPES_
#define EXTENSIONS_TLS_TYPES_

#include <cstdint>

#include <EnumFlags.hpp>

struct ocsp_response_st;
struct ssl_ctx_st;
struct ssl_st;
struct x509_st;
struct evp_pkey_st;

namespace tls {

/// \brief flags used to keep track of TLS extensions in the client hello
class StatusFlags {
private:
    enum class flags_t : std::uint8_t {
        status_request,
        status_request_v2,
        trusted_ca_keys,
        last = trusted_ca_keys,
    };

    util::AtomicEnumFlags<flags_t, std::uint8_t> flags;

public:
    void status_request_received() {
        flags.set(flags_t::status_request);
    }
    void status_request_v2_received() {
        flags.set(flags_t::status_request_v2);
    }
    void trusted_ca_keys_received() {
        flags.set(flags_t::status_request_v2);
    }
    [[nodiscard]] bool has_status_request() const {
        return flags.is_set(flags_t::status_request);
    }
    [[nodiscard]] bool has_status_request_v2() const {
        return flags.is_set(flags_t::status_request_v2);
    }
    [[nodiscard]] bool has_trusted_ca_keys() const {
        return flags.is_set(flags_t::status_request_v2);
    }
};

// opaque types

using Certificate = struct ::x509_st;
using OcspResponse = struct ::ocsp_response_st;
using PKey = struct ::evp_pkey_st;
using Ssl = struct ::ssl_st;
using SslContext = struct ::ssl_ctx_st;

// see https://datatracker.ietf.org/doc/html/rfc6961
constexpr int TLSEXT_TYPE_status_request_v2 = 17;

} // namespace tls

#endif // EXTENSIONS_TLS_TYPES_
