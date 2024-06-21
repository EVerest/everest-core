// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#ifndef EXTENSIONS_TRUSTED_CA_KEYS_
#define EXTENSIONS_TRUSTED_CA_KEYS_

#include <extensions/tls_types.hpp>
#include <openssl_util.hpp>

#include <mutex>

namespace tls::trusted_ca_keys {

using der_ptr = openssl::der_ptr;
using DER = openssl::DER;
using der_list = std::vector<DER>;

using digest_t = openssl::sha_1_digest_t;
using digest_list = std::vector<digest_t>;

// The trusted_ca_keys extension isn't DER encoded
// openssl::DER class can be reused though
using trusted_authority = openssl::DER;

using openssl::chain_list;
using openssl::chain_t;

/**
 * \brief items that can be requested in the trusted_ca_keys extension
 */
struct trusted_ca_keys_t {
    der_list x509_name;         //!< a list of DER encoded certificate subject names
    digest_list key_sha1_hash;  //!< a list of certificate key hashes
    digest_list cert_sha1_hash; //!< a list of certificate hashes
    bool pre_agreed{false};     //!< use the pre-agreed certificate
};

enum class IdentifierType : std::uint8_t {
    pre_agreed = 0,
    key_sha1_hash = 1,
    x509_name = 2,
    cert_sha1_hash = 3,
};

/**
 * \brief extract item from trusted_ca_keys extension
 * \param[out] result updated structure of parsed options
 * \param[inout] ptr a pointer to the first byte of the next item, updated to
 *                   be the first byte of the next item
 * \param[inout] remaining number of bytes left (updated with new value)
 * \return true on success
 */
bool extract_trusted_authority(trusted_ca_keys_t& result, const std::uint8_t*& ptr, std::int32_t& remaining);

/**
 * \brief calculate the size of array needed to hold the extension
 * \param[in] data the trusted items to include
 * \return the number of bytes needed
 */
std::size_t trusted_authority_size(const trusted_ca_keys_t& data);

/**
 * \brief calculate the digest over the certificate
 * \param[in] cert is the certificate
 * \param[out] digest the calculated digest
 * \return true on success
 */
bool certificate_digest(digest_t& digest, const x509_st* cert);

/**
 * \brief calculate the digest over the certificate's public key
 * \param[in] cert is the certificate
 * \param[out] digest the calculated digest
 * \return true on success
 */
bool public_key_digest(digest_t& digest, const x509_st* cert);

/**
 * \brief convert a trusted_ca_keys_t to the encoded extension
 * \param[in] keys the items to include in the extension
 * \return the encoded extension
 */
trusted_authority convert(const trusted_ca_keys_t& keys);

/**
 * \brief convert a encoded extension into trusted_ca_keys_t
 * \param[in] extension_data a pointer to the extension data
 * \param[in] len is the length of the extension data
 * \return the populated trusted_ca_keys_t
 * \note trusted_ca_keys_t will be empty if there is any problem decoding the
 *       extension. It will not contain partial results.
 */
trusted_ca_keys_t convert(const std::uint8_t* extension_data, std::size_t len);

/**
 * \brief convert a encoded extension into trusted_ca_keys_t
 * \param[in] extension_data the items to include in the extension
 * \return the populated trusted_ca_keys_t
 * \note trusted_ca_keys_t will be empty if there is any problem decoding the
 *       extension. It will not contain partial results.
 */
inline trusted_ca_keys_t convert(const trusted_authority& extension_data) {
    return convert(extension_data.get(), extension_data.size());
}

/**
 * \brief check if one of the trust anchors matches the trusted_ca_keys
 * \param[in] extension the information to check against
 * \param[in] chain the certificate chain to check against
 * \return true when the chain matches the extension
 */
bool match(const trusted_ca_keys_t& extension, const chain_t& chain);

/**
 * \brief select the certificate chain to use
 * \param[in] extension the information to check against
 * \param[in] chains the list of certificate chains to check against
 * \return a pointer to the chain (in chains) or nullptr when none match
 */
const chain_t* select(const trusted_ca_keys_t& extension, const chain_list& chains);

/// \brief per connection data
struct server_trusted_ca_keys_t {
    trusted_ca_keys_t& tck;  //!< parsed values
    tls::StatusFlags& flags; //!< flags to indicate which extensions were present
};

/// \brief trusted_ca_keys extension handler for a TLS server
class ServerTrustedCaKeys {
private:
    static int s_index;  //!< index used for storing per connection data
    chain_list m_chains; //!< known certificate chains
    std::mutex m_mux;    //!< protects m_chains

public:
    ServerTrustedCaKeys();
    ServerTrustedCaKeys(const ServerTrustedCaKeys&) = delete;
    ServerTrustedCaKeys(ServerTrustedCaKeys&&) = delete;
    ServerTrustedCaKeys& operator=(const ServerTrustedCaKeys&) = delete;
    ServerTrustedCaKeys& operator=(ServerTrustedCaKeys&&) = delete;
    ~ServerTrustedCaKeys() = default;

    /**
     * \brief add the extension to the SSL context
     * \param[inout] ctx the context to configure
     * \return true on success
     */
    bool init_ssl(SslContext* ctx);

    /**
     * \brief update m_chains with the new certificate chains to support
     * \param[in] new_chains the chains to support
     * \note new_chains is moved to m_chains and hence will be empty after
     *       calling update()
     */
    void update(chain_list&& new_chains);

    /**
     * \brief select chain to use based on parsed extension
     * \param[in] extension is the parsed extension
     * \return a pointer to the chain (in m_chains) or nullptr when none match
     * \note pointer will be invalid if update() called before it is used
     */
    const chain_t* select(const trusted_ca_keys_t& extension);

    /**
     * \brief select the default chain to use (first entry in m_chains)
     * \return a pointer to the first chain (in m_chains) or nullptr on error
     * \note pointer will be invalid if update() called before it is used
     */
    const chain_t* select_default();

    /**
     * \brief the OpenSSL callback for the trusted_ca_keys extension
     * \param[in] ctx the connection context
     * \param[in] ext_type the TLS extension
     * \param[in] context the extension context flags
     * \param[in] data pointer to the extension data
     * \param[in] datalen size of extension data
     * \param[in] cert certificate
     * \param[in] chainidx certificate chain index
     * \param[in] alert the alert to send on error
     * \param[in] object the instance of a CertificateStatusRequestV2
     * \return success = 1, error = zero or negative
     *
     * Parses the extension data to extract and populate trusted_ca_keys_t.
     * Calls StatusFlags::has_trusted_ca_keys() when a trusted_ca_keys extension
     * is present. Note that trusted_ca_keys_t may contain no information if
     * there was a parsing error.
     */
    static int trusted_ca_keys_cb(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char* data,
                                  std::size_t datalen, Certificate* cert, std::size_t chainidx, int* alert,
                                  void* object);

    /**
     * \brief the OpenSSL callback for the trusted_ca_keys extension
     * \param[in] ctx the connection context
     * \param[in] arg A ServerTrustedCaKeys object
     * \return success = 1, error = zero or negative
     *
     * Calls select() and select_default() after acquiring the mutex to ensure
     * that calls to update() don't invalidate the pointers.
     */
    static int handle_certificate_cb(Ssl* ssl, void* arg);

    /**
     * \brief store pointer to connection data
     * \param[in] ctx the connection context
     * \param[in] ptr pointer to the data
     */
    static void set_data(Ssl* ctx, server_trusted_ca_keys_t* ptr);

    /**
     * \brief retrieve pointer to connection data
     * \param[in] ctx the connection context
     * \return pointer to the data or nullptr
     */
    static server_trusted_ca_keys_t* get_data(Ssl* ctx);
};

/// \brief trusted_ca_keys extension handler for a TLS client
class ClientTrustedCaKeys {
public:
    ClientTrustedCaKeys() = default;
    ClientTrustedCaKeys(const ClientTrustedCaKeys&) = delete;
    ClientTrustedCaKeys(ClientTrustedCaKeys&&) = delete;
    ClientTrustedCaKeys& operator=(const ClientTrustedCaKeys&) = delete;
    ClientTrustedCaKeys& operator=(ClientTrustedCaKeys&&) = delete;
    ~ClientTrustedCaKeys() = default;

    /**
     * \brief add trusted_ca_keys extension to client hello
     * \param[in] ctx the connection context
     * \param[in] ext_type the TLS extension
     * \param[in] context the extension context flags
     * \param[in] out pointer to the extension data
     * \param[in] outlen size of extension data
     * \param[in] cert certificate
     * \param[in] chainidx certificate chain index
     * \param[in] alert the alert to send on error
     * \param[in] object pointer to trusted_ca_keys_t to populate the extension
     * \return success = 1, do not include = 0, error  == -1
     */
    static int trusted_ca_keys_add(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char** out,
                                   std::size_t* outlen, Certificate* cert, std::size_t chainidx, int* alert,
                                   void* object);

    /**
     * \brief free trusted_ca_keys extension added to server hello
     * \param[in] ctx the connection context
     * \param[in] ext_type the TLS extension
     * \param[in] context the extension context flags
     * \param[in] out pointer to the extension data
     * \param[in] object not used
     */
    static void trusted_ca_keys_free(Ssl* ctx, unsigned int ext_type, unsigned int context, const unsigned char* out,
                                     void* object);
};

} // namespace tls::trusted_ca_keys

#endif // EXTENSIONS_TRUSTED_CA_KEYS_
