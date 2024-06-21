// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#include "helpers.hpp"
#include "openssl_util.hpp"
#include <boost/smart_ptr/shared_ptr.hpp>
#include <extensions/trusted_ca_keys.hpp>

#include <cassert>
#include <limits>

#include <openssl/ssl.h>
#include <string>

namespace {

using namespace tls::trusted_ca_keys;
using namespace openssl;

using hash_fn = bool (*)(sha_1_digest_t& digest, const x509_st* cert);

/**
 * \brief check a list of certificates against trusted hashes
 * \param[in] hashes is the list of trusted hashes
 * \param[in] certs is the list of trust anchors
 * \param[in] gen_hash is the function to generate the hash
 * \return true when there is a match
 * \note this is a generic function that can be used to check certificate hashes
 *       or certificate public key hashes
 */
bool match_hash(const digest_list& hashes, const certificate_list& certs, hash_fn gen_hash) {
    bool result{false};
    if (!hashes.empty()) {
        // check for a match against all known trust anchors
        // select the first one to match
        for (const auto& ta : certs) {
            sha_1_digest_t digest;
            if (gen_hash(digest, ta.get())) {
                // with the digest from the trust anchor check against
                // the trusted CA keys hashes
                for (const auto& hash : hashes) {
                    result = digest == hash;
                    if (result) {
                        break;
                    }
                }
            }
            if (result) {
                break;
            }
        }
    }
    return result;
}

/**
 * \brief check trust anchors against certificate hashes
 * \param[in] extension contains the list of trusted certificate hashes
 * \param[in] chain contains the list of trust anchors
 * \return true when there is a match
 */
inline bool match_cert_hash(const trusted_ca_keys_t& extension, const chain_t& chain) {
    return match_hash(extension.cert_sha1_hash, chain.chain.trust_anchors, &certificate_sha_1);
}

/**
 * \brief check trust anchors against certificate public key hashes
 * \param[in] extension contains the list of trusted public key hashes
 * \param[in] chain contains the list of trust anchors
 * \return true when there is a match
 */
inline bool match_key_hash(const trusted_ca_keys_t& extension, const chain_t& chain) {
    return match_hash(extension.key_sha1_hash, chain.chain.trust_anchors, &certificate_subject_public_key_sha_1);
}

/**
 * \brief check trust anchors against certificate subject names
 * \param[in] extension contains the list of trusted certificate subject names
 * \param[in] chain contains the list of trust anchors
 * \return true when there is a match
 * \note compares the DER encoded certificate subject name
 */
bool match_name(const trusted_ca_keys_t& extension, const chain_t& chain) {
    bool result{false};
    if (!extension.x509_name.empty()) {
        for (const auto& ta : chain.chain.trust_anchors) {
            auto subject = certificate_subject_der(ta.get());
            for (const auto& name : extension.x509_name) {
                result = subject == name;
                if (result) {
                    break;
                }
            }
            if (result) {
                break;
            }
        }
    }
    return result;
}

} // namespace

namespace tls::trusted_ca_keys {

// ----------------------------------------------------------------------------
// TrustedCaKeys

bool extract_TrustedAuthority(trusted_ca_keys_t& result, const std::uint8_t*& ptr, std::int32_t& remaining) {
    bool bResult{false};
    if ((remaining > 0) && (ptr != nullptr)) {
        const auto identifier = static_cast<IdentifierType>(*ptr);
        update_position(ptr, remaining, 1);

        switch (identifier) {
        case IdentifierType::pre_agreed:
            result.pre_agreed = true;
            bResult = true;
            break;
        case IdentifierType::key_sha1_hash: {
            digest_t digest;
            bResult = struct_copy(digest, ptr, remaining, "trusted_ca_keys extension: key_sha1_hash decode error");
            if (bResult) {
                result.key_sha1_hash.emplace_back(digest);
            }
            break;
        }
        case IdentifierType::x509_name: {
            if (remaining >= 2) {
                const auto name_len = uint16(ptr);
                update_position(ptr, remaining, 2);
                if (remaining >= name_len) {
                    DER name(ptr, name_len);
                    update_position(ptr, remaining, name_len);
                    result.x509_name.emplace_back(std::move(name));
                    bResult = true;
                }
            }
            break;
        }
        case IdentifierType::cert_sha1_hash: {
            digest_t digest;
            bResult = struct_copy(digest, ptr, remaining, "trusted_ca_keys extension: cert_sha1_hash decode error");
            if (bResult) {
                result.cert_sha1_hash.emplace_back(digest);
            }
            break;
        }
        default:
            log_warning("trusted_ca_keys extension: IdentifierType decode error: " +
                        std::to_string(static_cast<int>(identifier)));
            break;
        }
    }
    return bResult;
}

std::size_t TrustedAuthority_size(const trusted_ca_keys_t& data) {
    // list length in bytes (2 bytes) + 1 if data.pre_agreed is true
    std::size_t size = (data.pre_agreed) ? 3 : 2;

    // IdentifierType (1 byte) + SHA1 digest size
    constexpr std::size_t hash_size = sizeof(digest_t) + 1;
    size += (data.key_sha1_hash.size() + data.cert_sha1_hash.size()) * hash_size;

    if (!data.x509_name.empty()) {
        for (const auto& i : data.x509_name) {
            // IdentifierType (1 byte) + 2 bytes length + DER X509 name
            size += i.size() + 3;
        }
    }
    return size;
}

bool certificate_digest(digest_t& digest, const x509_st* cert) {
    assert(cert != nullptr);
    return openssl::certificate_sha_1(digest, cert);
}

bool public_key_digest(digest_t& digest, const x509_st* cert) {
    assert(cert != nullptr);
    return openssl::certificate_subject_public_key_sha_1(digest, cert);
}

/*
 * Presentation Language
 * https://datatracker.ietf.org/doc/html/rfc5246
 *
 * extension_data (see https://datatracker.ietf.org/doc/html/rfc6066)
 * struct {
 *        TrustedAuthority trusted_authorities_list<0..2^16-1>;
 *    } TrustedAuthorities;
 *
 *    struct {
 *        IdentifierType identifier_type;
 *        select (identifier_type) {
 *            case pre_agreed: struct {};
 *            case key_sha1_hash: SHA1Hash;
 *            case x509_name: DistinguishedName;
 *            case cert_sha1_hash: SHA1Hash;
 *        } identifier;
 *    } TrustedAuthority;
 *
 *    enum {
 *        pre_agreed(0), key_sha1_hash(1), x509_name(2),
 *        cert_sha1_hash(3), (255)
 *    } IdentifierType;
 *
 *    opaque DistinguishedName<1..2^16-1>;
 *
 * (note the extension is not DER encoded, only DistinguishedName is)
 *
 * === captured traces ===
 *
 * PEV1
 * 0069 trusted_authorities_list length
 *   03 identifier_type cert_sha1_hash d8367e861f5807f8141fea572d676dbf58bb5f7c SHA1Hash
 *   03 identifier_type cert_sha1_hash b491ddd08fafe72d9f6f9bafc68eb04da84cc09a SHA1Hash
 *   03 identifier_type cert_sha1_hash 30aaaab25b1cc8a09a7b32652c33cc5a973c13f3 SHA1Hash
 *   03 identifier_type cert_sha1_hash 700bf78ad58e0819dac6fcaead5ed20f7bb0554f SHA1Hash
 *   03 identifier_type cert_sha1_hash 8c821f41604ed4c3431cf6d19f2ae107cf1f50e1 SHA1Hash
 *
 * PEV2
 * 002a trusted_authorities_list length
 *   03 identifier_type cert_sha1_hash d8367e861f5807f8141fea572d676dbf58bb5f7c SHA1Hash
 *   03 identifier_type cert_sha1_hash 8c821f41604ed4c3431cf6d19f2ae107cf1f50e1 SHA1Hash
 *
 * PEV3 (invalid missing the size of trusted_authorities_list)
 *   01 identifier_type key_sha1_hash 4cd7290bf592d2c1ba90f56e08946d4c8e99dc38 SHA1Hash
 *   01 identifier_type key_sha1_hash 00fae3900795c888a4d4d7bd9fdffa60418ac19f SHA1Hash
 */

trusted_authority convert(const trusted_ca_keys_t& keys) {
    const auto size = TrustedAuthority_size(keys);
    trusted_authority result{};

    // empty trusted_ca_keys_t has a size of 2
    if ((size > 2) && (size <= std::numeric_limits<std::uint16_t>::max())) {
        result = trusted_authority(size);
        auto* ptr = result.get();
        auto remaining = static_cast<std::int32_t>(size);
        uint16(ptr, size - 2);
        update_position(ptr, remaining, 2);
        if (keys.pre_agreed) {
            *ptr = static_cast<std::uint8_t>(IdentifierType::pre_agreed);
            update_position(ptr, remaining, 1);
        }
        for (const auto& i : keys.cert_sha1_hash) {
            *ptr = static_cast<std::uint8_t>(IdentifierType::cert_sha1_hash);
            update_position(ptr, remaining, 1);
            if (!struct_copy(ptr, i, remaining, "trusted_ca_keys extension: cert_sha1_hash encode error")) {
                break;
            }
        }
        for (const auto& i : keys.key_sha1_hash) {
            *ptr = static_cast<std::uint8_t>(IdentifierType::key_sha1_hash);
            update_position(ptr, remaining, 1);
            if (!struct_copy(ptr, i, remaining, "trusted_ca_keys extension: key_sha1_hash encode error")) {
                break;
            }
        }
        for (const auto& i : keys.x509_name) {
            *ptr = static_cast<std::uint8_t>(IdentifierType::x509_name);
            update_position(ptr, remaining, 1);
            uint16(ptr, i.size());
            update_position(ptr, remaining, 2);
            if (!der_copy(ptr, i, remaining, "trusted_ca_keys extension: x509_name encode error")) {
                break;
            }
        }
    }

    return result;
}

trusted_ca_keys_t convert(const std::uint8_t* extension_data, const std::size_t len) {
    trusted_ca_keys_t result{};
    bool bResult{false};

    if ((extension_data != nullptr) && (len <= std::numeric_limits<std::uint16_t>::max()) && (len >= 2)) {
        std::int32_t remaining = uint16(extension_data);
        extension_data += 2;
        if (remaining != (len - 2)) {
            log_warning("trusted_ca_keys extension: TrustedAuthorities decode error");
        } else {
            bResult = true;
            while (bResult && remaining > 0) {
                bResult = extract_TrustedAuthority(result, extension_data, remaining);
            }
        }
    }

    // do not return partially parsed extension
    return (bResult) ? std::move(result) : std::move(trusted_ca_keys_t());
}

bool match(const trusted_ca_keys_t& extension, const chain_t& chain) {
    bool result = match_cert_hash(extension, chain);
    result = result || match_key_hash(extension, chain);
    result = result || match_name(extension, chain);
    return result;
}

const chain_t* select(const trusted_ca_keys_t& extension, const chain_list& chains) {
    const chain_t* result{nullptr};
    for (const auto& chain : chains) {
        if (match(extension, chain)) {
            result = &chain;
            break;
        }
    }
    return result;
}

int ServerTrustedCaKeys::s_index{-1};

ServerTrustedCaKeys::ServerTrustedCaKeys() {
    if (s_index == -1) {
        s_index = CRYPTO_get_ex_new_index(CRYPTO_EX_INDEX_SSL, 0, nullptr, nullptr, nullptr, nullptr);
    }
}

bool ServerTrustedCaKeys::init_ssl(SslContext* ctx) {
    bool bRes{true};
    // TLS 1.2 and below only - use certificate_authorities in TLS 1.3
    constexpr int context_tck =
        SSL_EXT_TLS_ONLY | SSL_EXT_TLS1_2_AND_BELOW_ONLY | SSL_EXT_IGNORE_ON_RESUMPTION | SSL_EXT_CLIENT_HELLO;
    if (SSL_CTX_add_custom_ext(ctx, TLSEXT_TYPE_trusted_ca_keys, context_tck, nullptr, nullptr, nullptr,
                               &ServerTrustedCaKeys::trusted_ca_keys_cb, nullptr) != 1) {
        log_error("SSL_CTX_add_custom_ext");
        bRes = false;
    }

    // used to change the server certificate depending on trusted_ca_keys
    SSL_CTX_set_cert_cb(ctx, &ServerTrustedCaKeys::handle_certificate_cb, this);
    return bRes;
}

void ServerTrustedCaKeys::update(chain_list&& new_chains) {
    std::lock_guard lock(m_mux);
    m_chains = std::move(new_chains);
}

const chain_t* ServerTrustedCaKeys::select(const trusted_ca_keys_t& extension) {
    return trusted_ca_keys::select(extension, m_chains);
}

const chain_t* ServerTrustedCaKeys::select_default() {
    return (m_chains.empty()) ? nullptr : m_chains.data();
}

int ServerTrustedCaKeys::trusted_ca_keys_cb(SSL* ctx, unsigned int ext_type, unsigned int context,
                                            const unsigned char* data, std::size_t datalen, Certificate* cert,
                                            std::size_t chainidx, int* alert, void* object) {
    /*
     * return values:
     * - fatal, abort handshake and sent TLS Alert: result = 0 or negative and *alert = alert value
     * - success: result = 1
     */
    auto* keys_p = get_data(ctx);
    if (keys_p != nullptr) {
        keys_p->flags.trusted_ca_keys_received();
        keys_p->tck = convert(data, datalen);
    }
    return 1;
}

int ServerTrustedCaKeys::handle_certificate_cb(SSL* ssl, void* arg) {
    /*
     * return values:
     * - fatal, abort handshake and sent TLS Alert: result = 0 or negative
     * - success: result = 1
     */

    int result{1};

    auto* tck_p = reinterpret_cast<ServerTrustedCaKeys*>(arg);
    auto* keys_p = get_data(ssl);

    /*
     * From OpenSSL man page
     * An application will typically call SSL_use_certificate() and SSL_use_PrivateKey()
     * to set the end entity certificate and private key. It can add intermediate and
     * optionally the root CA certificates using SSL_add1_chain_cert().
     * It might also call SSL_certs_clear().
     */

    if ((tck_p != nullptr) && (keys_p != nullptr) && (keys_p->flags.has_trusted_ca_keys())) {
        // prevent update() from changing pointers
        std::lock_guard lock(tck_p->m_mux);

        const auto* selected = tck_p->select(keys_p->tck);
        if (selected != nullptr) {
            if (!use_certificate_and_key(ssl, *selected)) {
                // setting failed - try and use the default
                selected = tck_p->select_default();
                if (selected != nullptr) {
                    if (!use_certificate_and_key(ssl, *selected)) {
                        // there has been a problem setting the server
                        // certificate, key and chain
                        result = 0;
                        log_warning("terminating TLS handshake: trusted_ca_keys");
                    }
                }
            }
        }
    }
    return result;
}

void ServerTrustedCaKeys::set_data(SSL* ctx, server_trusted_ca_keys_t* ptr) {
    assert(ctx != nullptr);
    SSL_set_ex_data(ctx, s_index, ptr);
}

server_trusted_ca_keys_t* ServerTrustedCaKeys::get_data(SSL* ctx) {
    assert(ctx != nullptr);
    return reinterpret_cast<server_trusted_ca_keys_t*>(SSL_get_ex_data(ctx, s_index));
}

int ClientTrustedCaKeys::trusted_ca_keys_add(SSL* ctx, unsigned int ext_type, unsigned int context,
                                             const unsigned char** out, std::size_t* outlen, X509* cert,
                                             std::size_t chainidx, int* alert, void* object) {
    int result{0};
    if ((context == SSL_EXT_CLIENT_HELLO) && (object != nullptr)) {
        auto* config = reinterpret_cast<trusted_ca_keys_t*>(object);
        auto der = convert(*config);
        const auto len = der.size();
        auto* ptr = der.release();
        if (ptr != nullptr) {
            *out = ptr;
            *outlen = len;
            result = 1;
        }
    }
    return result;
}

void ClientTrustedCaKeys::trusted_ca_keys_free(SSL* ctx, unsigned int ext_type, unsigned int context,
                                               const unsigned char* out, void* object) {
    openssl::DER::free(const_cast<unsigned char*>(out));
}

} // namespace tls::trusted_ca_keys
