// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#include "gtest/gtest.h"

#include <string>
#include <vector>

#include <openssl_conv.hpp>
#include <openssl_util.hpp>

#include <evse_security/certificate/x509_hierarchy.hpp>
#include <evse_security/certificate/x509_wrapper.hpp>

namespace {

using evse_security::HashAlgorithm;
using evse_security::X509Wrapper;
using openssl::load_certificates;
using openssl::conversions::to_X509Wrapper;

TEST(evseSecurity, certificateHash) {
    auto chain = load_certificates("client_chain.pem");
    ASSERT_GT(chain.size(), 0);

    std::vector<X509Wrapper> certs;

    for (const auto& cert : chain) {
        certs.push_back(to_X509Wrapper(cert.get()));
    }

    for (std::uint8_t i = 0; i < certs.size() - 1; i++) {
        SCOPED_TRACE("i=" + std::to_string(i));
        const auto& cert = certs[i];
        const auto& issuer = certs[i + 1];
        const auto resA = cert.get_certificate_hash_data(issuer);
        EXPECT_EQ(resA.hash_algorithm, HashAlgorithm::SHA256);
    }
}

} // namespace
