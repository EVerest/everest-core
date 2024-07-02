// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <openssl_conv.hpp>

#include <evse_security/detail/openssl/openssl_types.hpp>
#include <memory>
#include <openssl/x509.h>

using evse_security::X509Handle_ptr;
using evse_security::X509HandleOpenSSL;
using evse_security::X509Wrapper;

namespace openssl::conversions {

X509Handle_ptr to_X509Handle_ptr(x509_st* cert) {
    X509Handle_ptr ptr;
    if (X509_up_ref(cert) == 1) {
        ptr = std::make_unique<X509HandleOpenSSL>(cert);
    }
    return ptr;
}

X509Wrapper to_X509Wrapper(x509_st* cert) {
    return X509Wrapper(to_X509Handle_ptr(cert));
}

} // namespace openssl::conversions
