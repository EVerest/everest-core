// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef OPENSSL_CONV_HPP_
#define OPENSSL_CONV_HPP_

#include <evse_security/certificate/x509_wrapper.hpp>
#include <evse_security/crypto/interface/crypto_types.hpp>
#include <openssl_util.hpp>

namespace openssl::conversions {

evse_security::X509Handle_ptr to_X509Handle_ptr(x509_st* cert);
evse_security::X509Wrapper to_X509Wrapper(x509_st* cert);

} // namespace openssl::conversions

#endif // OPENSSL_CONV_HPP_
