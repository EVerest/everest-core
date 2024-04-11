// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "evse_securityImpl.hpp"
#include "../conversions.hpp"

namespace module {
namespace main {

void evse_securityImpl::init() {

    const auto certs_path = this->mod->info.paths.etc / "certs";
    evse_security::FilePaths file_paths = {certs_path / this->mod->config.csms_ca_bundle,
                                           certs_path / this->mod->config.mf_ca_bundle,
                                           certs_path / this->mod->config.mo_ca_bundle,
                                           certs_path / this->mod->config.v2g_ca_bundle,
                                           certs_path / this->mod->config.csms_leaf_cert_directory,
                                           certs_path / this->mod->config.csms_leaf_key_directory,
                                           certs_path / this->mod->config.secc_leaf_cert_directory,
                                           certs_path / this->mod->config.secc_leaf_key_directory};
    this->evse_security =
        std::make_unique<evse_security::EvseSecurity>(file_paths, this->mod->config.private_key_password);
}

void evse_securityImpl::ready() {
}

types::evse_security::InstallCertificateResult
evse_securityImpl::handle_install_ca_certificate(std::string& certificate,
                                                 types::evse_security::CaCertificateType& certificate_type) {
    return conversions::to_everest(
        this->evse_security->install_ca_certificate(certificate, conversions::from_everest(certificate_type)));
}

types::evse_security::DeleteCertificateResult
evse_securityImpl::handle_delete_certificate(types::evse_security::CertificateHashData& certificate_hash_data) {
    return conversions::to_everest(
        this->evse_security->delete_certificate(conversions::from_everest(certificate_hash_data)));
}

types::evse_security::InstallCertificateResult
evse_securityImpl::handle_update_leaf_certificate(std::string& certificate_chain,
                                                  types::evse_security::LeafCertificateType& certificate_type) {
    return conversions::to_everest(
        this->evse_security->update_leaf_certificate(certificate_chain, conversions::from_everest(certificate_type)));
}

types::evse_security::CertificateValidationResult
evse_securityImpl::handle_verify_certificate(std::string& certificate_chain,
                                             types::evse_security::LeafCertificateType& certificate_type) {
    return conversions::to_everest(
        this->evse_security->verify_certificate(certificate_chain, conversions::from_everest(certificate_type)));
}

types::evse_security::GetInstalledCertificatesResult evse_securityImpl::handle_get_installed_certificates(
    std::vector<types::evse_security::CertificateType>& certificate_types) {
    std::vector<evse_security::CertificateType> _certificate_types;

    for (const auto& certificate_type : certificate_types) {
        _certificate_types.push_back(conversions::from_everest(certificate_type));
    }

    return conversions::to_everest(this->evse_security->get_installed_certificates(_certificate_types));
}

types::evse_security::OCSPRequestDataList evse_securityImpl::handle_get_v2g_ocsp_request_data() {
    return conversions::to_everest(this->evse_security->get_v2g_ocsp_request_data());
}

types::evse_security::OCSPRequestDataList
evse_securityImpl::handle_get_mo_ocsp_request_data(std::string& certificate_chain) {
    return conversions::to_everest(this->evse_security->get_mo_ocsp_request_data(certificate_chain));
}

void evse_securityImpl::handle_update_ocsp_cache(types::evse_security::CertificateHashData& certificate_hash_data,
                                                 std::string& ocsp_response) {
    this->evse_security->update_ocsp_cache(conversions::from_everest(certificate_hash_data), ocsp_response);
}

bool evse_securityImpl::handle_is_ca_certificate_installed(types::evse_security::CaCertificateType& certificate_type) {
    return this->evse_security->is_ca_certificate_installed(conversions::from_everest(certificate_type));
}

std::string evse_securityImpl::handle_generate_certificate_signing_request(
    types::evse_security::LeafCertificateType& certificate_type, std::string& country, std::string& organization,
    std::string& common, bool& use_tpm) {
    return this->evse_security->generate_certificate_signing_request(conversions::from_everest(certificate_type),
                                                                     country, organization, common, use_tpm);
}

types::evse_security::GetKeyPairResult
evse_securityImpl::handle_get_key_pair(types::evse_security::LeafCertificateType& certificate_type,
                                       types::evse_security::EncodingFormat& encoding) {
    types::evse_security::GetKeyPairResult response;
    const auto key_pair = this->evse_security->get_key_pair(conversions::from_everest(certificate_type),
                                                            conversions::from_everest(encoding));

    response.status = conversions::to_everest(key_pair.status);

    if (key_pair.status == evse_security::GetKeyPairStatus::Accepted && key_pair.pair.has_value()) {
        response.key_pair = conversions::to_everest(key_pair.pair.value());
    }

    return response;
}

std::string evse_securityImpl::handle_get_verify_file(types::evse_security::CaCertificateType& certificate_type) {
    return this->evse_security->get_verify_file(conversions::from_everest(certificate_type));
}

int evse_securityImpl::handle_get_leaf_expiry_days_count(types::evse_security::LeafCertificateType& certificate_type) {
    return this->evse_security->get_leaf_expiry_days_count(conversions::from_everest(certificate_type));
}

bool evse_securityImpl::handle_verify_file_signature(std::string& file_path, std::string& signing_certificate,
                                                     std::string& signature) {
    return evse_security::EvseSecurity::verify_file_signature(std::filesystem::path(file_path), signing_certificate,
                                                              signature);
}

} // namespace main
} // namespace module
