// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_EVSE_SECURITY_IMPL_HPP
#define MAIN_EVSE_SECURITY_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/evse_security/Implementation.hpp>

#include "../EvseSecurity.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include <evse_security/evse_security.hpp>
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {};

class evse_securityImpl : public evse_securityImplBase {
public:
    evse_securityImpl() = delete;
    evse_securityImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<EvseSecurity>& mod, Conf& config) :
        evse_securityImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual types::evse_security::InstallCertificateResult
    handle_install_ca_certificate(std::string& certificate,
                                  types::evse_security::CaCertificateType& certificate_type) override;
    virtual types::evse_security::DeleteCertificateResult
    handle_delete_certificate(types::evse_security::CertificateHashData& certificate_hash_data) override;
    virtual types::evse_security::InstallCertificateResult
    handle_update_leaf_certificate(std::string& certificate_chain,
                                   types::evse_security::LeafCertificateType& certificate_type) override;
    virtual types::evse_security::CertificateValidationResult
    handle_verify_certificate(std::string& certificate_chain,
                              types::evse_security::LeafCertificateType& certificate_type) override;
    virtual types::evse_security::GetInstalledCertificatesResult
    handle_get_installed_certificates(std::vector<types::evse_security::CertificateType>& certificate_types) override;
    virtual types::evse_security::OCSPRequestDataList handle_get_v2g_ocsp_request_data() override;
    virtual types::evse_security::OCSPRequestDataList
    handle_get_mo_ocsp_request_data(std::string& certificate_chain) override;
    virtual void handle_update_ocsp_cache(types::evse_security::CertificateHashData& certificate_hash_data,
                                          std::string& ocsp_response) override;
    virtual std::string
    handle_retrieve_ocsp_cache(types::evse_security::CertificateHashData& certificate_hash_data) override;
    virtual bool handle_is_ca_certificate_installed(types::evse_security::CaCertificateType& certificate_type) override;
    virtual std::string
    handle_generate_certificate_signing_request(types::evse_security::LeafCertificateType& certificate_type,
                                                std::string& country, std::string& organization, std::string& common,
                                                bool& use_tpm) override;
    virtual types::evse_security::GetCertificateInfoResult
    handle_get_leaf_certificate_info(types::evse_security::LeafCertificateType& certificate_type,
                                     types::evse_security::EncodingFormat& encoding, bool& include_ocsp) override;
    virtual std::string handle_get_verify_file(types::evse_security::CaCertificateType& certificate_type) override;
    virtual int handle_get_leaf_expiry_days_count(types::evse_security::LeafCertificateType& certificate_type) override;
    virtual bool handle_verify_file_signature(std::string& file_path, std::string& signing_certificate,
                                              std::string& signature) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<EvseSecurity>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    std::unique_ptr<evse_security::EvseSecurity> evse_security;
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_EVSE_SECURITY_IMPL_HPP
