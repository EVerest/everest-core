// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <generated/interfaces/ISO15118_charger/Implementation.hpp>
#include <ocpp/v201/messages/Authorize.hpp>

#include "auth_token_validatorImpl.hpp"

namespace module {
namespace auth_validator {

ocpp::v201::IdToken get_id_token(const types::authorization::ProvidedIdToken& provided_token) {
    ocpp::v201::IdToken id_token;
    id_token.idToken = provided_token.id_token;
    if (provided_token.type == types::authorization::TokenType::Autocharge) {
        id_token.type = ocpp::v201::IdTokenEnum::MacAddress;
    } else if (provided_token.type == types::authorization::TokenType::OCPP) {
        id_token.type = ocpp::v201::IdTokenEnum::Central;
    } else if (provided_token.type == types::authorization::TokenType::PlugAndCharge) {
        id_token.type = ocpp::v201::IdTokenEnum::eMAID;
    } else if (provided_token.type == types::authorization::TokenType::RFID) {
        id_token.type = ocpp::v201::IdTokenEnum::ISO14443;
    }
    return id_token;
}

std::vector<ocpp::v201::OCSPRequestData>
get_ocsp_request_data(const types::authorization::ProvidedIdToken& provided_token) {
    std::vector<ocpp::v201::OCSPRequestData> ocsp_request_data_list;

    for (const auto& certificate_hash_data : provided_token.iso15118CertificateHashData.value()) {
        ocpp::v201::OCSPRequestData ocsp_request_data;
        ocsp_request_data.hashAlgorithm = ocpp::v201::conversions::string_to_hash_algorithm_enum(
            types::iso15118_charger::hash_algorithm_to_string(certificate_hash_data.hashAlgorithm));
        ocsp_request_data.issuerKeyHash = certificate_hash_data.issuerKeyHash;
        ocsp_request_data.issuerNameHash = certificate_hash_data.issuerNameHash;
        ocsp_request_data.responderURL = certificate_hash_data.responderURL;
        ocsp_request_data.serialNumber = certificate_hash_data.serialNumber;
        ocsp_request_data_list.push_back(ocsp_request_data);
    }
    return ocsp_request_data_list;
}

types::authorization::ValidationResult get_validation_result(const ocpp::v201::AuthorizeResponse& response) {
    types::authorization::ValidationResult validation_result;

    validation_result.authorization_status = types::authorization::string_to_authorization_status(
        ocpp::v201::conversions::authorization_status_enum_to_string(response.idTokenInfo.status));
    if (response.idTokenInfo.cacheExpiryDateTime.has_value()) {
        validation_result.expiry_time.emplace(response.idTokenInfo.cacheExpiryDateTime.value().to_rfc3339());
    }
    if (response.idTokenInfo.groupIdToken.has_value()) {
        validation_result.parent_id_token.emplace(response.idTokenInfo.groupIdToken.value().idToken.get());
    }
    if (response.idTokenInfo.personalMessage.has_value()) {
        validation_result.reason.emplace(response.idTokenInfo.personalMessage.value().content.get());
    }
    if (response.certificateStatus.has_value()) {
        validation_result.certificate_status.emplace(types::authorization::string_to_certificate_status(
            ocpp::v201::conversions::authorize_certificate_status_enum_to_string(response.certificateStatus.value())));
    }
    if (response.idTokenInfo.evseId.has_value()) {
        validation_result.evse_ids = response.idTokenInfo.evseId.value();
    }
    return validation_result;
}

void auth_token_validatorImpl::init() {
}

void auth_token_validatorImpl::ready() {
}

types::authorization::ValidationResult
auth_token_validatorImpl::handle_validate_token(types::authorization::ProvidedIdToken& provided_token) {
        const auto id_token = get_id_token(provided_token);
    boost::optional<ocpp::CiString<5500>> certificate_opt;
    if (provided_token.certificate.has_value()) {
        certificate_opt.emplace(provided_token.certificate.value());
    }
    boost::optional<std::vector<ocpp::v201::OCSPRequestData>> ocsp_request_data_opt;
    if (provided_token.iso15118CertificateHashData.has_value()) {
        ocsp_request_data_opt.emplace(get_ocsp_request_data(provided_token));
    }

    // request response
    const auto response = this->mod->charge_point->validate_token(id_token, certificate_opt, ocsp_request_data_opt);
    return get_validation_result(response);
};

} // namespace auth_validator
} // namespace module
