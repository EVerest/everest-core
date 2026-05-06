#include <cstdlib>
#include <fstream>
#include <gtest/gtest.h>

#include <evse_security/certificate/x509_hierarchy.hpp>
#include <evse_security/certificate/x509_wrapper.hpp>
#include <evse_security/crypto/openssl/openssl_crypto_supplier.hpp>
#include <optional>

// #define OUTPUT_CSR

using namespace evse_security;

namespace {

static std::string getFile(const std::string name) {
    std::ifstream file(name);
    return std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
}

class OpenSSLSupplierTest : public testing::Test {
protected:
    static void SetUpTestSuite() {
        std::system("./create-pki.sh");
        std::system("./create-pki-critical-ski.sh");
    }
};

TEST_F(OpenSSLSupplierTest, generate_key_RSA_TPM20) {
    KeyGenerationInfo info = {
        CryptoKeyType::RSA_TPM20, false, std::nullopt, std::nullopt, std::nullopt,
    };
    KeyHandle_ptr key;
    auto res = OpenSSLSupplier::generate_key(info, key);
    ASSERT_TRUE(res);
}

TEST_F(OpenSSLSupplierTest, generate_key_RSA_3072) {
    KeyGenerationInfo info = {
        CryptoKeyType::RSA_3072, false, std::nullopt, std::nullopt, std::nullopt,
    };
    KeyHandle_ptr key;
    auto res = OpenSSLSupplier::generate_key(info, key);
    ASSERT_TRUE(res);
}

TEST_F(OpenSSLSupplierTest, generate_key_EC_prime256v1) {
    KeyGenerationInfo info = {
        CryptoKeyType::EC_prime256v1, false, std::nullopt, std::nullopt, std::nullopt,
    };
    KeyHandle_ptr key;
    auto res = OpenSSLSupplier::generate_key(info, key);
    ASSERT_TRUE(res);
}

TEST_F(OpenSSLSupplierTest, generate_key_EC_EC_secp384r1) {
    KeyGenerationInfo info = {
        CryptoKeyType::EC_secp384r1, false, std::nullopt, std::nullopt, std::nullopt,
    };
    KeyHandle_ptr key;
    auto res = OpenSSLSupplier::generate_key(info, key);
    ASSERT_TRUE(res);
}

TEST_F(OpenSSLSupplierTest, load_certificates) {
    auto file = getFile("pki/cert_path.pem");
    auto res = OpenSSLSupplier::load_certificates(file, EncodingFormat::PEM);
    ASSERT_EQ(res.size(), 2);
}

TEST_F(OpenSSLSupplierTest, x509_check_private_key) {
    auto cert_leaf = getFile("pki/server_cert.pem");
    auto res_leaf = OpenSSLSupplier::load_certificates(cert_leaf, EncodingFormat::PEM);
    auto cert = res_leaf[0].get();
    auto key = getFile("pki/server_priv.pem");
    auto res = OpenSSLSupplier::x509_check_private_key(cert, key, std::nullopt);
    ASSERT_TRUE(res == KeyValidationResult::Valid);
}

TEST_F(OpenSSLSupplierTest, x509_verify_certificate_chain) {
    auto cert_path = getFile("pki/cert_path.pem");
    auto cert_leaf = getFile("pki/server_cert.pem");

    auto res_path = OpenSSLSupplier::load_certificates(cert_path, EncodingFormat::PEM);
    auto res_leaf = OpenSSLSupplier::load_certificates(cert_leaf, EncodingFormat::PEM);

    std::vector<X509Handle*> parents;
    std::vector<X509Handle*> empty_untrusted;

    for (auto& i : res_path) {
        parents.push_back(i.get());
    }

    auto res = OpenSSLSupplier::x509_verify_certificate_chain(res_leaf[0].get(), parents, empty_untrusted, true,
                                                              std::nullopt, "pki/root_cert.pem");
    ASSERT_EQ(res, CertificateValidationResult::Valid);
}

TEST_F(OpenSSLSupplierTest, x509_verify_certificate_chain_ignore_unhandled_critical_extensions) {
    // Load a certificate chain where both the CA and the leaf have the Subject Key
    // Identifier extension marked critical (non-RFC-5280-compliant). All critical
    // extensions have well-known RFC 5280 NIDs, so the
    // ignore_unhandled_critical_extensions flag should allow verification.
    auto cert_path = getFile("pki_critical_ski/cert_path.pem");
    auto cert_leaf = getFile("pki_critical_ski/server_cert.pem");

    auto res_path = OpenSSLSupplier::load_certificates(cert_path, EncodingFormat::PEM);
    auto res_leaf = OpenSSLSupplier::load_certificates(cert_leaf, EncodingFormat::PEM);

    ASSERT_GE(res_path.size(), 1U);
    ASSERT_GE(res_leaf.size(), 1U);

    std::vector<X509Handle*> parents;
    std::vector<X509Handle*> empty_untrusted;

    parents.reserve(res_path.size());
    for (auto& cert : res_path) {
        parents.push_back(cert.get());
    }

    // Strict mode (default): OpenSSL must reject the chain with Unknown because
    // X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION is not mapped to a specific result.
    auto res_strict = OpenSSLSupplier::x509_verify_certificate_chain(
        res_leaf[0].get(), parents, empty_untrusted, true, std::nullopt, "pki_critical_ski/root_cert.pem", false);
    ASSERT_NE(res_strict, CertificateValidationResult::Valid)
        << "Strict mode must not accept a chain containing an unhandled critical extension";

    // Lenient mode: every critical extension is a well-known RFC 5280 NID, so the chain
    // must validate when ignore_unhandled_critical_extensions is set.
    auto res_lenient = OpenSSLSupplier::x509_verify_certificate_chain(
        res_leaf[0].get(), parents, empty_untrusted, true, std::nullopt, "pki_critical_ski/root_cert.pem", true);
    ASSERT_EQ(res_lenient, CertificateValidationResult::Valid)
        << "Lenient mode must accept a chain where all critical extensions have well-known RFC 5280 NIDs";
}

TEST_F(OpenSSLSupplierTest, x509_is_child_ignore_unhandled_critical_extensions) {
    // Parent/child pair where the parent CA has SKI marked critical (non-RFC-5280-compliant).
    // Without the bypass, X509_verify_cert inside x509_is_child fails with
    // X509_V_ERR_UNHANDLED_CRITICAL_EXTENSION, leaving the hierarchy unable to attach the child
    // to the parent at startup. With the bypass enabled (and every critical extension being a
    // well-known RFC 5280 NID), the relationship is detected.
    // PKI layout: root_cert -> ca_cert (intermediate) -> server_cert.
    auto parent_pem = getFile("pki_critical_ski/ca_cert.pem"); // intermediate — direct parent of the leaf
    auto child_pem = getFile("pki_critical_ski/server_cert.pem");

    auto parent = OpenSSLSupplier::load_certificates(parent_pem, EncodingFormat::PEM);
    auto child = OpenSSLSupplier::load_certificates(child_pem, EncodingFormat::PEM);

    ASSERT_GE(parent.size(), 1U);
    ASSERT_GE(child.size(), 1U);

    ASSERT_FALSE(OpenSSLSupplier::x509_is_child(child[0].get(), parent[0].get(), false))
        << "Strict mode must not accept child/parent links across an unhandled critical extension";

    ASSERT_TRUE(OpenSSLSupplier::x509_is_child(child[0].get(), parent[0].get(), true))
        << "Lenient mode must accept child/parent links when all critical extensions have well-known RFC 5280 NIDs";
}

TEST_F(OpenSSLSupplierTest, x509_hierarchy_build_ignore_unhandled_critical_extensions) {
    // Full end-to-end: a certificate hierarchy built from a bundle containing a non-compliant
    // root + intermediate + leaf. With the bypass off the intermediate and leaf stay orphaned
    // (no matching parent found via is_child). With the bypass on, they are correctly chained
    // underneath the self-signed root.
    const std::string root_pem = getFile("pki_critical_ski/root_cert.pem");
    const std::string intermediate_pem = getFile("pki_critical_ski/ca_cert.pem");
    const std::string leaf_pem = getFile("pki_critical_ski/server_cert.pem");

    auto build_with_flag = [&](bool ignore) {
        std::vector<X509Wrapper> certs;
        certs.emplace_back(root_pem, EncodingFormat::PEM);
        certs.emplace_back(intermediate_pem, EncodingFormat::PEM);
        certs.emplace_back(leaf_pem, EncodingFormat::PEM);
        return X509CertificateHierarchy::build_hierarchy(certs, ignore);
    };

    // Strict: no child chains under the self-signed root — the intermediate and leaf are
    // classified as orphans because is_child rejects them due to the unhandled critical extension.
    auto strict = build_with_flag(false);
    bool root_has_children_strict = false;
    for (const auto& node : strict.get_hierarchy()) {
        if (node.state.is_selfsigned && !node.children.empty()) {
            root_has_children_strict = true;
        }
    }
    ASSERT_FALSE(root_has_children_strict) << "Strict hierarchy must not chain children under the non-compliant root";

    // Lenient: root -> intermediate -> leaf must be assembled.
    auto lenient = build_with_flag(true);
    bool chain_assembled = false;
    for (const auto& node : lenient.get_hierarchy()) {
        if (!node.state.is_selfsigned) {
            continue;
        }
        for (const auto& intermediate : node.children) {
            if (!intermediate.children.empty()) {
                chain_assembled = true;
            }
        }
    }
    ASSERT_TRUE(chain_assembled)
        << "Lenient hierarchy must chain root -> intermediate -> leaf when the bypass is enabled";
}

TEST_F(OpenSSLSupplierTest, x509_generate_csr) {
    std::string csr;
    CertificateSigningRequestInfo csr_info = {
        0,
        "UK",
        "Pionix",
        "0123456789",
        .dns_name = std::nullopt,
        .ip_address = std::nullopt,
        {CryptoKeyType::EC_prime256v1, false, std::nullopt, "pki/csr_key.pem", std::nullopt}};
    auto res = OpenSSLSupplier::x509_generate_csr(csr_info, csr);
    ASSERT_EQ(res, CertificateSignRequestResult::Valid);

    std::ofstream out("csr.pem");
    out << csr;
    out.close();

    ASSERT_GT(csr.size(), 0);
}

TEST_F(OpenSSLSupplierTest, x509_generate_csr_dns) {
    std::string csr;
    CertificateSigningRequestInfo csr_info = {
        0,
        "UK",
        "Pionix",
        "0123456789",
        .dns_name = "cs.pionix.de",
        .ip_address = std::nullopt,
        {CryptoKeyType::EC_prime256v1, false, std::nullopt, "pki/csr_key.pem", std::nullopt}};
    auto res = OpenSSLSupplier::x509_generate_csr(csr_info, csr);
    ASSERT_EQ(res, CertificateSignRequestResult::Valid);

#ifdef OUTPUT_CSR
    std::ofstream out("csr_dns.pem");
    out << csr;
    out.close();
#endif

    ASSERT_GT(csr.size(), 0);
}

TEST_F(OpenSSLSupplierTest, x509_generate_csr_ip) {
    std::string csr;
    CertificateSigningRequestInfo csr_info = {
        0,
        "UK",
        "Pionix",
        "0123456789",
        .dns_name = std::nullopt,
        .ip_address = "127.0.0.1",
        {CryptoKeyType::EC_prime256v1, false, std::nullopt, "pki/csr_key.pem", std::nullopt}};
    auto res = OpenSSLSupplier::x509_generate_csr(csr_info, csr);
    ASSERT_EQ(res, CertificateSignRequestResult::Valid);

#ifdef OUTPUT_CSR
    std::ofstream out("csr_ip.pem");
    out << csr;
    out.close();
#endif

    ASSERT_GT(csr.size(), 0);
}

TEST_F(OpenSSLSupplierTest, x509_generate_csr_dns_ip) {
    std::string csr;
    CertificateSigningRequestInfo csr_info = {
        0,
        "UK",
        "Pionix",
        "0123456789",
        .dns_name = "cs.pionix.de",
        .ip_address = "127.0.0.1",
        {CryptoKeyType::EC_prime256v1, false, std::nullopt, "pki/csr_key.pem", std::nullopt}};
    auto res = OpenSSLSupplier::x509_generate_csr(csr_info, csr);
    ASSERT_EQ(res, CertificateSignRequestResult::Valid);

#ifdef OUTPUT_CSR
    std::ofstream out("csr_dns_ip.pem");
    out << csr;
    out.close();
#endif

    ASSERT_GT(csr.size(), 0);
}

} // namespace
