// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/evse_security.hpp>

namespace ocpp {

namespace evse_security_conversions {

ocpp::v2::GetCertificateIdUseEnum to_ocpp_v2(ocpp::CertificateType other) {
    switch (other) {
    case ocpp::CertificateType::V2GRootCertificate:
        return ocpp::v2::GetCertificateIdUseEnum::V2GRootCertificate;
    case ocpp::CertificateType::MORootCertificate:
        return ocpp::v2::GetCertificateIdUseEnum::MORootCertificate;
    case ocpp::CertificateType::CSMSRootCertificate:
        return ocpp::v2::GetCertificateIdUseEnum::CSMSRootCertificate;
    case ocpp::CertificateType::V2GCertificateChain:
        return ocpp::v2::GetCertificateIdUseEnum::V2GCertificateChain;
    case ocpp::CertificateType::MFRootCertificate:
        return ocpp::v2::GetCertificateIdUseEnum::ManufacturerRootCertificate;
    }

    throw std::runtime_error("Could not convert CertificateType to GetCertificateIdUseEnum");
}

ocpp::v2::InstallCertificateUseEnum to_ocpp_v2(ocpp::CaCertificateType other) {
    switch (other) {
    case ocpp::CaCertificateType::V2G:
        return ocpp::v2::InstallCertificateUseEnum::V2GRootCertificate;
    case ocpp::CaCertificateType::MO:
        return ocpp::v2::InstallCertificateUseEnum::MORootCertificate;
    case ocpp::CaCertificateType::CSMS:
        return ocpp::v2::InstallCertificateUseEnum::CSMSRootCertificate;
    case ocpp::CaCertificateType::MF:
        return ocpp::v2::InstallCertificateUseEnum::ManufacturerRootCertificate;
    }

    throw std::runtime_error("Could not convert CaCertificateType to InstallCertificateUseEnum");
}

ocpp::v2::CertificateSigningUseEnum to_ocpp_v2(ocpp::CertificateSigningUseEnum other) {
    switch (other) {
    case ocpp::CertificateSigningUseEnum::ChargingStationCertificate:
        return ocpp::v2::CertificateSigningUseEnum::ChargingStationCertificate;
    case ocpp::CertificateSigningUseEnum::V2GCertificate:
        return ocpp::v2::CertificateSigningUseEnum::V2GCertificate;
    case ocpp::CertificateSigningUseEnum::ManufacturerCertificate:
        // TODO: we don't have this value in ocpp
        throw std::runtime_error(
            "Could not convert ocpp::CertificateSigningUseEnum::ManufacturerCertificate missing type");
    }

    throw std::runtime_error("Could not convert CertificateSigningUseEnum to CertificateSigningUseEnum");
}

ocpp::v2::HashAlgorithmEnum to_ocpp_v2(ocpp::HashAlgorithmEnumType other) {
    switch (other) {
    case ocpp::HashAlgorithmEnumType::SHA256:
        return ocpp::v2::HashAlgorithmEnum::SHA256;
    case ocpp::HashAlgorithmEnumType::SHA384:
        return ocpp::v2::HashAlgorithmEnum::SHA384;
    case ocpp::HashAlgorithmEnumType::SHA512:
        return ocpp::v2::HashAlgorithmEnum::SHA512;
    }

    throw std::runtime_error("Could not convert HashAlgorithmEnumType to HashAlgorithmEnum");
}

ocpp::v2::InstallCertificateStatusEnum to_ocpp_v2(ocpp::InstallCertificateResult other) {
    switch (other) {
    case ocpp::InstallCertificateResult::InvalidSignature:
        return ocpp::v2::InstallCertificateStatusEnum::Rejected;
    case ocpp::InstallCertificateResult::InvalidCertificateChain:
        return ocpp::v2::InstallCertificateStatusEnum::Rejected;
    case ocpp::InstallCertificateResult::InvalidFormat:
        return ocpp::v2::InstallCertificateStatusEnum::Rejected;
    case ocpp::InstallCertificateResult::InvalidCommonName:
        return ocpp::v2::InstallCertificateStatusEnum::Rejected;
    case ocpp::InstallCertificateResult::NoRootCertificateInstalled:
        return ocpp::v2::InstallCertificateStatusEnum::Rejected;
    case ocpp::InstallCertificateResult::Expired:
        return ocpp::v2::InstallCertificateStatusEnum::Rejected;
    case ocpp::InstallCertificateResult::CertificateStoreMaxLengthExceeded:
        return ocpp::v2::InstallCertificateStatusEnum::Rejected;
    case ocpp::InstallCertificateResult::WriteError:
        return ocpp::v2::InstallCertificateStatusEnum::Failed;
    case ocpp::InstallCertificateResult::Accepted:
        return ocpp::v2::InstallCertificateStatusEnum::Accepted;
    }

    throw std::runtime_error("Could not convert InstallCertificateResult to InstallCertificateStatusEnum");
}

ocpp::v2::DeleteCertificateStatusEnum to_ocpp_v2(ocpp::DeleteCertificateResult other) {
    switch (other) {
    case ocpp::DeleteCertificateResult::Accepted:
        return ocpp::v2::DeleteCertificateStatusEnum ::Accepted;
    case ocpp::DeleteCertificateResult::Failed:
        return ocpp::v2::DeleteCertificateStatusEnum ::Failed;
    case ocpp::DeleteCertificateResult::NotFound:
        return ocpp::v2::DeleteCertificateStatusEnum ::NotFound;
    }

    throw std::runtime_error("Could not convert DeleteCertificateResult to DeleteCertificateResult");
}

ocpp::v2::CertificateHashDataType to_ocpp_v2(ocpp::CertificateHashDataType other) {
    ocpp::v2::CertificateHashDataType lhs;
    lhs.hashAlgorithm = to_ocpp_v2(other.hashAlgorithm);
    lhs.issuerNameHash = other.issuerNameHash;
    lhs.issuerKeyHash = other.issuerKeyHash;
    lhs.serialNumber = other.serialNumber;
    return lhs;
}

ocpp::v2::CertificateHashDataChain to_ocpp_v2(ocpp::CertificateHashDataChain other) {
    ocpp::v2::CertificateHashDataChain lhs;
    lhs.certificateType = to_ocpp_v2(other.certificateType);
    lhs.certificateHashData = to_ocpp_v2(other.certificateHashData);
    if (other.childCertificateHashData.has_value() && !other.childCertificateHashData.value().empty()) {
        std::vector<ocpp::v2::CertificateHashDataType> v;
        for (const auto& certificate_hash_data : other.childCertificateHashData.value()) {
            v.push_back(to_ocpp_v2(certificate_hash_data));
        }
        lhs.childCertificateHashData = v;
    }
    return lhs;
}

ocpp::v2::OCSPRequestData to_ocpp_v2(ocpp::OCSPRequestData other) {
    ocpp::v2::OCSPRequestData lhs;
    lhs.issuerNameHash = other.issuerNameHash;
    lhs.issuerKeyHash = other.issuerKeyHash;
    lhs.serialNumber = other.serialNumber;
    lhs.responderURL = other.responderUrl;
    lhs.hashAlgorithm = to_ocpp_v2(other.hashAlgorithm);
    return lhs;
}

std::vector<ocpp::v2::OCSPRequestData> to_ocpp_v2(const std::vector<ocpp::OCSPRequestData>& ocsp_request_data) {
    std::vector<ocpp::v2::OCSPRequestData> ocsp_request_data_list;
    for (const auto& ocsp_data : ocsp_request_data) {
        ocpp::v2::OCSPRequestData request = to_ocpp_v2(ocsp_data);
        ocsp_request_data_list.push_back(request);
    }
    return ocsp_request_data_list;
}

ocpp::CertificateType from_ocpp_v2(ocpp::v2::GetCertificateIdUseEnum other) {
    switch (other) {
    case ocpp::v2::GetCertificateIdUseEnum::V2GRootCertificate:
        return ocpp::CertificateType::V2GRootCertificate;
    case ocpp::v2::GetCertificateIdUseEnum::V2GCertificateChain:
        return ocpp::CertificateType::V2GCertificateChain;
    case ocpp::v2::GetCertificateIdUseEnum::MORootCertificate:
        return ocpp::CertificateType::MORootCertificate;
    case ocpp::v2::GetCertificateIdUseEnum::CSMSRootCertificate:
        return ocpp::CertificateType::CSMSRootCertificate;
    case ocpp::v2::GetCertificateIdUseEnum::ManufacturerRootCertificate:
        return ocpp::CertificateType::MFRootCertificate;
    }

    throw std::runtime_error("Could not convert GetCertificateIdUseEnum to CertificateType");
}

std::vector<ocpp::CertificateType> from_ocpp_v2(const std::vector<ocpp::v2::GetCertificateIdUseEnum>& other) {
    std::vector<ocpp::CertificateType> certificate_types;
    for (const auto& certificate_id_use_enum : other) {
        certificate_types.push_back(from_ocpp_v2(certificate_id_use_enum));
    }
    return certificate_types;
}

ocpp::CaCertificateType from_ocpp_v2(ocpp::v2::InstallCertificateUseEnum other) {
    switch (other) {
    case ocpp::v2::InstallCertificateUseEnum::V2GRootCertificate:
        return ocpp::CaCertificateType::V2G;
    case ocpp::v2::InstallCertificateUseEnum::MORootCertificate:
        return ocpp::CaCertificateType::MO;
    case ocpp::v2::InstallCertificateUseEnum::CSMSRootCertificate:
        return ocpp::CaCertificateType::CSMS;
    case ocpp::v2::InstallCertificateUseEnum::ManufacturerRootCertificate:
        return ocpp::CaCertificateType::MF;
    }

    throw std::runtime_error("Could not convert CaCertificateType to InstallCertificateUseEnum");
}

ocpp::CertificateSigningUseEnum from_ocpp_v2(ocpp::v2::CertificateSigningUseEnum other) {
    switch (other) {
    case ocpp::v2::CertificateSigningUseEnum::ChargingStationCertificate:
        return ocpp::CertificateSigningUseEnum::ChargingStationCertificate;
    case ocpp::v2::CertificateSigningUseEnum::V2GCertificate:
        return ocpp::CertificateSigningUseEnum::V2GCertificate;
    }

    throw std::runtime_error("Could not convert CertificateSigningUseEnum to CertificateSigningUseEnum");
}

ocpp::HashAlgorithmEnumType from_ocpp_v2(ocpp::v2::HashAlgorithmEnum other) {
    switch (other) {
    case ocpp::v2::HashAlgorithmEnum::SHA256:
        return ocpp::HashAlgorithmEnumType::SHA256;
    case ocpp::v2::HashAlgorithmEnum::SHA384:
        return ocpp::HashAlgorithmEnumType::SHA384;
    case ocpp::v2::HashAlgorithmEnum::SHA512:
        return ocpp::HashAlgorithmEnumType::SHA512;
    }

    throw std::runtime_error("Could not convert HashAlgorithmEnum to HashAlgorithmEnumType");
}

ocpp::InstallCertificateResult from_ocpp_v2(ocpp::v2::InstallCertificateStatusEnum other) {
    switch (other) {
    case ocpp::v2::InstallCertificateStatusEnum::Rejected:
        return ocpp::InstallCertificateResult::InvalidCertificateChain;
    case ocpp::v2::InstallCertificateStatusEnum::Failed:
        return ocpp::InstallCertificateResult::WriteError;
    case ocpp::v2::InstallCertificateStatusEnum::Accepted:
        return ocpp::InstallCertificateResult::Accepted;
    }

    throw std::runtime_error("Could not convert InstallCertificateResult to evse_security::InstallCertificateResult");
}

ocpp::DeleteCertificateResult from_ocpp_v2(ocpp::v2::DeleteCertificateStatusEnum other) {
    switch (other) {
    case ocpp::v2::DeleteCertificateStatusEnum::Accepted:
        return ocpp::DeleteCertificateResult::Accepted;
    case ocpp::v2::DeleteCertificateStatusEnum::Failed:
        return ocpp::DeleteCertificateResult::Failed;
    case ocpp::v2::DeleteCertificateStatusEnum::NotFound:
        return ocpp::DeleteCertificateResult::NotFound;
    }

    throw std::runtime_error("Could not convert DeleteCertificateResult to evse_security::DeleteCertificateResult");
}

ocpp::CertificateHashDataType from_ocpp_v2(ocpp::v2::CertificateHashDataType other) {
    ocpp::CertificateHashDataType lhs;
    lhs.hashAlgorithm = from_ocpp_v2(other.hashAlgorithm);
    lhs.issuerNameHash = other.issuerNameHash;
    lhs.issuerKeyHash = other.issuerKeyHash;
    lhs.serialNumber = other.serialNumber;
    return lhs;
}

ocpp::CertificateHashDataChain from_ocpp_v2(ocpp::v2::CertificateHashDataChain other) {
    ocpp::CertificateHashDataChain lhs;
    lhs.certificateType = from_ocpp_v2(other.certificateType);
    lhs.certificateHashData = from_ocpp_v2(other.certificateHashData);
    if (other.childCertificateHashData.has_value()) {
        std::vector<ocpp::CertificateHashDataType> v;
        for (const auto& certificate_hash_data : other.childCertificateHashData.value()) {
            v.push_back(from_ocpp_v2(certificate_hash_data));
        }
        lhs.childCertificateHashData = v;
    }
    return lhs;
}

ocpp::OCSPRequestData from_ocpp_v2(ocpp::v2::OCSPRequestData other) {
    ocpp::OCSPRequestData lhs;
    lhs.issuerNameHash = other.issuerNameHash;
    lhs.issuerKeyHash = other.issuerKeyHash;
    lhs.serialNumber = other.serialNumber;
    lhs.responderUrl = other.responderURL;
    return lhs;
}

} // namespace evse_security_conversions

} // namespace ocpp
