// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <filesystem>
#include <gtest/gtest.h>
#include <fstream>
#include <sstream>

#include <everest/logging.hpp>

#include <ocpp/common/pki_handler.hpp>

std::string read_file_to_string(const std::filesystem::path filepath) {
    std::ifstream t(filepath.string());
    std::stringstream buffer;
    buffer << t.rdbuf();
    return buffer.str();
}

void install_certs() {
    std::system("./generate_test_certs.sh");
}

class PkiHandlerTests : public ::testing::Test {
protected:
    std::unique_ptr<ocpp::PkiHandler> pki_handler;

    void SetUp() override {
        install_certs();
        this->pki_handler = std::make_unique<ocpp::PkiHandler>(std::filesystem::path("certs"), false);
    }

    void TearDown() override {
        std::filesystem::remove_all("certs");
    }
};

/// \brief test verifyChargepointCertificate with valid cert
TEST_F(PkiHandlerTests, verify_chargepoint_cert_01) {
    const auto client_certificate = read_file_to_string(std::filesystem::path("certs/client/csms/CSMS_LEAF.pem"));
    const auto result = this->pki_handler->verifyChargepointCertificate(client_certificate, "SECCCert");
    ASSERT_TRUE(result == ocpp::CertificateVerificationResult::Valid);
}

/// \brief test verifyChargepointCertificate with invalid cert
TEST_F(PkiHandlerTests, verify_chargepoint_cert_02) {
    const auto result = this->pki_handler->verifyChargepointCertificate("InvalidCertificate", "SECCCert");
    ASSERT_TRUE(result == ocpp::CertificateVerificationResult::InvalidCertificateChain);
}

/// \brief test verifyV2GChargingStationCertificate with valid cert
TEST_F(PkiHandlerTests, verify_v2g_cert_01) {
    const auto client_certificate = read_file_to_string(std::filesystem::path("certs/client/cso/SECC_LEAF.pem"));
    const auto result = this->pki_handler->verifyV2GChargingStationCertificate(client_certificate, "SECCCert");
    ASSERT_TRUE(result == ocpp::CertificateVerificationResult::Valid);
}

/// \brief test verifyV2GChargingStationCertificate with invalid cert
TEST_F(PkiHandlerTests, verify_v2g_cert_02) {
    const auto result = this->pki_handler->verifyV2GChargingStationCertificate("InvalidCertificate", "SECCCert");
    ASSERT_TRUE(result == ocpp::CertificateVerificationResult::InvalidCertificateChain);
}

TEST_F(PkiHandlerTests, install_root_ca_01) {
    const auto v2g_root_ca = read_file_to_string(std::filesystem::path("certs/ca/v2g/V2G_ROOT_CA.pem"));
    std::filesystem::remove("certs/ca/v2g/V2G_ROOT_CA.pem");
    const auto result = this->pki_handler->installRootCertificate(
        v2g_root_ca, ocpp::CertificateType::V2GRootCertificate, std::nullopt, std::nullopt);
    ASSERT_TRUE(result == ocpp::InstallCertificateResult::Ok);
}

TEST_F(PkiHandlerTests, install_root_ca_02) {
    const auto invalid_csms_ca = "InvalidCertificate";
    const auto result = this->pki_handler->installRootCertificate(
        invalid_csms_ca, ocpp::CertificateType::CentralSystemRootCertificate, std::nullopt, true);
    ASSERT_EQ(result, ocpp::InstallCertificateResult::InvalidFormat);
}

TEST_F(PkiHandlerTests, install_root_ca_03) {
    const auto invalid_ca = read_file_to_string(std::filesystem::path("certs/ca/invalid/INVALID_CA.pem"));
    const auto result = this->pki_handler->installRootCertificate(
        invalid_ca, ocpp::CertificateType::CentralSystemRootCertificate, std::nullopt, true);
    ASSERT_EQ(result, ocpp::InstallCertificateResult::InvalidCertificateChain);
}

TEST_F(PkiHandlerTests, install_root_ca_04) {
    const auto invalid_csms_ca = read_file_to_string(std::filesystem::path("certs/INVALID_CERT.pem"));
    const auto result = this->pki_handler->installRootCertificate(
        invalid_csms_ca, ocpp::CertificateType::CentralSystemRootCertificate, 2, true);
    ASSERT_EQ(result, ocpp::InstallCertificateResult::CertificateStoreMaxLengthExceeded);
}

TEST_F(PkiHandlerTests, delete_root_ca_01) {

    const auto root_certs = this->pki_handler->getRootCertificateHashData(std::nullopt);

    ocpp::CertificateHashDataType certificate_hash_data;
    certificate_hash_data.hashAlgorithm = ocpp::HashAlgorithmEnumType::SHA256;
    certificate_hash_data.issuerKeyHash = root_certs.value().at(0).certificateHashData.issuerKeyHash.get();
    certificate_hash_data.issuerNameHash = root_certs.value().at(0).certificateHashData.issuerNameHash.get();
    certificate_hash_data.serialNumber = root_certs.value().at(0).certificateHashData.serialNumber.get();
    const auto result = this->pki_handler->deleteRootCertificate(certificate_hash_data, 1);

    ASSERT_EQ(result, ocpp::DeleteCertificateResult::Accepted);
}

TEST_F(PkiHandlerTests, delete_root_ca_02) {
    ocpp::CertificateHashDataType certificate_hash_data;
    certificate_hash_data.hashAlgorithm = ocpp::HashAlgorithmEnumType::SHA256;
    certificate_hash_data.issuerKeyHash = "UnknownKeyHash";
    certificate_hash_data.issuerNameHash = "7da88c3366c19488ee810c5408f612db98164a34e05a0b15c93914fbed228c0f";
    certificate_hash_data.serialNumber = "3046";
    const auto result = this->pki_handler->deleteRootCertificate(certificate_hash_data, 1);

    ASSERT_EQ(result, ocpp::DeleteCertificateResult::NotFound);
}

TEST_F(PkiHandlerTests, get_root_certificate_hash_data) {
    std::vector<ocpp::CertificateType> certificateTypes;
    certificateTypes.push_back(ocpp::CertificateType::MORootCertificate);
    this->pki_handler->getRootCertificateHashData(certificateTypes);
}

// FIXME(piet): Add more tests for getRootCertificateHashData (incl. V2GCertificateChain etc.)
