// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef CRYPTO_MBEDTLS_HPP_
#define CRYPTO_MBEDTLS_HPP_

#include <mbedtls/x509.h>
#include <mbedtls/x509_crt.h>

#include "crypto_common.hpp"
#include "v2g.hpp"

/**
 * \file Mbed TLS implementation
 */

struct mbedtls_x509_crt;

namespace crypto::mbedtls {

/**
 * \brief Wrapper around a mbedtls_x509_crt to ensure it is freed properly
 */
class Certificate_ptr {
private:
    mbedtls_x509_crt cert = {};

public:
    Certificate_ptr() {
        mbedtls_x509_crt_init(&cert);
    }
    ~Certificate_ptr() {
        mbedtls_x509_crt_free(&cert);
    }

    Certificate_ptr(const Certificate_ptr&) = delete;
    Certificate_ptr(Certificate_ptr&&) = delete;
    Certificate_ptr& operator=(const Certificate_ptr&) = delete;
    Certificate_ptr& operator=(Certificate_ptr&&) = delete;

    [[nodiscard]] mbedtls_x509_crt* get() {
        return &cert;
    }

    [[nodiscard]] const mbedtls_x509_crt* get() const {
        return &cert;
    }

    explicit operator mbedtls_x509_crt*() {
        return &cert;
    }
};

/**
 * \brief check the signature of a signed 15118 message
 * \param iso2_signature the signature to check
 * \param public_key the public key from the contract certificate
 * \param iso2_exi_fragment the signed data
 * \return true when the signature is valid
 */
bool check_iso2_signature(const struct iso2_SignatureType* iso2_signature, mbedtls_ecdsa_context& public_key,
                          struct iso2_exiFragment* iso2_exi_fragment);

/**
 * \brief load the trust anchor for the contract certificate.
 *        Use the mobility operator certificate if it exists otherwise
 *        the V2G certificate
 * \param contract_root_crt the retrieved trust anchor
 * \param V2G_file_path the file containing the V2G trust anchor (PEM format)
 * \param MO_file_path the file containing the mobility operator trust anchor (PEM format)
 * \return true when a certificate was found
 */
bool load_contract_root_cert(Certificate_ptr& contract_root_crt, const char* V2G_file_path, const char* MO_file_path);

/**
 * \brief clear public key from previous connection
 * \param conn the V2G connection data
 */
void free_connection_crypto_data(v2g_connection* conn);

/**
 * \brief load a contract certificate's certification path certificate from the V2G message as DER bytes
 * \param crt the certificate path certificates (this certificate is added to the list)
 * \param bytes the DER (ASN.1) X509v3 certificate in the V2G message
 * \param bytesLen the length of the DER encoded certificate
 * \return 0 when certificate successfully loaded
 */
int load_certificate(Certificate_ptr& crt, const std::uint8_t* bytes, std::uint16_t bytesLen);

/**
 * \brief load the contract certificate from the V2G message as DER bytes
 * \param crt the certificate
 * \param bytes the DER (ASN.1) X509v3 certificate in the V2G message
 * \param bytesLen the length of the DER encoded certificate
 * \return 0 when certificate successfully loaded
 */
int parse_contract_certificate(Certificate_ptr& crt, const std::uint8_t* bytes, std::size_t bytesLen);

/**
 * \brief get the EMAID from the certificate (CommonName from the SubjectName)
 * \param crt the certificate
 * \return the EMAD or empty on error
 */
std::string getEmaidFromContractCert(const Certificate_ptr& crt);

/**
 * \brief convert a list of certificates into a PEM string starting with the contract certificate
 * \param contract_crt the contract certificate (when not the first certificate in the chain)
 * \param chain the certification path chain (might include the contract certificate as the first item)
 * \return PEM string or empty on error
 */
std::string chain_to_pem(const Certificate_ptr& contract_crt, const void* chain);

/**
 * \brief verify certification path of the contract certificate through to a trust anchor
 * \param contract_crt (single certificate or chain with the contract certificate as the first item)
 * \param chain intermediate certificates (may be nullptr)
 * \param v2g_root_cert_path V2G trust anchor file name
 * \param mo_root_cert_path mobility operator trust anchor file name
 * \param debugMode additional information on verification failures
 * \result a subset of possible verification failures where known or 'verified' on success
 */
verify_result_t verify_certificate(Certificate_ptr& contract_crt, const void* chain, const char* v2g_root_cert_path,
                                   const char* mo_root_cert_path, bool debugMode);

/**
 * \brief convert certificate public key into a suable form
 * \param pubkey the certificate public key
 * \param pk the key in a form usable for checking signatures
 * \return 0 when key successfully loaded
 */
int get_public_key(mbedtls_ecdsa_context* pubkey, mbedtls_pk_context& pk);

/**
 * \brief convert a certificate into a PEM string
 * \param crt the certificate
 * \return the PEM string or empty on error
 */
std::string certificate_to_pem(const mbedtls_x509_crt* crt);

} // namespace crypto::mbedtls

#endif // CRYPTO_MBEDTLS_HPP_
