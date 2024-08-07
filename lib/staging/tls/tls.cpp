// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include "tls.hpp"
#include "openssl_util.hpp"

#include <array>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <fcntl.h>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <openssl/types.h>
#include <poll.h>
#include <sstream>
#include <string>
#include <sys/socket.h>

#include <openssl/asn1.h>
#include <openssl/bio.h>
#include <openssl/crypto.h>
#include <openssl/err.h>
#include <openssl/ocsp.h>
#include <openssl/ssl.h>
#include <openssl/tls1.h>

#ifdef TLSEXT_STATUSTYPE_ocsp_multi
#define OPENSSL_PATCHED
#endif

namespace std {
template <> class default_delete<SSL> {
public:
    void operator()(SSL* ptr) const {
        ::SSL_free(ptr);
    }
};
template <> class default_delete<SSL_CTX> {
public:
    void operator()(SSL_CTX* ptr) const {
        ::SSL_CTX_free(ptr);
    }
};
} // namespace std

using ::openssl::log_error;
using ::openssl::log_warning;

namespace {

/**
 * \brief convert a big endian 3 byte (24 bit) unsigned value to uint32
 * \param[in] ptr the pointer to the most significant byte
 * \return the interpreted value
 */
constexpr std::uint32_t uint24(const std::uint8_t* ptr) {
    return (static_cast<std::uint32_t>(ptr[0]) << 16U) | (static_cast<std::uint32_t>(ptr[1]) << 8U) |
           static_cast<std::uint32_t>(ptr[2]);
}

/**
 * \brief convert a uint32 to big endian 3 byte (24 bit) value
 * \param[in] ptr the pointer to the most significant byte
 * \param[in] value the 24 bit value
 */
constexpr void uint24(std::uint8_t* ptr, std::uint32_t value) {
    ptr[0] = (value >> 16U) & 0xffU;
    ptr[1] = (value >> 8U) & 0xffU;
    ptr[2] = value & 0xffU;
}

// see https://datatracker.ietf.org/doc/html/rfc6961
constexpr int TLSEXT_TYPE_status_request_v2 = 17;

std::string to_string(const openssl::sha_256_digest_t& digest) {
    std::stringstream string_stream;
    string_stream << std::hex;
    for (const auto& c : digest) {
        string_stream << std::setw(2) << std::setfill('0') << static_cast<int>(c);
    }
    return string_stream.str();
}

constexpr std::uint32_t c_shutdown_timeout_ms = 5000; // 5 seconds

enum class ssl_error_t : std::uint8_t {
    error,
    error_ssl,
    error_syscall,
    none,
    want_accept,
    want_async,
    want_async_job,
    want_connect,
    want_hello_cb,
    want_read,
    want_write,
    want_x509_lookup,
    zero_return,
    timeout, // not an OpenSSL result
};

constexpr ssl_error_t convert(const int err) {
    ssl_error_t res{ssl_error_t::error};
    switch (err) {
    case SSL_ERROR_NONE:
        res = ssl_error_t::none;
        break;
    case SSL_ERROR_ZERO_RETURN:
        res = ssl_error_t::zero_return;
        break;
    case SSL_ERROR_WANT_READ:
        res = ssl_error_t::want_read;
        break;
    case SSL_ERROR_WANT_WRITE:
        res = ssl_error_t::want_write;
        break;
    case SSL_ERROR_WANT_CONNECT:
        res = ssl_error_t::want_connect;
        break;
    case SSL_ERROR_WANT_ACCEPT:
        res = ssl_error_t::want_accept;
        break;
    case SSL_ERROR_WANT_X509_LOOKUP:
        res = ssl_error_t::want_x509_lookup;
        break;
    case SSL_ERROR_WANT_ASYNC:
        res = ssl_error_t::want_async;
        break;
    case SSL_ERROR_WANT_ASYNC_JOB:
        res = ssl_error_t::want_async_job;
        break;
    case SSL_ERROR_WANT_CLIENT_HELLO_CB:
        res = ssl_error_t::want_hello_cb;
        break;
    case SSL_ERROR_SYSCALL:
        res = ssl_error_t::error_syscall;
        break;
    case SSL_ERROR_SSL:
        res = ssl_error_t::error_ssl;
        break;
    default:
        log_error(std::string("Unexpected SSL_get_error: ") + std::to_string(static_cast<int>(res)));
        break;
    };
    return res;
}

enum class ssl_result_t : std::uint8_t {
    error,
    error_syscall,
    success,
    closed,
    timeout,
};

constexpr ssl_result_t convert(ssl_error_t err) {
    switch (err) {
    case ssl_error_t::none:
        return ssl_result_t::success;
    case ssl_error_t::timeout:
        return ssl_result_t::timeout;
    case ssl_error_t::error_syscall:
    case ssl_error_t::error_ssl:
        return ssl_result_t::error_syscall;
    case ssl_error_t::zero_return:
        return ssl_result_t::closed;
    case ssl_error_t::error:
    case ssl_error_t::want_accept:
    case ssl_error_t::want_async:
    case ssl_error_t::want_async_job:
    case ssl_error_t::want_connect:
    case ssl_error_t::want_hello_cb:
    case ssl_error_t::want_read:
    case ssl_error_t::want_write:
    case ssl_error_t::want_x509_lookup:
    default:
        return ssl_result_t::error;
    }
}

constexpr tls::Connection::result_t convert(ssl_result_t err) {
    switch (err) {
    case ssl_result_t::success:
        return tls::Connection::result_t::success;
    case ssl_result_t::timeout:
        return tls::Connection::result_t::timeout;
    case ssl_result_t::closed:
    case ssl_result_t::error:
    case ssl_result_t::error_syscall:
    default:
        return tls::Connection::result_t::error;
    }
}

int wait_for(int soc, bool forWrite, std::int32_t timeout_ms) {
    std::int16_t event = POLLIN;
    if (forWrite) {
        event = POLLOUT;
    }
    std::array<pollfd, 1> fds = {{{soc, event, 0}}};
    int poll_res{0};

    for (;;) {
        poll_res = poll(fds.data(), fds.size(), timeout_ms);
        if (poll_res == -1) {
            if (errno != EINTR) {
                log_error(std::string("wait_for poll: ") + std::to_string(errno));
                break;
            }
        }
        // timeout or event(s)
        break;
    }

    return poll_res;
}

[[nodiscard]] ssl_result_t ssl_read(SSL* ctx, std::byte* buf, std::size_t num, std::size_t& readbytes,
                                    std::int32_t timeout_ms);
[[nodiscard]] ssl_result_t ssl_write(SSL* ctx, const std::byte* buf, std::size_t num, std::size_t& writebytes,
                                     std::int32_t timeout_ms);
[[nodiscard]] ssl_result_t ssl_accept(SSL* ctx, std::int32_t timeout_ms);
[[nodiscard]] ssl_result_t ssl_connect(SSL* ctx, std::int32_t timeout_ms);
void ssl_shutdown(SSL* ctx, std::int32_t timeout_ms);

bool process_result(SSL* ctx, const std::string& operation, const int res, ssl_error_t& result,
                    std::int32_t timeout_ms) {
    bool bLoop = false;

    if (res <= 0) {
        const auto sslerr_raw = SSL_get_error(ctx, res);
        result = convert(sslerr_raw);
        switch (result) {
        case ssl_error_t::none:
        case ssl_error_t::zero_return:
            break;
        case ssl_error_t::want_accept:
        case ssl_error_t::want_connect:
        case ssl_error_t::want_read:
        case ssl_error_t::want_write:
            if (wait_for(SSL_get_fd(ctx), result == ssl_error_t::want_write, timeout_ms) > 0) {
                bLoop = true;
            }
            result = ssl_error_t::timeout;
            break;
        case ssl_error_t::error_syscall:
            if (errno != 0) {
                log_error(operation + "SSL_ERROR_SYSCALL " + std::to_string(errno));
            }
            break;
        case ssl_error_t::error:
        case ssl_error_t::error_ssl:
        case ssl_error_t::want_async:
        case ssl_error_t::want_async_job:
        case ssl_error_t::want_hello_cb:
        case ssl_error_t::want_x509_lookup:
        default:
            log_error(operation + std::to_string(res) + " " + std::to_string(sslerr_raw));
            break;
        }
    } else {
        result = ssl_error_t::none;
    }

    return bLoop;
}

ssl_result_t ssl_read(SSL* ctx, std::byte* buf, const std::size_t num, std::size_t& readbytes,
                      std::int32_t timeout_ms) {
    ssl_error_t result = ssl_error_t::error;
    bool bLoop = ctx != nullptr;
    while (bLoop) {
        const auto res = SSL_read_ex(ctx, buf, num, &readbytes);
        bLoop = process_result(ctx, "SSL_read: ", res, result, timeout_ms);
    }
    return convert(result);
};

ssl_result_t ssl_write(SSL* ctx, const std::byte* buf, const std::size_t num, std::size_t& writebytes,
                       std::int32_t timeout_ms) {
    ssl_error_t result = ssl_error_t::error;
    bool bLoop = ctx != nullptr;
    while (bLoop) {
        const auto res = SSL_write_ex(ctx, buf, num, &writebytes);
        bLoop = process_result(ctx, "SSL_write: ", res, result, timeout_ms);
    }
    return convert(result);
}

ssl_result_t ssl_accept(SSL* ctx, std::int32_t timeout_ms) {
    ssl_error_t result = ssl_error_t::error;
    bool bLoop = ctx != nullptr;
    while (bLoop) {
        const auto res = SSL_accept(ctx);
        // 0 is handshake not successful
        // < 0 is other error
        bLoop = process_result(ctx, "SSL_accept: ", res, result, timeout_ms);
    }
    return convert(result);
}

ssl_result_t ssl_connect(SSL* ctx, std::int32_t timeout_ms) {
    ssl_error_t result = ssl_error_t::error;
    bool bLoop = ctx != nullptr;

    while (bLoop) {
        const auto res = SSL_connect(ctx);
        // 0 is handshake not successful
        // < 0 is other error
        bLoop = process_result(ctx, "SSL_connect: ", res, result, timeout_ms);
    }
    return convert(result);
}

void ssl_shutdown(SSL* ctx, std::int32_t timeout_ms) {
    ssl_error_t result = ssl_error_t::error;
    bool bLoop = ctx != nullptr;
    while (bLoop) {
        const auto res = SSL_shutdown(ctx);
        bLoop = process_result(ctx, "SSL_shutdown: ", res, result, timeout_ms);
    }
}

bool configure_ssl_ctx(SSL_CTX* ctx, const char* ciphersuites, const char* cipher_list,
                       const char* certificate_chain_file, const char* private_key_file,
                       const char* private_key_password, bool required) {
    bool bRes{true};

    if (ctx == nullptr) {
        log_error("server_init::SSL_CTX_new");
        bRes = false;
    } else {
        if (SSL_CTX_set_min_proto_version(ctx, TLS1_2_VERSION) == 0) {
            log_error("SSL_CTX_set_min_proto_version");
            bRes = false;
        }
        if ((ciphersuites != nullptr) && (ciphersuites[0] == '\0')) {
            // no cipher suites configured - don't use TLS 1.3
            // nullptr means use the defaults
            if (SSL_CTX_set_max_proto_version(ctx, TLS1_2_VERSION) == 0) {
                log_error("SSL_CTX_set_max_proto_version");
                bRes = false;
            }
        }
        if (cipher_list != nullptr) {
            if (SSL_CTX_set_cipher_list(ctx, cipher_list) == 0) {
                log_error("SSL_CTX_set_cipher_list");
                bRes = false;
            }
        }
        if (ciphersuites != nullptr) {
            if (SSL_CTX_set_ciphersuites(ctx, ciphersuites) == 0) {
                log_error("SSL_CTX_set_ciphersuites");
                bRes = false;
            }
        }

        if (certificate_chain_file != nullptr) {
            if (SSL_CTX_use_certificate_chain_file(ctx, certificate_chain_file) != 1) {
                log_error("SSL_CTX_use_certificate_chain_file");
                bRes = false;
            }
        } else {
            bRes = !required;
        }

        if (private_key_file != nullptr) {
            // the password callback uses a non-const argument
            void* pass_ptr{nullptr};
            std::string pass_str;
            if (private_key_password != nullptr) {
                pass_str = private_key_password;
                pass_ptr = pass_str.data();
            }
            SSL_CTX_set_default_passwd_cb_userdata(ctx, pass_ptr);

            if (SSL_CTX_use_PrivateKey_file(ctx, private_key_file, SSL_FILETYPE_PEM) != 1) {
                log_error("SSL_CTX_use_PrivateKey_file");
                bRes = false;
            }
            if (SSL_CTX_check_private_key(ctx) != 1) {
                log_error("SSL_CTX_check_private_key");
                bRes = false;
            }
        } else {
            bRes = !required;
        }
    }

    return bRes;
}

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

constexpr char* dup(const char* value) {
    char* res = nullptr;
    if (value != nullptr) {
        res = strdup(value);
    }
    return res;
}

} // namespace

namespace tls {

ConfigItem::ConfigItem(const char* value) : m_ptr(dup(value)) {
}
ConfigItem& ConfigItem::operator=(const char* value) {
    m_ptr = dup(value);
    return *this;
}
ConfigItem::ConfigItem(const ConfigItem& obj) : m_ptr(dup(obj.m_ptr)) {
}
ConfigItem& ConfigItem::operator=(const ConfigItem& obj) {
    m_ptr = dup(obj.m_ptr);
    return *this;
}
ConfigItem::ConfigItem(ConfigItem&& obj) noexcept : m_ptr(obj.m_ptr) {
    obj.m_ptr = nullptr;
}
ConfigItem& ConfigItem::operator=(ConfigItem&& obj) noexcept {
    m_ptr = obj.m_ptr;
    obj.m_ptr = nullptr;
    return *this;
}
ConfigItem::~ConfigItem() {
    free(m_ptr);
    m_ptr = nullptr;
}

bool ConfigItem::operator==(const char* ptr) const {
    bool result{false};
    if (m_ptr == ptr) {
        // both nullptr, or both point to the same string
        result = true;
    } else if ((m_ptr != nullptr) && (ptr != nullptr)) {
        result = strcmp(m_ptr, ptr) == 0;
    }
    return result;
}

using SSL_ptr = std::unique_ptr<SSL>;
using SSL_CTX_ptr = std::unique_ptr<SSL_CTX>;
using OCSP_RESPONSE_ptr = std::shared_ptr<OCSP_RESPONSE>;

struct connection_ctx {
    SSL_ptr ctx;
    BIO* soc_bio{nullptr};
    int soc{0};
};

struct ocsp_cache_ctx {
    std::map<openssl::sha_256_digest_t, OCSP_RESPONSE_ptr> cache;
};

struct server_ctx {
    SSL_CTX_ptr ctx;
};

struct client_ctx {
    SSL_CTX_ptr ctx;
};

// ----------------------------------------------------------------------------
// OcspCache
OcspCache::OcspCache() : m_context(std::make_unique<ocsp_cache_ctx>()) {
}

OcspCache::~OcspCache() = default;

bool OcspCache::load(const std::vector<ocsp_entry_t>& filenames) {
    assert(m_context != nullptr);

    bool bResult{true};

    if (filenames.empty()) {
        // clear the cache
        std::lock_guard lock(mux);
        m_context->cache.clear();
    } else {
        std::map<openssl::sha_256_digest_t, OCSP_RESPONSE_ptr> updates;
        for (const auto& entry : filenames) {
            const auto& digest = std::get<openssl::sha_256_digest_t>(entry);
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

std::shared_ptr<OcspResponse> OcspCache::lookup(const openssl::sha_256_digest_t& digest) {
    assert(m_context != nullptr);

    std::shared_ptr<OcspResponse> resp;
    std::lock_guard lock(mux);
    if (const auto itt = m_context->cache.find(digest); itt != m_context->cache.end()) {
        resp = itt->second;
    } else {
        log_error("OcspCache::lookup: not in cache: " + to_string(digest));
    }

    return resp;
}

bool OcspCache::digest(openssl::sha_256_digest_t& digest, const x509_st* cert) {
    assert(cert != nullptr);

    bool bResult{false};
    const ASN1_BIT_STRING* signature{nullptr};
    const X509_ALGOR* alg{nullptr};
    X509_get0_signature(&signature, &alg, cert);
    if (signature != nullptr) {
        unsigned char* data{nullptr};
        const auto len = i2d_ASN1_BIT_STRING(signature, &data);
        if (len > 0) {
            bResult = openssl::sha_256(data, len, digest);
        }
        OPENSSL_free(data);
    }

    return bResult;
}

// ----------------------------------------------------------------------------
// CertificateStatusRequestV2

bool CertificateStatusRequestV2::set_ocsp_response(const openssl::sha_256_digest_t& digest, SSL* ctx) {
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
                log_error((std::string("SSL_set_tlsext_status_ocsp_resp")));
                OPENSSL_free(der);
            }
        }
    }
    return bResult;
}

int CertificateStatusRequestV2::status_request_cb(SSL* ctx, void* object) {
    // returns:
    // - SSL_TLSEXT_ERR_OK response to client via SSL_set_tlsext_status_ocsp_resp
    // - SSL_TLSEXT_ERR_NOACK no response to client
    // - SSL_TLSEXT_ERR_ALERT_FATAL abort connection
    bool bSet{false};
    bool tls_1_3{false};
    int result = SSL_TLSEXT_ERR_NOACK;
    openssl::sha_256_digest_t digest{};

    if (ctx != nullptr) {
        const auto* cert = SSL_get_certificate(ctx);
        bSet = OcspCache::digest(digest, cert);
    }

    const auto* session = SSL_get0_session(ctx);
    if (session != nullptr) {
        tls_1_3 = SSL_SESSION_get_protocol_version(session) == TLS1_3_VERSION;
    }

    if (!tls_1_3) {
        auto* connection = reinterpret_cast<ServerConnection*>(SSL_get_app_data(ctx));
        if (connection != nullptr) {
            /*
             * if there is a status_request_v2 then don't provide a status_request response
             * unless this is TLS 1.3 where status_request_v2 is deprecated (not to be used)
             */
            if (connection->has_status_request_v2()) {
                bSet = false;
                result = SSL_TLSEXT_ERR_NOACK;
            }
        }
    }

    auto* ptr = reinterpret_cast<CertificateStatusRequestV2*>(object);
    if (bSet && (ptr != nullptr)) {
        if (ptr->set_ocsp_response(digest, ctx)) {
            result = SSL_TLSEXT_ERR_OK;
        }
    }
    return result;
}

bool CertificateStatusRequestV2::set_ocsp_v2_response(const std::vector<openssl::sha_256_digest_t>& digests, SSL* ctx) {
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

        // don't include the extension when there are no OCSP responses
        if (total_size > 0) {
            std::size_t resp_len = total_size;
            auto* resp = static_cast<std::uint8_t*>(OPENSSL_malloc(resp_len));
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

int CertificateStatusRequestV2::status_request_v2_add(Ssl* ctx, unsigned int ext_type, unsigned int context,
                                                      const unsigned char** out, std::size_t* outlen, Certificate* cert,
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
    openssl::sha_256_digest_t digest{};
    std::vector<openssl::sha_256_digest_t> digest_chain;

    if (ctx != nullptr) {
        const auto* cert = SSL_get_certificate(ctx);
        const auto* name = X509_get_subject_name(cert);
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
                name = X509_get_subject_name(cert);
                if (OcspCache::digest(digest, cert)) {
                    digest_chain.push_back(digest);
                }
            }
        }
    }

    auto* ptr = reinterpret_cast<CertificateStatusRequestV2*>(object);
    if (!digest_chain.empty() && (ptr != nullptr)) {
        if (ptr->set_ocsp_v2_response(digest_chain, ctx)) {
            result = 1;
        }
    }
#endif // OPENSSL_PATCHED

    return result;
}

void CertificateStatusRequestV2::status_request_v2_free(Ssl* ctx, unsigned int ext_type, unsigned int context,
                                                        const unsigned char* out, void* object) {
    OPENSSL_free(const_cast<unsigned char*>(out));
}

int CertificateStatusRequestV2::status_request_v2_cb(Ssl* ctx, unsigned int ext_type, unsigned int context,
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

int CertificateStatusRequestV2::client_hello_cb(Ssl* ctx, int* alert, void* object) {
    /*
     * return values:
     * - fatal, abort handshake and sent TLS Alert: result = 0 or negative and *alert = alert value
     * - success: result = 1
     */

    auto* connection = reinterpret_cast<ServerConnection*>(SSL_get_app_data(ctx));
    if (connection != nullptr) {
        int* extensions{nullptr};
        std::size_t length{0};
        if (SSL_client_hello_get1_extensions_present(ctx, &extensions, &length) == 1) {
            for (std::size_t i = 0; i < length; i++) {
                if (extensions[i] == TLSEXT_TYPE_status_request) {
                    connection->status_request_received();
                } else if (extensions[i] == TLSEXT_TYPE_status_request_v2) {
                    connection->status_request_v2_received();
                }
            }
            OPENSSL_free(extensions);
        }
    }
    return 1;
}

// ----------------------------------------------------------------------------
// Connection represents a TLS connection

Connection::Connection(SslContext* ctx, int soc, const char* ip_in, const char* service_in, std::int32_t timeout_ms) :
    m_context(std::make_unique<connection_ctx>()), m_ip(ip_in), m_service(service_in), m_timeout_ms(timeout_ms) {
    m_context->ctx = SSL_ptr(SSL_new(ctx));
    m_context->soc = soc;

    if (m_context->ctx == nullptr) {
        log_error("Connection::SSL_new");
    } else {
        m_context->soc_bio = BIO_new_socket(soc, BIO_CLOSE);
    }

    if (m_context->soc_bio == nullptr) {
        log_error("Connection::BIO_new_socket");
    }
}

Connection::~Connection() = default;

Connection::result_t Connection::read(std::byte* buf, std::size_t num, std::size_t& readbytes) {
    assert(m_context != nullptr);
    ssl_result_t result{ssl_result_t::error};
    if (m_state == state_t::connected) {
        result = ssl_read(m_context->ctx.get(), buf, num, readbytes, m_timeout_ms);
        switch (result) {
        case ssl_result_t::success:
        case ssl_result_t::timeout:
            break;
        case ssl_result_t::error_syscall:
            m_state = state_t::fault;
            break;
        case ssl_result_t::closed:
            shutdown();
            break;
        case ssl_result_t::error:
        default:
            shutdown();
            m_state = state_t::fault;
            break;
        }
    }
    return convert(result);
}

Connection::result_t Connection::write(const std::byte* buf, std::size_t num, std::size_t& writebytes) {
    assert(m_context != nullptr);
    ssl_result_t result{ssl_result_t::error};
    if (m_state == state_t::connected) {
        result = ssl_write(m_context->ctx.get(), buf, num, writebytes, m_timeout_ms);
        switch (result) {
        case ssl_result_t::success:
        case ssl_result_t::timeout:
            break;
        case ssl_result_t::error_syscall:
            m_state = state_t::fault;
            break;
        case ssl_result_t::closed:
            shutdown();
            break;
        case ssl_result_t::error:
        default:
            shutdown();
            m_state = state_t::fault;
            break;
        }
    }
    return convert(result);
}

void Connection::shutdown() {
    assert(m_context != nullptr);
    if (m_state == state_t::connected) {
        ssl_shutdown(m_context->ctx.get(), c_shutdown_timeout_ms);
        m_state = state_t::closed;
    }
}

int Connection::socket() const {
    return m_context->soc;
}

SSL* Connection::ssl_context() const {
    return m_context->ctx.get();
}

// ----------------------------------------------------------------------------
// ServerConnection represents a TLS server connection

std::uint32_t ServerConnection::m_count{0};
std::mutex ServerConnection::m_cv_mutex;
std::condition_variable ServerConnection::m_cv;

ServerConnection::ServerConnection(SslContext* ctx, int soc, const char* ip_in, const char* service_in,
                                   std::int32_t timeout_ms) :
    Connection(ctx, soc, ip_in, service_in, timeout_ms) {
    {
        std::lock_guard lock(m_cv_mutex);
        m_count++;
    }
    if (m_context->soc_bio != nullptr) {
        // BIO_free is handled when SSL_free is done (SSL_ptr)
        SSL_set_bio(m_context->ctx.get(), m_context->soc_bio, m_context->soc_bio);
        SSL_set_accept_state(m_context->ctx.get());
        SSL_set_app_data(m_context->ctx.get(), this);
    }
}

ServerConnection::~ServerConnection() {
    {
        std::lock_guard lock(m_cv_mutex);
        m_count--;
    }
    m_cv.notify_all();
};

bool ServerConnection::accept() {
    assert(m_context != nullptr);
    ssl_result_t result{ssl_result_t::error};
    if (m_state == state_t::idle) {
        result = ssl_accept(m_context->ctx.get(), m_timeout_ms);
        switch (result) {
        case ssl_result_t::success:
            m_state = state_t::connected;
            break;
        case ssl_result_t::error_syscall:
            m_state = state_t::fault;
            break;
        case ssl_result_t::closed:
            shutdown();
            break;
        case ssl_result_t::error:
        default:
            shutdown();
            m_state = state_t::fault;
            break;
        }
    }
    return result == ssl_result_t::success;
}

void ServerConnection::wait_all_closed() {
    std::unique_lock lock(m_cv_mutex);
    m_cv.wait(lock, [] { return m_count == 0; });
    lock.unlock();
}

// ----------------------------------------------------------------------------
// ClientConnection represents a TLS server connection

ClientConnection::ClientConnection(SslContext* ctx, int soc, const char* ip_in, const char* service_in,
                                   std::int32_t timeout_ms) :
    Connection(ctx, soc, ip_in, service_in, timeout_ms) {
    if (m_context->soc_bio != nullptr) {
        BIO_set_nbio(m_context->soc_bio, 1);
        //  BIO_free is handled when SSL_free is done (SSL_ptr)
        SSL_set_bio(m_context->ctx.get(), m_context->soc_bio, m_context->soc_bio);
        SSL_set_connect_state(m_context->ctx.get());
    }
}

ClientConnection::~ClientConnection() = default;

bool ClientConnection::connect() {
    assert(m_context != nullptr);
    ssl_result_t result{ssl_result_t::error};
    if (m_state == state_t::idle) {
        result = ssl_connect(m_context->ctx.get(), m_timeout_ms);
        switch (result) {
        case ssl_result_t::success:
            m_state = state_t::connected;
            break;
        case ssl_result_t::error_syscall:
            m_state = state_t::fault;
            break;
        case ssl_result_t::closed:
            shutdown();
            break;
        case ssl_result_t::error:
        default:
            shutdown();
            m_state = state_t::fault;
            break;
        }
    }
    return result == ssl_result_t::success;
}

// ----------------------------------------------------------------------------
// TLS Server

Server::Server() : m_context(std::make_unique<server_ctx>()), m_status_request_v2(m_cache) {
}

Server::~Server() {
    stop();
    wait_stopped();
}

bool Server::init_socket(const config_t& cfg) {
    bool bRes = false;
    if (cfg.socket == INVALID_SOCKET) {
        BIO_ADDRINFO* addrinfo{nullptr};

        bRes = BIO_lookup_ex(cfg.host, cfg.service, BIO_LOOKUP_SERVER, AF_UNSPEC, SOCK_STREAM, IPPROTO_TCP,
                             &addrinfo) != 0;

        if (!bRes) {
            log_error("init_socket::BIO_lookup_ex");
        } else {
            const auto sock_family = BIO_ADDRINFO_family(addrinfo);
            const auto sock_type = BIO_ADDRINFO_socktype(addrinfo);
            const auto sock_protocol = BIO_ADDRINFO_protocol(addrinfo);
            const auto* sock_address = BIO_ADDRINFO_address(addrinfo);
            int sock_options{BIO_SOCK_REUSEADDR | BIO_SOCK_NONBLOCK};
            if (cfg.ipv6_only) {
                sock_options = BIO_SOCK_REUSEADDR | BIO_SOCK_V6_ONLY | BIO_SOCK_NONBLOCK;
            }

            m_socket = BIO_socket(sock_family, sock_type, sock_protocol, 0);

            if (m_socket == INVALID_SOCKET) {
                log_error("init_socket::BIO_socket");
            } else {
                bRes = BIO_listen(m_socket, sock_address, sock_options) != 0;
                if (!bRes) {
                    log_error("init_socket::BIO_listen");
                    BIO_closesocket(m_socket);
                    m_socket = INVALID_SOCKET;
                }
            }
        }

        BIO_ADDRINFO_free(addrinfo);
    } else {
        // the code that sets cfg.socket is responsible for
        // all socket initialisation
        m_socket = cfg.socket;
        bRes = true;
    }

    return bRes;
}

bool Server::init_ssl(const config_t& cfg) {
    assert(m_context != nullptr);

    // TODO(james-ctc) TPM2 support

    const SSL_METHOD* method = TLS_server_method();
    auto* ctx = SSL_CTX_new(method);
    auto bRes = configure_ssl_ctx(ctx, cfg.ciphersuites, cfg.cipher_list, cfg.certificate_chain_file,
                                  cfg.private_key_file, cfg.private_key_password, true);
    if (bRes) {
        int mode = SSL_VERIFY_NONE;

        if (cfg.verify_client) {
            mode = SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;
            if (SSL_CTX_load_verify_locations(ctx, cfg.verify_locations_file, cfg.verify_locations_path) != 1) {
                log_error("SSL_CTX_load_verify_locations");
            }
        } else {
            if (SSL_CTX_set_default_verify_paths(ctx) != 1) {
                log_error("SSL_CTX_set_default_verify_paths");
                bRes = false;
            }
        }

        SSL_CTX_set_verify(ctx, mode, nullptr);
        SSL_CTX_set_client_hello_cb(ctx, &CertificateStatusRequestV2::client_hello_cb, nullptr);

        if (SSL_CTX_set_tlsext_status_cb(ctx, &CertificateStatusRequestV2::status_request_cb) != 1) {
            log_error("SSL_CTX_set_tlsext_status_cb");
            bRes = false;
        }

        if (SSL_CTX_set_tlsext_status_arg(ctx, &m_status_request_v2) != 1) {
            log_error("SSL_CTX_set_tlsext_status_arg");
            bRes = false;
        }

        constexpr int context = SSL_EXT_TLS_ONLY | SSL_EXT_TLS1_2_AND_BELOW_ONLY | SSL_EXT_IGNORE_ON_RESUMPTION |
                                SSL_EXT_CLIENT_HELLO | SSL_EXT_TLS1_2_SERVER_HELLO;
        if (SSL_CTX_add_custom_ext(ctx, TLSEXT_TYPE_status_request_v2, context,
                                   &CertificateStatusRequestV2::status_request_v2_add,
                                   &CertificateStatusRequestV2::status_request_v2_free, &m_status_request_v2,
                                   &CertificateStatusRequestV2::status_request_v2_cb, nullptr) != 1) {
            log_error("SSL_CTX_add_custom_ext");
            bRes = false;
        }
    }

    if (!bRes) {
        SSL_CTX_free(ctx);
        ctx = nullptr;
    }

    m_context->ctx = SSL_CTX_ptr(ctx);
    return ctx != nullptr;
}

Server::state_t Server::init(const config_t& cfg, const std::function<bool(Server& server)>& init_ssl) {
    std::lock_guard lock(m_mutex);
    m_timeout_ms = cfg.io_timeout_ms;
    m_init_callback = init_ssl;
    m_state = state_t::init_needed;
    if (init_socket(cfg)) {
        m_state = state_t::init_socket;
        if (update(cfg)) {
            m_state = state_t::init_complete;
        }
    }
    return m_state;
}

bool Server::update(const config_t& cfg) {
    bool bRes = init_ssl(cfg);

    if (bRes) {
        std::vector<OcspCache::ocsp_entry_t> entries;
        auto chain = openssl::load_certificates(cfg.certificate_chain_file);
        if (chain.size() == cfg.ocsp_response_files.size()) {
            for (std::size_t i = 0; i < chain.size(); i++) {
                const auto& file = cfg.ocsp_response_files[i];
                const auto& cert = chain[i];

                if (file != nullptr) {
                    openssl::sha_256_digest_t digest{};
                    if (OcspCache::digest(digest, cert.get())) {
                        entries.emplace_back(digest, file);
                    }
                }
            }

            bRes = m_cache.load(entries);
        } else {
            log_warning(std::string("update_ocsp: ocsp files != ") + std::to_string(chain.size()));
        }
    }

    return bRes;
}

Server::state_t Server::serve(const std::function<void(std::shared_ptr<ServerConnection>& ctx)>& handler) {
    assert(m_context != nullptr);
    // prevent init() or server() being called while serve is running
    std::lock_guard lock(m_mutex);
    bool bRes = false;

    state_t tmp = m_state;

    switch (tmp) {
    case state_t::init_socket:
        if (m_init_callback != nullptr) {
            bRes = m_socket != INVALID_SOCKET;
        }
        break;
    case state_t::init_complete:
        bRes = m_socket != INVALID_SOCKET;
        break;
    case state_t::init_needed:
    case state_t::running:
    case state_t::stopped:
    default:
        break;
    }

    {
        std::lock_guard lock(m_cv_mutex);
        m_running = true;
    }
    m_cv.notify_all();

    if (bRes) {
        m_exit = false;
        m_state = (m_state == state_t::init_complete) ? state_t::running : state_t::init_socket;
        while (!m_exit) {
            auto* peer = BIO_ADDR_new();
            if (peer == nullptr) {
                log_error("serve::BIO_ADDR_new");
                m_exit = true;
            } else {
                int soc{INVALID_SOCKET};
                while ((soc < 0) && !m_exit) {
                    auto poll_res = wait_for(m_socket, false, m_timeout_ms);
                    if (poll_res == -1) {
                        log_error(std::string("Server::serve poll: ") + std::to_string(errno));
                        m_exit = true;
                    } else if (poll_res == 0) {
                        // timeout
                    } else {
                        soc = BIO_accept_ex(m_socket, peer, BIO_SOCK_NONBLOCK);
                        if (BIO_sock_should_retry(soc) == 0) {
                            break;
                        }
                    }
                };

                if ((soc >= 0) && (m_state == state_t::init_socket)) {
                    if (m_init_callback(*this)) {
                        m_state = state_t::running;
                    } else {
                        BIO_closesocket(soc);
                        soc = INVALID_SOCKET;
                    }
                }

                if (m_exit) {
                    if (soc >= 0) {
                        BIO_closesocket(soc);
                    }
                } else {
                    if (soc < 0) {
                        log_error("serve::BIO_accept_ex");
                    } else {
                        auto* ip = BIO_ADDR_hostname_string(peer, 1);
                        auto* service = BIO_ADDR_service_string(peer, 1);
                        auto connection =
                            std::make_shared<ServerConnection>(m_context->ctx.get(), soc, ip, service, m_timeout_ms);
                        handler(connection);
                        OPENSSL_free(ip);
                        OPENSSL_free(service);
                    }
                }
            }

            BIO_ADDR_free(peer);
        }

        BIO_closesocket(m_socket);
        m_socket = INVALID_SOCKET;
        bRes = true;
        m_state = state_t::stopped;
    }

    {
        std::lock_guard lock(m_cv_mutex);
        m_running = false;
    }
    m_cv.notify_all();
    return m_state;
}

void Server::stop() {
    m_exit = true;
}

void Server::wait_running() {
    std::unique_lock lock(m_cv_mutex);
    m_cv.wait(lock, [this] { return m_running; });
    lock.unlock();
}

void Server::wait_stopped() {
    std::unique_lock lock(m_cv_mutex);
    m_cv.wait(lock, [this] { return !m_running; });
    lock.unlock();
}

// ----------------------------------------------------------------------------
// Client

Client::Client() : m_context(std::make_unique<client_ctx>()) {
}

Client::~Client() = default;

bool Client::print_ocsp_response(FILE* stream, const unsigned char*& response, std::size_t length) {
    OCSP_RESPONSE* ocsp{nullptr};

    if (response != nullptr) {
        ocsp = d2i_OCSP_RESPONSE(nullptr, &response, static_cast<std::int64_t>(length));
        if (ocsp == nullptr) {
            std::cerr << "d2i_OCSP_RESPONSE: decode error" << std::endl;
        } else {
            BIO* bio_out = BIO_new_fp(stream, BIO_NOCLOSE);
            OCSP_RESPONSE_print(bio_out, ocsp, 0);
            OCSP_RESPONSE_free(ocsp);
            BIO_free(bio_out);
        }
    }

    return ocsp != nullptr;
}

bool Client::init(const config_t& cfg) {
    assert(m_context != nullptr);

    // TODO(james-ctc) TPM2 support

    const SSL_METHOD* method = TLS_client_method();
    auto* ctx = SSL_CTX_new(method);
    auto bRes = configure_ssl_ctx(ctx, cfg.ciphersuites, cfg.cipher_list, cfg.certificate_chain_file,
                                  cfg.private_key_file, cfg.private_key_password, false);
    if (bRes) {
        int mode = SSL_VERIFY_NONE;

        if (cfg.verify_server) {
            mode = SSL_VERIFY_PEER;
            if (SSL_CTX_load_verify_locations(ctx, cfg.verify_locations_file, cfg.verify_locations_path) != 1) {
                log_error("SSL_CTX_load_verify_locations");
            }
        } else {
            if (SSL_CTX_set_default_verify_paths(ctx) != 1) {
                log_error("SSL_CTX_set_default_verify_paths");
                bRes = false;
            }
        }

        SSL_CTX_set_verify(ctx, mode, nullptr);

        if (cfg.status_request) {
            if (SSL_CTX_set_tlsext_status_cb(ctx, &Client::status_request_v2_multi_cb) != 1) {
                log_error("SSL_CTX_set_tlsext_status_cb");
                bRes = false;
            }
            if (SSL_CTX_set_tlsext_status_type(ctx, TLSEXT_STATUSTYPE_ocsp) != 1) {
                log_error("SSL_CTX_set_tlsext_status_type");
                bRes = false;
            }
        }

        if (cfg.status_request_v2) {
            constexpr int context = SSL_EXT_TLS_ONLY | SSL_EXT_TLS1_2_AND_BELOW_ONLY | SSL_EXT_IGNORE_ON_RESUMPTION |
                                    SSL_EXT_CLIENT_HELLO | SSL_EXT_TLS1_2_SERVER_HELLO;
            if (SSL_CTX_add_custom_ext(ctx, TLSEXT_TYPE_status_request_v2, context, &Client::status_request_v2_add,
                                       nullptr, nullptr, &Client::status_request_v2_cb, this) != 1) {
                log_error("SSL_CTX_add_custom_ext");
                bRes = false;
            }
            if (SSL_CTX_set_tlsext_status_cb(ctx, &Client::status_request_v2_multi_cb) != 1) {
                log_error("SSL_CTX_set_tlsext_status_cb");
                bRes = false;
            }
        }

        if (cfg.status_request || cfg.status_request_v2) {
            if (SSL_CTX_set_tlsext_status_arg(ctx, this) != 1) {
                log_error("SSL_CTX_set_tlsext_status_arg");
                bRes = false;
            }
        }
    }

    if (bRes) {
        m_context->ctx = SSL_CTX_ptr(ctx);
        m_state = state_t::init;
    } else {
        SSL_CTX_free(ctx);
        ctx = nullptr;
    }

    return ctx != nullptr;
}

std::unique_ptr<ClientConnection> Client::connect(const char* host, const char* service, bool ipv6_only) {
    BIO_ADDRINFO* addrinfo{nullptr};
    std::unique_ptr<ClientConnection> result;

    const int family = (ipv6_only) ? AF_INET6 : AF_UNSPEC;
    bool bRes = BIO_lookup_ex(host, service, BIO_LOOKUP_CLIENT, family, SOCK_STREAM, IPPROTO_TCP, &addrinfo) != 0;

    if (!bRes) {
        log_error("connect::BIO_lookup_ex");
    } else {
        const auto sock_family = BIO_ADDRINFO_family(addrinfo);
        const auto sock_type = BIO_ADDRINFO_socktype(addrinfo);
        const auto sock_protocol = BIO_ADDRINFO_protocol(addrinfo);
        const auto* sock_address = BIO_ADDRINFO_address(addrinfo);

        // set non-blocking after a successful connection
        // using BIO_SOCK_NONBLOCK on connect is problematic
        // int sock_options{BIO_SOCK_NONBLOCK};

        auto socket = BIO_socket(sock_family, sock_type, sock_protocol, 0);

        if (socket == INVALID_SOCKET) {
            log_error("connect::BIO_socket");
        } else {
            if (BIO_connect(socket, sock_address, 0) != 1) {
                log_error("connect::BIO_connect");
            } else {
                result = std::make_unique<ClientConnection>(m_context->ctx.get(), socket, host, service, m_timeout_ms);
            }
        }
    }

    BIO_ADDRINFO_free(addrinfo);
    return result;
}

int Client::status_request_cb(Ssl* ctx) {
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

    if ((response != nullptr) && (total_length > 0)) {
        // there is a response

        if (response[0] == 0x30) {
            // not a multi response
            if (!print_ocsp_response(stdout, response, total_length)) {
                result = 0;
            }
        } else {
            // multiple responses
            auto remaining{total_length};
            const unsigned char* ptr{response};

            while (remaining > 0) {
                bool b_okay = remaining > 3;
                std::uint32_t len{0};

                if (b_okay) {
                    len = uint24(ptr);
                    remaining -= len + 3;
                    b_okay = remaining >= 0;
                }

                if (b_okay) {
                    ptr += 3;
                    b_okay = print_ocsp_response(stdout, ptr, len);
                }

                if (!b_okay) {
                    result = 0;
                    remaining = -1;
                }
            }
        }
    }

    return result;
}

int Client::status_request_v2_multi_cb(Ssl* ctx, void* object) {
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

    auto* client_ptr = reinterpret_cast<Client*>(object);

    int result{1};
    if (client_ptr != nullptr) {
        result = client_ptr->status_request_cb(ctx);
    } else {
        log_error("Client::status_request_v2_multi_cb missing Client *");
    }
    return result;
}

int Client::status_request_v2_add(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char** out,
                                  std::size_t* outlen, Certificate* cert, std::size_t chainidx, int* alert,
                                  void* object) {
    if (context == SSL_EXT_CLIENT_HELLO) {
        /*
         * CertificateStatusRequestListV2:
         * 0x0007   struct CertificateStatusRequestItemV2 + length
         *   0x02     CertificateStatusType - OCSP multi
         *   0x0004   request_length (uint 16)
         *     0x0000   struct ResponderID list + length
         *     0x0000   struct Extensions + length
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
        return 1;
    }
    return 0;
}

int Client::status_request_v2_cb(SSL* ctx, unsigned int ext_type, unsigned int context, const unsigned char* data,
                                 std::size_t datalen, X509* cert, std::size_t chainidx, int* alert, void* object) {
#ifdef OPENSSL_PATCHED
    SSL_set_tlsext_status_type(ctx, TLSEXT_STATUSTYPE_ocsp_multi);
    SSL_set_tlsext_status_expected(ctx, 1);
#endif // OPENSSL_PATCHED

    return 1;
}

} // namespace tls
