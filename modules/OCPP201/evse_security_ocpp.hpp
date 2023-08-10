// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EVEREST_SECURITY_OCPP_HPP
#define EVEREST_SECURITY_OCPP_HPP

#include <optional>

#include <ocpp/common/evse_security.hpp>

namespace module {

class EvseSecurity : public ocpp::EvseSecurity {
public:
    EvseSecurity();
    ~EvseSecurity();
    ocpp::InstallCertificateResult install_ca_certificate(const std::string& certificate,
                                                          const ocpp::CaCertificateType& certificate_type) override;
    ocpp::DeleteCertificateResult
    delete_certificate(const ocpp::CertificateHashDataType& certificate_hash_data) override;
    ocpp::InstallCertificateResult
    update_leaf_certificate(const std::string& certificate_chain,
                            const ocpp::CertificateSigningUseEnum& certificate_type) override;
    std::vector<ocpp::CertificateHashDataChain>
    get_installed_certificates(const std::vector<ocpp::CaCertificateType>& ca_certificate_types,
                               const std::vector<ocpp::CertificateSigningUseEnum>& leaf_certificate_types) override;
    std::vector<ocpp::OCSPRequestData> get_ocsp_request_data() override;
    void update_ocsp_cache(const ocpp::CertificateHashDataType& certificate_hash_data,
                           const std::string& ocsp_response) override;
    bool is_ca_certificate_installed(const ocpp::CaCertificateType& certificate_type) override;
    std::string generate_certificate_signing_request(const ocpp::CertificateSigningUseEnum& certificate_type,
                                                     const std::string& country, const std::string& organization,
                                                     const std::string& common) override;
    std::optional<ocpp::KeyPair> get_key_pair(const ocpp::CertificateSigningUseEnum& certificate_type) override;
    std::string get_verify_file(const ocpp::CaCertificateType& certificate_type) override;
    std::string get_verify_path(const ocpp::CaCertificateType& certificate_type) override;
};

} // namespace module

#endif // EVEREST_SECURITY_OCPP_HPP