// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include "ocpp/common/types.hpp"
#include <ocpp/common/evse_security.hpp>

namespace ocpp::v16::stubs {

class EvseSecurityStub : public ocpp::EvseSecurity {

public:
    virtual ~EvseSecurityStub() = default;

    InstallCertificateResult install_ca_certificate(const std::string& certificate,
                                                    const CaCertificateType& certificate_type) override {
        return InstallCertificateResult::Accepted;
    }

    DeleteCertificateResult delete_certificate(const CertificateHashDataType& certificate_hash_data) override {
        return DeleteCertificateResult::Accepted;
    }

    InstallCertificateResult update_leaf_certificate(const std::string& certificate_chain,
                                                     const CertificateSigningUseEnum& certificate_type) override {
        return InstallCertificateResult::Accepted;
    }
    CertificateValidationResult verify_certificate(const std::string& certificate_chain,
                                                   const LeafCertificateType& certificate_type) override {
        return CertificateValidationResult::Valid;
    }
    CertificateValidationResult verify_certificate(const std::string& certificate_chain,
                                                   const std::vector<LeafCertificateType>& certificate_types) override {
        return CertificateValidationResult::Valid;
    }
    std::vector<CertificateHashDataChain>
    get_installed_certificates(const std::vector<CertificateType>& certificate_types) override {
        return {};
    }
    std::vector<OCSPRequestData> get_v2g_ocsp_request_data() override {
        return {};
    }
    std::vector<OCSPRequestData> get_mo_ocsp_request_data(const std::string& certificate_chain) override {
        return {};
    }
    void update_ocsp_cache(const CertificateHashDataType& certificate_hash_data,
                           const std::string& ocsp_response) override {
    }
    bool is_ca_certificate_installed(const CaCertificateType& certificate_type) override {
        return true;
    }
    GetCertificateSignRequestResult
    generate_certificate_signing_request(const CertificateSigningUseEnum& certificate_type, const std::string& country,
                                         const std::string& organization, const std::string& common,
                                         bool use_tpm) override {
        return {GetCertificateSignRequestStatus::KeyGenError, {}};
    }
    GetCertificateInfoResult get_leaf_certificate_info(const CertificateSigningUseEnum& certificate_type,
                                                       bool include_ocsp = false) override {
        return {};
    }

    bool update_certificate_links(const CertificateSigningUseEnum& certificate_type) override {
        return true;
    }
    std::string get_verify_file(const CaCertificateType& certificate_type) override {
        return {};
    }
    std::string get_verify_location(const CaCertificateType& certificate_type) override {
        return {};
    }
    int get_leaf_expiry_days_count(const CertificateSigningUseEnum& certificate_type) override {
        return 365;
    }
};
} // namespace ocpp::v16::stubs