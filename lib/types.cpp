// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <types.hpp>

namespace evse_security {

namespace conversions {

std::string encoding_format_to_string(EncodingFormat e) {
    switch (e) {
    case EncodingFormat::DER:
        return "DER";
    case EncodingFormat::PEM:
        return "PEM";
    default:
        throw std::out_of_range("Could not convert EncodingFormat to string");
    }
};

std::string ca_certificate_type_to_string(CaCertificateType e) {
    switch (e) {
    case CaCertificateType::V2G:
        return "V2G";
    case CaCertificateType::MO:
        return "MO";
    case CaCertificateType::CSMS:
        return "CSMS";
    case CaCertificateType::MF:
        return "MF";
    default:
        throw std::out_of_range("Could not convert CaCertificateType to string");
    }
};

std::string leaf_certificate_type_to_string(LeafCertificateType e) {
    switch (e) {
    case LeafCertificateType::CSMS:
        return "CSMS";
    case LeafCertificateType::V2G:
        return "V2G";
    case LeafCertificateType::MF:
        return "MF";
    default:
        throw std::out_of_range("Could not convert LeafCertificateType to string");
    }
};

std::string certificate_type_to_string(CertificateType e) {
    switch (e) {
    case CertificateType::V2GRootCertificate:
        return "V2GRootCertificate";
    case CertificateType::MORootCertificate:
        return "MORootCertificate";
    case CertificateType::CSMSRootCertificate:
        return "CSMSRootCertificate";
    case CertificateType::V2GCertificateChain:
        return "V2GCertificateChain";
    case CertificateType::MFRootCertificate:
        return "MFRootCertificate";
    default:
        throw std::out_of_range("Could not convert CertificateType to string");
    }
};

std::string hash_algorithm_to_string(HashAlgorithm e) {
    switch (e) {
    case HashAlgorithm::SHA256:
        return "SHA256";
    case HashAlgorithm::SHA384:
        return "SHA384";
    case HashAlgorithm::SHA512:
        return "SHA512";
    default:
        throw std::out_of_range("Could not convert HashAlgorithm to string");
    }
};

std::string install_certificate_result_to_string(InstallCertificateResult e) {
    switch (e) {
    case InstallCertificateResult::InvalidSignature:
        return "InvalidSignature";
    case InstallCertificateResult::InvalidCertificateChain:
        return "InvalidCertificateChain";
    case InstallCertificateResult::InvalidFormat:
        return "InvalidFormat";
    case InstallCertificateResult::InvalidCommonName:
        return "InvalidCommonName";
    case InstallCertificateResult::NoRootCertificateInstalled:
        return "NoRootCertificateInstalled";
    case InstallCertificateResult::Expired:
        return "Expired";
    case InstallCertificateResult::CertificateStoreMaxLengthExceeded:
        return "CertificateStoreMaxLengthExceeded";
    case InstallCertificateResult::WriteError:
        return "WriteError";
    case InstallCertificateResult::Accepted:
        return "Accepted";
    default:
        throw std::out_of_range("Could not convert InstallCertificateResult to string");
    }
};

std::string delete_certificate_result_to_string(DeleteCertificateResult e) {
    switch (e) {
    case DeleteCertificateResult::Accepted:
        return "Accepted";
    case DeleteCertificateResult::Failed:
        return "Failed";
    case DeleteCertificateResult::NotFound:
        return "NotFound";
    default:
        throw std::out_of_range("Could not convert DeleteCertificateResult to string");
    }
};

std::string get_installed_certificates_status_to_string(GetInstalledCertificatesStatus e) {
    switch (e) {
    case GetInstalledCertificatesStatus::Accepted:
        return "Accepted";
    case GetInstalledCertificatesStatus::NotFound:
        return "NotFound";
    default:
        throw std::out_of_range("Could not convert GetInstalledCertificatesStatus to string");
    }
};

std::string get_key_pair_status_to_string(GetKeyPairStatus e) {
    switch (e) {
    case GetKeyPairStatus::Accepted:
        return "Accepted";
    case GetKeyPairStatus::Rejected:
        return "Rejected";
    case GetKeyPairStatus::NotFound:
        return "NotFound";
    case GetKeyPairStatus::NotFoundValid:
        return "NotFoundValid";
    case GetKeyPairStatus::PrivateKeyNotFound:
        return "PrivateKeyNotFound";
    default:
        throw std::out_of_range("Could not convert GetKeyPairStatus to string");
    }
};

} // namespace conversions

} // namespace evse_security
