// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef CRYPTO_OPENSSL_HPP_
#define CRYPTO_OPENSSL_HPP_

#include <cstddef>
#include <string>

#include "crypto_common.hpp"
#include <openssl_util.hpp>

/**
 * \file OpenSSL implementation
 */

struct evp_pkey_st;
struct iso2_SignatureType;
struct iso2_exiFragment;
struct x509_st;
struct v2g_connection;

namespace crypto::openssl {

/**
 * \brief check the signature of a signed 15118 message
 * \param iso2_signature the signature to check
 * \param public_key the public key from the contract certificate
 * \param iso2_exi_fragment the signed data
 * \return true when the signature is valid
 */
bool check_iso2_signature(const struct iso2_SignatureType* iso2_signature, evp_pkey_st* pkey,
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
bool load_contract_root_cert(::openssl::certificate_list& trust_anchors, const char* V2G_file_path,
                             const char* MO_file_path);

/**
 * \brief clear certificate and public key from previous connection
 * \param conn the V2G connection data
 * \note not needed for the OpenSSL implementation
 */
constexpr void free_connection_crypto_data(v2g_connection* conn) {
}

/**
 * \brief load a contract certificate's certification path certificate from the V2G message as DER bytes
 * \param chain the certificate path certificates (this certificate is added to the list)
 * \param bytes the DER (ASN.1) X509v3 certificate in the V2G message
 * \param bytesLen the length of the DER encoded certificate
 * \return 0 when certificate successfully loaded
 */
int load_certificate(::openssl::certificate_list* chain, const std::uint8_t* bytes, std::uint16_t bytesLen);

/**
 * \brief load the contract certificate from the V2G message as DER bytes
 * \param crt the certificate
 * \param bytes the DER (ASN.1) X509v3 certificate in the V2G message
 * \param bytesLen the length of the DER encoded certificate
 * \return 0 when certificate successfully loaded
 */
int parse_contract_certificate(::openssl::certificate_ptr& crt, const std::uint8_t* buf, std::size_t buflen);

/**
 * \brief get the EMAID from the certificate (CommonName from the SubjectName)
 * \param crt the certificate
 * \return the EMAD or empty on error
 */
std::string getEmaidFromContractCert(const ::openssl::certificate_ptr& crt);

/**
 * \brief convert a list of certificates into a PEM string starting with the contract certificate
 * \param contract_crt the contract certificate (when not the first certificate in the chain)
 * \param chain the certification path chain (might include the contract certificate as the first item)
 * \return PEM string or empty on error
 */
std::string chain_to_pem(const ::openssl::certificate_ptr& cert, const ::openssl::certificate_list* chain);

/**
 * \brief verify certification path of the contract certificate through to a trust anchor
 * \param contract_crt (single certificate or chain with the contract certificate as the first item)
 * \param chain intermediate certificates (may be nullptr)
 * \param v2g_root_cert_path V2G trust anchor file name
 * \param mo_root_cert_path mobility operator trust anchor file name
 * \param debugMode additional information on verification failures
 * \result a subset of possible verification failures where known or 'verified' on success
 */
verify_result_t verify_certificate(const ::openssl::certificate_ptr& cert, const ::openssl::certificate_list* chain,
                                   const char* v2g_root_cert_path, const char* mo_root_cert_path, bool debugMode);

} // namespace crypto::openssl

#endif // CRYPTO_OPENSSL_HPP_
