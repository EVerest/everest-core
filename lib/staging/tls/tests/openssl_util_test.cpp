// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include "gtest/gtest.h"
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl_util.hpp>

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

// Test vectors from ISO 15118-2 Section J.2
// checked okay (see iso_priv.pem)
constexpr std::uint8_t iso_private_key[] = {0xb9, 0x13, 0x49, 0x63, 0xf5, 0x1c, 0x44, 0x14, 0x73, 0x84, 0x35,
                                            0x05, 0x7f, 0x97, 0xbb, 0xf1, 0x01, 0x0c, 0xab, 0xcb, 0x8d, 0xbd,
                                            0xe9, 0xc5, 0xd4, 0x81, 0x38, 0x39, 0x6a, 0xa9, 0x4b, 0x9d};
// checked okay (see iso_priv.pem)
constexpr std::uint8_t iso_public_key[] = {0x43, 0xe4, 0xfc, 0x4c, 0xcb, 0x64, 0x39, 0x04, 0x27, 0x9c, 0x7a, 0x5e, 0x65,
                                           0x76, 0xb3, 0x23, 0xe5, 0x5e, 0xc7, 0x9f, 0xf0, 0xe5, 0xa4, 0x05, 0x6e, 0x33,
                                           0x40, 0x84, 0xcb, 0xc3, 0x36, 0xff, 0x46, 0xe4, 0x4c, 0x1a, 0xdd, 0xf6, 0x91,
                                           0x62, 0xe5, 0x19, 0x2c, 0x2a, 0x83, 0xfc, 0x2b, 0xca, 0x9d, 0x8f, 0x46, 0xec,
                                           0xf4, 0xb7, 0x80, 0x67, 0xc2, 0x47, 0x6f, 0x6b, 0x3f, 0x34, 0x60, 0x0e};

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
    auto* bio = BIO_new_file("server_priv.pem", "r");
    ASSERT_NE(bio, nullptr);
    auto* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    ASSERT_NE(pkey, nullptr);

    std::array<std::uint8_t, 256> sig_der{};
    std::size_t sig_der_len{sig_der.size()};
    openssl::sha_256_digest_t digest;
    EXPECT_TRUE(openssl::sha_256(&sign_test[0], openssl::sha_256_digest_size, digest));

    EXPECT_TRUE(openssl::sign(pkey, sig_der.data(), sig_der_len, digest.data(), digest.size()));
    EXPECT_TRUE(openssl::verify(pkey, sig_der.data(), sig_der_len, digest.data(), digest.size()));

    BIO_free(bio);
    EVP_PKEY_free(pkey);
}

TEST(openssl, signVerifyBn) {
    auto* bio = BIO_new_file("server_priv.pem", "r");
    ASSERT_NE(bio, nullptr);
    auto* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    ASSERT_NE(pkey, nullptr);

    openssl::bn_t r;
    openssl::bn_t s;

    openssl::sha_256_digest_t digest;
    EXPECT_TRUE(openssl::sha_256(&sign_test[0], openssl::sha_256_digest_size, digest));

    EXPECT_TRUE(openssl::sign(pkey, r, s, digest));
    EXPECT_TRUE(openssl::verify(pkey, r, s, digest));

    BIO_free(bio);
    EVP_PKEY_free(pkey);
}

TEST(openssl, signVerifyMix) {
    auto* bio = BIO_new_file("server_priv.pem", "r");
    ASSERT_NE(bio, nullptr);
    auto* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    ASSERT_NE(pkey, nullptr);

    std::array<std::uint8_t, 80> sig_der;
    std::size_t sig_der_len{sig_der.size()};
    openssl::sha_256_digest_t digest;
    EXPECT_TRUE(openssl::sha_256(&sign_test[0], openssl::sha_256_digest_size, digest));

    EXPECT_TRUE(openssl::sign(pkey, sig_der.data(), sig_der_len, digest.data(), digest.size()));

    openssl::bn_t r;
    openssl::bn_t s;
    EXPECT_TRUE(openssl::signature_to_bn(r, s, sig_der.data(), sig_der_len));
    EXPECT_TRUE(openssl::verify(pkey, r, s, digest));

    BIO_free(bio);
    EVP_PKEY_free(pkey);
}

TEST(openssl, signVerifyFail) {
    auto bio = BIO_new_file("server_priv.pem", "r");
    ASSERT_NE(bio, nullptr);
    auto* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    ASSERT_NE(pkey, nullptr);
    BIO_free(bio);

    bio = BIO_new_file("client_priv.pem", "r");
    ASSERT_NE(bio, nullptr);
    auto* pkey_inv = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    ASSERT_NE(pkey, nullptr);
    BIO_free(bio);

    std::array<std::uint8_t, 256> sig_der;
    std::size_t sig_der_len{sig_der.size()};
    openssl::sha_256_digest_t digest;
    EXPECT_TRUE(openssl::sha_256(&sign_test[0], openssl::sha_256_digest_size, digest));

    EXPECT_TRUE(openssl::sign(pkey, sig_der.data(), sig_der_len, digest.data(), digest.size()));
    EXPECT_FALSE(openssl::verify(pkey_inv, sig_der.data(), sig_der_len, digest.data(), digest.size()));

    EVP_PKEY_free(pkey);
    EVP_PKEY_free(pkey_inv);
}

TEST(openssl, verifyIso) {
    auto* bio = BIO_new_file("iso_priv.pem", "r");
    ASSERT_NE(bio, nullptr);
    auto* pkey = PEM_read_bio_PrivateKey(bio, nullptr, nullptr, nullptr);
    ASSERT_NE(pkey, nullptr);
    BIO_free(bio);

    auto [sig, siglen] = openssl::bn_to_signature(&iso_exi_sig[0], &iso_exi_sig[32]);
    EXPECT_TRUE(openssl::verify(pkey, sig.get(), siglen, &iso_exi_b_hash[0], sizeof(iso_exi_b_hash)));
    EVP_PKEY_free(pkey);
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

    EXPECT_EQ(::openssl::verify_certificate(client[0].get(), root, chain), openssl::verify_result_t::verified);
}

TEST(certificate, verifyRemoveClientFromChain) {
    auto client = ::openssl::load_certificates("client_cert.pem");
    auto chain = ::openssl::load_certificates("client_chain.pem");
    auto root = ::openssl::load_certificates("client_root_cert.pem");

    ASSERT_EQ(client.size(), 1);
    EXPECT_GT(chain.size(), 0);
    EXPECT_EQ(root.size(), 1);

    // client certificate is 1st in the list
    openssl::CertificateList new_chain;
    for (auto itt = std::next(chain.begin()); itt != chain.end(); itt++) {
        new_chain.push_back(std::move(*itt));
    }

    EXPECT_EQ(::openssl::verify_certificate(client[0].get(), root, new_chain), openssl::verify_result_t::verified);
}

TEST(certificate, verifyNoClient) {
    // client certificate is in the chain
    auto chain = ::openssl::load_certificates("client_chain.pem");
    auto root = ::openssl::load_certificates("client_root_cert.pem");

    EXPECT_GT(chain.size(), 0);
    EXPECT_EQ(root.size(), 1);

    EXPECT_EQ(::openssl::verify_certificate(nullptr, root, chain), openssl::verify_result_t::verified);
}

TEST(certificate, verifyFailWrongClient) {
    auto client = ::openssl::load_certificates("server_cert.pem");
    auto chain = ::openssl::load_certificates("client_chain.pem");
    auto root = ::openssl::load_certificates("client_root_cert.pem");

    ASSERT_EQ(client.size(), 1);
    EXPECT_GT(chain.size(), 0);
    EXPECT_EQ(root.size(), 1);

    EXPECT_NE(::openssl::verify_certificate(client[0].get(), root, chain), openssl::verify_result_t::verified);
}

TEST(certificate, verifyFailWrongRoot) {
    auto client = ::openssl::load_certificates("client_cert.pem");
    auto chain = ::openssl::load_certificates("client_chain.pem");
    auto root = ::openssl::load_certificates("server_root_cert.pem");

    ASSERT_EQ(client.size(), 1);
    EXPECT_GT(chain.size(), 0);
    EXPECT_EQ(root.size(), 1);

    EXPECT_NE(::openssl::verify_certificate(client[0].get(), root, chain), openssl::verify_result_t::verified);
}

TEST(certificate, verifyFailWrongChain) {
    auto client = ::openssl::load_certificates("client_cert.pem");
    auto chain = ::openssl::load_certificates("server_chain.pem");
    auto root = ::openssl::load_certificates("client_root_cert.pem");

    ASSERT_EQ(client.size(), 1);
    EXPECT_GT(chain.size(), 0);
    EXPECT_EQ(root.size(), 1);

    EXPECT_NE(::openssl::verify_certificate(client[0].get(), root, chain), openssl::verify_result_t::verified);
}

TEST(certificate, subjectName) {
    auto chain = ::openssl::load_certificates("client_chain.pem");
    EXPECT_GT(chain.size(), 0);

    for (const auto& cert : chain) {
        auto subject = ::openssl::certificate_subject(cert.get());
        EXPECT_GT(subject.size(), 0);
#if 0
        for (const auto& itt : subject) {
            std::cout << itt.first << ": " << itt.second << std::endl;
        }
#endif
    }
}

} // namespace
