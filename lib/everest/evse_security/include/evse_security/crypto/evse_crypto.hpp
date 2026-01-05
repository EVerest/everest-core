// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#pragma once

#include <evse_security/crypto/interface/crypto_supplier.hpp>

// Include other required suppliers here
#ifdef LIBEVSE_CRYPTO_SUPPLIER_OPENSSL
#include <evse_security/crypto/openssl/openssl_crypto_supplier.hpp>
namespace evse_security {
using CryptoSupplier = OpenSSLSupplier; // Define others with the same 'CryptoSupplier' name
}
#endif