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

TEST(strdup, usage) {
    // auto* r1 = strdup(nullptr); need to ensure non-nullptr
    auto* r2 = strdup("");
    auto* r3 = strdup("hello");
    // free(r1);
    free(r2);
    free(r3);
    free(nullptr);
}

TEST(string, use) {
    // was hoping to use std::string for config, but ...
    std::string empty;
    std::string space{""};
    std::string value{"something"};

    EXPECT_TRUE(empty.empty());
    // EXPECT_FALSE(space.empty()); was hoping it would be true
    EXPECT_FALSE(value.empty());

    // EXPECT_EQ(empty.c_str(), nullptr); was hoping it would be nullptr
    EXPECT_NE(space.c_str(), nullptr);
    EXPECT_NE(value.c_str(), nullptr);
}

TEST(ConfigItem, test) {
    tls::ConfigItem i1;
    tls::ConfigItem i2{nullptr};
    tls::ConfigItem i3{"Hello"};
    tls::ConfigItem i4 = nullptr;
    tls::ConfigItem i5(nullptr);
    tls::ConfigItem i6("Hello");

    EXPECT_EQ(i1, nullptr);
    EXPECT_EQ(i4, nullptr);
    EXPECT_EQ(i5, nullptr);

    EXPECT_EQ(i2, i5);
    EXPECT_EQ(i3, i6);

    EXPECT_EQ(i1, i2);
    EXPECT_NE(i1, i3);
    EXPECT_EQ(i1, i5);
    EXPECT_NE(i1, i6);

    auto j1(std::move(i3));
    auto j2 = std::move(i6);
    EXPECT_EQ(i6, i3);
    EXPECT_EQ(j1, j2);
    EXPECT_EQ(j1, "Hello");
    EXPECT_NE(j1, i6);

    EXPECT_NE(j1, nullptr);
    EXPECT_NE(j2, nullptr);

    EXPECT_EQ(i3, nullptr);
    EXPECT_EQ(i6, nullptr);
    EXPECT_EQ(i6, i3);

    std::vector<tls::ConfigItem> j3 = {"one", "two", nullptr};
    EXPECT_EQ(j3[0], "one");
    EXPECT_EQ(j3[1], "two");
    EXPECT_EQ(j3[2], nullptr);

    const char* p = j1;
    EXPECT_STREQ(p, "Hello");
}

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
