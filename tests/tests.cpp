// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <fstream>
#include <gtest/gtest.h>
#include <regex>
#include <sstream>

#include <openssl/err.h>
#include <openssl/x509v3.h>

#include <evse_security/certificate/x509_bundle.hpp>
#include <evse_security/certificate/x509_wrapper.hpp>
#include <evse_security/evse_security.hpp>
#include <evse_security/utils/fix_pem_string.hpp>

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

void install_certs() {
    std::system("./generate_test_certs.sh");
}

namespace evse_security {

class EvseSecurityTests : public ::testing::Test {
protected:
    std::unique_ptr<EvseSecurity> evse_security;

    void SetUp() override {
        install_certs();

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

    std::cout << "Bundle hierarchy: " << std::endl << bundle.get_certficate_hierarchy().to_debug_string();

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

TEST_F(EvseSecurityTests, verify_bundle_management) {
    const char* directory_path = "certs/ca/csms/";
    X509CertificateBundle bundle(fs::path(directory_path), EncodingFormat::PEM);
    ASSERT_TRUE(bundle.split().size() == 2);

    std::cout << "Bundle hierarchy: " << std::endl << bundle.get_certficate_hierarchy().to_debug_string();

    // Lowest in hierarchy
    X509Wrapper intermediate_cert = bundle.get_certficate_hierarchy().get_hierarchy().at(0).children.at(0).certificate;

    CertificateHashData hash = bundle.get_certficate_hierarchy().get_certificate_hash(intermediate_cert);
    bundle.delete_certificate(hash, true);

    // Sync deleted
    bundle.sync_to_certificate_store();

    std::cout << "Deleted intermediate: " << std::endl << bundle.get_certficate_hierarchy().to_debug_string();

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
    ASSERT_EQ(this->evse_security->get_count_of_installed_certificates({CertificateType::MORootCertificate}), 0);
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
    ASSERT_TRUE(result == InstallCertificateResult::InvalidCertificateChain);
}

TEST_F(EvseSecurityTests, install_root_ca_01) {
    const auto v2g_root_ca = read_file_to_string(fs::path("certs/ca/v2g/V2G_ROOT_CA_NEW.pem"));
    const auto result = this->evse_security->install_ca_certificate(v2g_root_ca, CaCertificateType::V2G);
    ASSERT_TRUE(result == InstallCertificateResult::Accepted);
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

TEST_F(EvseSecurityTests, delete_root_ca_01) {

    std::vector<CertificateType> certificate_types;
    certificate_types.push_back(CertificateType::V2GRootCertificate);
    certificate_types.push_back(CertificateType::MORootCertificate);
    certificate_types.push_back(CertificateType::CSMSRootCertificate);
    certificate_types.push_back(CertificateType::V2GCertificateChain);
    certificate_types.push_back(CertificateType::MFRootCertificate);

    const auto root_certs = this->evse_security->get_installed_certificates(certificate_types);

    CertificateHashData certificate_hash_data;
    certificate_hash_data.hash_algorithm = HashAlgorithm::SHA256;
    certificate_hash_data.issuer_key_hash =
        root_certs.certificate_hash_data_chain.at(0).certificate_hash_data.issuer_key_hash;
    certificate_hash_data.issuer_name_hash =
        root_certs.certificate_hash_data_chain.at(0).certificate_hash_data.issuer_name_hash;
    certificate_hash_data.serial_number =
        root_certs.certificate_hash_data_chain.at(0).certificate_hash_data.serial_number;
    const auto result = this->evse_security->delete_certificate(certificate_hash_data);

    ASSERT_EQ(result, DeleteCertificateResult::Accepted);
}

TEST_F(EvseSecurityTests, delete_root_ca_02) {
    CertificateHashData certificate_hash_data;
    certificate_hash_data.hash_algorithm = HashAlgorithm::SHA256;
    certificate_hash_data.issuer_key_hash = "UnknownKeyHash";
    certificate_hash_data.issuer_name_hash = "7da88c3366c19488ee810c5408f612db98164a34e05a0b15c93914fbed228c0f";
    certificate_hash_data.serial_number = "3046";
    const auto result = this->evse_security->delete_certificate(certificate_hash_data);

    ASSERT_EQ(result, DeleteCertificateResult::NotFound);
}

TEST_F(EvseSecurityTests, delete_sub_ca_1) {
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
    const auto subca1_hash_data = subca1_x509.get_certificate_hash_data(root_x509);

    ASSERT_EQ(this->evse_security->delete_certificate(subca1_hash_data), DeleteCertificateResult::Accepted);

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

    ASSERT_EQ(this->evse_security->delete_certificate(subca2_hash_data), DeleteCertificateResult::Accepted);

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

TEST_F(EvseSecurityTests, get_installed_certificates_and_delete_secc_leaf) {
    std::vector<CertificateType> certificate_types;
    certificate_types.push_back(CertificateType::V2GRootCertificate);
    certificate_types.push_back(CertificateType::MORootCertificate);
    certificate_types.push_back(CertificateType::CSMSRootCertificate);
    certificate_types.push_back(CertificateType::V2GCertificateChain);
    certificate_types.push_back(CertificateType::MFRootCertificate);

    const auto r = this->evse_security->get_installed_certificates(certificate_types);

    ASSERT_EQ(r.status, GetInstalledCertificatesStatus::Accepted);
    ASSERT_EQ(r.certificate_hash_data_chain.size(), 4);
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
    ASSERT_EQ(delete_response, DeleteCertificateResult::Failed);
}

TEST_F(EvseSecurityTests, fix_pem_string) {
    std::string test_input("-----BEGIN TEST INPUT-----\n");
    test_input += "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+/";
    test_input += "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+/\n";
    test_input += "abcdefghijklmnopqrstuvwxyzABCD EFGHIJKLMNOPQRSTUVWXYZ0123456789+/";
    test_input += "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKL\r\nMNOPQRSTUVWXYZ0123456789+/";
    test_input += "abcdefghijklmnopqrstuvwxyzABCDEFG==";
    test_input += "\n\n-----END TEST INPUT-----";

    std::string expected_output("-----BEGIN TEST INPUT-----\n");
    expected_output += "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+/\n";
    expected_output += "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+/\n";
    expected_output += "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+/\n";
    expected_output += "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789+/\n";
    expected_output += "abcdefghijklmnopqrstuvwxyzABCDEFG==\n";
    expected_output += "-----END TEST INPUT-----";

    ASSERT_EQ(fix_pem_string(test_input), expected_output);
}

TEST_F(EvseSecurityTests, leaf_cert_starts_in_future_accepted) {
    const auto v2g_keypair_before = this->evse_security->get_key_pair(LeafCertificateType::V2G, EncodingFormat::PEM);

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
    const auto v2g_keypair_after = this->evse_security->get_key_pair(LeafCertificateType::V2G, EncodingFormat::PEM);
    ASSERT_EQ(v2g_keypair_after.pair.value().certificate, v2g_keypair_before.pair.value().certificate);
    ASSERT_EQ(v2g_keypair_after.pair.value().key, v2g_keypair_before.pair.value().key);
    ASSERT_EQ(v2g_keypair_after.pair.value().password, v2g_keypair_before.pair.value().password);
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

} // namespace evse_security

// FIXME(piet): Add more tests for getRootCertificateHashData (incl. V2GCertificateChain etc.)
