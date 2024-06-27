// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <gtest/gtest.h>
#include <tls.hpp>

#include <cstring>

std::string to_string(const openssl::sha_256_digest_t& digest) {
    std::stringstream string_stream;
    string_stream << std::hex;

    for (int idx = 0; idx < digest.size(); ++idx)
        string_stream << std::setw(2) << std::setfill('0') << (int)digest[idx];

    return string_stream.str();
}

namespace {

TEST(OcspCache, initEmpty) {
    tls::OcspCache cache;
    openssl::sha_256_digest_t digest{};
    auto res = cache.lookup(digest);
    EXPECT_EQ(res.get(), nullptr);
}

TEST(OcspCache, init) {
    tls::OcspCache cache;

    auto chain = openssl::load_certificates("client_chain.pem");
    std::vector<tls::OcspCache::ocsp_entry_t> entries;

    openssl::sha_256_digest_t digest{};
    for (const auto& cert : chain) {
        ASSERT_TRUE(tls::OcspCache::digest(digest, cert.get()));
        // std::cout << "digest: " << to_string(digest) << std::endl;
        entries.emplace_back(digest, "ocsp_response.der");
    }

    EXPECT_TRUE(cache.load(entries));
    // std::cout << "digest: " << to_string(digest) << std::endl;
    auto res = cache.lookup(digest);
    EXPECT_NE(res.get(), nullptr);
}

} // namespace
