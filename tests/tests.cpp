// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>

#include "evse_security.hpp"

std::string read_file_to_string(const std::filesystem::path filepath) {
    std::ifstream t(filepath.string());
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
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
        file_paths.csms_ca_bundle = std::filesystem::path("certs/ca/v2g/V2G_CA_BUNDLE.pem");
        file_paths.mf_ca_bundle = std::filesystem::path("certs/ca/v2g/V2G_CA_BUNDLE.pem");
        file_paths.mo_ca_bundle = std::filesystem::path("certs/ca/mo/MO_CA_BUNDLE.pem");
        file_paths.v2g_ca_bundle = std::filesystem::path("certs/ca/v2g/V2G_CA_BUNDLE.pem");
        file_paths.csms_leaf_cert_directory = std::filesystem::path("certs/client/csms/");
        file_paths.csms_leaf_key_directory = std::filesystem::path("certs/client/csms/");
        file_paths.secc_leaf_cert_directory = std::filesystem::path("certs/client/cso/");
        file_paths.secc_leaf_key_directory = std::filesystem::path("certs/client/cso/");

        this->evse_security = std::make_unique<EvseSecurity>(file_paths, "123456");
    }

    void TearDown() override {
        std::filesystem::remove_all("certs");
    }
};

/// \brief test verifyChargepointCertificate with valid cert
TEST_F(EvseSecurityTests, verify_chargepoint_cert_01) {
    const auto client_certificate = read_file_to_string(std::filesystem::path("certs/client/csms/CSMS_LEAF.pem"));
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
    const auto client_certificate = read_file_to_string(std::filesystem::path("certs/client/cso/SECC_LEAF.pem"));
    const auto result = this->evse_security->update_leaf_certificate(client_certificate, LeafCertificateType::V2G);
    ASSERT_TRUE(result == InstallCertificateResult::Accepted);
}

/// \brief test verifyV2GChargingStationCertificate with invalid cert
TEST_F(EvseSecurityTests, verify_v2g_cert_02) {
    const auto invalid_certificate =
        read_file_to_string(std::filesystem::path("certs/client/invalid/INVALID_CSMS.pem"));
    const auto result = this->evse_security->update_leaf_certificate(invalid_certificate, LeafCertificateType::V2G);
    ASSERT_TRUE(result == InstallCertificateResult::InvalidCertificateChain);
}

TEST_F(EvseSecurityTests, install_root_ca_01) {
    const auto v2g_root_ca = read_file_to_string(std::filesystem::path("certs/ca/v2g/V2G_ROOT_CA_NEW.pem"));
    const auto result = this->evse_security->install_ca_certificate(v2g_root_ca, CaCertificateType::V2G);
    ASSERT_TRUE(result == InstallCertificateResult::Accepted);
}

TEST_F(EvseSecurityTests, install_root_ca_02) {
    const auto invalid_csms_ca = "InvalidCertificate";
    const auto result = this->evse_security->install_ca_certificate(invalid_csms_ca, CaCertificateType::CSMS);
    ASSERT_EQ(result, InstallCertificateResult::InvalidFormat);
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

TEST_F(EvseSecurityTests, get_installed_certificates_and_delete_secc_leaf) {
    std::vector<CertificateType> certificate_types;
    certificate_types.push_back(CertificateType::V2GRootCertificate);
    certificate_types.push_back(CertificateType::MORootCertificate);
    certificate_types.push_back(CertificateType::CSMSRootCertificate);
    certificate_types.push_back(CertificateType::V2GCertificateChain);
    certificate_types.push_back(CertificateType::MFRootCertificate);

    const auto r = this->evse_security->get_installed_certificates(certificate_types);
    // ASSERT_EQ(r.status, GetInstalledCertificatesStatus::Accepted);

    ASSERT_EQ(r.status, GetInstalledCertificatesStatus::Accepted);
    ASSERT_EQ(r.certificate_hash_data_chain.size(), 4);
    bool found_v2g_chain = false;

    CertificateHashData secc_leaf_data;

    for (const auto& certificate_hash_data_chain : r.certificate_hash_data_chain) {
        if (certificate_hash_data_chain.certificate_type == CertificateType::V2GCertificateChain) {
            found_v2g_chain = true;
            secc_leaf_data = certificate_hash_data_chain.certificate_hash_data;
            ASSERT_EQ(2, certificate_hash_data_chain.child_certificate_hash_data.value().size());
        }
    }
    ASSERT_TRUE(found_v2g_chain);

    auto delete_response = this->evse_security->delete_certificate(secc_leaf_data);
    ASSERT_EQ(delete_response, DeleteCertificateResult::Accepted);

    const auto get_certs_response = this->evse_security->get_installed_certificates(certificate_types);
    // ASSERT_EQ(r.status, GetInstalledCertificatesStatus::Accepted);
    ASSERT_EQ(get_certs_response.certificate_hash_data_chain.size(), 3);

    delete_response = this->evse_security->delete_certificate(secc_leaf_data);
    ASSERT_EQ(delete_response, DeleteCertificateResult::NotFound);
}

} // namespace evse_security

// FIXME(piet): Add more tests for getRootCertificateHashData (incl. V2GCertificateChain etc.)
