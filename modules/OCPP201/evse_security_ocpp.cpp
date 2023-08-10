// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <evse_security_ocpp.hpp>

namespace module {

EvseSecurity::EvseSecurity() {
}

EvseSecurity::~EvseSecurity() {
}

ocpp::InstallCertificateResult EvseSecurity::install_ca_certificate(const std::string& certificate,
                                                                    const ocpp::CaCertificateType& certificate_type) {
    return ocpp::InstallCertificateResult::WriteError;
}

ocpp::DeleteCertificateResult
EvseSecurity::delete_certificate(const ocpp::CertificateHashDataType& certificate_hash_data) {
    return ocpp::DeleteCertificateResult::Failed;
}

ocpp::InstallCertificateResult
EvseSecurity::update_leaf_certificate(const std::string& certificate_chain,
                                      const ocpp::CertificateSigningUseEnum& certificate_type) {
    return ocpp::InstallCertificateResult::WriteError;
}

std::vector<ocpp::CertificateHashDataChain>
EvseSecurity::get_installed_certificates(const std::vector<ocpp::CaCertificateType>& ca_certificate_types,
                                         const std::vector<ocpp::CertificateSigningUseEnum>& leaf_certificate_types) {
    std::vector<ocpp::CertificateHashDataChain> v;
    return v;
}

std::vector<ocpp::OCSPRequestData> EvseSecurity::get_ocsp_request_data() {
    std::vector<ocpp::OCSPRequestData> v;
    return v;
}
void EvseSecurity::update_ocsp_cache(const ocpp::CertificateHashDataType& certificate_hash_data,
                                     const std::string& ocsp_response) {
}
bool EvseSecurity::is_ca_certificate_installed(const ocpp::CaCertificateType& certificate_type) {
    return false;
}

std::string EvseSecurity::generate_certificate_signing_request(const ocpp::CertificateSigningUseEnum& certificate_type,
                                                               const std::string& country,
                                                               const std::string& organization,
                                                               const std::string& common) {
    return "";
}

std::optional<ocpp::KeyPair> EvseSecurity::get_key_pair(const ocpp::CertificateSigningUseEnum& certificate_type) {
    return std::nullopt;
}

std::string EvseSecurity::get_verify_file(const ocpp::CaCertificateType& certificate_type) {
    return "";
}

std::string EvseSecurity::get_verify_path(const ocpp::CaCertificateType& certificate_type) {
    return "";
}

} // namespace module