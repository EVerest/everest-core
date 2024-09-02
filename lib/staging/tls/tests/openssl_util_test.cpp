// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#include "gtest/gtest.h"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl_util.hpp>
#include <vector>

bool operator==(const ::openssl::certificate_ptr& lhs, const ::openssl::certificate_ptr& rhs) {
    using ::openssl::certificate_to_pem;
    if (lhs && rhs) {
        const auto res_lhs = certificate_to_pem(lhs.get());
        const auto res_rhs = certificate_to_pem(rhs.get());
        return res_lhs == res_rhs;
    }

    return false;
}

namespace {

template <typename T> constexpr void setCharacters(T& dest, const std::string& s) {
    dest.charactersLen = s.size();
    std::memcpy(&dest.characters[0], s.c_str(), s.size());
}

template <typename T> constexpr void setBytes(T& dest, const std::uint8_t* b, std::size_t len) {
    dest.bytesLen = len;
    std::memcpy(&dest.bytes[0], b, len);
}

struct test_vectors_t {
    const char* input;
    const std::uint8_t digest[32];
};

constexpr std::uint8_t sign_test[] = {0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c,
                                      0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55};

constexpr test_vectors_t sha_256_test[] = {
    {"", {0xe3, 0xb0, 0xc4, 0x42, 0x98, 0xfc, 0x1c, 0x14, 0x9a, 0xfb, 0xf4, 0xc8, 0x99, 0x6f, 0xb9, 0x24,
          0x27, 0xae, 0x41, 0xe4, 0x64, 0x9b, 0x93, 0x4c, 0xa4, 0x95, 0x99, 0x1b, 0x78, 0x52, 0xb8, 0x55}},
    {"abc", {0xba, 0x78, 0x16, 0xbf, 0x8f, 0x01, 0xcf, 0xea, 0x41, 0x41, 0x40, 0xde, 0x5d, 0xae, 0x22, 0x23,
             0xb0, 0x03, 0x61, 0xa3, 0x96, 0x17, 0x7a, 0x9c, 0xb4, 0x10, 0xff, 0x61, 0xf2, 0x00, 0x15, 0xad}}};

// EXI AuthorizationReq: checked okay (hash computes correctly)
constexpr std::uint8_t iso_exi_a[] = {0x80, 0x04, 0x01, 0x52, 0x51, 0x0c, 0x40, 0x82, 0x9b, 0x7b, 0x6b, 0x29, 0x02,
                                      0x93, 0x0b, 0x73, 0x23, 0x7b, 0x69, 0x02, 0x23, 0x0b, 0xa3, 0x09, 0xe8};

// checked okay
constexpr std::uint8_t iso_exi_a_hash[] = {0xd1, 0xb5, 0xe0, 0x3d, 0x00, 0x65, 0xbe, 0xe5, 0x6b, 0x31, 0x79,
                                           0x84, 0x45, 0x30, 0x51, 0xeb, 0x54, 0xca, 0x18, 0xfc, 0x0e, 0x09,
                                           0x16, 0x17, 0x4f, 0x8b, 0x3c, 0x77, 0xa9, 0x8f, 0x4a, 0xa9};

// EXI AuthorizationReq signature block: checked okay (hash computes correctly)
constexpr std::uint8_t iso_exi_b[] = {
    0x80, 0x81, 0x12, 0xb4, 0x3a, 0x3a, 0x38, 0x1d, 0x17, 0x97, 0xbb, 0xbb, 0xbb, 0x97, 0x3b, 0x99, 0x97, 0x37, 0xb9,
    0x33, 0x97, 0xaa, 0x29, 0x17, 0xb1, 0xb0, 0xb7, 0x37, 0xb7, 0x34, 0xb1, 0xb0, 0xb6, 0x16, 0xb2, 0xbc, 0x34, 0x97,
    0xa1, 0xab, 0x43, 0xa3, 0xa3, 0x81, 0xd1, 0x79, 0x7b, 0xbb, 0xbb, 0xb9, 0x73, 0xb9, 0x99, 0x73, 0x7b, 0x93, 0x39,
    0x79, 0x91, 0x81, 0x81, 0x89, 0x79, 0x81, 0xa1, 0x7b, 0xc3, 0x6b, 0x63, 0x23, 0x9b, 0x4b, 0x39, 0x6b, 0x6b, 0x7b,
    0x93, 0x29, 0x1b, 0x2b, 0x1b, 0x23, 0x9b, 0x09, 0x6b, 0x9b, 0x43, 0x09, 0x91, 0xa9, 0xb2, 0x20, 0x62, 0x34, 0x94,
    0x43, 0x10, 0x25, 0x68, 0x74, 0x74, 0x70, 0x3a, 0x2f, 0x2f, 0x77, 0x77, 0x77, 0x2e, 0x77, 0x33, 0x2e, 0x6f, 0x72,
    0x67, 0x2f, 0x54, 0x52, 0x2f, 0x63, 0x61, 0x6e, 0x6f, 0x6e, 0x69, 0x63, 0x61, 0x6c, 0x2d, 0x65, 0x78, 0x69, 0x2f,
    0x48, 0x52, 0xd0, 0xe8, 0xe8, 0xe0, 0x74, 0x5e, 0x5e, 0xee, 0xee, 0xee, 0x5c, 0xee, 0x66, 0x5c, 0xde, 0xe4, 0xce,
    0x5e, 0x64, 0x60, 0x60, 0x62, 0x5e, 0x60, 0x68, 0x5e, 0xf0, 0xda, 0xd8, 0xca, 0xdc, 0xc6, 0x46, 0xe6, 0xd0, 0xc2,
    0x64, 0x6a, 0x6c, 0x84, 0x1a, 0x36, 0xbc, 0x07, 0xa0, 0x0c, 0xb7, 0xdc, 0xad, 0x66, 0x2f, 0x30, 0x88, 0xa6, 0x0a,
    0x3d, 0x6a, 0x99, 0x43, 0x1f, 0x81, 0xc1, 0x22, 0xc2, 0xe9, 0xf1, 0x67, 0x8e, 0xf5, 0x31, 0xe9, 0x55, 0x23, 0x70};

// checked okay
constexpr std::uint8_t iso_exi_b_hash[] = {0xa4, 0xe9, 0x03, 0xe1, 0x82, 0x43, 0x04, 0x1b, 0x55, 0x4e, 0x11,
                                           0x64, 0x7e, 0x10, 0x1e, 0xd2, 0x5f, 0xc9, 0xf2, 0x15, 0x2a, 0xf4,
                                           0x67, 0x40, 0x14, 0xfe, 0x2a, 0xde, 0xac, 0x1e, 0x1c, 0xf7};

// checked okay (verifies iso_exi_b_hash with iso_priv.pem)
constexpr std::uint8_t iso_exi_sig[] = {0x4c, 0x8f, 0x20, 0xc1, 0x40, 0x0b, 0xa6, 0x76, 0x06, 0xaa, 0x48, 0x11, 0x57,
                                        0x2a, 0x2f, 0x1a, 0xd3, 0xc1, 0x50, 0x89, 0xd9, 0x54, 0x20, 0x36, 0x34, 0x30,
                                        0xbb, 0x26, 0xb4, 0x9d, 0xb1, 0x04, 0xf0, 0x8d, 0xfa, 0x8b, 0xf8, 0x05, 0x5e,
                                        0x63, 0xa4, 0xb7, 0x5a, 0x8d, 0x31, 0x69, 0x20, 0x6f, 0xa8, 0xd5, 0x43, 0x08,
                                        0xba, 0x58, 0xf0, 0x56, 0x6b, 0x96, 0xba, 0xf6, 0x92, 0xce, 0x59, 0x50};

const char iso_exi_a_hash_b64[] = "0bXgPQBlvuVrMXmERTBR61TKGPwOCRYXT4s8d6mPSqk=";
const char iso_exi_a_hash_b64_nl[] = "0bXgPQBlvuVrMXmERTBR61TKGPwOCRYXT4s8d6mPSqk=\n";

const char iso_exi_sig_b64[] =
    "TI8gwUALpnYGqkgRVyovGtPBUInZVCA2NDC7JrSdsQTwjfqL+AVeY6S3Wo0xaSBvqNVDCLpY8FZrlrr2ks5ZUA==";
const char iso_exi_sig_b64_nl[] =
    "TI8gwUALpnYGqkgRVyovGtPBUInZVCA2NDC7JrSdsQTwjfqL+AVeY6S3Wo0xaSBv\nqNVDCLpY8FZrlrr2ks5ZUA==\n";

TEST(util, removeHyphen) {
    const std::string expected{"UKSWI123456791A"};
    std::string cert_emaid{"UKSWI123456791A"};

    EXPECT_EQ(cert_emaid, expected);
    cert_emaid.erase(std::remove(cert_emaid.begin(), cert_emaid.end(), '-'), cert_emaid.end());
    EXPECT_EQ(cert_emaid, expected);

    cert_emaid = std::string{"-UKSWI-123456791-A-"};
    cert_emaid.erase(std::remove(cert_emaid.begin(), cert_emaid.end(), '-'), cert_emaid.end());
    EXPECT_EQ(cert_emaid, expected);
}

TEST(DER, equal) {
    const std::uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    openssl::DER a;
    openssl::DER b(sizeof(data));
    openssl::DER c1(&data[0], sizeof(data));
    openssl::DER c2(&data[0], sizeof(data));
    openssl::DER d(&data[0], sizeof(data) - 1);

    EXPECT_FALSE(a);
    EXPECT_TRUE(b);
    EXPECT_TRUE(c1);
    EXPECT_TRUE(c2);
    EXPECT_TRUE(d);

    EXPECT_EQ(a, nullptr);
    EXPECT_NE(b, nullptr);
    EXPECT_NE(c1, nullptr);
    EXPECT_NE(c2, nullptr);
    EXPECT_NE(d, nullptr);

    EXPECT_NE(c1.get(), c2.get());
    EXPECT_EQ(c1.size(), c2.size());
    EXPECT_EQ(c1, c2);
    EXPECT_EQ(c1, c1);

    EXPECT_NE(c1, a);
    EXPECT_NE(c1, b);
    EXPECT_NE(c1, d);
    EXPECT_NE(a, c1);
    EXPECT_NE(b, c1);
    EXPECT_NE(d, c1);
}

TEST(DER, construct) {
    const std::uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    const openssl::DER a(&data[0], sizeof(data));

    auto b = a;
    EXPECT_NE(a.get(), b.get());
    EXPECT_EQ(a, b);

    auto c{a};
    EXPECT_NE(a.get(), c.get());
    EXPECT_EQ(a, c);
    EXPECT_EQ(b, c);

    auto d = std::move(b);
    EXPECT_EQ(a, d);
    EXPECT_EQ(b.size(), 0);
    EXPECT_EQ(b, nullptr);
    EXPECT_FALSE(b);

    auto e(std::move(c));
    EXPECT_EQ(a, e);
    EXPECT_EQ(c.size(), 0);
    EXPECT_EQ(c, nullptr);
    EXPECT_FALSE(c);

    const std::uint8_t alt[] = {9, 8, 7, 6, 5, 4};
    openssl::DER x(&alt[0], sizeof(alt));
    x = e;
    EXPECT_EQ(x, a);
    EXPECT_NE(x, a.get());
    EXPECT_NE(x, e.get());
}

TEST(DER, release) {
    const std::uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    openssl::DER a(&data[0], sizeof(data));
    EXPECT_EQ(a.size(), sizeof(data));
    EXPECT_NE(a.get(), &data[0]);
    const auto* tmp_p = a.get();

    auto* ptr = a.release();
    EXPECT_EQ(a.size(), 0);
    EXPECT_EQ(a.get(), nullptr);
    EXPECT_EQ(ptr, tmp_p);
    openssl::DER::free(ptr);
}

TEST(openssl, sizes) {
    EXPECT_EQ(sizeof(openssl::sha_1_digest_t), openssl::sha_1_digest_size);
    EXPECT_EQ(sizeof(openssl::sha_256_digest_t), openssl::sha_256_digest_size);
    EXPECT_EQ(sizeof(openssl::sha_384_digest_t), openssl::sha_384_digest_size);
    EXPECT_EQ(sizeof(openssl::sha_512_digest_t), openssl::sha_512_digest_size);
}

TEST(openssl, base64Encode) {
    auto res = openssl::base64_encode(&iso_exi_a_hash[0], sizeof(iso_exi_a_hash));
    EXPECT_EQ(res, iso_exi_a_hash_b64);
    res = openssl::base64_encode(&iso_exi_sig[0], sizeof(iso_exi_sig));
    EXPECT_EQ(res, iso_exi_sig_b64);
}

TEST(openssl, base64EncodeNl) {
    auto res = openssl::base64_encode(&iso_exi_a_hash[0], sizeof(iso_exi_a_hash), true);
    EXPECT_EQ(res, iso_exi_a_hash_b64_nl);
    res = openssl::base64_encode(&iso_exi_sig[0], sizeof(iso_exi_sig), true);
    EXPECT_EQ(res, iso_exi_sig_b64_nl);
}

TEST(openssl, base64Decode) {
    auto res = openssl::base64_decode(&iso_exi_a_hash_b64[0], sizeof(iso_exi_a_hash_b64));
    ASSERT_EQ(res.size(), sizeof(iso_exi_a_hash));
    EXPECT_EQ(std::memcmp(res.data(), &iso_exi_a_hash[0], res.size()), 0);
    res = openssl::base64_decode(&iso_exi_sig_b64[0], sizeof(iso_exi_sig_b64));
    ASSERT_EQ(res.size(), sizeof(iso_exi_sig));
    EXPECT_EQ(std::memcmp(res.data(), &iso_exi_sig[0], res.size()), 0);

    std::array<std::uint8_t, 512> buffer{};
    std::size_t buffer_len = buffer.size();

    EXPECT_TRUE(openssl::base64_decode(&iso_exi_a_hash_b64[0], sizeof(iso_exi_a_hash_b64), buffer.data(), buffer_len));
    ASSERT_EQ(buffer_len, sizeof(iso_exi_a_hash));
    EXPECT_EQ(std::memcmp(buffer.data(), &iso_exi_a_hash[0], buffer_len), 0);
}

TEST(openssl, base64DecodeNl) {
    auto res = openssl::base64_decode(&iso_exi_a_hash_b64_nl[0], sizeof(iso_exi_a_hash_b64_nl));
    ASSERT_EQ(res.size(), sizeof(iso_exi_a_hash));
    EXPECT_EQ(std::memcmp(res.data(), &iso_exi_a_hash[0], res.size()), 0);
    res = openssl::base64_decode(&iso_exi_sig_b64_nl[0], sizeof(iso_exi_sig_b64_nl));
    ASSERT_EQ(res.size(), sizeof(iso_exi_sig));
    EXPECT_EQ(std::memcmp(res.data(), &iso_exi_sig[0], res.size()), 0);

    std::array<std::uint8_t, 512> buffer{};
    std::size_t buffer_len = buffer.size();

    EXPECT_TRUE(
        openssl::base64_decode(&iso_exi_a_hash_b64_nl[0], sizeof(iso_exi_a_hash_b64_nl), buffer.data(), buffer_len));
    ASSERT_EQ(buffer_len, sizeof(iso_exi_a_hash));
    EXPECT_EQ(std::memcmp(buffer.data(), &iso_exi_a_hash[0], buffer_len), 0);
}

TEST(openssl, sha256) {
    openssl::sha_256_digest_t digest;
    EXPECT_TRUE(openssl::sha_256(sha_256_test[0].input, 0, digest));
    EXPECT_EQ(std::memcmp(digest.data(), &sha_256_test[0].digest[0], 32), 0);
    EXPECT_TRUE(openssl::sha_256(sha_256_test[1].input, 3, digest));
    EXPECT_EQ(std::memcmp(digest.data(), &sha_256_test[1].digest[0], 32), 0);
}

TEST(openssl, sha256Exi) {
    openssl::sha_256_digest_t digest;
    EXPECT_TRUE(openssl::sha_256(&iso_exi_a[0], sizeof(iso_exi_a), digest));
    EXPECT_EQ(std::memcmp(digest.data(), &iso_exi_a_hash[0], 32), 0);

    EXPECT_TRUE(openssl::sha_256(&iso_exi_b[0], sizeof(iso_exi_b), digest));
    EXPECT_EQ(std::memcmp(digest.data(), &iso_exi_b_hash[0], 32), 0);
}

TEST(openssl, signVerify) {
    auto pkey = openssl::load_private_key("server_priv.pem", nullptr);
    ASSERT_TRUE(pkey);

    std::array<std::uint8_t, 256> sig_der{};
    std::size_t sig_der_len{sig_der.size()};
    openssl::sha_256_digest_t digest;
    EXPECT_TRUE(openssl::sha_256(&sign_test[0], openssl::sha_256_digest_size, digest));

    EXPECT_TRUE(openssl::sign(pkey.get(), sig_der.data(), sig_der_len, digest.data(), digest.size()));
    // std::cout << "signature size: " << sig_der_len << std::endl;
    EXPECT_TRUE(openssl::verify(pkey.get(), sig_der.data(), sig_der_len, digest.data(), digest.size()));
}

TEST(openssl, signVerifyBn) {
    auto pkey = openssl::load_private_key("server_priv.pem", nullptr);
    ASSERT_TRUE(pkey);

    openssl::bn_t r;
    openssl::bn_t s;

    openssl::sha_256_digest_t digest;
    EXPECT_TRUE(openssl::sha_256(&sign_test[0], openssl::sha_256_digest_size, digest));

    EXPECT_TRUE(openssl::sign(pkey.get(), r, s, digest));
    // std::cout << "signature size: " << sig_der_len << std::endl;
    EXPECT_TRUE(openssl::verify(pkey.get(), r, s, digest));
}

TEST(openssl, signVerifyMix) {
    auto pkey = openssl::load_private_key("server_priv.pem", nullptr);
    ASSERT_TRUE(pkey);

    std::array<std::uint8_t, 80> sig_der;
    std::size_t sig_der_len{sig_der.size()};
    openssl::sha_256_digest_t digest;
    EXPECT_TRUE(openssl::sha_256(&sign_test[0], openssl::sha_256_digest_size, digest));

    EXPECT_TRUE(openssl::sign(pkey.get(), sig_der.data(), sig_der_len, digest.data(), digest.size()));

    openssl::bn_t r;
    openssl::bn_t s;
    EXPECT_TRUE(openssl::signature_to_bn(r, s, sig_der.data(), sig_der_len));
    EXPECT_TRUE(openssl::verify(pkey.get(), r, s, digest));
}

TEST(openssl, signVerifyFail) {
    auto pkey = openssl::load_private_key("server_priv.pem", nullptr);
    ASSERT_TRUE(pkey);
    auto pkey_inv = openssl::load_private_key("client_priv.pem", nullptr);
    ASSERT_TRUE(pkey);

    std::array<std::uint8_t, 256> sig_der;
    std::size_t sig_der_len{sig_der.size()};
    openssl::sha_256_digest_t digest;
    EXPECT_TRUE(openssl::sha_256(&sign_test[0], openssl::sha_256_digest_size, digest));

    EXPECT_TRUE(openssl::sign(pkey.get(), sig_der.data(), sig_der_len, digest.data(), digest.size()));
    // std::cout << "signature size: " << sig_der_len << std::endl;
    EXPECT_FALSE(openssl::verify(pkey_inv.get(), sig_der.data(), sig_der_len, digest.data(), digest.size()));
}

TEST(openssl, verifyIso) {
    auto pkey = openssl::load_private_key("iso_priv.pem", nullptr);
    ASSERT_TRUE(pkey);

    auto sig = openssl::bn_to_signature(&iso_exi_sig[0], &iso_exi_sig[32]);
    EXPECT_TRUE(openssl::verify(pkey.get(), sig.get(), sig.size(), &iso_exi_b_hash[0], sizeof(iso_exi_b_hash)));
}

TEST(certificateLoad, single) {
    auto certs = ::openssl::load_certificates("server_cert.pem");
    EXPECT_EQ(certs.size(), 1);
}

TEST(certificateLoad, chain) {
    auto certs = ::openssl::load_certificates("server_chain.pem");
    EXPECT_EQ(certs.size(), 2);
}

TEST(certificateLoad, key) {
    auto certs = ::openssl::load_certificates("server_priv.pem");
    EXPECT_EQ(certs.size(), 0);
}

TEST(certificateLoad, missing) {
    auto certs = ::openssl::load_certificates("server_priv.pem-not-found");
    EXPECT_EQ(certs.size(), 0);
}

TEST(certificateLoad, nullptr) {
    auto certs = ::openssl::load_certificates(nullptr);
    EXPECT_EQ(certs.size(), 0);
}

TEST(certificateLoad, empty) {
    auto certs = ::openssl::load_certificates("");
    EXPECT_EQ(certs.size(), 0);
}

TEST(certificateLoad, multiFile) {
    std::vector<const char*> files{"server_chain.pem", "alt_server_chain.pem"};

    auto certs = ::openssl::load_certificates(files);
    ASSERT_EQ(certs.size(), 4);

    auto server = ::openssl::load_certificates("server_cert.pem");
    auto ca = ::openssl::load_certificates("server_ca_cert.pem");
    auto alt_server = ::openssl::load_certificates("alt_server_cert.pem");
    auto alt_ca = ::openssl::load_certificates("alt_server_ca_cert.pem");

    EXPECT_EQ(certs[0], server[0]);
    EXPECT_EQ(certs[1], ca[0]);
    EXPECT_EQ(certs[2], alt_server[0]);
    EXPECT_EQ(certs[3], alt_ca[0]);
}

TEST(certificateLoad, multiFileEmpty) {
    std::vector<const char*> files{};

    auto certs = ::openssl::load_certificates(files);
    ASSERT_EQ(certs.size(), 0);
}

TEST(certificateLoad, multiFileMissing) {
    std::vector<const char*> files{"server_chain.pem", "alt_server_chain.pem-not-found"};

    auto certs = ::openssl::load_certificates(files);
    ASSERT_EQ(certs.size(), 2);

    auto server = ::openssl::load_certificates("server_cert.pem");
    auto ca = ::openssl::load_certificates("server_ca_cert.pem");

    EXPECT_EQ(certs[0], server[0]);
    EXPECT_EQ(certs[1], ca[0]);
}

TEST(certificateLoad, multiFileNullptr) {
    std::vector<const char*> files{nullptr, "alt_server_chain.pem"};

    auto certs = ::openssl::load_certificates(files);
    ASSERT_EQ(certs.size(), 2);

    auto alt_server = ::openssl::load_certificates("alt_server_cert.pem");
    auto alt_ca = ::openssl::load_certificates("alt_server_ca_cert.pem");

    EXPECT_EQ(certs[0], alt_server[0]);
    EXPECT_EQ(certs[1], alt_ca[0]);
}

TEST(certificateLoadPki, none) {
    auto certs = ::openssl::load_certificates(nullptr, nullptr, nullptr);
    EXPECT_EQ(certs.leaf, nullptr);
    EXPECT_EQ(certs.chain.size(), 0);
    EXPECT_EQ(certs.trust_anchors.size(), 0);

    certs = ::openssl::load_certificates("server_cert.pem", nullptr, nullptr);
    EXPECT_EQ(certs.leaf, nullptr);
    EXPECT_EQ(certs.chain.size(), 0);
    EXPECT_EQ(certs.trust_anchors.size(), 0);

    certs = ::openssl::load_certificates(nullptr, "server_chain.pem", nullptr);
    EXPECT_EQ(certs.leaf, nullptr);
    EXPECT_EQ(certs.chain.size(), 0);
    EXPECT_EQ(certs.trust_anchors.size(), 0);

    certs = ::openssl::load_certificates(nullptr, nullptr, "server_root_cert.pem");
    EXPECT_EQ(certs.leaf, nullptr);
    EXPECT_EQ(certs.chain.size(), 0);
    EXPECT_EQ(certs.trust_anchors.size(), 0);
}

TEST(certificateLoadPki, full) {
    auto certs = ::openssl::load_certificates("server_cert.pem", "server_ca_cert.pem", "server_root_cert.pem");

    auto server = ::openssl::load_certificates("server_cert.pem");
    ASSERT_EQ(server.size(), 1);
    auto chain = ::openssl::load_certificates("server_ca_cert.pem");
    ASSERT_EQ(chain.size(), 1);
    auto trust_anchors = ::openssl::load_certificates("server_root_cert.pem");
    ASSERT_EQ(trust_anchors.size(), 1);

    ASSERT_NE(certs.leaf, nullptr);
    EXPECT_EQ(certs.leaf, server[0]);
    ASSERT_EQ(certs.chain.size(), 1);
    ASSERT_EQ(certs.trust_anchors.size(), 1);

    EXPECT_EQ(certs.chain, chain);
    EXPECT_EQ(certs.trust_anchors, trust_anchors);
}

TEST(certificateLoadPki, noLeaf) {
    // should work since leaf is 1st certificate in server_chain.pem
    auto certs = ::openssl::load_certificates(nullptr, "server_chain.pem", "server_root_cert.pem");

    auto server = ::openssl::load_certificates("server_cert.pem");
    ASSERT_EQ(server.size(), 1);
    auto chain = ::openssl::load_certificates("server_ca_cert.pem");
    ASSERT_EQ(chain.size(), 1);
    auto trust_anchors = ::openssl::load_certificates("server_root_cert.pem");
    ASSERT_EQ(trust_anchors.size(), 1);

    EXPECT_EQ(certs.leaf, server[0]);
    ASSERT_EQ(certs.chain.size(), 1);
    ASSERT_EQ(certs.trust_anchors.size(), 1);

    EXPECT_EQ(certs.chain, chain);
    EXPECT_EQ(certs.trust_anchors, trust_anchors);
}

TEST(certificateLoadPki, invalid) {
    auto certs = ::openssl::load_certificates("client_cert.pem", "server_ca_cert.pem", "server_root_cert.pem");
    EXPECT_EQ(certs.leaf, nullptr);
    EXPECT_EQ(certs.chain.size(), 0);
    EXPECT_EQ(certs.trust_anchors.size(), 0);

    certs = ::openssl::load_certificates("server_cert.pem", "client_ca_cert.pem", "server_root_cert.pem");
    EXPECT_EQ(certs.leaf, nullptr);
    EXPECT_EQ(certs.chain.size(), 0);
    EXPECT_EQ(certs.trust_anchors.size(), 0);

    certs = ::openssl::load_certificates("server_cert.pem", "server_ca_cert.pem", "client_root_cert.pem");
    EXPECT_EQ(certs.leaf, nullptr);
    EXPECT_EQ(certs.chain.size(), 0);
    EXPECT_EQ(certs.trust_anchors.size(), 0);

    certs = ::openssl::load_certificates(nullptr, "server_chain.pem", "client_root_cert.pem");
    EXPECT_EQ(certs.leaf, nullptr);
    EXPECT_EQ(certs.chain.size(), 0);
    EXPECT_EQ(certs.trust_anchors.size(), 0);
}

TEST(certificate, toPem) {
    auto certs = ::openssl::load_certificates("client_ca_cert.pem");
    ASSERT_EQ(certs.size(), 1);
    auto pem = ::openssl::certificate_to_pem(certs[0].get());
    EXPECT_FALSE(pem.empty());
    // std::cout << pem << std::endl;
}

TEST(certificate, verify) {
    auto client = ::openssl::load_certificates("client_cert.pem");
    auto chain = ::openssl::load_certificates("client_chain.pem");
    auto root = ::openssl::load_certificates("client_root_cert.pem");

    ASSERT_EQ(client.size(), 1);
    EXPECT_GT(chain.size(), 0);
    EXPECT_EQ(root.size(), 1);

    EXPECT_EQ(::openssl::verify_certificate(client[0].get(), root, chain), openssl::verify_result_t::Verified);
}

TEST(certificate, verifyCross) {
    auto client = ::openssl::load_certificates("server_cert.pem");
    auto chain = ::openssl::load_certificates("cross_ca_cert.pem");
    auto root = ::openssl::load_certificates("client_root_cert.pem");

    ASSERT_EQ(client.size(), 1);
    EXPECT_GT(chain.size(), 0);
    EXPECT_EQ(root.size(), 1);

    EXPECT_EQ(::openssl::verify_certificate(client[0].get(), root, chain), openssl::verify_result_t::Verified);
}

TEST(certificate, verifyRemoveClientFromChain) {
    auto client = ::openssl::load_certificates("client_cert.pem");
    auto chain = ::openssl::load_certificates("client_chain.pem");
    auto root = ::openssl::load_certificates("client_root_cert.pem");

    ASSERT_EQ(client.size(), 1);
    EXPECT_GT(chain.size(), 0);
    EXPECT_EQ(root.size(), 1);

    // client certificate is 1st in the list
    openssl::certificate_list new_chain;
    for (auto itt = std::next(chain.begin()); itt != chain.end(); itt++) {
        new_chain.push_back(std::move(*itt));
    }

    EXPECT_EQ(::openssl::verify_certificate(client[0].get(), root, new_chain), openssl::verify_result_t::Verified);
}

TEST(certificate, verifyNoClient) {
    // client certificate is in the chain
    auto chain = ::openssl::load_certificates("client_chain.pem");
    auto root = ::openssl::load_certificates("client_root_cert.pem");

    EXPECT_GT(chain.size(), 0);
    EXPECT_EQ(root.size(), 1);

    EXPECT_EQ(::openssl::verify_certificate(nullptr, root, chain), openssl::verify_result_t::Verified);
}

TEST(certificate, verifyFailWrongClient) {
    auto client = ::openssl::load_certificates("server_cert.pem");
    auto chain = ::openssl::load_certificates("client_chain.pem");
    auto root = ::openssl::load_certificates("client_root_cert.pem");

    ASSERT_EQ(client.size(), 1);
    EXPECT_GT(chain.size(), 0);
    EXPECT_EQ(root.size(), 1);

    EXPECT_NE(::openssl::verify_certificate(client[0].get(), root, chain), openssl::verify_result_t::Verified);
}

TEST(certificate, verifyFailWrongRoot) {
    auto client = ::openssl::load_certificates("client_cert.pem");
    auto chain = ::openssl::load_certificates("client_chain.pem");
    auto root = ::openssl::load_certificates("server_root_cert.pem");

    ASSERT_EQ(client.size(), 1);
    EXPECT_GT(chain.size(), 0);
    EXPECT_EQ(root.size(), 1);

    EXPECT_NE(::openssl::verify_certificate(client[0].get(), root, chain), openssl::verify_result_t::Verified);
}

TEST(certificate, verifyFailWrongChain) {
    auto client = ::openssl::load_certificates("client_cert.pem");
    auto chain = ::openssl::load_certificates("server_chain.pem");
    auto root = ::openssl::load_certificates("client_root_cert.pem");

    ASSERT_EQ(client.size(), 1);
    EXPECT_GT(chain.size(), 0);
    EXPECT_EQ(root.size(), 1);

    EXPECT_NE(::openssl::verify_certificate(client[0].get(), root, chain), openssl::verify_result_t::Verified);
}

TEST(certificate, subjectName) {
    auto chain = ::openssl::load_certificates("client_chain.pem");
    EXPECT_GT(chain.size(), 0);

    for (const auto& cert : chain) {
        auto subject = ::openssl::certificate_subject(cert.get());
        EXPECT_GT(subject.size(), 0);
    }
}

TEST(certificateChainInfo, valid) {
    auto chain = openssl::load_certificates("client_cert.pem", "client_ca_cert.pem", "client_root_cert.pem");
    EXPECT_TRUE(openssl::verify_chain(chain));
}

TEST(certificateChainInfo, invalid) {
    auto chain = openssl::load_certificates("server_cert.pem", "client_ca_cert.pem", "client_root_cert.pem");
    EXPECT_FALSE(openssl::verify_chain(chain));
    chain = openssl::load_certificates("client_cert.pem", "server_ca_cert.pem", "client_root_cert.pem");
    EXPECT_FALSE(openssl::verify_chain(chain));
    chain = openssl::load_certificates("client_cert.pem", "client_ca_cert.pem", "server_root_cert.pem");
    EXPECT_FALSE(openssl::verify_chain(chain));
}

TEST(certificateChain, valid) {
    auto pkey = openssl::load_private_key("client_priv.pem", nullptr);
    auto chain_info = openssl::load_certificates("client_cert.pem", "client_ca_cert.pem", "client_root_cert.pem");
    openssl::chain_t chain = {std::move(chain_info), std::move(pkey)};
    EXPECT_TRUE(openssl::verify_chain(chain));
}

TEST(certificateChain, invalid) {
    auto pkey = openssl::load_private_key("server_priv.pem", nullptr);
    auto chain_info = openssl::load_certificates("client_cert.pem", "client_ca_cert.pem", "client_root_cert.pem");
    openssl::chain_t chain = {std::move(chain_info), std::move(pkey)};
    EXPECT_FALSE(openssl::verify_chain(chain));
    chain_info = openssl::load_certificates("server_cert.pem", "client_ca_cert.pem", "client_root_cert.pem");
    chain.chain = std::move(chain_info);
    EXPECT_FALSE(openssl::verify_chain(chain));
    chain_info = openssl::load_certificates("client_cert.pem", "server_ca_cert.pem", "client_root_cert.pem");
    chain.chain = std::move(chain_info);
    EXPECT_FALSE(openssl::verify_chain(chain));
    chain_info = openssl::load_certificates("client_cert.pem", "client_ca_cert.pem", "server_root_cert.pem");
    chain.chain = std::move(chain_info);
    EXPECT_FALSE(openssl::verify_chain(chain));
}

TEST(certificate, apply) {
    auto pkey = openssl::load_private_key("client_priv.pem", nullptr);
    auto chain_info = openssl::load_certificates("client_cert.pem", "client_ca_cert.pem", "client_root_cert.pem");
    openssl::chain_t chain = {std::move(chain_info), std::move(pkey)};
    auto* ctx = SSL_CTX_new(TLS_server_method());
    auto* ssl = SSL_new(ctx);
    EXPECT_TRUE(openssl::use_certificate_and_key(ssl, chain));
    SSL_free(ssl);
    SSL_CTX_free(ctx);
}

} // namespace
