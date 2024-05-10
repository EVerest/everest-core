// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#ifndef OCPP_COMMON_EVSE_SECURITY_IMPL
#define OCPP_COMMON_EVSE_SECURITY_IMPL

#include <filesystem>
#include <optional>

#include <evse_security/evse_security.hpp>
#include <ocpp/common/evse_security.hpp>
#include <ocpp/common/support_older_cpp_versions.hpp>

namespace ocpp {

struct SecurityConfiguration {
    fs::path csms_ca_bundle;
    fs::path mf_ca_bundle;
    fs::path mo_ca_bundle;
    fs::path v2g_ca_bundle;
    fs::path csms_leaf_cert_directory;
    fs::path csms_leaf_key_directory;
    fs::path secc_leaf_cert_directory;
    fs::path secc_leaf_key_directory;
    fs::path secc_leaf_cert_link;
    fs::path secc_leaf_key_link;
    fs::path cpo_cert_chain_link;
    std::optional<std::string> private_key_password;
};

class EvseSecurityImpl : public EvseSecurity {

private:
    std::unique_ptr<evse_security::EvseSecurity> evse_security;

public:
    explicit EvseSecurityImpl(const SecurityConfiguration& file_paths);
    InstallCertificateResult install_ca_certificate(const std::string& certificate,
                                                    const CaCertificateType& certificate_type) override;
    DeleteCertificateResult delete_certificate(const CertificateHashDataType& certificate_hash_data) override;
    InstallCertificateResult update_leaf_certificate(const std::string& certificate_chain,
                                                     const CertificateSigningUseEnum& certificate_type) override;
    CertificateValidationResult verify_certificate(const std::string& certificate_chain,
                                                   const LeafCertificateType& certificate_type) override;
    std::vector<CertificateHashDataChain>
    get_installed_certificates(const std::vector<CertificateType>& certificate_types) override;
    std::vector<OCSPRequestData> get_v2g_ocsp_request_data() override;
    std::vector<OCSPRequestData> get_mo_ocsp_request_data(const std::string& certificate_chain) override;
    void update_ocsp_cache(const CertificateHashDataType& certificate_hash_data,
                           const std::string& ocsp_response) override;
    bool is_ca_certificate_installed(const CaCertificateType& certificate_type) override;
    GetCertificateSignRequestResult
    generate_certificate_signing_request(const CertificateSigningUseEnum& certificate_type, const std::string& country,
                                         const std::string& organization, const std::string& common,
                                         bool use_tpm) override;
    GetCertificateInfoResult get_leaf_certificate_info(const CertificateSigningUseEnum& certificate_type,
                                                       bool include_ocsp = false) override;
    bool update_certificate_links(const CertificateSigningUseEnum& certificate_type) override;
    std::string get_verify_file(const CaCertificateType& certificate_type) override;
    int get_leaf_expiry_days_count(const CertificateSigningUseEnum& certificate_type) override;
};

namespace conversions {

GetCertificateSignRequestStatus to_ocpp(evse_security::GetCertificateSignRequestStatus other);
CaCertificateType to_ocpp(evse_security::CaCertificateType other);
CertificateSigningUseEnum to_ocpp(evse_security::LeafCertificateType other);
CertificateType to_ocpp(evse_security::CertificateType other);
HashAlgorithmEnumType to_ocpp(evse_security::HashAlgorithm other);
GetCertificateInfoStatus to_ocpp(evse_security::GetCertificateInfoStatus other);
InstallCertificateResult to_ocpp(evse_security::InstallCertificateResult other);
CertificateValidationResult to_ocpp(evse_security::CertificateValidationResult other);
DeleteCertificateResult to_ocpp(evse_security::DeleteCertificateResult other);

CertificateHashDataType to_ocpp(evse_security::CertificateHashData other);
CertificateHashDataChain to_ocpp(evse_security::CertificateHashDataChain other);
OCSPRequestData to_ocpp(evse_security::OCSPRequestData other);
CertificateOCSP to_ocpp(evse_security::CertificateOCSP other);
CertificateInfo to_ocpp(evse_security::CertificateInfo other);

evse_security::CaCertificateType from_ocpp(CaCertificateType other);
evse_security::LeafCertificateType from_ocpp(LeafCertificateType other);
evse_security::LeafCertificateType from_ocpp(CertificateSigningUseEnum other);
evse_security::CertificateType from_ocpp(CertificateType other);
evse_security::HashAlgorithm from_ocpp(HashAlgorithmEnumType other);
evse_security::InstallCertificateResult from_ocpp(InstallCertificateResult other);
evse_security::DeleteCertificateResult from_ocpp(DeleteCertificateResult other);

evse_security::CertificateHashData from_ocpp(CertificateHashDataType other);
evse_security::CertificateHashDataChain from_ocpp(CertificateHashDataChain other);
evse_security::OCSPRequestData from_ocpp(OCSPRequestData other);
evse_security::CertificateOCSP from_ocpp(CertificateOCSP other);
evse_security::CertificateInfo from_ocpp(CertificateInfo other);

}; // namespace conversions

} // namespace ocpp

#endif
