// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EVEREST_SECURITY_OCPP_HPP
#define EVEREST_SECURITY_OCPP_HPP

#include <optional>

#include <generated/interfaces/evse_security/Interface.hpp>
#include <ocpp/common/evse_security.hpp>

class EvseSecurity : public ocpp::EvseSecurity {
private:
    evse_securityIntf& r_security;

public:
    EvseSecurity() = delete;
    EvseSecurity(evse_securityIntf& r_security);
    ~EvseSecurity();
    ocpp::InstallCertificateResult install_ca_certificate(const std::string& certificate,
                                                          const ocpp::CaCertificateType& certificate_type) override;
    ocpp::DeleteCertificateResult
    delete_certificate(const ocpp::CertificateHashDataType& certificate_hash_data) override;
    ocpp::CertificateValidationResult verify_certificate(const std::string& certificate_chain,
                                                         const ocpp::LeafCertificateType& certificate_type) override;
    ocpp::InstallCertificateResult
    update_leaf_certificate(const std::string& certificate_chain,
                            const ocpp::CertificateSigningUseEnum& certificate_type) override;
    std::vector<ocpp::CertificateHashDataChain>
    get_installed_certificates(const std::vector<ocpp::CertificateType>& certificate_types) override;
    std::vector<ocpp::OCSPRequestData> get_v2g_ocsp_request_data() override;
    std::vector<ocpp::OCSPRequestData> get_mo_ocsp_request_data(const std::string& certificate_chain) override;
    void update_ocsp_cache(const ocpp::CertificateHashDataType& certificate_hash_data,
                           const std::string& ocsp_response) override;
    bool is_ca_certificate_installed(const ocpp::CaCertificateType& certificate_type) override;
    ocpp::GetCertificateSignRequestResult
    generate_certificate_signing_request(const ocpp::CertificateSigningUseEnum& certificate_type,
                                         const std::string& country, const std::string& organization,
                                         const std::string& common, bool use_tpm) override;
    std::optional<ocpp::CertificateInfo>
    get_leaf_certificate_info(const ocpp::CertificateSigningUseEnum& certificate_type, bool include_ocsp) override;
    bool update_certificate_links(const ocpp::CertificateSigningUseEnum& certificate_type) override;
    std::string get_verify_file(const ocpp::CaCertificateType& certificate_type) override;
    int get_leaf_expiry_days_count(const ocpp::CertificateSigningUseEnum& certificate_type) override;
};

namespace conversions {

ocpp::CaCertificateType to_ocpp(types::evse_security::CaCertificateType other);
ocpp::LeafCertificateType to_ocpp(types::evse_security::LeafCertificateType other);
ocpp::CertificateType to_ocpp(types::evse_security::CertificateType other);
ocpp::HashAlgorithmEnumType to_ocpp(types::evse_security::HashAlgorithm other);
ocpp::InstallCertificateResult to_ocpp(types::evse_security::InstallCertificateResult other);
ocpp::CertificateValidationResult to_ocpp(types::evse_security::CertificateValidationResult other);
ocpp::GetCertificateSignRequestStatus to_ocpp(types::evse_security::GetCertificateSignRequestStatus other);
ocpp::DeleteCertificateResult to_ocpp(types::evse_security::DeleteCertificateResult other);

ocpp::CertificateHashDataType to_ocpp(types::evse_security::CertificateHashData other);
ocpp::CertificateHashDataChain to_ocpp(types::evse_security::CertificateHashDataChain other);
ocpp::OCSPRequestData to_ocpp(types::evse_security::OCSPRequestData other);
ocpp::CertificateOCSP to_ocpp(types::evse_security::CertificateOCSP other);
ocpp::CertificateInfo to_ocpp(types::evse_security::CertificateInfo other);

types::evse_security::CaCertificateType from_ocpp(ocpp::CaCertificateType other);
types::evse_security::LeafCertificateType from_ocpp(ocpp::CertificateSigningUseEnum other);
types::evse_security::LeafCertificateType from_ocpp(ocpp::LeafCertificateType other);
types::evse_security::CertificateType from_ocpp(ocpp::CertificateType other);
types::evse_security::HashAlgorithm from_ocpp(ocpp::HashAlgorithmEnumType other);
types::evse_security::InstallCertificateResult from_ocpp(ocpp::InstallCertificateResult other);
types::evse_security::GetCertificateSignRequestStatus from_ocpp(ocpp::GetCertificateSignRequestStatus other);
types::evse_security::DeleteCertificateResult from_ocpp(ocpp::DeleteCertificateResult other);

types::evse_security::CertificateHashData from_ocpp(ocpp::CertificateHashDataType other);
types::evse_security::CertificateHashDataChain from_ocpp(ocpp::CertificateHashDataChain other);
types::evse_security::OCSPRequestData from_ocpp(ocpp::OCSPRequestData other);
types::evse_security::CertificateInfo from_ocpp(ocpp::CertificateInfo other);

}; // namespace conversions

#endif // EVEREST_SECURITY_OCPP_HPP
