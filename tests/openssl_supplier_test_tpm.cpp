#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <iostream>

#include <evse_security/crypto/openssl/openssl_supplier.hpp>
#include <evse_security/crypto/openssl/openssl_tpm.hpp>

using namespace evse_security;

namespace {

static std::string getFile(const std::string name) {
    std::ifstream file(name);
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

class OpenSSLSupplierTpmTest : public testing::Test {
protected:
    static void SetUpTestSuite() {
        std::system("./create-pki.sh tpm");
    }
};

TEST_F(OpenSSLSupplierTpmTest, supports_tpm) {
    OpenSSLProvider::cleanup();
    ASSERT_FALSE(OpenSSLProvider::supports_tpm());
    // calculates
    OpenSSLProvider provider;
    // returns cached
    ASSERT_TRUE(OpenSSLProvider::supports_tpm());
}

TEST_F(OpenSSLSupplierTpmTest, supports_tpm_key_creation) {
    OpenSSLProvider::cleanup();
    ASSERT_FALSE(OpenSSLProvider::supports_tpm());
    // should calculate
    ASSERT_TRUE(OpenSSLSupplier::supports_tpm_key_creation());
}

TEST_F(OpenSSLSupplierTpmTest, generate_key_RSA_TPM20) {
    KeyGenerationInfo info = {
        CryptoKeyType::RSA_TPM20, true, std::nullopt, std::nullopt, std::nullopt,
    };
    KeyHandle_ptr key;
    auto res = OpenSSLSupplier::generate_key(info, key);
    ASSERT_TRUE(res);
}

TEST_F(OpenSSLSupplierTpmTest, generate_key_RSA_3072) {
    // Enable this test manually only if your platform supports 3072 TPM keys
    GTEST_SKIP() << "Skipping TPM2.0 GEN_RSA_3072 test since it is a non-spec value"
                    "which probably will not be supported on many platforms!";

    KeyGenerationInfo info = {
        CryptoKeyType::RSA_3072, true, std::nullopt, std::nullopt, std::nullopt,
    };
    KeyHandle_ptr key;
    auto res = OpenSSLSupplier::generate_key(info, key);
    ASSERT_TRUE(res);
}

TEST_F(OpenSSLSupplierTpmTest, generate_key_EC_prime256v1) {
    KeyGenerationInfo info = {
        CryptoKeyType::EC_prime256v1, true, std::nullopt, std::nullopt, std::nullopt,
    };
    KeyHandle_ptr key;
    auto res = OpenSSLSupplier::generate_key(info, key);
    ASSERT_TRUE(res);
}

TEST_F(OpenSSLSupplierTpmTest, generate_key_EC_EC_secp384r1) {
    KeyGenerationInfo info = {
        CryptoKeyType::EC_secp384r1, true, std::nullopt, std::nullopt, std::nullopt,
    };
    KeyHandle_ptr key;
    auto res = OpenSSLSupplier::generate_key(info, key);
    ASSERT_TRUE(res);
}

TEST_F(OpenSSLSupplierTpmTest, load_certificates) {
    auto file = getFile("tpm_pki/cert_path.pem");
    auto res = OpenSSLSupplier::load_certificates(file, EncodingFormat::PEM);
    ASSERT_EQ(res.size(), 2);
}

TEST_F(OpenSSLSupplierTpmTest, x509_check_private_key) {
    auto cert_leaf = getFile("tpm_pki/server_cert.pem");
    auto res_leaf = OpenSSLSupplier::load_certificates(cert_leaf, EncodingFormat::PEM);
    auto cert = res_leaf[0].get();
    auto key = getFile("tpm_pki/server_priv.pem");
    auto res = OpenSSLSupplier::x509_check_private_key(cert, key, std::nullopt);
    ASSERT_EQ(res, KeyValidationResult::Valid);
}

TEST_F(OpenSSLSupplierTpmTest, x509_verify_certificate_chain) {
    auto cert_path = getFile("tpm_pki/cert_path.pem");
    auto cert_leaf = getFile("tpm_pki/server_cert.pem");

    auto res_path = OpenSSLSupplier::load_certificates(cert_path, EncodingFormat::PEM);
    auto res_leaf = OpenSSLSupplier::load_certificates(cert_leaf, EncodingFormat::PEM);

    std::vector<X509Handle*> parents;

    for (auto& i : res_path) {
        parents.push_back(i.get());
    }

    auto res = OpenSSLSupplier::x509_verify_certificate_chain(res_leaf[0].get(), parents, {}, true, std::nullopt,
                                                              "tpm_pki/root_cert.pem");
    ASSERT_EQ(res, CertificateValidationResult::Valid);
}

TEST_F(OpenSSLSupplierTpmTest, x509_generate_csr) {
    std::string csr;
    CertificateSigningRequestInfo csr_info = {
        0,
        "UK",
        "Pionix",
        "0123456789",
        .dns_name = std::nullopt,
        .ip_address = std::nullopt,
        {CryptoKeyType::EC_prime256v1, true, std::nullopt, "tpm_pki/csr_key.tkey", std::nullopt}};

    // std::cout << "tpm2 pre: " << OSSL_PROVIDER_available(nullptr, "tpm2") << std::endl;
    // std::cout << "base pre: " << OSSL_PROVIDER_available(nullptr, "base") << std::endl;
    auto res = OpenSSLSupplier::x509_generate_csr(csr_info, csr);
    // std::cout << "tpm2 post: " << OSSL_PROVIDER_available(nullptr, "tpm2") << std::endl;
    // std::cout << "base post: " << OSSL_PROVIDER_available(nullptr, "base") << std::endl;

    ASSERT_EQ(res, CertificateSignRequestResult::Valid);
    ASSERT_GT(csr.size(), 0);
}

TEST_F(OpenSSLSupplierTpmTest, x509_generate_csr2) {
    std::string csr;
    CertificateSigningRequestInfo csr_info = {
        0,
        "UK",
        "Pionix",
        "0123456789",
        .dns_name = std::nullopt,
        .ip_address = std::nullopt,
        {CryptoKeyType::RSA_TPM20, true, std::nullopt, "tpm_pki/csr_key.tkey", std::nullopt}};

    auto res = OpenSSLSupplier::x509_generate_csr(csr_info, csr);

    ASSERT_EQ(res, CertificateSignRequestResult::Valid);
    ASSERT_GT(csr.size(), 0);
}

} // namespace
