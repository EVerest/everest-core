// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "auth_token_validatorImpl.hpp"
#include <ocpp/common/types.hpp>
#include <ocpp/v16/enums.hpp>

namespace module {
namespace auth_validator {

void auth_token_validatorImpl::init() {
}

void auth_token_validatorImpl::ready() {
}

types::authorization::ValidationResult auth_token_validatorImpl::handle_validate_token(std::string& token) {
    const auto id_tag_info = mod->charge_point->authorize_id_token(ocpp::CiString<20>(token));
    types::authorization::ValidationResult result;

    result.authorization_status = types::authorization::string_to_authorization_status(
        ocpp::v16::conversions::authorization_status_to_string(id_tag_info.status));
    if (id_tag_info.expiryDate) {
        result.expiry_time = id_tag_info.expiryDate.get().to_rfc3339();
    }
    if (id_tag_info.parentIdTag) {
        result.parent_id_token = id_tag_info.parentIdTag.get().get();
    }
    result.reason = "Validation by OCPP 1.6 Central System";
    return result;
};

} // namespace auth_validator
} // namespace module
