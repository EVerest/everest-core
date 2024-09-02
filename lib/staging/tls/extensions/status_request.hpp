// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#ifndef EXTENSIONS_STATUS_REQUEST_
#define EXTENSIONS_STATUS_REQUEST_

#include <extensions/tls_types.hpp>
#include <openssl_util.hpp>

#include <cstddef>
#include <memory>
#include <mutex>

namespace tls {

struct ocsp_cache_ctx;

// ----------------------------------------------------------------------------
// Cache of OCSP responses for status_request and status_request_v2 extensions

/**
 * \brief cache of OCSP responses
 * \note responses can be updated at any time via load()
 */
class OcspCache {
public:
    using digest_t = openssl::sha_1_digest_t;
    using ocsp_entry_t = std::tuple<digest_t, const char*>;
    using ocsp_entry_list_t = std::vector<ocsp_entry_t>;
    using OcspResponse_t = std::shared_ptr<OcspResponse>;

private:
    std::unique_ptr<ocsp_cache_ctx> m_context; //!< opaque cache data
    std::mutex mux;                            //!< protects the cached OCSP responses

public:
    OcspCache();
    OcspCache(const OcspCache&) = delete;
    OcspCache(OcspCache&&) = delete;
    OcspCache& operator=(const OcspCache&) = delete;
    OcspCache& operator=(OcspCache&&) = delete;
    ~OcspCache();

    /**
     * \brief populate the cache from a list of (digest, filename) pairs
     * \param[in] filenames is a list of (digest, filename) pairs
     * \return true when successfully loaded
     * \note the OCSP response from the file is cached rather than the filename
     */
    bool load(const ocsp_entry_list_t& filenames);

    /**
     * \brief return a pointer to the OCSP response for the given digest
     * \param[in] digest is the lookup key
     * \return the OCSP response or nullptr
     */
    OcspResponse_t lookup(const digest_t& digest);

    /**
     * \brief calculate the digest for the specified certificate
     * \param[out] digest is the calculated hash
     * \param[in] cert is the certificate to hash
     * \return true on success
     */
    static bool digest(digest_t& digest, const x509_st* cert);
};

namespace status_request {
// ----------------------------------------------------------------------------
// TLS handshake extension status_request and status_request_v2 support

using digest_t = OcspCache::digest_t;
using digest_list_t = std::vector<digest_t>;

/// \brief status_request and status_request_v2 handler for TLS server
class ServerStatusRequestV2 {
private:
    static int s_index; //!< index used for storing per connection data
    OcspCache& m_cache; //!< reference to the OCSP cache

public:
    using StatusFlags = tls::StatusFlags;

    explicit ServerStatusRequestV2(OcspCache& cache);
    ServerStatusRequestV2() = delete;
    ServerStatusRequestV2(const ServerStatusRequestV2&) = delete;
    ServerStatusRequestV2(ServerStatusRequestV2&&) = delete;
    ServerStatusRequestV2& operator=(const ServerStatusRequestV2&) = delete;
    ServerStatusRequestV2& operator=(ServerStatusRequestV2&&) = delete;
    ~ServerStatusRequestV2() = default;

    /**
     * \brief add the extension to the SSL context
     * \param[inout] ctx the context to configure
     * \return true on success
     */
    bool init_ssl(SslContext* ctx);

    /**
     * \brief set the OCSP response for the SSL context
     * \param[in] digest the certificate requested
     * \param[in] ctx the connection context
     * \return true on success
     * \note for status_request extension
     */
    bool set_ocsp_response(const digest_t& digest, Ssl* ctx);

    /**
     * \brief the OpenSSL callback for the status_request extension
     * \param[in] ctx the connection context
     * \param[in] object the instance of a ServerStatusRequestV2
     * \return SSL_TLSEXT_ERR_OK on success and SSL_TLSEXT_ERR_NOACK on error
     */
    static int status_request_cb(Ssl* ctx, void* object);

    /**
     * \brief set the OCSP response for the SSL context
     * \param[in] digest the certificate requested
     * \param[in] ctx the connection context
     * \return true on success
     * \note for status_request_v2 extension
     */
    bool set_ocsp_v2_response(const digest_list_t& digests, Ssl* ctx);

    /**
     * \brief add status_request_v2 extension to server hello
     * \param[in] ctx the connection context
     * \param[in] ext_type the TLS extension
     * \param[in] context the extension context flags
     * \param[in] out pointer to the extension data
     * \param[in] outlen size of extension data
     * \param[in] cert certificate
     * \param[in] chainidx certificate chain index
     * \param[in] alert the alert to send on error
     * \param[in] object the instance of a ServerStatusRequestV2
     * \return success = 1, do not include = 0, error  == -1
     */
    static int status_request_v2_add(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char** out,
                                     std::size_t* outlen, Certificate* cert, std::size_t chainidx, int* alert,
                                     void* object);

    /**
     * \brief free status_request_v2 extension added to server hello
     * \param[in] ctx the connection context
     * \param[in] ext_type the TLS extension
     * \param[in] context the extension context flags
     * \param[in] out pointer to the extension data
     * \param[in] object the instance of a ServerStatusRequestV2 - not used
     */
    static void status_request_v2_free(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char* out,
                                       void* object);

    /**
     * \brief the OpenSSL callback for the status_request_v2 extension
     * \param[in] ctx the connection context
     * \param[in] ext_type the TLS extension
     * \param[in] context the extension context flags
     * \param[in] data pointer to the extension data
     * \param[in] datalen size of extension data
     * \param[in] cert certificate
     * \param[in] chainidx certificate chain index
     * \param[in] alert the alert to send on error
     * \param[in] object not used
     * \return success = 1, error = zero or negative
     */
    static int status_request_v2_cb(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char* data,
                                    std::size_t datalen, Certificate* cert, std::size_t chainidx, int* alert,
                                    void* object);

    /**
     * \brief the OpenSSL callback for the client hello record
     * \param[in] ctx the connection context
     * \param[in] alert the alert to send on error
     * \param[in] object not used
     * \return success = 1, error = zero or negative
     *
     * This callback has early access to the extensions requested by the client.
     * It is used to determine whether status_request and status_request_v2
     * have been requested so that status_request_v2 can take priority.
     *
     * Calls StatusFlags::has_status_request() when a status_request extension
     * is present.
     *
     * Calls StatusFlags::has_status_request_v2() when a status_request_v2
     * extension is present.
     */
    static int client_hello_cb(Ssl* ctx, int* alert, void* object);

    /**
     * \brief store pointer to connection data
     * \param[in] ctx the connection context
     * \param[in] ptr pointer to the data
     */
    static void set_data(Ssl* ctx, StatusFlags* ptr);

    /**
     * \brief retrieve pointer to connection data
     * \param[in] ctx the connection context
     * \return pointer to the data or nullptr
     */
    static StatusFlags* get_data(Ssl* ctx);
};

/// \brief status_request and status_request_v2 handler for TLS client
class ClientStatusRequestV2 {
public:
    ClientStatusRequestV2() = default;
    ClientStatusRequestV2(const ClientStatusRequestV2&) = delete;
    ClientStatusRequestV2(ClientStatusRequestV2&&) = delete;
    ClientStatusRequestV2& operator=(const ClientStatusRequestV2&) = delete;
    ClientStatusRequestV2& operator=(ClientStatusRequestV2&&) = delete;
    virtual ~ClientStatusRequestV2() = default;

    /**
     * \brief print an OCSP response to the specified stream
     * \param[in] stream the output stream to use
     * \param[in] response the OCSP response
     * \param[in] length of the OCSP response
     * \return true on success
     */
    static bool print_ocsp_response(FILE* stream, const unsigned char*& response, std::size_t length);

    /**
     * \brief the OpenSSL callback for the status_request and status_request_v2 extensions
     * \param[in] ctx the connection context
     * \return SSL_TLSEXT_ERR_OK on success and SSL_TLSEXT_ERR_NOACK on error
     */
    virtual int status_request_cb(Ssl* ctx);

    /**
     * \brief the OpenSSL callback for the status_request_v2 extension
     * \param[in] ctx the connection context
     * \param[in] object the instance of ClientStatusRequestV2
     * \return SSL_TLSEXT_ERR_OK on success and SSL_TLSEXT_ERR_NOACK on error
     */
    static int status_request_v2_multi_cb(Ssl* ctx, void* object);

    /**
     * \brief add status_request_v2 extension to client hello
     * \param[in] ctx the connection context
     * \param[in] ext_type the TLS extension
     * \param[in] context the extension context flags
     * \param[in] out pointer to the extension data
     * \param[in] outlen size of extension data
     * \param[in] cert certificate
     * \param[in] chainidx certificate chain index
     * \param[in] alert the alert to send on error
     * \param[in] object not used
     * \return success = 1, do not include = 0, error  == -1
     */
    static int status_request_v2_add(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char** out,
                                     std::size_t* outlen, Certificate* cert, std::size_t chainidx, int* alert,
                                     void* object);

    /**
     * \brief the OpenSSL callback for the status_request_v2 extension
     * \param[in] ctx the connection context
     * \param[in] ext_type the TLS extension
     * \param[in] context the extension context flags
     * \param[in] data pointer to the extension data
     * \param[in] datalen size of extension data
     * \param[in] cert certificate
     * \param[in] chainidx certificate chain index
     * \param[in] alert the alert to send on error
     * \param[in] object not used
     * \return success = 1, error = zero or negative
     */
    static int status_request_v2_cb(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char* data,
                                    std::size_t datalen, Certificate* cert, std::size_t chainidx, int* alert,
                                    void* object);
};

} // namespace status_request
} // namespace tls

#endif // EXTENSIONS_STATUS_REQUEST_
