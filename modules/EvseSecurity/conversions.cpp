// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <exception>

#include <conversions.hpp>

namespace module {

namespace conversions {

evse_security::EncodingFormat from_everest(types::evse_security::EncodingFormat other) {
    switch (other) {
    case types::evse_security::EncodingFormat::PEM:
        return evse_security::EncodingFormat::PEM;
    case types::evse_security::EncodingFormat::DER:
        return evse_security::EncodingFormat::DER;
    default:
        throw std::runtime_error(
            "Could not convert types::evse_security::EncodingFormat to evse_security::EncodingFormat");
    }
}

evse_security::CaCertificateType from_everest(types::evse_security::CaCertificateType other) {
    switch (other) {
    case types::evse_security::CaCertificateType::V2G:
        return evse_security::CaCertificateType::V2G;
    case types::evse_security::CaCertificateType::MO:
        return evse_security::CaCertificateType::MO;
    case types::evse_security::CaCertificateType::CSMS:
        return evse_security::CaCertificateType::CSMS;
    case types::evse_security::CaCertificateType::MF:
        return evse_security::CaCertificateType::MF;
    default:
        throw std::runtime_error(
            "Could not convert types::evse_security::CaCertificateType to evse_security::CaCertificateType");
    }
}

evse_security::LeafCertificateType from_everest(types::evse_security::LeafCertificateType other) {
    switch (other) {
    case types::evse_security::LeafCertificateType::CSMS:
        return evse_security::LeafCertificateType::CSMS;
    case types::evse_security::LeafCertificateType::V2G:
        return evse_security::LeafCertificateType::V2G;
    case types::evse_security::LeafCertificateType::MF:
        return evse_security::LeafCertificateType::MF;
    case types::evse_security::LeafCertificateType::MO:
        return evse_security::LeafCertificateType::MO;
    default:
        throw std::runtime_error(
            "Could not convert types::evse_security::LeafCertificateType to evse_security::LeafCertificateType");
    }
}

evse_security::CertificateType from_everest(types::evse_security::CertificateType other) {
    switch (other) {
    case types::evse_security::CertificateType::V2GRootCertificate:
        return evse_security::CertificateType::V2GRootCertificate;
    case types::evse_security::CertificateType::MORootCertificate:
        return evse_security::CertificateType::MORootCertificate;
    case types::evse_security::CertificateType::CSMSRootCertificate:
        return evse_security::CertificateType::CSMSRootCertificate;
    case types::evse_security::CertificateType::V2GCertificateChain:
        return evse_security::CertificateType::V2GCertificateChain;
    case types::evse_security::CertificateType::MFRootCertificate:
        return evse_security::CertificateType::MFRootCertificate;
    default:
        throw std::runtime_error(
            "Could not convert types::evse_security::CertificateType to evse_security::CertificateType");
    }
}

evse_security::HashAlgorithm from_everest(types::evse_security::HashAlgorithm other) {
    switch (other) {
    case types::evse_security::HashAlgorithm::SHA256:
        return evse_security::HashAlgorithm::SHA256;
    case types::evse_security::HashAlgorithm::SHA384:
        return evse_security::HashAlgorithm::SHA384;
    case types::evse_security::HashAlgorithm::SHA512:
        return evse_security::HashAlgorithm::SHA512;
    default:
        throw std::runtime_error(
            "Could not convert types::evse_security::HashAlgorithm to evse_security::HashAlgorithm");
    }
}

evse_security::InstallCertificateResult from_everest(types::evse_security::InstallCertificateResult other) {
    switch (other) {
    case types::evse_security::InstallCertificateResult::InvalidSignature:
        return evse_security::InstallCertificateResult::InvalidSignature;
    case types::evse_security::InstallCertificateResult::InvalidCertificateChain:
        return evse_security::InstallCertificateResult::InvalidCertificateChain;
    case types::evse_security::InstallCertificateResult::InvalidFormat:
        return evse_security::InstallCertificateResult::InvalidFormat;
    case types::evse_security::InstallCertificateResult::InvalidCommonName:
        return evse_security::InstallCertificateResult::InvalidCommonName;
    case types::evse_security::InstallCertificateResult::NoRootCertificateInstalled:
        return evse_security::InstallCertificateResult::NoRootCertificateInstalled;
    case types::evse_security::InstallCertificateResult::Expired:
        return evse_security::InstallCertificateResult::Expired;
    case types::evse_security::InstallCertificateResult::CertificateStoreMaxLengthExceeded:
        return evse_security::InstallCertificateResult::CertificateStoreMaxLengthExceeded;
    case types::evse_security::InstallCertificateResult::WriteError:
        return evse_security::InstallCertificateResult::WriteError;
    case types::evse_security::InstallCertificateResult::Accepted:
        return evse_security::InstallCertificateResult::Accepted;
    default:
        throw std::runtime_error("Could not convert types::evse_security::InstallCertificateResult to "
                                 "evse_security::InstallCertificateResult");
    }
}

evse_security::DeleteCertificateResult from_everest(types::evse_security::DeleteCertificateResult other) {
    switch (other) {
    case types::evse_security::DeleteCertificateResult::Accepted:
        return evse_security::DeleteCertificateResult::Accepted;
    case types::evse_security::DeleteCertificateResult::Failed:
        return evse_security::DeleteCertificateResult::Failed;
    case types::evse_security::DeleteCertificateResult::NotFound:
        return evse_security::DeleteCertificateResult::NotFound;
    default:
        throw std::runtime_error("Could not convert types::evse_security::DeleteCertificateResult to "
                                 "evse_security::DeleteCertificateResult");
    }
}

evse_security::GetInstalledCertificatesStatus from_everest(types::evse_security::GetInstalledCertificatesStatus other) {
    switch (other) {
    case types::evse_security::GetInstalledCertificatesStatus::Accepted:
        return evse_security::GetInstalledCertificatesStatus::Accepted;
    case types::evse_security::GetInstalledCertificatesStatus::NotFound:
        return evse_security::GetInstalledCertificatesStatus::NotFound;
    default:
        throw std::runtime_error("Could not convert types::evse_security::GetInstalledCertificatesStatus to "
                                 "evse_security::GetInstalledCertificatesStatus");
    }
}

evse_security::CertificateHashData from_everest(types::evse_security::CertificateHashData other) {
    evse_security::CertificateHashData lhs;
    lhs.hash_algorithm = from_everest(other.hash_algorithm);
    lhs.issuer_name_hash = other.issuer_name_hash;
    lhs.issuer_key_hash = other.issuer_key_hash;
    lhs.serial_number = other.serial_number;
    return lhs;
}

evse_security::CertificateHashDataChain from_everest(types::evse_security::CertificateHashDataChain other) {
    evse_security::CertificateHashDataChain lhs;
    lhs.certificate_type = from_everest(other.certificate_type);
    lhs.certificate_hash_data = from_everest(other.certificate_hash_data);
    if (other.child_certificate_hash_data.has_value()) {
        std::vector<evse_security::CertificateHashData> v;
        for (const auto& certificate_hash_data : other.child_certificate_hash_data.value()) {
            v.push_back(from_everest(certificate_hash_data));
        }
        lhs.child_certificate_hash_data = v;
    }
    return lhs;
}

evse_security::GetInstalledCertificatesResult from_everest(types::evse_security::GetInstalledCertificatesResult other) {
    evse_security::GetInstalledCertificatesResult lhs;
    lhs.status = from_everest(other.status);
    for (const auto& certificate_hash_data_chain : other.certificate_hash_data_chain) {
        lhs.certificate_hash_data_chain.push_back(from_everest(certificate_hash_data_chain));
    }
    return lhs;
}

evse_security::OCSPRequestData from_everest(types::evse_security::OCSPRequestData other) {
    evse_security::OCSPRequestData lhs;
    if (other.certificate_hash_data.has_value()) {
        lhs.certificate_hash_data = from_everest(other.certificate_hash_data.value());
    }
    if (other.responder_url.has_value()) {
        lhs.responder_url = other.responder_url.value();
    }
    return lhs;
}

evse_security::OCSPRequestDataList from_everest(types::evse_security::OCSPRequestDataList other) {
    evse_security::OCSPRequestDataList lhs;
    for (const auto& ocsp_request_data : other.ocsp_request_data_list) {
        lhs.ocsp_request_data_list.push_back(from_everest(ocsp_request_data));
    }
    return lhs;
}

evse_security::CertificateOCSP from_everest(types::evse_security::CertificateOCSP other) {
    evse_security::CertificateOCSP lhs;
    lhs.hash = from_everest(other.hash);

    if (other.ocsp_path.has_value()) {
        lhs.ocsp_path = other.ocsp_path.value();
    }

    return lhs;
}

evse_security::CertificateInfo from_everest(types::evse_security::CertificateInfo other) {
    evse_security::CertificateInfo lhs;
    lhs.key = other.key;
    lhs.certificate = other.certificate;
    lhs.certificate_single = other.certificate_single;
    lhs.certificate_count = other.certificate_count;
    lhs.password = other.password;
    if (other.ocsp.has_value()) {
        for (auto& ocsp_data : other.ocsp.value()) {
            lhs.ocsp.push_back(from_everest(ocsp_data));
        }
    }
    return lhs;
}

types::evse_security::EncodingFormat to_everest(evse_security::EncodingFormat other) {
    switch (other) {
    case evse_security::EncodingFormat::PEM:
        return types::evse_security::EncodingFormat::PEM;
    case evse_security::EncodingFormat::DER:
        return types::evse_security::EncodingFormat::DER;
    default:
        throw std::runtime_error(
            "Could not convert evse_security::EncodingFormat to types::evse_security::EncodingFormat");
    }
}

types::evse_security::CaCertificateType to_everest(evse_security::CaCertificateType other) {
    switch (other) {
    case evse_security::CaCertificateType::V2G:
        return types::evse_security::CaCertificateType::V2G;
    case evse_security::CaCertificateType::MO:
        return types::evse_security::CaCertificateType::MO;
    case evse_security::CaCertificateType::CSMS:
        return types::evse_security::CaCertificateType::CSMS;
    case evse_security::CaCertificateType::MF:
        return types::evse_security::CaCertificateType::MF;
    default:
        throw std::runtime_error(
            "Could not convert evse_security::CaCertificateType to types::evse_security::CaCertificateType");
    }
}

types::evse_security::LeafCertificateType to_everest(evse_security::LeafCertificateType other) {
    switch (other) {
    case evse_security::LeafCertificateType::CSMS:
        return types::evse_security::LeafCertificateType::CSMS;
    case evse_security::LeafCertificateType::V2G:
        return types::evse_security::LeafCertificateType::V2G;
    case evse_security::LeafCertificateType::MF:
        return types::evse_security::LeafCertificateType::MF;
    case evse_security::LeafCertificateType::MO:
        return types::evse_security::LeafCertificateType::MO;
    default:
        throw std::runtime_error(
            "Could not convert evse_security::LeafCertificateType to types::evse_security::LeafCertificateType");
    }
}

types::evse_security::CertificateType to_everest(evse_security::CertificateType other) {
    switch (other) {
    case evse_security::CertificateType::V2GRootCertificate:
        return types::evse_security::CertificateType::V2GRootCertificate;
    case evse_security::CertificateType::MORootCertificate:
        return types::evse_security::CertificateType::MORootCertificate;
    case evse_security::CertificateType::CSMSRootCertificate:
        return types::evse_security::CertificateType::CSMSRootCertificate;
    case evse_security::CertificateType::V2GCertificateChain:
        return types::evse_security::CertificateType::V2GCertificateChain;
    case evse_security::CertificateType::MFRootCertificate:
        return types::evse_security::CertificateType::MFRootCertificate;
    default:
        throw std::runtime_error(
            "Could not convert evse_security::CertificateType to types::evse_security::CertificateType");
    }
}

types::evse_security::HashAlgorithm to_everest(evse_security::HashAlgorithm other) {
    switch (other) {
    case evse_security::HashAlgorithm::SHA256:
        return types::evse_security::HashAlgorithm::SHA256;
    case evse_security::HashAlgorithm::SHA384:
        return types::evse_security::HashAlgorithm::SHA384;
    case evse_security::HashAlgorithm::SHA512:
        return types::evse_security::HashAlgorithm::SHA512;
    default:
        throw std::runtime_error(
            "Could not convert evse_security::HashAlgorithm to types::evse_security::HashAlgorithm");
    }
}

types::evse_security::InstallCertificateResult to_everest(evse_security::InstallCertificateResult other) {
    switch (other) {
    case evse_security::InstallCertificateResult::InvalidSignature:
        return types::evse_security::InstallCertificateResult::InvalidSignature;
    case evse_security::InstallCertificateResult::InvalidCertificateChain:
        return types::evse_security::InstallCertificateResult::InvalidCertificateChain;
    case evse_security::InstallCertificateResult::InvalidFormat:
        return types::evse_security::InstallCertificateResult::InvalidFormat;
    case evse_security::InstallCertificateResult::InvalidCommonName:
        return types::evse_security::InstallCertificateResult::InvalidCommonName;
    case evse_security::InstallCertificateResult::NoRootCertificateInstalled:
        return types::evse_security::InstallCertificateResult::NoRootCertificateInstalled;
    case evse_security::InstallCertificateResult::Expired:
        return types::evse_security::InstallCertificateResult::Expired;
    case evse_security::InstallCertificateResult::CertificateStoreMaxLengthExceeded:
        return types::evse_security::InstallCertificateResult::CertificateStoreMaxLengthExceeded;
    case evse_security::InstallCertificateResult::WriteError:
        return types::evse_security::InstallCertificateResult::WriteError;
    case evse_security::InstallCertificateResult::Accepted:
        return types::evse_security::InstallCertificateResult::Accepted;
    default:
        throw std::runtime_error("Could not convert evse_security::InstallCertificateResult to "
                                 "types::evse_security::InstallCertificateResult");
    }
}

types::evse_security::CertificateValidationResult to_everest(evse_security::CertificateValidationResult other) {
    switch (other) {
    case evse_security::CertificateValidationResult::Valid:
        return types::evse_security::CertificateValidationResult::Valid;
    case evse_security::CertificateValidationResult::InvalidSignature:
        return types::evse_security::CertificateValidationResult::InvalidSignature;
    case evse_security::CertificateValidationResult::IssuerNotFound:
        return types::evse_security::CertificateValidationResult::IssuerNotFound;
    case evse_security::CertificateValidationResult::InvalidLeafSignature:
        return types::evse_security::CertificateValidationResult::InvalidLeafSignature;
    case evse_security::CertificateValidationResult::InvalidChain:
        return types::evse_security::CertificateValidationResult::InvalidChain;
    case evse_security::CertificateValidationResult::Unknown:
        return types::evse_security::CertificateValidationResult::Unknown;
        ;
    default:
        throw std::runtime_error("Could not convert evse_security::CertificateValidationResult to "
                                 "types::evse_security::CertificateValidationResult");
    }
}

types::evse_security::DeleteCertificateResult to_everest(evse_security::DeleteCertificateResult other) {
    switch (other) {
    case evse_security::DeleteCertificateResult::Accepted:
        return types::evse_security::DeleteCertificateResult::Accepted;
    case evse_security::DeleteCertificateResult::Failed:
        return types::evse_security::DeleteCertificateResult::Failed;
    case evse_security::DeleteCertificateResult::NotFound:
        return types::evse_security::DeleteCertificateResult::NotFound;
    default:
        throw std::runtime_error("Could not convert evse_security::DeleteCertificateResult to "
                                 "types::evse_security::DeleteCertificateResult");
    }
}

types::evse_security::GetInstalledCertificatesStatus to_everest(evse_security::GetInstalledCertificatesStatus other) {
    switch (other) {
    case evse_security::GetInstalledCertificatesStatus::Accepted:
        return types::evse_security::GetInstalledCertificatesStatus::Accepted;
    case evse_security::GetInstalledCertificatesStatus::NotFound:
        return types::evse_security::GetInstalledCertificatesStatus::NotFound;
    default:
        throw std::runtime_error("Could not convert evse_security::GetInstalledCertificatesStatus to "
                                 "types::evse_security::GetInstalledCertificatesStatus");
    }
}

types::evse_security::GetCertificateSignRequestStatus to_everest(evse_security::GetCertificateSignRequestStatus other) {
    switch (other) {
    case evse_security::GetCertificateSignRequestStatus::Accepted:
        return types::evse_security::GetCertificateSignRequestStatus::Accepted;
    case evse_security::GetCertificateSignRequestStatus::InvalidRequestedType:
        return types::evse_security::GetCertificateSignRequestStatus::InvalidRequestedType;
    case evse_security::GetCertificateSignRequestStatus::KeyGenError:
        return types::evse_security::GetCertificateSignRequestStatus::KeyGenError;
    case evse_security::GetCertificateSignRequestStatus::GenerationError:
        return types::evse_security::GetCertificateSignRequestStatus::GenerationError;
    default:
        throw std::runtime_error("Could not convert evse_security::GetCertificateSignRequestStatus to "
                                 "types::evse_security::GetCertificateSignRequestStatus");
    }
}

types::evse_security::GetCertificateInfoStatus to_everest(evse_security::GetCertificateInfoStatus other) {
    switch (other) {
    case evse_security::GetCertificateInfoStatus::Accepted:
        return types::evse_security::GetCertificateInfoStatus::Accepted;
    case evse_security::GetCertificateInfoStatus::Rejected:
        return types::evse_security::GetCertificateInfoStatus::Rejected;
    case evse_security::GetCertificateInfoStatus::NotFound:
        return types::evse_security::GetCertificateInfoStatus::NotFound;
    case evse_security::GetCertificateInfoStatus::NotFoundValid:
        return types::evse_security::GetCertificateInfoStatus::NotFoundValid;
    case evse_security::GetCertificateInfoStatus::PrivateKeyNotFound:
        return types::evse_security::GetCertificateInfoStatus::PrivateKeyNotFound;
    default:
        throw std::runtime_error("Could not convert evse_security::GetCertificateInfoStatus to "
                                 "types::evse_security::GetCertificateInfoStatus");
    }
}

types::evse_security::CertificateHashData to_everest(evse_security::CertificateHashData other) {
    types::evse_security::CertificateHashData lhs;
    lhs.hash_algorithm = to_everest(other.hash_algorithm);
    lhs.issuer_name_hash = other.issuer_name_hash;
    lhs.issuer_key_hash = other.issuer_key_hash;
    lhs.serial_number = other.serial_number;
    return lhs;
}

types::evse_security::CertificateHashDataChain to_everest(evse_security::CertificateHashDataChain other) {
    types::evse_security::CertificateHashDataChain lhs;
    lhs.certificate_type = to_everest(other.certificate_type);
    lhs.certificate_hash_data = to_everest(other.certificate_hash_data);

    std::vector<types::evse_security::CertificateHashData> v;
    for (const auto& certificate_hash_data : other.child_certificate_hash_data) {
        v.push_back(to_everest(certificate_hash_data));
    }
    lhs.child_certificate_hash_data = v;

    return lhs;
}

types::evse_security::GetInstalledCertificatesResult to_everest(evse_security::GetInstalledCertificatesResult other) {
    types::evse_security::GetInstalledCertificatesResult lhs;
    lhs.status = to_everest(other.status);
    for (const auto& certificate_hash_data_chain : other.certificate_hash_data_chain) {
        lhs.certificate_hash_data_chain.push_back(to_everest(certificate_hash_data_chain));
    }
    return lhs;
}

types::evse_security::OCSPRequestData to_everest(evse_security::OCSPRequestData other) {
    types::evse_security::OCSPRequestData lhs;
    if (other.certificate_hash_data.has_value()) {
        lhs.certificate_hash_data = to_everest(other.certificate_hash_data.value());
    }
    if (other.responder_url.has_value()) {
        lhs.responder_url = other.responder_url;
    }
    return lhs;
}

types::evse_security::OCSPRequestDataList to_everest(evse_security::OCSPRequestDataList other) {
    types::evse_security::OCSPRequestDataList lhs;
    for (const auto& ocsp_request_data : other.ocsp_request_data_list) {
        lhs.ocsp_request_data_list.push_back(to_everest(ocsp_request_data));
    }
    return lhs;
}

types::evse_security::CertificateInfo to_everest(evse_security::CertificateInfo other) {
    types::evse_security::CertificateInfo lhs;
    lhs.key = other.key;
    lhs.certificate = other.certificate;
    lhs.certificate_single = other.certificate_single;
    lhs.password = other.password;
    lhs.certificate_count = other.certificate_count;
    return lhs;
}

} // namespace conversions

} // namespace module
