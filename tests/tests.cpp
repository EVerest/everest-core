// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <fstream>
#include <gtest/gtest.h>
#include <openssl/crypto.h>
#include <regex>
#include <sstream>
#include <string>
#include <thread>

#include <evse_security/certificate/x509_bundle.hpp>
#include <evse_security/certificate/x509_wrapper.hpp>
#include <evse_security/evse_security.hpp>
#include <evse_security/utils/evse_filesystem.hpp>

#include <evse_security/crypto/evse_crypto.hpp>

#include <openssl/opensslv.h>

#ifdef USING_TPM2

// updates so that existing tests run with the OpenSSLProvider
#include <evse_security/crypto/openssl/openssl_provider.hpp>
#include <openssl/provider.h>

namespace evse_security {
const char* PROVIDER_TPM = "tpm2";
const char* PROVIDER_DEFAULT = "default";
typedef OpenSSLProvider TPMScopedProvider;

} // namespace evse_security
#endif // USING_TPM2

std::string read_file_to_string(const fs::path filepath) {
    fsstd::ifstream t(filepath.string());
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

bool equal_certificate_strings(const std::string& cert1, const std::string& cert2) {
    for (int i = 0; i < cert1.length(); ++i) {
        if (i < cert1.length() && i < cert2.length()) {
            if (isalnum(cert1[i]) && isalnum(cert2[i]) && cert1[i] != cert2[i])
                return false;
        }
    }

    return true;
}

namespace evse_security {

class EvseSecurityTests : public ::testing::Test {
protected:
    std::unique_ptr<EvseSecurity> evse_security;

    void SetUp() override {
        fs::remove_all("certs");
        fs::remove_all("csr");

        install_certs();

        if (!fs::exists("key"))
            fs::create_directory("key");

        FilePaths file_paths;
        file_paths.csms_ca_bundle = fs::path("certs/ca/v2g/V2G_CA_BUNDLE.pem");
        file_paths.mf_ca_bundle = fs::path("certs/ca/v2g/V2G_CA_BUNDLE.pem");
        file_paths.mo_ca_bundle = fs::path("certs/ca/mo/MO_CA_BUNDLE.pem");
        file_paths.v2g_ca_bundle = fs::path("certs/ca/v2g/V2G_CA_BUNDLE.pem");
        file_paths.directories.csms_leaf_cert_directory = fs::path("certs/client/csms/");
        file_paths.directories.csms_leaf_key_directory = fs::path("certs/client/csms/");
        file_paths.directories.secc_leaf_cert_directory = fs::path("certs/client/cso/");
        file_paths.directories.secc_leaf_key_directory = fs::path("certs/client/cso/");

        this->evse_security = std::make_unique<EvseSecurity>(file_paths, "123456");
    }

    void TearDown() override {
        fs::remove_all("certs");
        fs::remove_all("csr");
    }

    virtual void install_certs() {
        std::system("./generate_test_certs.sh");
    }
};

class EvseSecurityTestsMulti : public EvseSecurityTests {
protected:
    void install_certs() override {
        std::system("./generate_test_certs_root_multi.sh");
    }
};

class EvseSecurityTestsMultiLeaf : public EvseSecurityTests {
protected:
    void install_certs() override {
        std::system("./generate_test_certs_leaf_multi.sh");
    }
};

class EvseSecurityTestsExpired : public EvseSecurityTests {
protected:
    static constexpr int GEN_CERTIFICATES = 30;

    std::set<fs::path> generated_bulk_certificates;

    void SetUp() override {
        EvseSecurityTests::SetUp();
        fs::remove_all("expired_bulk");

        fs::create_directory("expired_bulk");
        std::system("touch expired_bulk/index.txt");
        std::system("echo \"1000\" > expired_bulk/serial");

        // Generate many expired certificates
        int serial = 4096; // Hex 1000

        // Generate N certificates, N-5 expired, 5 non-expired
        std::time_t t = std::time(nullptr);
        std::tm* const time_info = std::localtime(&t);
        int current_year = 1900 + time_info->tm_year;

        for (int i = 0; i < GEN_CERTIFICATES; i++) {
            std::string CN = "Pionix";
            CN += std::to_string(i);

            std::vector<char> buffer;
            buffer.resize(2048);

            bool expired = (i < (GEN_CERTIFICATES - 5));
            int start_year;
            int end_year;

            if (expired) {
                start_year = (current_year - 5 - i);
                end_year = (current_year - 1 - i);
            } else {
                start_year = current_year;
                end_year = (current_year + i);
            }

            std::sprintf(
                buffer.data(),
                "openssl req -newkey rsa:512 -keyout expired_bulk/cert.key -out expired_bulk/cert.csr -nodes -subj "
                "\"/C=DE/L=Schonborn/CN=[%s]/emailAddress=email@pionix.com\"",
                CN.c_str());
            std::system(buffer.data());

            std::sprintf(
                buffer.data(),
                "openssl ca -selfsign -config expired_runtime/conf.cnf -batch -keyfile expired_bulk/cert.key -in "
                "expired_bulk/cert.csr -out expired_bulk/cert.pem -notext -startdate %d1213000000Z -enddate "
                "%d1213000000Z",
                start_year, end_year);
            std::system(buffer.data());

            // Copy certificates/keys over
            std::string cert_filename = "expired_bulk/cert.pem";
            std::string ckey_filename = "expired_bulk/cert.key";

            std::string target_cert =
                std::string(expired ? "certs/client/cso/SECC_LEAF_EXPIRED_" : "certs/client/cso/SECC_LEAF_VALID_") +
                +"st_" + std::to_string(start_year) + "_en_" + std::to_string(end_year) + ".pem";
            std::string target_ckey =
                std::string(expired ? "certs/client/cso/SECC_LEAF_EXPIRED_" : "certs/client/cso/SECC_LEAF_VALID_") +
                +"st_" + std::to_string(start_year) + "_en_" + std::to_string(end_year) + ".key";

            fs::copy(cert_filename, target_cert);
            fs::copy(ckey_filename, target_ckey);

            generated_bulk_certificates.emplace(target_cert);
            generated_bulk_certificates.emplace(target_ckey);

            fs::remove(cert_filename);
            fs::remove(ckey_filename);
        }
    }

    void TearDown() override {
        EvseSecurityTests::TearDown();

        fs::remove_all("expired_bulk");
    }
};

class EvseSecurityTestsCSMS : public ::testing::Test {
protected:
    std::unique_ptr<EvseSecurity> evse_security;

    void SetUp() override {
        fs::remove_all("csms_certs_temp");
        fs::create_directory("csms_certs_temp");
        fs::copy("csms_certs", "csms_certs_temp", fs::copy_options::recursive);

        FilePaths file_paths;
        file_paths.v2g_ca_bundle = fs::path("csms_certs_temp/ca/V2G_ROOT_CA.pem");
        file_paths.csms_ca_bundle = fs::path("certs/ca/v2g/V2G_CA_BUNDLE.pem");
        file_paths.mf_ca_bundle = fs::path("certs/ca/v2g/V2G_CA_BUNDLE.pem");
        file_paths.mo_ca_bundle = fs::path("certs/ca/mo/MO_CA_BUNDLE.pem");

        file_paths.directories.csms_leaf_cert_directory = fs::path("certs/client/csms/");
        file_paths.directories.csms_leaf_key_directory = fs::path("certs/client/csms/");
        file_paths.directories.secc_leaf_cert_directory = fs::path("csms_certs_temp/client/");
        file_paths.directories.secc_leaf_key_directory = fs::path("csms_certs_temp/client/");

        this->evse_security = std::make_unique<EvseSecurity>(file_paths, "123456");
    }

    void TearDown() override {
        fs::remove_all("csms_certs_temp");
    }
};

TEST_F(EvseSecurityTests, verify_basics) {
    const char* bundle_path = "certs/ca/v2g/V2G_CA_BUNDLE.pem";

    fsstd::ifstream file(bundle_path, std::ios::binary);
    std::string certificate_file((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    std::vector<std::string> certificate_strings;

    static const std::regex cert_regex("-----BEGIN CERTIFICATE-----[\\s\\S]*?-----END CERTIFICATE-----");
    std::string::const_iterator search_start(certificate_file.begin());

    std::smatch match;
    while (std::regex_search(search_start, certificate_file.cend(), match, cert_regex)) {
        std::string cert_data = match.str();
        try {
            certificate_strings.emplace_back(cert_data);
        } catch (const CertificateLoadException& e) {
            std::cout << "Could not load single certificate while splitting CA bundle: " << e.what() << std::endl;
        }
        search_start = match.suffix().first;
    }

    ASSERT_TRUE(certificate_strings.size() == 3);

    X509CertificateBundle bundle(fs::path(bundle_path), EncodingFormat::PEM);
    ASSERT_TRUE(bundle.is_using_bundle_file());

    std::cout << "Bundle hierarchy: " << std::endl << bundle.get_certificate_hierarchy().to_debug_string();

    auto certificates = bundle.split();
    ASSERT_TRUE(certificates.size() == 3);

    for (int i = 0; i < certificate_strings.size() - 1; ++i) {
        X509Wrapper cert(certificate_strings[i], EncodingFormat::PEM);
        X509Wrapper parent(certificate_strings[i + 1], EncodingFormat::PEM);

        ASSERT_TRUE(certificates[i].get_certificate_hash_data(parent) == cert.get_certificate_hash_data(parent));
        ASSERT_TRUE(equal_certificate_strings(cert.get_export_string(), certificate_strings[i]));
    }

    auto root_cert_idx = certificate_strings.size() - 1;
    X509Wrapper root_cert(certificate_strings[root_cert_idx], EncodingFormat::PEM);
    ASSERT_TRUE(certificates[root_cert_idx].get_certificate_hash_data() == root_cert.get_certificate_hash_data());
    ASSERT_TRUE(equal_certificate_strings(root_cert.get_export_string(), certificate_strings[root_cert_idx]));
}

TEST_F(EvseSecurityTests, verify_directory_bundles) {
    const auto child_cert_str = read_file_to_string(fs::path("certs/client/csms/CSMS_LEAF.pem"));

    ASSERT_EQ(this->evse_security->verify_certificate(child_cert_str, LeafCertificateType::CSMS),
              CertificateValidationResult::Valid);

    // Verifies that directory bundles properly function when verifying a certificate
    this->evse_security->ca_bundle_path_map[CaCertificateType::CSMS] = fs::path("certs/ca/v2g/");
    this->evse_security->ca_bundle_path_map[CaCertificateType::V2G] = fs::path("certs/ca/v2g/");

    // Verify a leaf
    ASSERT_EQ(this->evse_security->verify_certificate(child_cert_str, LeafCertificateType::CSMS),
              CertificateValidationResult::Valid);
}

TEST_F(EvseSecurityTests, verify_bundle_management) {
    const char* directory_path = "certs/ca/csms/";
    X509CertificateBundle bundle(fs::path(directory_path), EncodingFormat::PEM);
    ASSERT_TRUE(bundle.split().size() == 2);

    std::cout << "Bundle hierarchy: " << std::endl << bundle.get_certificate_hierarchy().to_debug_string();

    // Lowest in hierarchy
    X509Wrapper intermediate_cert = bundle.get_certificate_hierarchy().get_hierarchy().at(0).children.at(0).certificate;

    CertificateHashData hash;
    ASSERT_TRUE(bundle.get_certificate_hierarchy().get_certificate_hash(intermediate_cert, hash));
    bundle.delete_certificate(hash, true, false);

    // Sync deleted
    bundle.sync_to_certificate_store();

    std::cout << "Deleted intermediate: " << std::endl << bundle.get_certificate_hierarchy().to_debug_string();

    int items = 0;
    for (const auto& entry : fs::recursive_directory_iterator(directory_path)) {
        if (X509CertificateBundle::is_certificate_file(entry)) {
            items++;
        }
    }
    ASSERT_TRUE(items == 1);
}

TEST_F(EvseSecurityTests, verify_certificate_counts) {
    // This contains the 'real' fs certifs, we have the leaf chain + the leaf in a seaparate folder
    ASSERT_EQ(this->evse_security->get_count_of_installed_certificates({CertificateType::V2GCertificateChain}), 4);
    // We have 3 certs in the root bundle
    ASSERT_EQ(this->evse_security->get_count_of_installed_certificates({CertificateType::V2GRootCertificate}), 3);
    // MF is using the same V2G bundle in our case
    ASSERT_EQ(this->evse_security->get_count_of_installed_certificates({CertificateType::MFRootCertificate}), 3);
    // None were defined
    ASSERT_EQ(this->evse_security->get_count_of_installed_certificates({CertificateType::MORootCertificate}), 3);
}

TEST_F(EvseSecurityTestsMulti, verify_multi_root_leaf_retrieval) {
    auto result =
        this->evse_security->get_all_valid_certificates_info(LeafCertificateType::CSMS, EncodingFormat::PEM, false);

    ASSERT_EQ(result.status, GetCertificateInfoStatus::Accepted);

    // We have 2 leafs
    ASSERT_EQ(result.info.size(), 2);

    fs::path leaf_csms = fs::path("certs/client/csms/CSMS_LEAF.pem");
    fs::path leaf_grid = fs::path("certs/client/csms/SECC_LEAF_GRIDSYNC.pem");

    // File order is not guaranteed
    ASSERT_TRUE(leaf_csms == result.info[0].certificate_single.value() ||
                leaf_grid == result.info[0].certificate_single.value());
    ASSERT_TRUE(leaf_csms == result.info[1].certificate_single.value() ||
                leaf_grid == result.info[1].certificate_single.value());

    ASSERT_TRUE(result.info[0].certificate_root.has_value());
    ASSERT_TRUE(result.info[1].certificate_root.has_value());

    std::string root_v2g = read_file_to_string("certs/ca/v2g/V2G_ROOT_CA.pem");
    std::string root_grid = read_file_to_string("certs/ca/v2g/V2G_ROOT_GRIDSYNC_CA.pem");

    ASSERT_TRUE(equal_certificate_strings(result.info[0].certificate_root.value(), root_v2g) ||
                equal_certificate_strings(result.info[0].certificate_root.value(), root_grid));
    ASSERT_TRUE(equal_certificate_strings(result.info[1].certificate_root.value(), root_v2g) ||
                equal_certificate_strings(result.info[1].certificate_root.value(), root_grid));
}

TEST_F(EvseSecurityTestsMultiLeaf, verify_multi_leaf_retrieval) {
    std::vector<CertificateType> certificate_types;
    certificate_types.push_back(CertificateType::V2GCertificateChain);

    const auto r = this->evse_security->get_installed_certificates(certificate_types);

    ASSERT_EQ(r.status, GetInstalledCertificatesStatus::Accepted);
    ASSERT_EQ(r.certificate_hash_data_chain.size(), 2);

    auto& v2g_chain = r.certificate_hash_data_chain.front();

    // Assert the order with the SECCLeaf first
    ASSERT_EQ(v2g_chain.certificate_hash_data.debug_common_name, std::string("SECCCert"));
    ASSERT_EQ(v2g_chain.child_certificate_hash_data.size(), 2);
    ASSERT_EQ(v2g_chain.child_certificate_hash_data[0].debug_common_name, std::string("CPOSubCA2"));
    ASSERT_EQ(v2g_chain.child_certificate_hash_data[1].debug_common_name, std::string("CPOSubCA1"));

    auto& v2g_chain_alternate = r.certificate_hash_data_chain.back();

    // Assert the order with the SECCLeaf first
    ASSERT_EQ(v2g_chain_alternate.certificate_hash_data.debug_common_name, std::string("SECCGridSyncCert"));
    ASSERT_EQ(v2g_chain_alternate.child_certificate_hash_data.size(), 2);
    ASSERT_EQ(v2g_chain_alternate.child_certificate_hash_data[0].debug_common_name, std::string("CPOSubCA2"));
    ASSERT_EQ(v2g_chain_alternate.child_certificate_hash_data[1].debug_common_name, std::string("CPOSubCA1"));
}

TEST_F(EvseSecurityTests, verify_normal_keygen) {
    KeyGenerationInfo info;
    KeyHandle_ptr key;

    info.key_type = CryptoKeyType::RSA_3072;
    info.generate_on_custom = false;

    info.public_key_file = fs::path("key/nrm_pubkey.key");
    info.private_key_file = fs::path("key/nrm_privkey.key");

    bool gen = CryptoSupplier::generate_key(info, key);
    ASSERT_TRUE(gen);
}

TEST_F(EvseSecurityTests, verify_keygen_csr) {
    KeyGenerationInfo info;
    KeyHandle_ptr key;

    info.key_type = CryptoKeyType::EC_prime256v1;
    info.generate_on_custom = false;

    info.public_key_file = fs::path("key/pubkey.key");
    info.private_key_file = fs::path("key/privkey.key");

    bool gen = CryptoSupplier::generate_key(info, key);
    ASSERT_TRUE(gen);

    CertificateSigningRequestInfo csr_info;
    csr_info.n_version = 0;
    csr_info.commonName = "pionix_01";
    csr_info.organization = "PionixDE";
    csr_info.country = "DE";

    info.public_key_file = fs::path("key/csr_pubkey.tkey");
    info.private_key_file = fs::path("key/csr_privkey.tkey");
    info.key_type = CryptoKeyType::RSA_2048;

    csr_info.key_info = info;

    std::string csr;

    auto csr_gen = CryptoSupplier::x509_generate_csr(csr_info, csr);
    ASSERT_EQ(csr_gen, CertificateSignRequestResult::Valid);

    std::cout << "Csr: " << std::endl << csr << std::endl;
}

/// \brief get_certificate_hash_data() throws exception if called with no issuer and a non-self-signed cert
TEST_F(EvseSecurityTests, get_certificate_hash_data_non_self_signed_requires_issuer) {
    const auto non_self_signed_cert_str =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA3_SUBCA2.pem"));
    const X509Wrapper non_self_signed_cert(non_self_signed_cert_str, EncodingFormat::PEM);
    ASSERT_THROW(non_self_signed_cert.get_certificate_hash_data(), std::logic_error);
}

/// \brief get_certificate_hash_data() throws exception if called with the wrong issuer
TEST_F(EvseSecurityTests, get_certificate_hash_data_wrong_issuer) {
    const auto child_cert_str =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA3_SUBCA2.pem"));
    const X509Wrapper child_cert(child_cert_str, EncodingFormat::PEM);

    const auto wrong_parent_cert_str =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA3.pem"));
    const X509Wrapper wrong_parent_cert(wrong_parent_cert_str, EncodingFormat::PEM);

    ASSERT_THROW(child_cert.get_certificate_hash_data(wrong_parent_cert), std::logic_error);
}

/// \brief test verifyChargepointCertificate with valid cert
TEST_F(EvseSecurityTests, verify_chargepoint_cert_01) {
    const auto client_certificate = read_file_to_string(fs::path("certs/client/csms/CSMS_LEAF.pem"));
    std::cout << client_certificate << std::endl;
    const auto result = this->evse_security->update_leaf_certificate(client_certificate, LeafCertificateType::CSMS);
    ASSERT_TRUE(result == InstallCertificateResult::Accepted);
}

/// \brief test verifyChargepointCertificate with invalid cert
TEST_F(EvseSecurityTests, verify_chargepoint_cert_02) {
    const auto result = this->evse_security->update_leaf_certificate("InvalidCertificate", LeafCertificateType::CSMS);
    ASSERT_TRUE(result == InstallCertificateResult::InvalidFormat);
}

/// \brief test verifyV2GChargingStationCertificate with valid cert
TEST_F(EvseSecurityTests, verify_v2g_cert_01) {
    const auto client_certificate = read_file_to_string(fs::path("certs/client/cso/SECC_LEAF.pem"));
    const auto result = this->evse_security->update_leaf_certificate(client_certificate, LeafCertificateType::V2G);
    ASSERT_TRUE(result == InstallCertificateResult::Accepted);
}

/// \brief test verifyV2GChargingStationCertificate with invalid cert
TEST_F(EvseSecurityTests, verify_v2g_cert_02) {
    const auto invalid_certificate = read_file_to_string(fs::path("certs/client/invalid/INVALID_CSMS.pem"));
    const auto result = this->evse_security->update_leaf_certificate(invalid_certificate, LeafCertificateType::V2G);
    ASSERT_TRUE(result != InstallCertificateResult::Accepted);
}

TEST_F(EvseSecurityTests, retrieve_root_ca) {
    std::string path = "certs/ca/v2g/V2G_CA_BUNDLE.pem";
    std::string retrieved_path = this->evse_security->get_verify_file(CaCertificateType::V2G);

    ASSERT_EQ(path, retrieved_path);
}

TEST_F(EvseSecurityTests, retrieve_root_location) {
    std::string file_path = "certs/ca/v2g/V2G_CA_BUNDLE.pem";
    std::string retrieved_file_location = this->evse_security->get_verify_location(CaCertificateType::V2G);

    ASSERT_EQ(file_path, retrieved_file_location);
}

TEST_F(EvseSecurityTests, install_root_ca_01) {
    const auto v2g_root_ca = read_file_to_string(fs::path("certs/ca/v2g/V2G_ROOT_CA_NEW.pem"));
    const auto result = this->evse_security->install_ca_certificate(v2g_root_ca, CaCertificateType::V2G);
    ASSERT_TRUE(result == InstallCertificateResult::Accepted);

    std::string path = "certs/ca/v2g/V2G_CA_BUNDLE.pem";
    ASSERT_EQ(this->evse_security->get_verify_file(CaCertificateType::V2G), path);

    const auto read_v2g_root_ca = read_file_to_string(path);
    X509CertificateBundle root_bundle(read_v2g_root_ca, EncodingFormat::PEM);
    X509Wrapper new_root(v2g_root_ca, EncodingFormat::PEM);

    // Assert it was really installed
    ASSERT_TRUE(root_bundle.contains_certificate(new_root));
}

TEST_F(EvseSecurityTests, install_root_ca_02) {
    const auto invalid_csms_ca = "-----BEGIN CERTIFICATE-----InvalidCertificate-----END CERTIFICATE-----";
    const auto result = this->evse_security->install_ca_certificate(invalid_csms_ca, CaCertificateType::CSMS);
    ASSERT_EQ(result, InstallCertificateResult::InvalidFormat);
}

/// \brief test install two new root certificates
TEST_F(EvseSecurityTests, install_root_ca_03) {
    const auto pre_installed_certificates =
        this->evse_security->get_installed_certificates({CertificateType::CSMSRootCertificate});

    const auto new_root_ca_1 =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA1.pem"));
    const auto result = this->evse_security->install_ca_certificate(new_root_ca_1, CaCertificateType::CSMS);
    ASSERT_TRUE(result == InstallCertificateResult::Accepted);

    const auto new_root_ca_2 =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA2.pem"));
    const auto result2 = this->evse_security->install_ca_certificate(new_root_ca_2, CaCertificateType::CSMS);
    ASSERT_TRUE(result2 == InstallCertificateResult::Accepted);

    const auto post_installed_certificates =
        this->evse_security->get_installed_certificates({CertificateType::CSMSRootCertificate});

    ASSERT_EQ(post_installed_certificates.certificate_hash_data_chain.size(),
              pre_installed_certificates.certificate_hash_data_chain.size() + 2);
    for (auto& old_cert : pre_installed_certificates.certificate_hash_data_chain) {
        ASSERT_NE(
            std::find_if(post_installed_certificates.certificate_hash_data_chain.begin(),
                         post_installed_certificates.certificate_hash_data_chain.end(),
                         [&](auto value) { return value.certificate_hash_data == old_cert.certificate_hash_data; }),
            post_installed_certificates.certificate_hash_data_chain.end());
    }
    ASSERT_NE(std::find_if(post_installed_certificates.certificate_hash_data_chain.begin(),
                           post_installed_certificates.certificate_hash_data_chain.end(),
                           [&](auto value) {
                               return X509Wrapper(new_root_ca_1, EncodingFormat::PEM).get_certificate_hash_data() ==
                                      value.certificate_hash_data;
                           }),
              post_installed_certificates.certificate_hash_data_chain.end());
    ASSERT_NE(std::find_if(post_installed_certificates.certificate_hash_data_chain.begin(),
                           post_installed_certificates.certificate_hash_data_chain.end(),
                           [&](auto value) {
                               return X509Wrapper(new_root_ca_2, EncodingFormat::PEM).get_certificate_hash_data() ==
                                      value.certificate_hash_data;
                           }),
              post_installed_certificates.certificate_hash_data_chain.end());
}

/// \brief test install new root certificates + two child certificates
TEST_F(EvseSecurityTests, install_root_ca_04) {
    const auto pre_installed_certificates =
        this->evse_security->get_installed_certificates({CertificateType::CSMSRootCertificate});

    const auto new_root_ca_1 =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA3.pem"));
    const auto result = this->evse_security->install_ca_certificate(new_root_ca_1, CaCertificateType::CSMS);
    ASSERT_TRUE(result == InstallCertificateResult::Accepted);

    const auto new_root_sub_ca_1 =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA3_SUBCA1.pem"));
    const auto result2 = this->evse_security->install_ca_certificate(new_root_sub_ca_1, CaCertificateType::CSMS);
    ASSERT_TRUE(result2 == InstallCertificateResult::Accepted);

    const auto new_root_sub_ca_2 =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA3_SUBCA2.pem"));
    const auto result3 = this->evse_security->install_ca_certificate(new_root_sub_ca_2, CaCertificateType::CSMS);
    ASSERT_TRUE(result3 == InstallCertificateResult::Accepted);

    const auto post_installed_certificates =
        this->evse_security->get_installed_certificates({CertificateType::CSMSRootCertificate});
    ASSERT_EQ(post_installed_certificates.certificate_hash_data_chain.size(),
              pre_installed_certificates.certificate_hash_data_chain.size() + 1);

    const auto root_x509 = X509Wrapper(new_root_ca_1, EncodingFormat::PEM);
    const auto subca1_x509 = X509Wrapper(new_root_sub_ca_1, EncodingFormat::PEM);
    const auto subca2_x509 = X509Wrapper(new_root_sub_ca_2, EncodingFormat::PEM);
    const auto root_hash_data = root_x509.get_certificate_hash_data();
    const auto subca1_hash_data = subca1_x509.get_certificate_hash_data(root_x509);
    const auto subca2_hash_data = subca2_x509.get_certificate_hash_data(subca1_x509);
    auto result_hash_chain = std::find_if(post_installed_certificates.certificate_hash_data_chain.begin(),
                                          post_installed_certificates.certificate_hash_data_chain.end(),
                                          [&](auto chain) { return chain.certificate_hash_data == root_hash_data; });
    ASSERT_NE(result_hash_chain, post_installed_certificates.certificate_hash_data_chain.end());
    ASSERT_EQ(result_hash_chain->certificate_hash_data, root_hash_data);
    ASSERT_EQ(result_hash_chain->child_certificate_hash_data.size(), 2);
    ASSERT_EQ(result_hash_chain->child_certificate_hash_data[0], subca1_hash_data);
    ASSERT_EQ(result_hash_chain->child_certificate_hash_data[1], subca2_hash_data);
}

/// \brief test install expired certificate must be rejected
TEST_F(EvseSecurityTests, install_root_ca_05) {
    const auto expired_cert = std::string("-----BEGIN CERTIFICATE-----\n") +
                              "MIICsjCCAZqgAwIBAgICMDkwDQYJKoZIhvcNAQELBQAwHDEaMBgGA1UEAwwRT0NU\n" +
                              "VEV4cGlyZWRSb290Q0EwHhcNMjAwMTAxMDAwMDAwWhcNMjEwMTAxMDAwMDAwWjAc\n" +
                              "MRowGAYDVQQDDBFPQ1RURXhwaXJlZFJvb3RDQTCCASIwDQYJKoZIhvcNAQEBBQAD\n" +
                              "ggEPADCCAQoCggEBALA3xfKUgMaFfRHabFy27PhWvaeVDL6yd4qv4w4pe0NMJ0pE\n" +
                              "gr9ynzvXleVlOHF09rabgH99bW/ohLx3l7OliOjMk82e/77oGf0O8ZxViFrppA+z\n" +
                              "6WVhvRn7opso8KkrTCNUYyuzTH9u/n3EU9uFfueu+ifzD2qke7YJqTz7GY7aEqSb\n" +
                              "x7+3GDKhZV8lOw68T+WKkJxfuuafzczewHhu623ztc0bo5fTr3FSqWkuJXhB4Zg/\n" +
                              "GBMt1hS+O4IZeho8Ik9uu5zW39HQQNcJKN6dYDTIZdtQ8vNp6hYdOaRd05v77Ye0\n" +
                              "ywqqYVyUTgdfmqE5u7YeWUfO9vab3Qxq1IeHVd8CAwEAATANBgkqhkiG9w0BAQsF\n" +
                              "AAOCAQEAfDeemUzKXtqfCfuaGwTKTsj+Ld3A6VRiT/CSx1rh6BNAZZrve8OV2ckr\n" +
                              "2Ia+fol9mEkZPCBNLDzgxs5LLiJIOy4prjSTX4HJS5iqJBO8UJGakqXOAz0qBG1V\n" +
                              "8xWCJLeLGni9vi+dLVVFWpSfzTA/4iomtJPuvoXLdYzMvjLcGFT9RsE9q0oEbGHq\n" +
                              "ezKIzFaOdpCOtAt+FgW1lqqGHef2wNz15iWQLAU1juip+lgowI5YdhVJVPyqJTNz\n" +
                              "RUletvBeY2rFUKFWhj8QRPBwBlEDZqxRJSyIwQCe9t7Nhvbd9eyCFvRm9z3a8FDf\n" +
                              "FRmmZMWQkhBDQt15vxoDyyWn3hdwRA==\n" + "-----END CERTIFICATE-----";

    const auto result = this->evse_security->install_ca_certificate(expired_cert, CaCertificateType::CSMS);
    ASSERT_EQ(result, InstallCertificateResult::Expired);
}

TEST_F(EvseSecurityTestsCSMS, delete_csms_provided_certs) {
    auto path = fs::path("csms_certs_temp/client/");

    // Filesystem tests
    ASSERT_TRUE(fs::exists("csms_certs_temp/client/SECC_LEAF_A.pem"));
    ASSERT_TRUE(fs::exists("csms_certs_temp/client/SECC_LEAF_A.key"));
    ASSERT_TRUE(fs::exists("csms_certs_temp/client/SECC_LEAF_B.pem"));
    ASSERT_TRUE(fs::exists("csms_certs_temp/client/SECC_LEAF_B.key"));
    ASSERT_TRUE(fs::exists("csms_certs_temp/client/CPO_CERT_SECC_LEAF_CHAIN_A.pem"));
    ASSERT_TRUE(fs::exists("csms_certs_temp/client/CPO_CERT_SECC_LEAF_CHAIN_B.pem"));
    ASSERT_TRUE(fs::exists("csms_certs_temp/client/ocsp/SECC_LEAF_ocsp.der"));
    ASSERT_TRUE(fs::exists("csms_certs_temp/client/ocsp/SECC_LEAF_ocsp.hash"));

    auto cached_secc_leaf1 = read_file_to_string("csms_certs_temp/client/SECC_LEAF_A.pem");
    auto cached_secc_leaf2 = read_file_to_string("csms_certs_temp/client/SECC_LEAF_A.key");
    auto cached_secc_chain = read_file_to_string("csms_certs_temp/client/CPO_CERT_SECC_LEAF_CHAIN_A.pem");

    // SECC_LEAF_B
    CertificateHashData certificate_hash_data;
    certificate_hash_data.hash_algorithm = HashAlgorithm::SHA256;
    certificate_hash_data.issuer_name_hash = "82addb4b47026c702b9ed9d482c6e3570bbae9c49b963ec18b0a3523dfb47fe3";
    certificate_hash_data.issuer_key_hash = "e9d2a6d245233edbf5a8319b99087313e16307ca29b388373d951b50e93090aa";
    certificate_hash_data.serial_number = "4ed698d63c724c6a61a0ccc4ff80b383192dfd7a";

    // Code hash tests
    try {
        X509CertificateBundle leaf_bundle(path, EncodingFormat::PEM);
        auto& hierarchy = leaf_bundle.get_certificate_hierarchy();
        std::cout << hierarchy.to_debug_string();

        ASSERT_TRUE(hierarchy.contains_certificate_hash(certificate_hash_data, true));
    } catch (const CertificateLoadException& e) {
        FAIL();
    }

    // Delete
    DeleteResult result = this->evse_security->delete_certificate(certificate_hash_data);
    ASSERT_EQ(result.result, DeleteCertificateResult::Accepted);
    ASSERT_TRUE(result.leaf_certificate_type.has_value());
    ASSERT_EQ(result.leaf_certificate_type.value(), LeafCertificateType::V2G);

    try {
        X509CertificateBundle leaf_bundle(path, EncodingFormat::PEM);
        auto& hierarchy = leaf_bundle.get_certificate_hierarchy();
        std::cout << hierarchy.to_debug_string();

        ASSERT_FALSE(hierarchy.contains_certificate_hash(certificate_hash_data, true));
    } catch (const CertificateLoadException& e) {
        FAIL();
    }

    ASSERT_TRUE(fs::exists("csms_certs_temp/client/SECC_LEAF_A.pem"));
    ASSERT_TRUE(fs::exists("csms_certs_temp/client/SECC_LEAF_A.key"));
    ASSERT_TRUE(fs::exists("csms_certs_temp/client/CPO_CERT_SECC_LEAF_CHAIN_A.pem"));
    ASSERT_FALSE(fs::exists("csms_certs_temp/client/SECC_LEAF_B.pem"));
    ASSERT_FALSE(fs::exists("csms_certs_temp/client/SECC_LEAF_B.key"));
    ASSERT_FALSE(fs::exists("csms_certs_temp/client/CPO_CERT_SECC_LEAF_CHAIN_B.pem"));
    ASSERT_FALSE(fs::exists("csms_certs_temp/client/ocsp/SECC_LEAF_ocsp.der"));
    ASSERT_FALSE(fs::exists("csms_certs_temp/client/ocsp/SECC_LEAF_ocsp.hash"));

    ASSERT_EQ(cached_secc_leaf1, read_file_to_string("csms_certs_temp/client/SECC_LEAF_A.pem"));
    ASSERT_EQ(cached_secc_leaf2, read_file_to_string("csms_certs_temp/client/SECC_LEAF_A.key"));
    ASSERT_EQ(cached_secc_chain, read_file_to_string("csms_certs_temp/client/CPO_CERT_SECC_LEAF_CHAIN_A.pem"));
}

TEST_F(EvseSecurityTests, delete_root_ca_01) {
    std::vector<CertificateType> certificate_types;
    certificate_types.push_back(CertificateType::V2GRootCertificate);
    certificate_types.push_back(CertificateType::MORootCertificate);
    certificate_types.push_back(CertificateType::CSMSRootCertificate);
    certificate_types.push_back(CertificateType::V2GCertificateChain);
    certificate_types.push_back(CertificateType::MFRootCertificate);

    const auto root_certs = this->evse_security->get_installed_certificates(certificate_types);

    CaCertificateType root_type;
    CertificateType deleted_type = root_certs.certificate_hash_data_chain.at(0).certificate_type;

    switch (deleted_type) {
    case CertificateType::V2GRootCertificate:
        root_type = CaCertificateType::V2G;
        break;
    case CertificateType::MORootCertificate:
        root_type = CaCertificateType::MO;
        break;
    case CertificateType::CSMSRootCertificate:
        root_type = CaCertificateType::CSMS;
        break;
    case CertificateType::V2GCertificateChain:
        root_type = CaCertificateType::V2G;
        break;
    case CertificateType::MFRootCertificate:
        root_type = CaCertificateType::MF;
        break;
    }

    ASSERT_TRUE(fs::exists("certs/ca/v2g/V2G_CA_BUNDLE.pem"));
    auto cached_root_bundle = read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem");
    ASSERT_NE(cached_root_bundle.find("BEGIN CERTIFICATE"), std::string::npos);

    CertificateHashData certificate_hash_data;
    certificate_hash_data.hash_algorithm = HashAlgorithm::SHA256;
    certificate_hash_data.issuer_key_hash =
        root_certs.certificate_hash_data_chain.at(0).certificate_hash_data.issuer_key_hash;
    certificate_hash_data.issuer_name_hash =
        root_certs.certificate_hash_data_chain.at(0).certificate_hash_data.issuer_name_hash;
    certificate_hash_data.serial_number =
        root_certs.certificate_hash_data_chain.at(0).certificate_hash_data.serial_number;

    const auto result = this->evse_security->delete_certificate(certificate_hash_data);

    ASSERT_EQ(result.result, DeleteCertificateResult::Accepted);
    ASSERT_TRUE(result.ca_certificate_type.has_value());
    ASSERT_EQ(result.ca_certificate_type.value(), root_type);

    ASSERT_TRUE(fs::exists("certs/ca/v2g/V2G_CA_BUNDLE.pem"));
    ASSERT_TRUE(read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem").empty());
}

TEST_F(EvseSecurityTests, delete_root_ca_02) {
    CertificateHashData certificate_hash_data;
    certificate_hash_data.hash_algorithm = HashAlgorithm::SHA256;
    certificate_hash_data.issuer_key_hash = "UnknownKeyHash";
    certificate_hash_data.issuer_name_hash = "7da88c3366c19488ee810c5408f612db98164a34e05a0b15c93914fbed228c0f";
    certificate_hash_data.serial_number = "3046";

    ASSERT_TRUE(fs::exists("certs/ca/v2g/V2G_CA_BUNDLE.pem"));
    auto cached_root_bundle = read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem");
    ASSERT_NE(cached_root_bundle.find("BEGIN CERTIFICATE"), std::string::npos);

    const auto result = this->evse_security->delete_certificate(certificate_hash_data);

    ASSERT_EQ(result.result, DeleteCertificateResult::NotFound);
    ASSERT_FALSE(result.ca_certificate_type.has_value());
    ASSERT_FALSE(result.leaf_certificate_type.has_value());

    ASSERT_TRUE(fs::exists("certs/ca/v2g/V2G_CA_BUNDLE.pem"));
    ASSERT_EQ(cached_root_bundle, read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem"));
}

TEST_F(EvseSecurityTests, delete_sub_ca_1) {
    ASSERT_TRUE(fs::exists("certs/ca/v2g/V2G_CA_BUNDLE.pem"));
    std::string root_bundle_content;

    const auto new_root_ca_1 =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA3.pem"));

    root_bundle_content = read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem");
    const auto result = this->evse_security->install_ca_certificate(new_root_ca_1, CaCertificateType::CSMS);
    ASSERT_TRUE(result == InstallCertificateResult::Accepted);

    // Filesystem tests
    ASSERT_NE(root_bundle_content, read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem"));
    root_bundle_content = read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem");
    ASSERT_NE(root_bundle_content.find(new_root_ca_1), std::string::npos);

    const auto new_root_sub_ca_1 =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA3_SUBCA1.pem"));

    root_bundle_content = read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem");
    const auto result2 = this->evse_security->install_ca_certificate(new_root_sub_ca_1, CaCertificateType::CSMS);
    ASSERT_TRUE(result2 == InstallCertificateResult::Accepted);

    // Filesystem tests
    ASSERT_NE(root_bundle_content, read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem"));
    root_bundle_content = read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem");
    ASSERT_NE(root_bundle_content.find(new_root_sub_ca_1), std::string::npos);

    const auto new_root_sub_ca_2 =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA3_SUBCA2.pem"));

    root_bundle_content = read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem");
    const auto result3 = this->evse_security->install_ca_certificate(new_root_sub_ca_2, CaCertificateType::CSMS);
    ASSERT_TRUE(result3 == InstallCertificateResult::Accepted);

    // Filesystem tests
    ASSERT_NE(root_bundle_content, read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem"));
    root_bundle_content = read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem");
    ASSERT_NE(root_bundle_content.find(new_root_sub_ca_2), std::string::npos);

    const auto root_x509 = X509Wrapper(new_root_ca_1, EncodingFormat::PEM);
    const auto subca1_x509 = X509Wrapper(new_root_sub_ca_1, EncodingFormat::PEM);
    const auto subca1_hash_data = subca1_x509.get_certificate_hash_data(root_x509);

    const auto delete_result = this->evse_security->delete_certificate(subca1_hash_data);
    ASSERT_EQ(delete_result.result, DeleteCertificateResult::Accepted);
    ASSERT_TRUE(delete_result.ca_certificate_type.has_value());
    ASSERT_EQ(delete_result.ca_certificate_type.value(), CaCertificateType::V2G);

    // Filesystem tests
    root_bundle_content = read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem");
    ASSERT_EQ(root_bundle_content.find(new_root_sub_ca_1), std::string::npos);

    std::vector<CertificateType> certificate_types;
    certificate_types.push_back(CertificateType::V2GRootCertificate);
    certificate_types.push_back(CertificateType::MORootCertificate);
    certificate_types.push_back(CertificateType::CSMSRootCertificate);
    certificate_types.push_back(CertificateType::V2GCertificateChain);
    certificate_types.push_back(CertificateType::MFRootCertificate);
    const auto certs_after_delete =
        this->evse_security->get_installed_certificates(certificate_types).certificate_hash_data_chain;
    ASSERT_EQ(std::find_if(certs_after_delete.begin(), certs_after_delete.end(),
                           [&](auto value) {
                               return value.certificate_hash_data == subca1_hash_data ||
                                      (std::find_if(value.child_certificate_hash_data.begin(),
                                                    value.child_certificate_hash_data.end(), [&](auto child_value) {
                                                        return child_value == subca1_hash_data;
                                                    }) != value.child_certificate_hash_data.end());
                           }),
              certs_after_delete.end());
}

TEST_F(EvseSecurityTests, delete_sub_ca_2) {
    const auto new_root_ca_1 =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA3.pem"));
    const auto result = this->evse_security->install_ca_certificate(new_root_ca_1, CaCertificateType::CSMS);
    ASSERT_TRUE(result == InstallCertificateResult::Accepted);

    const auto new_root_sub_ca_1 =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA3_SUBCA1.pem"));
    const auto result2 = this->evse_security->install_ca_certificate(new_root_sub_ca_1, CaCertificateType::CSMS);
    ASSERT_TRUE(result2 == InstallCertificateResult::Accepted);

    const auto new_root_sub_ca_2 =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA3_SUBCA2.pem"));
    const auto result3 = this->evse_security->install_ca_certificate(new_root_sub_ca_2, CaCertificateType::CSMS);
    ASSERT_TRUE(result3 == InstallCertificateResult::Accepted);

    const auto root_x509 = X509Wrapper(new_root_ca_1, EncodingFormat::PEM);
    const auto subca1_x509 = X509Wrapper(new_root_sub_ca_1, EncodingFormat::PEM);
    const auto subca2_x509 = X509Wrapper(new_root_sub_ca_2, EncodingFormat::PEM);
    const auto subca2_hash_data = subca2_x509.get_certificate_hash_data(subca1_x509);

    auto root_bundle_content = read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem");
    const auto delete_result = this->evse_security->delete_certificate(subca2_hash_data);
    ASSERT_EQ(delete_result.result, DeleteCertificateResult::Accepted);
    ASSERT_TRUE(delete_result.ca_certificate_type.has_value());
    ASSERT_EQ(delete_result.ca_certificate_type.value(), CaCertificateType::V2G);

    // Filesystem tests
    ASSERT_NE(root_bundle_content, read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem"));
    root_bundle_content = read_file_to_string("certs/ca/v2g/V2G_CA_BUNDLE.pem");
    ASSERT_EQ(root_bundle_content.find(new_root_sub_ca_2), std::string::npos);

    std::vector<CertificateType> certificate_types;
    certificate_types.push_back(CertificateType::V2GRootCertificate);
    certificate_types.push_back(CertificateType::MORootCertificate);
    certificate_types.push_back(CertificateType::CSMSRootCertificate);
    certificate_types.push_back(CertificateType::V2GCertificateChain);
    certificate_types.push_back(CertificateType::MFRootCertificate);
    const auto certs_after_delete =
        this->evse_security->get_installed_certificates(certificate_types).certificate_hash_data_chain;

    ASSERT_EQ(std::find_if(certs_after_delete.begin(), certs_after_delete.end(),
                           [&](auto value) {
                               return value.certificate_hash_data == subca2_hash_data ||
                                      (std::find_if(value.child_certificate_hash_data.begin(),
                                                    value.child_certificate_hash_data.end(), [&](auto child_value) {
                                                        return child_value == subca2_hash_data;
                                                    }) != value.child_certificate_hash_data.end());
                           }),
              certs_after_delete.end());
}

TEST_F(EvseSecurityTests, get_installed_certificates_chain_order) {
    std::vector<CertificateType> certificate_types;
    certificate_types.push_back(CertificateType::V2GCertificateChain);

    const auto r = this->evse_security->get_installed_certificates(certificate_types);

    ASSERT_EQ(r.status, GetInstalledCertificatesStatus::Accepted);
    ASSERT_EQ(r.certificate_hash_data_chain.size(), 1);

    auto& v2g_chain = r.certificate_hash_data_chain.front();

    // Assert the order with the SECCLeaf first
    ASSERT_EQ(v2g_chain.certificate_hash_data.debug_common_name, std::string("SECCCert"));
    ASSERT_EQ(v2g_chain.child_certificate_hash_data.size(), 2);
    ASSERT_EQ(v2g_chain.child_certificate_hash_data[0].debug_common_name, std::string("CPOSubCA2"));
    ASSERT_EQ(v2g_chain.child_certificate_hash_data[1].debug_common_name, std::string("CPOSubCA1"));
}

TEST_F(EvseSecurityTests, get_installed_certificates_and_delete_secc_leaf) {
    std::vector<CertificateType> certificate_types;
    certificate_types.push_back(CertificateType::V2GRootCertificate);
    certificate_types.push_back(CertificateType::MORootCertificate);
    certificate_types.push_back(CertificateType::CSMSRootCertificate);
    certificate_types.push_back(CertificateType::V2GCertificateChain);
    certificate_types.push_back(CertificateType::MFRootCertificate);

    const auto r = this->evse_security->get_installed_certificates(certificate_types);

    ASSERT_EQ(r.status, GetInstalledCertificatesStatus::Accepted);
    ASSERT_EQ(r.certificate_hash_data_chain.size(), 5);
    bool found_v2g_chain = false;

    CertificateHashData secc_leaf_data;

    for (const auto& certificate_hash_data_chain : r.certificate_hash_data_chain) {
        if (certificate_hash_data_chain.certificate_type == CertificateType::V2GCertificateChain) {
            found_v2g_chain = true;
            secc_leaf_data = certificate_hash_data_chain.certificate_hash_data;
            ASSERT_EQ(2, certificate_hash_data_chain.child_certificate_hash_data.size());
        }
    }
    ASSERT_TRUE(found_v2g_chain);

    // Do not allow the SECC delete since it's the ChargingStationCertificate
    auto delete_response = this->evse_security->delete_certificate(secc_leaf_data);
    ASSERT_EQ(delete_response.result, DeleteCertificateResult::Failed);
}

TEST_F(EvseSecurityTests, leaf_cert_starts_in_future_accepted) {
    const auto v2g_keypair_before =
        this->evse_security->get_leaf_certificate_info(LeafCertificateType::V2G, EncodingFormat::PEM);

    const auto new_root_ca = read_file_to_string(std::filesystem::path("future_leaf/V2G_ROOT_CA.pem"));
    const auto result_ca = this->evse_security->install_ca_certificate(new_root_ca, CaCertificateType::V2G);
    ASSERT_TRUE(result_ca == InstallCertificateResult::Accepted);

    std::filesystem::copy("future_leaf/SECC_LEAF_FUTURE.key", "certs/client/cso/SECC_LEAF_FUTURE.key");

    const auto client_certificate = read_file_to_string(fs::path("future_leaf/SECC_LEAF_FUTURE.pem"));
    std::cout << client_certificate << std::endl;
    const auto result_client =
        this->evse_security->update_leaf_certificate(client_certificate, LeafCertificateType::V2G);
    ASSERT_TRUE(result_client == InstallCertificateResult::Accepted);

    // Check: The certificate is installed, but it isn't actually used
    const auto v2g_keypair_after =
        this->evse_security->get_leaf_certificate_info(LeafCertificateType::V2G, EncodingFormat::PEM);
    ASSERT_EQ(v2g_keypair_after.info.value().certificate, v2g_keypair_before.info.value().certificate);
    ASSERT_EQ(v2g_keypair_after.info.value().key, v2g_keypair_before.info.value().key);
    ASSERT_EQ(v2g_keypair_after.info.value().password, v2g_keypair_before.info.value().password);
}

TEST_F(EvseSecurityTests, expired_leaf_cert_rejected) {
    const auto new_root_ca = read_file_to_string(std::filesystem::path("expired_leaf/V2G_ROOT_CA.pem"));
    const auto result_ca = this->evse_security->install_ca_certificate(new_root_ca, CaCertificateType::V2G);
    ASSERT_TRUE(result_ca == InstallCertificateResult::Accepted);

    std::filesystem::copy("expired_leaf/SECC_LEAF_EXPIRED.key", "certs/client/cso/SECC_LEAF_EXPIRED.key");

    const auto client_certificate = read_file_to_string(fs::path("expired_leaf/SECC_LEAF_EXPIRED.pem"));
    std::cout << client_certificate << std::endl;
    const auto result_client =
        this->evse_security->update_leaf_certificate(client_certificate, LeafCertificateType::V2G);
    ASSERT_TRUE(result_client == InstallCertificateResult::Expired);
}

TEST_F(EvseSecurityTests, verify_full_filesystem) {
    ASSERT_EQ(evse_security->is_filesystem_full(), false);

    evse_security->max_fs_usage_bytes = 1;
    ASSERT_EQ(evse_security->is_filesystem_full(), true);
}

TEST_F(EvseSecurityTests, verify_full_filesystem_install_reject) {
    evse_security->max_fs_usage_bytes = 1;
    ASSERT_EQ(evse_security->is_filesystem_full(), true);

    // Must have a rejection
    const auto new_root_ca_1 =
        read_file_to_string(std::filesystem::path("certs/to_be_installed/INSTALL_TEST_ROOT_CA1.pem"));
    const auto result = this->evse_security->install_ca_certificate(new_root_ca_1, CaCertificateType::CSMS);
    ASSERT_TRUE(result == InstallCertificateResult::CertificateStoreMaxLengthExceeded);
}

TEST_F(EvseSecurityTestsMultiLeaf, verify_ocsp_request_multi_valid) {
    // Verify the OCSP request when we have multiple possible valid certificates
    OCSPRequestDataList data = this->evse_security->get_v2g_ocsp_request_data();
    ASSERT_EQ(data.ocsp_request_data_list.size(), 4);

    ASSERT_TRUE(data.ocsp_request_data_list[0].certificate_hash_data.has_value());
    ASSERT_TRUE(data.ocsp_request_data_list[1].certificate_hash_data.has_value());
    ASSERT_TRUE(data.ocsp_request_data_list[2].certificate_hash_data.has_value());
    ASSERT_TRUE(data.ocsp_request_data_list[3].certificate_hash_data.has_value());

    ASSERT_TRUE(
        std::find_if(data.ocsp_request_data_list.begin(), data.ocsp_request_data_list.end(), [](const auto& ocsp_data) {
            return ocsp_data.certificate_hash_data.value().debug_common_name == std::string("CPOSubCA2");
        }) != data.ocsp_request_data_list.end());

    ASSERT_TRUE(
        std::find_if(data.ocsp_request_data_list.begin(), data.ocsp_request_data_list.end(), [](const auto& ocsp_data) {
            return ocsp_data.certificate_hash_data.value().debug_common_name == std::string("CPOSubCA1");
        }) != data.ocsp_request_data_list.end());

    ASSERT_TRUE(
        std::find_if(data.ocsp_request_data_list.begin(), data.ocsp_request_data_list.end(), [](const auto& ocsp_data) {
            return ocsp_data.certificate_hash_data.value().debug_common_name == std::string("SECCCert");
        }) != data.ocsp_request_data_list.end());

    ASSERT_TRUE(
        std::find_if(data.ocsp_request_data_list.begin(), data.ocsp_request_data_list.end(), [](const auto& ocsp_data) {
            return ocsp_data.certificate_hash_data.value().debug_common_name == std::string("SECCGridSyncCert");
        }) != data.ocsp_request_data_list.end());
}

TEST_F(EvseSecurityTests, verify_ocsp_request_mo_generate) {
    // Read a leaf, should work since this SECC will be tested against both MO and V2G
    const auto secc_leaf = read_file_to_string("certs/client/cso/SECC_LEAF.pem");
    OCSPRequestDataList data = this->evse_security->get_mo_ocsp_request_data(secc_leaf);

    // Expect 3 chain certifs, since SECC_LEAF has an responder URL
    ASSERT_EQ(data.ocsp_request_data_list.size(), 3);

    // Assert a leaf->sub2->sub1 order
    ASSERT_TRUE(data.ocsp_request_data_list[0].certificate_hash_data.has_value());
    ASSERT_TRUE(data.ocsp_request_data_list[1].certificate_hash_data.has_value());
    ASSERT_TRUE(data.ocsp_request_data_list[2].certificate_hash_data.has_value());

    bool has_intermediate_1 =
        std::find_if(data.ocsp_request_data_list.begin(), data.ocsp_request_data_list.end(), [](const auto& ocsp_data) {
            return ocsp_data.certificate_hash_data.value().debug_common_name == std::string("CPOSubCA1");
        }) != data.ocsp_request_data_list.end();
    ASSERT_TRUE(has_intermediate_1);

    bool has_intermediate_2 =
        std::find_if(data.ocsp_request_data_list.begin(), data.ocsp_request_data_list.end(), [](const auto& ocsp_data) {
            return ocsp_data.certificate_hash_data.value().debug_common_name == std::string("CPOSubCA2");
        }) != data.ocsp_request_data_list.end();
    ASSERT_TRUE(has_intermediate_2);

    bool has_leaf =
        std::find_if(data.ocsp_request_data_list.begin(), data.ocsp_request_data_list.end(), [](const auto& ocsp_data) {
            return ocsp_data.certificate_hash_data.value().debug_common_name == std::string("SECCCert");
        }) != data.ocsp_request_data_list.end();
    ASSERT_TRUE(has_leaf);

    // Read the MO leaf
    const auto mo_leaf = read_file_to_string("certs/client/mo/MO_LEAF.pem");
    data = this->evse_security->get_mo_ocsp_request_data(mo_leaf);

    // Expect 2 chain certifs, since leaf does not have an responder URL
    ASSERT_EQ(data.ocsp_request_data_list.size(), 2);
    ASSERT_TRUE(data.ocsp_request_data_list[0].certificate_hash_data.has_value());
    ASSERT_TRUE(data.ocsp_request_data_list[1].certificate_hash_data.has_value());

    has_intermediate_1 =
        std::find_if(data.ocsp_request_data_list.begin(), data.ocsp_request_data_list.end(), [](const auto& ocsp_data) {
            return ocsp_data.certificate_hash_data.value().debug_common_name == std::string("MOSubCA2");
        }) != data.ocsp_request_data_list.end();
    ASSERT_TRUE(has_intermediate_1);

    has_intermediate_2 =
        std::find_if(data.ocsp_request_data_list.begin(), data.ocsp_request_data_list.end(), [](const auto& ocsp_data) {
            return ocsp_data.certificate_hash_data.value().debug_common_name == std::string("MOSubCA1");
        }) != data.ocsp_request_data_list.end();
    ASSERT_TRUE(has_intermediate_2);

    // Read the MO signed by V2G leaf
    const auto mo_v2g_leaf = read_file_to_string("certs/client/mo/MO_LEAF_V2G.pem");
    data = this->evse_security->get_mo_ocsp_request_data(mo_v2g_leaf);

    // Again expect 2 since the mo leaf does not have a responder URL
    ASSERT_EQ(data.ocsp_request_data_list.size(), 2);
    ASSERT_TRUE(data.ocsp_request_data_list[0].certificate_hash_data.has_value());
    ASSERT_TRUE(data.ocsp_request_data_list[1].certificate_hash_data.has_value());

    has_intermediate_1 =
        std::find_if(data.ocsp_request_data_list.begin(), data.ocsp_request_data_list.end(), [](const auto& ocsp_data) {
            return ocsp_data.certificate_hash_data.value().debug_common_name == std::string("CPOSubCA1");
        }) != data.ocsp_request_data_list.end();
    ASSERT_TRUE(has_intermediate_1);

    has_intermediate_2 =
        std::find_if(data.ocsp_request_data_list.begin(), data.ocsp_request_data_list.end(), [](const auto& ocsp_data) {
            return ocsp_data.certificate_hash_data.value().debug_common_name == std::string("CPOSubCA2");
        }) != data.ocsp_request_data_list.end();
    ASSERT_TRUE(has_intermediate_2);
}

TEST_F(EvseSecurityTests, verify_ocsp_cache) {
    std::string ocsp_mock_response_data = "OCSP_MOCK_RESPONSE_DATA";
    std::string ocsp_mock_response_data_v2 = "OCSP_MOCK_RESPONSE_DATA_V2";

    OCSPRequestDataList data = this->evse_security->get_v2g_ocsp_request_data();

    ASSERT_EQ(data.ocsp_request_data_list.size(), 3);

    // Mock a response
    for (auto& ocsp : data.ocsp_request_data_list) {
        this->evse_security->update_ocsp_cache(ocsp.certificate_hash_data.value(), ocsp_mock_response_data);
    }

    // Make sure all info was written and that it is correct
    fs::path ocsp_path = "certs/client/cso/ocsp";

    ASSERT_TRUE(fs::exists(ocsp_path));

    for (auto& ocsp : data.ocsp_request_data_list) {
        std::optional<std::string> data = this->evse_security->retrieve_ocsp_cache(ocsp.certificate_hash_data.value());
        ASSERT_TRUE(data.has_value());
        ASSERT_EQ(read_file_to_string(data.value()), ocsp_mock_response_data);
    }

    int entries = 0;
    for (const auto& ocsp_entry : fs::directory_iterator(ocsp_path)) {
        ASSERT_TRUE(ocsp_entry.is_regular_file());
        ASSERT_TRUE(ocsp_entry.path().has_extension());

        auto ext = ocsp_entry.path().extension();

        ASSERT_TRUE(ext == DER_EXTENSION || ext == CERT_HASH_EXTENSION);

        if (ext == DER_EXTENSION) {
            ASSERT_EQ(read_file_to_string(ocsp_entry.path()), ocsp_mock_response_data);
        } else if (ext == CERT_HASH_EXTENSION) {
            CertificateHashData hash;
            ASSERT_TRUE(filesystem_utils::read_hash_from_file(ocsp_entry.path(), hash));

            // Check that is is contained
            auto it =
                std::find_if(data.ocsp_request_data_list.begin(), data.ocsp_request_data_list.end(),
                             [&hash](OCSPRequestData& req_data) { return (hash == req_data.certificate_hash_data); });

            ASSERT_NE(it, data.ocsp_request_data_list.end());
        }

        entries++;
    }

    ASSERT_EQ(entries, 6); // 3 for hash, 3 for data

    // Write data again to test over-writing
    for (auto& ocsp : data.ocsp_request_data_list) {
        this->evse_security->update_ocsp_cache(ocsp.certificate_hash_data.value(), ocsp_mock_response_data_v2);
    }

    for (auto& ocsp : data.ocsp_request_data_list) {
        std::optional<std::string> data = this->evse_security->retrieve_ocsp_cache(ocsp.certificate_hash_data.value());
        ASSERT_TRUE(data.has_value());
        ASSERT_EQ(read_file_to_string(data.value()), ocsp_mock_response_data_v2);
    }

    // Make sure the info was over-written
    entries = 0;
    for (const auto& ocsp_entry : fs::directory_iterator(ocsp_path)) {
        ASSERT_TRUE(ocsp_entry.is_regular_file());
        ASSERT_TRUE(ocsp_entry.path().has_extension());

        auto ext = ocsp_entry.path().extension();

        ASSERT_TRUE(ext == DER_EXTENSION || ext == CERT_HASH_EXTENSION);

        if (ext == DER_EXTENSION) {
            ASSERT_EQ(read_file_to_string(ocsp_entry.path()), ocsp_mock_response_data_v2);
        } else if (ext == CERT_HASH_EXTENSION) {
            CertificateHashData hash;
            ASSERT_TRUE(filesystem_utils::read_hash_from_file(ocsp_entry.path(), hash));

            // Check that is is contained
            auto it =
                std::find_if(data.ocsp_request_data_list.begin(), data.ocsp_request_data_list.end(),
                             [&hash](OCSPRequestData& req_data) { return (hash == req_data.certificate_hash_data); });

            ASSERT_NE(it, data.ocsp_request_data_list.end());
        }

        entries++;
    }

    ASSERT_EQ(entries, 6); // 6 still, since we have to over-write

    // Retrieve OCSP data along with certificates
    GetCertificateInfoResult response =
        this->evse_security->get_leaf_certificate_info(LeafCertificateType::V2G, EncodingFormat::PEM, true);

    ASSERT_EQ(response.status, GetCertificateInfoStatus::Accepted);
    ASSERT_TRUE(response.info.has_value());

    CertificateInfo info = response.info.value();

    ASSERT_EQ(info.certificate_count, 3);
    ASSERT_EQ(info.ocsp.size(), 3);

    // Skip first that does not have OCSP data
    for (int i = 1; i < info.ocsp.size(); ++i) {
        auto& ocsp = info.ocsp[i];

        ASSERT_TRUE(ocsp.ocsp_path.has_value());
        ASSERT_EQ(read_file_to_string(ocsp.ocsp_path.value()), ocsp_mock_response_data_v2);
    }
}

TEST_F(EvseSecurityTests, verify_ocsp_garbage_collect) {
    std::string ocsp_mock_response_data = "OCSP_MOCK_RESPONSE_DATA";

    OCSPRequestDataList data = this->evse_security->get_v2g_ocsp_request_data();
    ASSERT_EQ(data.ocsp_request_data_list.size(), 3);

    // Mock a response
    for (auto& ocsp : data.ocsp_request_data_list) {
        this->evse_security->update_ocsp_cache(ocsp.certificate_hash_data.value(), ocsp_mock_response_data);
    }

    // Make sure all info was written and that it is correct
    fs::path ocsp_path = "certs/ca/v2g/ocsp";
    fs::path ocsp_path2 = "certs/client/cso/ocsp";

    ASSERT_TRUE(fs::exists(ocsp_path));

    for (auto& ocsp : data.ocsp_request_data_list) {
        std::optional<fs::path> data = this->evse_security->retrieve_ocsp_cache(ocsp.certificate_hash_data.value());
        ASSERT_TRUE(data.has_value());
        ASSERT_EQ(read_file_to_string(data.value()), ocsp_mock_response_data);
    }

    evse_security->max_fs_certificate_store_entries = 1;
    ASSERT_TRUE(evse_security->is_filesystem_full());

    // Garbage collect to see that we don't delete improper data
    this->evse_security->garbage_collect();

    ASSERT_TRUE(fs::exists(ocsp_path));
    ASSERT_TRUE(fs::exists(ocsp_path2));

    // Check existence of OCSP data
    int existing = 0;
    for (auto& ocsp_path : {ocsp_path, ocsp_path2}) {
        for (auto& ocsp_entry : fs::directory_iterator(ocsp_path)) {
            auto ext = ocsp_entry.path().extension();
            if (ext == DER_EXTENSION || ext == CERT_HASH_EXTENSION) {
                existing++;
            }
        }
    }

    ASSERT_EQ(existing, 10);

    // Delete the certificates that had their OCSP data appended
    fs::remove("certs/ca/v2g/V2G_CA_BUNDLE.pem");
    fs::remove("certs/ca/v2g/V2G_ROOT_CA.pem");
    fs::remove("certs/client/cso/CPO_CERT_CHAIN.pem");

    // Garbage collect again
    this->evse_security->garbage_collect();

    // Check deletion
    existing = 0;
    for (auto& ocsp_path : {ocsp_path, ocsp_path2}) {
        for (auto& ocsp_entry : fs::directory_iterator(ocsp_path)) {
            auto ext = ocsp_entry.path().extension();
            if (ext == DER_EXTENSION || ext == CERT_HASH_EXTENSION) {
                existing++;
            }
        }
    }

    ASSERT_EQ(existing, 0);
}

TEST_F(EvseSecurityTestsExpired, verify_expired_leaf_deletion) {
    // Check that the FS is not full
    ASSERT_FALSE(evse_security->is_filesystem_full());

    // List of date sorted certificates
    std::vector<X509Wrapper> sorted;
    std::vector<fs::path> sorted_should_delete;
    std::vector<fs::path> sorted_should_keep;

    // Ensure that we have GEN_CERTIFICATES + 2 (CPO_CERT_CHAIN.pem + SECC_LEAF.pem)
    {
        X509CertificateBundle full_certs(fs::path("certs/client/cso"), EncodingFormat::PEM);
        ASSERT_EQ(full_certs.get_certificate_chains_count(), GEN_CERTIFICATES + 2);

        full_certs.for_each_chain([&sorted](const fs::path& path, const std::vector<X509Wrapper>& certifs) {
            sorted.push_back(certifs.at(0));

            return true;
        });

        ASSERT_EQ(sorted.size(), GEN_CERTIFICATES + 2);
    }

    // Sort by end expiry date
    std::sort(std::begin(sorted), std::end(sorted),
              [](X509Wrapper& a, X509Wrapper& b) { return (a.get_valid_to() > b.get_valid_to()); });

    // Collect all should-delete and kept certificates
    int skipped = 0;

    for (const auto& cert : sorted) {
        if (++skipped > DEFAULT_MINIMUM_CERTIFICATE_ENTRIES) {
            sorted_should_delete.push_back(cert.get_file().value());
        } else {
            sorted_should_keep.push_back(cert.get_file().value());
        }
    }

    // Fill the disk
    evse_security->max_fs_certificate_store_entries = 20;

    ASSERT_TRUE(evse_security->is_filesystem_full());

    // Garbage collect
    evse_security->garbage_collect();

    // Ensure that we have 10 certificates, since we only keep 10, the newest
    {
        X509CertificateBundle full_certs(fs::path("certs/client/cso"), EncodingFormat::PEM);
        ASSERT_EQ(full_certs.get_certificate_chains_count(), DEFAULT_MINIMUM_CERTIFICATE_ENTRIES);

        // Ensure that we only have the newest ones
        for (const auto& deleted : sorted_should_delete) {
            ASSERT_FALSE(fs::exists(deleted));
        }

        for (const auto& not_deleted : sorted_should_keep) {
            fs::path key_file = not_deleted;
            key_file.replace_extension(".key");

            ASSERT_TRUE(fs::exists(not_deleted));

            // Ignore the CPO chain that does not have a properly
            if (not_deleted.string().find("CPO_CERT_CHAIN") != std::string::npos) {
                key_file = "certs/client/cso/SECC_LEAF.key";
            }

            // Check their respective keys exist
            std::cout << key_file;
            ASSERT_TRUE(fs::exists(key_file));

            X509Wrapper cert = X509CertificateBundle(not_deleted, EncodingFormat::PEM).split().at(0);

            fsstd::ifstream file(key_file, std::ios::binary);
            std::string private_key((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

            ASSERT_EQ(KeyValidationResult::Valid, CryptoSupplier::x509_check_private_key(
                                                      cert.get(), private_key, evse_security->private_key_password));
        }
    }
}

TEST_F(EvseSecurityTests, verify_expired_csr_deletion) {
    // Generate a CSR
    auto csr = evse_security->generate_certificate_signing_request(LeafCertificateType::CSMS, "DE", "Pionix", "NA");
    fs::path csr_key_path = evse_security->managed_csr.begin()->first;

    // Simulate a full fs else no deletion will take place
    evse_security->max_fs_usage_bytes = 1;

    ASSERT_EQ(evse_security->managed_csr.size(), 1);
    ASSERT_TRUE(fs::exists(csr_key_path));

    // Check that is is NOT deleted
    evse_security->garbage_collect();
    ASSERT_TRUE(fs::exists(csr_key_path));

    // Sleep 1 second AND it must be deleted
    evse_security->csr_expiry = std::chrono::seconds(0);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    evse_security->garbage_collect();

    ASSERT_FALSE(fs::exists(csr_key_path));
    ASSERT_EQ(evse_security->managed_csr.size(), 0);

    // Delete unmanaged, future expired CSRs
    csr = evse_security->generate_certificate_signing_request(LeafCertificateType::CSMS, "DE", "Pionix", "NA");
    csr_key_path = evse_security->managed_csr.begin()->first;

    ASSERT_EQ(evse_security->managed_csr.size(), 1);

    // Remove from managed (simulate a reboot/reinit)
    evse_security->managed_csr.clear();

    // at this GC the should re-add the key to our managed list
    evse_security->csr_expiry = std::chrono::seconds(10);
    evse_security->garbage_collect();
    ASSERT_EQ(evse_security->managed_csr.size(), 1);
    ASSERT_TRUE(fs::exists(csr_key_path));

    // Now it is technically expired again
    evse_security->csr_expiry = std::chrono::seconds(0);
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Garbage collect should delete the expired managed key
    evse_security->garbage_collect();
    ASSERT_FALSE(fs::exists(csr_key_path));
}

TEST_F(EvseSecurityTests, verify_base64) {
    std::string test_string1 = "U29tZSBkYXRhIGZvciB0ZXN0IGNhc2VzLiBTb21lIGRhdGEgZm9yIHRlc3QgY2FzZXMuIFNvbWUgZGF0YSBmb3I"
                               "gdGVzdCBjYXNlcy4gU29tZSBkYXRhIGZvciB0ZXN0IGNhc2VzLg==";

    std::string decoded = this->evse_security->base64_decode_to_string(test_string1);
    ASSERT_EQ(
        decoded,
        std::string(
            "Some data for test cases. Some data for test cases. Some data for test cases. Some data for test cases."));

    std::string out_encoded = this->evse_security->base64_encode_from_string(decoded);
    out_encoded.erase(std::remove(out_encoded.begin(), out_encoded.end(), '\n'), out_encoded.cend());

    ASSERT_EQ(test_string1, out_encoded);
}

TEST_F(EvseSecurityTestsMulti, verify_with_multiple_leaf_types_accepts_valid_cert) {
    const std::string certificate_chain = read_file_to_string("certs/client/csms/SECC_LEAF_GRIDSYNC.pem");

    // Validate with both V2G and CSMS trust anchors
    std::vector<LeafCertificateType> types = {LeafCertificateType::V2G, LeafCertificateType::CSMS};

    auto result = this->evse_security->verify_certificate(certificate_chain, types);

    ASSERT_EQ(result, CertificateValidationResult::Valid);
}

TEST_F(EvseSecurityTestsMulti, verify_with_missing_trust_anchor_fails) {
    const std::string certificate_chain = read_file_to_string("certs/client/csms/SECC_LEAF_GRIDSYNC.pem");

    // Intentionally omit the correct anchor (V2G or CSMS)
    std::vector<LeafCertificateType> types = {LeafCertificateType::MO};

    auto result = this->evse_security->verify_certificate(certificate_chain, types);

    ASSERT_NE(result, CertificateValidationResult::Valid);
    ASSERT_EQ(result, CertificateValidationResult::IssuerNotFound); // since MO is empty
}

TEST_F(EvseSecurityTestsMulti, verify_with_invalid_cert_fails) {
    const std::string invalid_cert = "-----BEGIN CERTIFICATE-----\nINVALID\n-----END CERTIFICATE-----";

    std::vector<LeafCertificateType> types = {LeafCertificateType::CSMS, LeafCertificateType::V2G};

    auto result = this->evse_security->verify_certificate(invalid_cert, types);

    ASSERT_EQ(result, CertificateValidationResult::Unknown);
}

} // namespace evse_security

// FIXME(piet): Add more tests for getRootCertificateHashData (incl. V2GCertificateChain etc.)