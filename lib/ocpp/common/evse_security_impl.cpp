// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/evse_security_impl.hpp>

namespace ocpp {
EvseSecurityImpl::EvseSecurityImpl(const SecurityConfiguration& security_configuration) {
    evse_security::FilePaths file_paths;
    file_paths.csms_ca_bundle = security_configuration.csms_ca_bundle;
    file_paths.mf_ca_bundle = security_configuration.mf_ca_bundle;
    file_paths.mo_ca_bundle = security_configuration.mo_ca_bundle;
    file_paths.v2g_ca_bundle = security_configuration.v2g_ca_bundle;

    file_paths.directories.csms_leaf_cert_directory = security_configuration.csms_leaf_cert_directory;
    file_paths.directories.csms_leaf_key_directory = security_configuration.csms_leaf_key_directory;
    file_paths.directories.secc_leaf_cert_directory = security_configuration.secc_leaf_cert_directory;
    file_paths.directories.secc_leaf_key_directory = security_configuration.secc_leaf_key_directory;

    this->evse_security =
        std::make_unique<evse_security::EvseSecurity>(file_paths, security_configuration.private_key_password);
}

InstallCertificateResult EvseSecurityImpl::install_ca_certificate(const std::string& certificate,
                                                                  const CaCertificateType& certificate_type) {
    return conversions::to_ocpp(
        this->evse_security->install_ca_certificate(certificate, conversions::from_ocpp(certificate_type)));
}

DeleteCertificateResult EvseSecurityImpl::delete_certificate(const CertificateHashDataType& certificate_hash_data) {
    return conversions::to_ocpp(this->evse_security->delete_certificate(conversions::from_ocpp(certificate_hash_data)));
}

InstallCertificateResult EvseSecurityImpl::update_leaf_certificate(const std::string& certificate_chain,
                                                                   const CertificateSigningUseEnum& certificate_type) {
    return conversions::to_ocpp(
        this->evse_security->update_leaf_certificate(certificate_chain, conversions::from_ocpp(certificate_type)));
}

InstallCertificateResult EvseSecurityImpl::verify_certificate(const std::string& certificate_chain,
                                                              const CertificateSigningUseEnum& certificate_type) {
    return conversions::to_ocpp(
        this->evse_security->verify_certificate(certificate_chain, conversions::from_ocpp(certificate_type)));
}

std::vector<CertificateHashDataChain>
EvseSecurityImpl::get_installed_certificates(const std::vector<CertificateType>& certificate_types) {
    std::vector<CertificateHashDataChain> result;

    std::vector<evse_security::CertificateType> _certificate_types;

    for (const auto& certificate_type : certificate_types) {
        _certificate_types.push_back(conversions::from_ocpp(certificate_type));
    }

    const auto installed_certificates = this->evse_security->get_installed_certificates(_certificate_types);

    for (const auto& certificate_hash_data : installed_certificates.certificate_hash_data_chain) {
        result.push_back(conversions::to_ocpp(certificate_hash_data));
    }
    return result;
}

std::vector<OCSPRequestData> EvseSecurityImpl::get_ocsp_request_data() {
    std::vector<OCSPRequestData> result;

    const auto ocsp_request_data = this->evse_security->get_ocsp_request_data();
    for (const auto& ocsp_request_entry : ocsp_request_data.ocsp_request_data_list) {
        result.push_back(conversions::to_ocpp(ocsp_request_entry));
    }

    return result;
}
void EvseSecurityImpl::update_ocsp_cache(const CertificateHashDataType& certificate_hash_data,
                                         const std::string& ocsp_response) {
    this->evse_security->update_ocsp_cache(conversions::from_ocpp(certificate_hash_data), ocsp_response);
}

bool EvseSecurityImpl::is_ca_certificate_installed(const CaCertificateType& certificate_type) {
    return this->evse_security->is_ca_certificate_installed(conversions::from_ocpp(certificate_type));
}

std::string EvseSecurityImpl::generate_certificate_signing_request(const CertificateSigningUseEnum& certificate_type,
                                                                   const std::string& country,
                                                                   const std::string& organization,
                                                                   const std::string& common, bool use_tpm) {
    return this->evse_security->generate_certificate_signing_request(conversions::from_ocpp(certificate_type), country,
                                                                     organization, common, use_tpm);
}

std::optional<KeyPair> EvseSecurityImpl::get_key_pair(const CertificateSigningUseEnum& certificate_type) {
    const auto key_pair =
        this->evse_security->get_key_pair(conversions::from_ocpp(certificate_type), evse_security::EncodingFormat::PEM);

    if (key_pair.status == evse_security::GetKeyPairStatus::Accepted && key_pair.pair.has_value()) {
        return conversions::to_ocpp(key_pair.pair.value());
    } else {
        return std::nullopt;
    }
}

std::string EvseSecurityImpl::get_verify_file(const CaCertificateType& certificate_type) {
    return this->evse_security->get_verify_file(conversions::from_ocpp(certificate_type));
}

int EvseSecurityImpl::get_leaf_expiry_days_count(const CertificateSigningUseEnum& certificate_type) {
    return this->evse_security->get_leaf_expiry_days_count(conversions::from_ocpp(certificate_type));
}

namespace conversions {

CaCertificateType to_ocpp(evse_security::CaCertificateType other) {
    switch (other) {
    case evse_security::CaCertificateType::V2G:
        return CaCertificateType::V2G;
    case evse_security::CaCertificateType::MO:
        return CaCertificateType::MO;
    case evse_security::CaCertificateType::CSMS:
        return CaCertificateType::CSMS;
    case evse_security::CaCertificateType::MF:
        return CaCertificateType::MF;
    default:
        throw std::runtime_error("Could not convert evse_security::CaCertificateType to CaCertificateType");
    }
}

CertificateSigningUseEnum to_ocpp(evse_security::LeafCertificateType other) {
    switch (other) {
    case evse_security::LeafCertificateType::CSMS:
        return CertificateSigningUseEnum::ChargingStationCertificate;
    case evse_security::LeafCertificateType::V2G:
        return CertificateSigningUseEnum::V2GCertificate;
    case evse_security::LeafCertificateType::MF:
        return CertificateSigningUseEnum::ManufacturerCertificate;
    default:
        throw std::runtime_error("Could not convert evse_security::LeafCertificateType to CertificateSigningUseEnum");
    }
}

CertificateType to_ocpp(evse_security::CertificateType other) {
    switch (other) {
    case evse_security::CertificateType::V2GRootCertificate:
        return CertificateType::V2GRootCertificate;
    case evse_security::CertificateType::MORootCertificate:
        return CertificateType::MORootCertificate;
    case evse_security::CertificateType::CSMSRootCertificate:
        return CertificateType::CSMSRootCertificate;
    case evse_security::CertificateType::V2GCertificateChain:
        return CertificateType::V2GCertificateChain;
    case evse_security::CertificateType::MFRootCertificate:
        return CertificateType::MFRootCertificate;
    default:
        throw std::runtime_error("Could not convert evse_security::CertificateType to CertificateType");
    }
}

HashAlgorithmEnumType to_ocpp(evse_security::HashAlgorithm other) {
    switch (other) {
    case evse_security::HashAlgorithm::SHA256:
        return HashAlgorithmEnumType::SHA256;
    case evse_security::HashAlgorithm::SHA384:
        return HashAlgorithmEnumType::SHA384;
    case evse_security::HashAlgorithm::SHA512:
        return HashAlgorithmEnumType::SHA512;
    default:
        throw std::runtime_error("Could not convert evse_security::HashAlgorithm to HashAlgorithmEnumType");
    }
}

InstallCertificateResult to_ocpp(evse_security::InstallCertificateResult other) {
    switch (other) {
    case evse_security::InstallCertificateResult::InvalidSignature:
        return InstallCertificateResult::InvalidSignature;
    case evse_security::InstallCertificateResult::InvalidCertificateChain:
        return InstallCertificateResult::InvalidCertificateChain;
    case evse_security::InstallCertificateResult::InvalidFormat:
        return InstallCertificateResult::InvalidFormat;
    case evse_security::InstallCertificateResult::InvalidCommonName:
        return InstallCertificateResult::InvalidCommonName;
    case evse_security::InstallCertificateResult::NoRootCertificateInstalled:
        return InstallCertificateResult::NoRootCertificateInstalled;
    case evse_security::InstallCertificateResult::Expired:
        return InstallCertificateResult::Expired;
    case evse_security::InstallCertificateResult::CertificateStoreMaxLengthExceeded:
        return InstallCertificateResult::CertificateStoreMaxLengthExceeded;
    case evse_security::InstallCertificateResult::WriteError:
        return InstallCertificateResult::WriteError;
    case evse_security::InstallCertificateResult::Accepted:
        return InstallCertificateResult::Accepted;
    default:
        throw std::runtime_error(
            "Could not convert evse_security::InstallCertificateResult to InstallCertificateResult");
    }
}

DeleteCertificateResult to_ocpp(evse_security::DeleteCertificateResult other) {
    switch (other) {
    case evse_security::DeleteCertificateResult::Accepted:
        return DeleteCertificateResult::Accepted;
    case evse_security::DeleteCertificateResult::Failed:
        return DeleteCertificateResult::Failed;
    case evse_security::DeleteCertificateResult::NotFound:
        return DeleteCertificateResult::NotFound;
    default:
        throw std::runtime_error("Could not convert evse_security::DeleteCertificateResult to DeleteCertificateResult");
    }
}

CertificateHashDataType to_ocpp(evse_security::CertificateHashData other) {
    CertificateHashDataType lhs;
    lhs.hashAlgorithm = to_ocpp(other.hash_algorithm);
    lhs.issuerNameHash = other.issuer_name_hash;
    lhs.issuerKeyHash = other.issuer_key_hash;
    lhs.serialNumber = other.serial_number;
    return lhs;
}

CertificateHashDataChain to_ocpp(evse_security::CertificateHashDataChain other) {
    CertificateHashDataChain lhs;
    lhs.certificateType = to_ocpp(other.certificate_type);
    lhs.certificateHashData = to_ocpp(other.certificate_hash_data);

    std::vector<CertificateHashDataType> v;
    for (const auto& certificate_hash_data : other.child_certificate_hash_data) {
        v.push_back(to_ocpp(certificate_hash_data));
    }
    lhs.childCertificateHashData = v;

    return lhs;
}

OCSPRequestData to_ocpp(evse_security::OCSPRequestData other) {
    OCSPRequestData lhs;
    if (other.certificate_hash_data.has_value()) {
        lhs.issuerNameHash = other.certificate_hash_data.value().issuer_name_hash;
        lhs.issuerKeyHash = other.certificate_hash_data.value().issuer_key_hash;
        lhs.serialNumber = other.certificate_hash_data.value().serial_number;
        lhs.hashAlgorithm = to_ocpp(other.certificate_hash_data.value().hash_algorithm);
    }
    if (other.responder_url.has_value()) {
        lhs.responderUrl = other.responder_url.value();
    }
    return lhs;
}

KeyPair to_ocpp(evse_security::KeyPair other) {
    KeyPair lhs;
    lhs.certificate_path = other.certificate;
    lhs.key_path = other.key;
    lhs.password = other.password;
    return lhs;
}

evse_security::CaCertificateType from_ocpp(CaCertificateType other) {
    switch (other) {
    case CaCertificateType::V2G:
        return evse_security::CaCertificateType::V2G;
    case CaCertificateType::MO:
        return evse_security::CaCertificateType::MO;
    case CaCertificateType::CSMS:
        return evse_security::CaCertificateType::CSMS;
    case CaCertificateType::MF:
        return evse_security::CaCertificateType::MF;
    default:
        throw std::runtime_error("Could not convert evse_security::CaCertificateType to CaCertificateType");
    }
}

evse_security::LeafCertificateType from_ocpp(CertificateSigningUseEnum other) {
    switch (other) {
    case CertificateSigningUseEnum::ChargingStationCertificate:
        return evse_security::LeafCertificateType::CSMS;
    case CertificateSigningUseEnum::V2GCertificate:
        return evse_security::LeafCertificateType::V2G;
    case CertificateSigningUseEnum::ManufacturerCertificate:
        return evse_security::LeafCertificateType::MF;
    default:
        throw std::runtime_error("Could not convert CertificateSigningUseEnum to evse_security::LeafCertificateType");
    }
}

evse_security::CertificateType from_ocpp(CertificateType other) {
    switch (other) {
    case CertificateType::V2GRootCertificate:
        return evse_security::CertificateType::V2GRootCertificate;
    case CertificateType::MORootCertificate:
        return evse_security::CertificateType::MORootCertificate;
    case CertificateType::CSMSRootCertificate:
        return evse_security::CertificateType::CSMSRootCertificate;
    case CertificateType::V2GCertificateChain:
        return evse_security::CertificateType::V2GCertificateChain;
    case CertificateType::MFRootCertificate:
        return evse_security::CertificateType::MFRootCertificate;
    default:
        throw std::runtime_error("Could not convert CertificateType to evse_security::CertificateType");
    }
}

evse_security::HashAlgorithm from_ocpp(HashAlgorithmEnumType other) {
    switch (other) {
    case HashAlgorithmEnumType::SHA256:
        return evse_security::HashAlgorithm::SHA256;
    case HashAlgorithmEnumType::SHA384:
        return evse_security::HashAlgorithm::SHA384;
    case HashAlgorithmEnumType::SHA512:
        return evse_security::HashAlgorithm::SHA512;
    default:
        throw std::runtime_error("Could not convert HashAlgorithmEnumType to evse_security::HashAlgorithm");
    }
}

evse_security::InstallCertificateResult from_ocpp(InstallCertificateResult other) {
    switch (other) {
    case InstallCertificateResult::InvalidSignature:
        return evse_security::InstallCertificateResult::InvalidSignature;
    case InstallCertificateResult::InvalidCertificateChain:
        return evse_security::InstallCertificateResult::InvalidCertificateChain;
    case InstallCertificateResult::InvalidFormat:
        return evse_security::InstallCertificateResult::InvalidFormat;
    case InstallCertificateResult::InvalidCommonName:
        return evse_security::InstallCertificateResult::InvalidCommonName;
    case InstallCertificateResult::NoRootCertificateInstalled:
        return evse_security::InstallCertificateResult::NoRootCertificateInstalled;
    case InstallCertificateResult::Expired:
        return evse_security::InstallCertificateResult::Expired;
    case InstallCertificateResult::CertificateStoreMaxLengthExceeded:
        return evse_security::InstallCertificateResult::CertificateStoreMaxLengthExceeded;
    case InstallCertificateResult::WriteError:
        return evse_security::InstallCertificateResult::WriteError;
    case InstallCertificateResult::Accepted:
        return evse_security::InstallCertificateResult::Accepted;
    default:
        throw std::runtime_error(
            "Could not convert InstallCertificateResult to evse_security::InstallCertificateResult");
    }
}

evse_security::DeleteCertificateResult from_ocpp(DeleteCertificateResult other) {
    switch (other) {
    case DeleteCertificateResult::Accepted:
        return evse_security::DeleteCertificateResult::Accepted;
    case DeleteCertificateResult::Failed:
        return evse_security::DeleteCertificateResult::Failed;
    case DeleteCertificateResult::NotFound:
        return evse_security::DeleteCertificateResult::NotFound;
    default:
        throw std::runtime_error("Could not convert DeleteCertificateResult to evse_security::DeleteCertificateResult");
    }
}

evse_security::CertificateHashData from_ocpp(CertificateHashDataType other) {
    evse_security::CertificateHashData lhs;
    lhs.hash_algorithm = from_ocpp(other.hashAlgorithm);
    lhs.issuer_name_hash = other.issuerNameHash;
    lhs.issuer_key_hash = other.issuerKeyHash;
    lhs.serial_number = other.serialNumber;
    return lhs;
}

evse_security::CertificateHashDataChain from_ocpp(CertificateHashDataChain other) {
    evse_security::CertificateHashDataChain lhs;
    lhs.certificate_type = from_ocpp(other.certificateType);
    lhs.certificate_hash_data = from_ocpp(other.certificateHashData);
    if (other.childCertificateHashData.has_value()) {
        std::vector<evse_security::CertificateHashData> v;
        for (const auto& certificate_hash_data : other.childCertificateHashData.value()) {
            v.push_back(from_ocpp(certificate_hash_data));
        }
        lhs.child_certificate_hash_data = v;
    }
    return lhs;
}

evse_security::OCSPRequestData from_ocpp(OCSPRequestData other) {
    evse_security::OCSPRequestData lhs;
    evse_security::CertificateHashData certificate_hash_data;
    certificate_hash_data.issuer_name_hash = other.issuerNameHash;
    certificate_hash_data.issuer_key_hash = other.issuerKeyHash;
    certificate_hash_data.serial_number = other.serialNumber;
    certificate_hash_data.hash_algorithm = from_ocpp(other.hashAlgorithm);
    lhs.certificate_hash_data = certificate_hash_data;
    lhs.responder_url = other.responderUrl;
    return lhs;
}

evse_security::KeyPair from_ocpp(KeyPair other) {
    evse_security::KeyPair lhs;
    lhs.key = other.certificate_path;
    lhs.certificate = other.key_path;
    return lhs;
}

} // namespace conversions

} // namespace ocpp