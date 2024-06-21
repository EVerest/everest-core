// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#include "helpers.hpp"
#include "openssl_util.hpp"
#include <cstdint>
#include <extensions/status_request.hpp>

#include <cassert>

#include <limits>
#include <openssl/ocsp.h>
#include <openssl/ssl.h>
#include <openssl/tls1.h>
#include <string>

#ifdef TLSEXT_STATUSTYPE_ocsp_multi
#define OPENSSL_PATCHED
#endif

using openssl::log_debug;
using openssl::log_error;

namespace {

/**
 * \brief load OCSP response from file
 * \param[in] filename is the file to read
 * \return a pointer to the OCSP reponse which will need to be freed
 *         using OCSP_RESPONSE_free()
 */
OCSP_RESPONSE* load_ocsp(const char* filename) {
    // update the cache
    OCSP_RESPONSE* resp{nullptr};

    if (filename != nullptr) {

        BIO* bio_file = BIO_new_file(filename, "r");
        if (bio_file == nullptr) {
            log_error(std::string("BIO_new_file: ") + filename);
        } else {
            resp = d2i_OCSP_RESPONSE_bio(bio_file, nullptr);
            BIO_free(bio_file);
        }

        if (resp == nullptr) {
            log_error("d2i_OCSP_RESPONSE_bio");
        }
    }

    return resp;
}

} // namespace

namespace tls {

using OCSP_RESPONSE_ptr = std::shared_ptr<OCSP_RESPONSE>;

struct ocsp_cache_ctx {
    std::map<OcspCache::digest_t, OCSP_RESPONSE_ptr> cache;
};

// ----------------------------------------------------------------------------
// OcspCache
OcspCache::OcspCache() : m_context(std::make_unique<ocsp_cache_ctx>()) {
}

OcspCache::~OcspCache() = default;

bool OcspCache::load(const ocsp_entry_list_t& filenames) {
    assert(m_context != nullptr);

    bool bResult{true};

    if (filenames.empty()) {
        // clear the cache
        std::lock_guard lock(mux);
        m_context->cache.clear();
    } else {
        std::map<digest_t, OCSP_RESPONSE_ptr> updates;
        for (const auto& entry : filenames) {
            const auto& digest = std::get<digest_t>(entry);
            const auto* filename = std::get<const char*>(entry);

            OCSP_RESPONSE* resp{nullptr};

            if (filename != nullptr) {
                resp = load_ocsp(filename);
                if (resp == nullptr) {
                    bResult = false;
                }
            }

            if (resp != nullptr) {
                updates[digest] = std::shared_ptr<OCSP_RESPONSE>(resp, &::OCSP_RESPONSE_free);
            }
        }

        {
            std::lock_guard lock(mux);
            m_context->cache.swap(updates);
        }
    }

    return bResult;
}

OcspCache::OcspResponse_t OcspCache::lookup(const digest_t& digest) {
    assert(m_context != nullptr);

    OcspResponse_t resp;
    std::lock_guard lock(mux);
    if (const auto itt = m_context->cache.find(digest); itt != m_context->cache.end()) {
        resp = itt->second;
    } else {
        log_error("OcspCache::lookup: not in cache: " + to_string(digest));
    }

    return resp;
}

bool OcspCache::digest(digest_t& digest, const x509_st* cert) {
    assert(cert != nullptr);
    return openssl::certificate_sha_1(digest, cert);
}

namespace status_request {

// ----------------------------------------------------------------------------
// ServerStatusRequestV2

int ServerStatusRequestV2::s_index{-1};

ServerStatusRequestV2::ServerStatusRequestV2(OcspCache& cache) : m_cache(cache) {
    if (s_index == -1) {
        s_index = CRYPTO_get_ex_new_index(CRYPTO_EX_INDEX_SSL, 0, nullptr, nullptr, nullptr, nullptr);
    }
}

bool ServerStatusRequestV2::init_ssl(SSL_CTX* ctx) {
    bool bRes{true};
    SSL_CTX_set_client_hello_cb(ctx, &client_hello_cb, nullptr);

    if (SSL_CTX_set_tlsext_status_cb(ctx, &status_request_cb) != 1) {
        log_error("SSL_CTX_set_tlsext_status_cb");
        bRes = false;
    }

    if (SSL_CTX_set_tlsext_status_arg(ctx, this) != 1) {
        log_error("SSL_CTX_set_tlsext_status_arg");
        bRes = false;
    }

    // TLS 1.2 and below only - managed differently in TLS 1.3
    constexpr int context_srv2 = SSL_EXT_TLS_ONLY | SSL_EXT_TLS1_2_AND_BELOW_ONLY | SSL_EXT_IGNORE_ON_RESUMPTION |
                                 SSL_EXT_CLIENT_HELLO | SSL_EXT_TLS1_2_SERVER_HELLO;
    if (SSL_CTX_add_custom_ext(ctx, TLSEXT_TYPE_status_request_v2, context_srv2, &status_request_v2_add,
                               &status_request_v2_free, this, &status_request_v2_cb, nullptr) != 1) {
        log_error("SSL_CTX_add_custom_ext status_request_v2");
        bRes = false;
    }
    return bRes;
}

bool ServerStatusRequestV2::set_ocsp_response(const digest_t& digest, SSL* ctx) {
    bool bResult{false};
    auto response = m_cache.lookup(digest);
    if (response) {
        unsigned char* der{nullptr};
        auto len = i2d_OCSP_RESPONSE(response.get(), &der);
        if (len > 0) {
            bResult = SSL_set_tlsext_status_ocsp_resp(ctx, der, len) == 1;
            if (bResult) {
                SSL_set_tlsext_status_type(ctx, TLSEXT_STATUSTYPE_ocsp);
            } else {
                log_error("SSL_set_tlsext_status_ocsp_resp");
                OPENSSL_free(der);
            }
        }
    }
    return bResult;
}

int ServerStatusRequestV2::status_request_cb(SSL* ctx, void* object) {
    // returns:
    // - SSL_TLSEXT_ERR_OK response to client via SSL_set_tlsext_status_ocsp_resp
    // - SSL_TLSEXT_ERR_NOACK no response to client
    // - SSL_TLSEXT_ERR_ALERT_FATAL abort connection
    bool bSet{false};
    int result = SSL_TLSEXT_ERR_NOACK;
    digest_t digest{};

    if (ctx != nullptr) {
        const auto* cert = SSL_get_certificate(ctx);
        bSet = OcspCache::digest(digest, cert);
    }

    bool tls_1_3{false};
    const auto* session = SSL_get0_session(ctx);
    if (session != nullptr) {
        tls_1_3 = SSL_SESSION_get_protocol_version(session) == TLS1_3_VERSION;
    }

    if (!tls_1_3) {
        auto* flags_p = get_data(ctx);
        if (flags_p != nullptr) {
            /*
             * if there is a status_request_v2 then don't provide a status_request response
             * unless this is TLS 1.3 where status_request_v2 is deprecated (not to be used)
             */
            if (flags_p->has_status_request_v2()) {
                bSet = false;
                result = SSL_TLSEXT_ERR_NOACK;
            }
        }
    }

    auto* ptr = reinterpret_cast<ServerStatusRequestV2*>(object);
    if (bSet && (ptr != nullptr)) {
        if (ptr->set_ocsp_response(digest, ctx)) {
            result = SSL_TLSEXT_ERR_OK;
        }
    }
    return result;
}

bool ServerStatusRequestV2::set_ocsp_v2_response(const digest_list_t& digests, SSL* ctx) {
    /*
     * There is no response in the extension. An additional handshake message is
     * sent after the certificate (certificate status) that includes the
     * actual response.
     */

    /*
     * s->ext.status_expected, set to 1 to include the certificate status message
     * s->ext.status_type, ocsp(1), ocsp_multi(2)
     * s->ext.ocsp.resp, set by SSL_set_tlsext_status_ocsp_resp
     * s->ext.ocsp.resp_len, set by SSL_set_tlsext_status_ocsp_resp
     */

    bool bResult{false};

#ifdef OPENSSL_PATCHED
    if (ctx != nullptr) {
        std::vector<std::pair<std::size_t, std::uint8_t*>> response_list;
        std::size_t total_size{0};

        for (const auto& digest : digests) {
            auto response = m_cache.lookup(digest);
            if (response) {
                unsigned char* der{nullptr};
                auto len = i2d_OCSP_RESPONSE(response.get(), &der);
                if (len > 0) {
                    const std::size_t adjusted_len = len + 3;
                    total_size += adjusted_len;
                    // prefix the length of the DER encoded OCSP response
                    auto* der_len = static_cast<std::uint8_t*>(OPENSSL_malloc(adjusted_len));
                    if (der_len != nullptr) {
                        uint24(der_len, len);
                        std::memcpy(&der_len[3], der, len);
                        response_list.emplace_back(adjusted_len, der_len);
                    }
                    OPENSSL_free(der);
                }
            }
        }

        std::size_t resp_len{0};
        std::uint8_t* resp{nullptr};

        // don't include the extension when there are no OCSP responses
        if (total_size > 0) {
            resp_len = total_size;
            resp = static_cast<std::uint8_t*>(OPENSSL_malloc(resp_len));
            if (resp == nullptr) {
                resp_len = 0;
            } else {
                std::size_t idx{0};

                for (auto& entry : response_list) {
                    auto len = entry.first;
                    auto* der = entry.second;
                    std::memcpy(&resp[idx], der, len);
                    OPENSSL_free(der);
                    idx += len;
                }
            }

            // SSL_set_tlsext_status_ocsp_resp sets the correct overall length
            bResult = SSL_set_tlsext_status_ocsp_resp(ctx, resp, resp_len) == 1;
            if (bResult) {
                SSL_set_tlsext_status_type(ctx, TLSEXT_STATUSTYPE_ocsp_multi);
                SSL_set_tlsext_status_expected(ctx, 1);
            } else {
                log_error((std::string("SSL_set_tlsext_status_ocsp_resp")));
            }
        }
    }
#endif // OPENSSL_PATCHED

    return bResult;
}

int ServerStatusRequestV2::status_request_v2_add(SSL* ctx, unsigned int ext_type, unsigned int context,
                                                 const unsigned char** out, std::size_t* outlen, X509* cert,
                                                 std::size_t chainidx, int* alert, void* object) {
    /*
     * return values:
     * - fatal, abort handshake and sent TLS Alert: result = -1 and *alert = alert value
     * - do not include extension: result = 0
     * - include extension: result = 1
     */

    *out = nullptr;
    *outlen = 0;

    int result = 0;

#ifdef OPENSSL_PATCHED
    digest_t digest{};
    digest_list_t digest_chain;

    if (ctx != nullptr) {
        const auto* cert = SSL_get_certificate(ctx);
        if (OcspCache::digest(digest, cert)) {
            digest_chain.push_back(digest);
        }

        STACK_OF(X509) * chain{nullptr};

        if (SSL_get0_chain_certs(ctx, &chain) != 1) {
            log_error((std::string("SSL_get0_chain_certs")));
        } else {
            const auto num = sk_X509_num(chain);
            for (std::size_t i = 0; i < num; i++) {
                cert = sk_X509_value(chain, i);
                if (OcspCache::digest(digest, cert)) {
                    digest_chain.push_back(digest);
                }
            }
        }
    }

    auto* ptr = reinterpret_cast<ServerStatusRequestV2*>(object);
    if (!digest_chain.empty() && (ptr != nullptr)) {
        if (ptr->set_ocsp_v2_response(digest_chain, ctx)) {
            result = 1;
        }
    }
#endif // OPENSSL_PATCHED

    return result;
}

void ServerStatusRequestV2::status_request_v2_free(SSL* ctx, unsigned int ext_type, unsigned int context,
                                                   const unsigned char* out, void* object) {
    OPENSSL_free(const_cast<unsigned char*>(out));
}

int ServerStatusRequestV2::status_request_v2_cb(Ssl* ctx, unsigned int ext_type, unsigned int context,
                                                const unsigned char* data, std::size_t datalen, X509* cert,
                                                std::size_t chainidx, int* alert, void* object) {
    /*
     * return values:
     * - fatal, abort handshake and sent TLS Alert: result = 0 or negative and *alert = alert value
     * - success: result = 1
     */

    // TODO(james-ctc): check requested type std, or multi
    return 1;
}

int ServerStatusRequestV2::client_hello_cb(SSL* ctx, int* alert, void* object) {
    /*
     * return values:
     * - fatal, abort handshake and sent TLS Alert: result = 0 or negative and *alert = alert value
     * - success: result = 1
     */

    auto* flags_p = get_data(ctx);
    if (flags_p != nullptr) {
        int* extensions{nullptr};
        std::size_t length{0};
        if (SSL_client_hello_get1_extensions_present(ctx, &extensions, &length) == 1) {
            std::string logstr("Extensions:");
            for (std::size_t i = 0; i < length; i++) {
                logstr += " " + std::to_string(extensions[i]);
                if (extensions[i] == TLSEXT_TYPE_status_request) {
                    flags_p->status_request_received();
                } else if (extensions[i] == TLSEXT_TYPE_status_request_v2) {
                    flags_p->status_request_v2_received();
                }
            }
            log_debug(logstr);
            OPENSSL_free(extensions);
        }
    }
    return 1;
}

void ServerStatusRequestV2::set_data(SSL* ctx, StatusFlags* ptr) {
    assert(ctx != nullptr);
    SSL_set_ex_data(ctx, s_index, ptr);
}

StatusFlags* ServerStatusRequestV2::get_data(SSL* ctx) {
    assert(ctx != nullptr);
    return reinterpret_cast<StatusFlags*>(SSL_get_ex_data(ctx, s_index));
}

bool ClientStatusRequestV2::print_ocsp_response(FILE* stream, const unsigned char*& response, std::size_t length) {
    OCSP_RESPONSE* ocsp{nullptr};

    if (response != nullptr) {
        ocsp = d2i_OCSP_RESPONSE(nullptr, &response, static_cast<std::int64_t>(length));
        if (ocsp == nullptr) {
            log_error("d2i_OCSP_RESPONSE: decode error");
        } else {
            BIO* bio_out = BIO_new_fp(stream, BIO_NOCLOSE);
            OCSP_RESPONSE_print(bio_out, ocsp, 0);
            OCSP_RESPONSE_free(ocsp);
            BIO_free(bio_out);
        }
    }

    return ocsp != nullptr;
}

int ClientStatusRequestV2::status_request_cb(SSL* ctx) {
    /*
     * This callback is called when status_request or status_request_v2 extensions
     * were present in the Client Hello. It doesn't mean that the extension is in
     * the Server Hello SSL_get_tlsext_status_ocsp_resp() returns -1 in that case
     */

    /*
     * The callback when used on the client side should return
     * a negative value on error,
     * 0 if the response is not acceptable (in which case the handshake will fail), or
     * a positive value if it is acceptable.
     */

    int result{1};

    const unsigned char* response{nullptr};
    const auto total_length = SSL_get_tlsext_status_ocsp_resp(ctx, &response);
    // length == -1 on no response and response will be nullptr

    if ((response != nullptr) && (total_length > 0) && (total_length <= std::numeric_limits<std::int32_t>::max())) {
        // there is a response

        const auto* tmp_p = &response[0];

        if (response[0] == 0x30) {
            // not a multi response
            auto* resp = d2i_OCSP_RESPONSE(nullptr, &tmp_p, total_length);
            if (resp == nullptr) {
                log_error("d2i_OCSP_RESPONSE (single)");
                result = 0;
            }
            OCSP_RESPONSE_free(resp);
        } else {
            // multiple responses
            auto remaining = static_cast<std::int32_t>(total_length);
            const unsigned char* ptr{response};

            while (remaining >= 3) {
                const auto len = uint24(ptr);
                update_position(ptr, remaining, 3);
                // d2i_OCSP_RESPONSE updates tmp_p
                tmp_p = ptr;
                auto* resp = d2i_OCSP_RESPONSE(nullptr, &tmp_p, len);
                update_position(ptr, remaining, len);
                if ((resp == nullptr) || (ptr != tmp_p)) {
                    log_error("d2i_OCSP_RESPONSE (multi)");
                    result = 0;
                    remaining = -1;
                }
                OCSP_RESPONSE_free(resp);
            }

            if (remaining != 0) {
                log_error("OCSP_RESPONSE decode error (multi)");
                result = 0;
            }
        }
    }

    return result;
}

int ClientStatusRequestV2::status_request_v2_multi_cb(SSL* ctx, void* object) {
    /*
     * This callback is called when status_request or status_request_v2 extensions
     * were present in the Client Hello. It doesn't mean that the extension is in
     * the Server Hello SSL_get_tlsext_status_ocsp_resp() returns -1 in that case
     */

    /*
     * The callback when used on the client side should return
     * a negative value on error,
     * 0 if the response is not acceptable (in which case the handshake will fail), or
     * a positive value if it is acceptable.
     */

    auto* client_ptr = reinterpret_cast<ClientStatusRequestV2*>(object);

    int result{1};
    if (client_ptr != nullptr) {
        result = client_ptr->status_request_cb(ctx);
    } else {
        log_error("ClientStatusRequestV2::status_request_v2_multi_cb missing Client *");
    }
    return result;
}

int ClientStatusRequestV2::status_request_v2_add(SSL* ctx, unsigned int ext_type, unsigned int context,
                                                 const unsigned char** out, std::size_t* outlen, X509* cert,
                                                 std::size_t chainidx, int* alert, void* object) {
    int result{0};
    if (context == SSL_EXT_CLIENT_HELLO) {
        /*
         * struct {
         *   CertificateStatusType status_type;
         *   uint16 request_length; // Length of request field in bytes
         *   select (status_type) {
         *     case ocsp: OCSPStatusRequest;
         *     case ocsp_multi: OCSPStatusRequest;
         *   } request;
         * } CertificateStatusRequestItemV2;
         *
         * enum { ocsp(1), ocsp_multi(2), (255) } CertificateStatusType;
         *
         * struct {
         *   ResponderID responder_id_list<0..2^16-1>;
         *   Extensions request_extensions;
         * } OCSPStatusRequest;
         *
         * opaque ResponderID<1..2^16-1>;
         * opaque Extensions<0..2^16-1>;
         *
         * struct {
         *   CertificateStatusRequestItemV2
         *   certificate_status_req_list<1..2^16-1>;
         * } CertificateStatusRequestListV2;
         *
         * Minimal request:
         * 0x0007   certificate_status_req_list length
         *   0x02     CertificateStatusType - OCSP multi
         *   0x0004   request_length (uint 16)
         *     0x0000   responder_id_list length
         *     0x0000   request_extensions length
         */

        // don't use constexpr
        static const std::uint8_t asn1[] = {0x00, 0x07, 0x02, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00};
        *out = &asn1[0];
        *outlen = sizeof(asn1);
#ifdef OPENSSL_PATCHED
        /*
         * ensure client callback is called - SSL_set_tlsext_status_type() needs to have a value
         * TLSEXT_STATUSTYPE_ocsp_multi for status_request_v2, or
         * TLSEXT_STATUSTYPE_ocsp for status_request and status_request_v2
         */

        if (SSL_get_tlsext_status_type(ctx) != TLSEXT_STATUSTYPE_ocsp) {
            SSL_set_tlsext_status_type(ctx, TLSEXT_STATUSTYPE_ocsp_multi);
        }
#endif // OPENSSL_PATCHED
        result = 1;
    }
    return result;
}

int ClientStatusRequestV2::status_request_v2_cb(SSL* ctx, unsigned int ext_type, unsigned int context,
                                                const unsigned char* data, std::size_t datalen, X509* cert,
                                                std::size_t chainidx, int* alert, void* object) {
#ifdef OPENSSL_PATCHED
    SSL_set_tlsext_status_type(ctx, TLSEXT_STATUSTYPE_ocsp_multi);
    SSL_set_tlsext_status_expected(ctx, 1);
#endif // OPENSSL_PATCHED

    return 1;
}

} // namespace status_request
} // namespace tls
