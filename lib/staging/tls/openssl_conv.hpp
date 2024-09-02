// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#ifndef OPENSSL_CONV_HPP_
#define OPENSSL_CONV_HPP_

#include <evse_security/certificate/x509_wrapper.hpp>
#include <evse_security/crypto/interface/crypto_types.hpp>
#include <openssl_util.hpp>

/**
 * \file Convert OpenSSL X509 certificates for use with EVSE Security
 *
 * The approach is to increase the reference count on the X509 certificate
 * rather than passing ownership. This is done so that certificates that are
 * already part of a structure (e.g. client certificate and chain from a SSL
 * connection) can be used easily. Otherwise the caller would need to remember
 * to make a copy before using these functions.
 *
 * example using an existing certificate:
 * \code
 * auto cert = to_X509Wrapper(SSL_get0_peer_certificate(ssl));
 * std::cout << cert.get_common_name() << std::endl;
 * \endcode
 *
 * example using a new certificate:
 * \code
 * auto* fp = fopen("certificate.pem", "r");
 * auto* x509 = PEM_read_X509(fp, nullptr, nullptr, nullptr);
 * fclose(fp);
 * auto cert = to_X509Wrapper(x509);
 * X509_free(x509);
 * std::cout << cert.get_common_name() << std::endl;
 * \endcode
 *
 * The following would lead to a memory leak because 'get1' returns a reference
 * to the certificate that needs to be freed.
 * \code
 * auto cert = to_X509Wrapper(SSL_get1_peer_certificate(ssl));
 * std::cout << cert.get_common_name() << std::endl;
 * \endcode
 *
 * The solution is to keep the result so it can be freed.
 * \code
 * auto* x509 = SSL_get1_peer_certificate(ssl);
 * auto cert = to_X509Wrapper(x509);
 * X509_free(x509);
 * std::cout << cert.get_common_name() << std::endl;
 * \endcode
 */

namespace openssl::conversions {

/**
 * \brief create an EVSE Security certificate handle from a OpenSSL X509 pointer
 * \param[in] cert a pointer to an OpenSSL X509 structure
 * \return a EVSE Security managed pointer
 * \note the reference count on cert is incremented. It may still need to be
 *       freed.
 */
evse_security::X509Handle_ptr to_X509Handle_ptr(x509_st* cert);
/**
 * \brief create an EVSE Security certificate wrapper from a OpenSSL X509 pointer
 * \param[in] cert a pointer to an OpenSSL X509 structure
 * \return a EVSE Security managed wrapper
 * \note the reference count on cert is incremented. It may still need to be
 *       freed.
 */
evse_security::X509Wrapper to_X509Wrapper(x509_st* cert);

} // namespace openssl::conversions

#endif // OPENSSL_CONV_HPP_
