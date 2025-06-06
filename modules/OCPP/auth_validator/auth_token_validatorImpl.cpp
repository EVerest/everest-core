// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "auth_token_validatorImpl.hpp"
#include <conversions.hpp>
#include <ocpp/common/types.hpp>
#include <ocpp/v16/ocpp_enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>
#include <ocpp_conversions.hpp>

namespace module {
namespace auth_validator {

void auth_token_validatorImpl::init() {
}

void auth_token_validatorImpl::ready() {
}

types::authorization::ValidationResult
auth_token_validatorImpl::handle_validate_token(types::authorization::ProvidedIdToken& provided_token) {

    if (provided_token.authorization_type == types::authorization::AuthorizationType::PlugAndCharge) {
        return validate_pnc_request(provided_token);
    } else {
        return validate_standard_request(provided_token);
    }
};

types::authorization::ValidationResult
auth_token_validatorImpl::validate_pnc_request(const types::authorization::ProvidedIdToken& provided_token) {

    types::authorization::ValidationResult validation_result;
    try {
        // preparing payload for data_transfer_pnc_authorize
        std::optional<std::vector<ocpp::v2::OCSPRequestData>> iso15118_certificate_hash_data_opt;
        if (provided_token.iso15118CertificateHashData.has_value()) {
            std::vector<ocpp::v2::OCSPRequestData> iso15118_certificate_hash_data;
            for (const auto& certificate_hash_data : provided_token.iso15118CertificateHashData.value()) {
                ocpp::v2::OCSPRequestData v2_certificate_hash_data;
                v2_certificate_hash_data.hashAlgorithm =
                    conversions::to_ocpp_hash_algorithm_enum(certificate_hash_data.hashAlgorithm);
                v2_certificate_hash_data.issuerKeyHash = certificate_hash_data.issuerKeyHash;
                v2_certificate_hash_data.issuerNameHash = certificate_hash_data.issuerNameHash;
                v2_certificate_hash_data.responderURL = certificate_hash_data.responderURL;
                v2_certificate_hash_data.serialNumber = certificate_hash_data.serialNumber;
                iso15118_certificate_hash_data.push_back(v2_certificate_hash_data);
            }
            iso15118_certificate_hash_data_opt.emplace(iso15118_certificate_hash_data);
        }

        // this is the actual OCPP request via DataTransfer.req to CSMS according to
        // PnC1.6 whitepaper
        const auto authorize_response = mod->charge_point->data_transfer_pnc_authorize(
            provided_token.id_token.value, provided_token.certificate, iso15118_certificate_hash_data_opt);

        validation_result.authorization_status =
            conversions::to_everest_authorization_status(authorize_response.idTokenInfo.status);
        validation_result.evse_ids = authorize_response.idTokenInfo.evseId;
        if (authorize_response.certificateStatus.has_value()) {
            validation_result.certificate_status.emplace(
                conversions::to_everest_certificate_status(authorize_response.certificateStatus.value()));
        }
        if (authorize_response.idTokenInfo.cacheExpiryDateTime.has_value()) {
            validation_result.expiry_time.emplace(
                authorize_response.idTokenInfo.cacheExpiryDateTime.value().to_rfc3339());
        }
        if (authorize_response.idTokenInfo.groupIdToken.has_value()) {
            validation_result.parent_id_token = {authorize_response.idTokenInfo.groupIdToken.value().idToken.get(),
                                                 types::authorization::IdTokenType::Central};
        }
    } catch (const ocpp::StringConversionException& e) {
        EVLOG_warning << "Error converting id token to validate: " << e.what();
        validation_result.authorization_status = types::authorization::AuthorizationStatus::Unknown;
    } catch (const std::exception& e) {
        EVLOG_warning << "Unknown error during validation of id token: " << e.what();
        validation_result.authorization_status = types::authorization::AuthorizationStatus::Unknown;
    }

    return validation_result;
}

types::authorization::ValidationResult
auth_token_validatorImpl::validate_standard_request(const types::authorization::ProvidedIdToken& provided_token) {
    types::authorization::ValidationResult result;
    try {
        const auto enhanced_id_tag_info =
            mod->charge_point->authorize_id_token(ocpp::CiString<20>(provided_token.id_token.value));
        const auto id_tag_info = enhanced_id_tag_info.id_tag_info;
        result.authorization_status = conversions::to_everest_authorization_status(id_tag_info.status);
        if (id_tag_info.expiryDate) {
            result.expiry_time = id_tag_info.expiryDate->to_rfc3339();
        }
        if (id_tag_info.parentIdTag) {
            result.parent_id_token = {
                id_tag_info.parentIdTag->get(),
                types::authorization::IdTokenType::Central}; // For OCPP1.6 no IdTokenType is given,
                                                             // so we assume it is a central token
        }
        if (enhanced_id_tag_info.tariff_message.has_value()) {
            // this can be used as the TT field of the OCMF.
            const auto& tariff_message = enhanced_id_tag_info.tariff_message.value();
            for (const auto& message : tariff_message.message) {
                result.tariff_messages.push_back(ocpp_conversions::to_everest_display_message_content(message));
            }
        }
    } catch (const ocpp::StringConversionException& e) {
        EVLOG_warning << "Error converting id token to validate: " << e.what();
        result.authorization_status = types::authorization::AuthorizationStatus::Unknown;
    } catch (const std::exception& e) {
        EVLOG_warning << "Unknown error during validation of id token: " << e.what();
        result.authorization_status = types::authorization::AuthorizationStatus::Unknown;
    }
    return result;
};

} // namespace auth_validator
} // namespace module
