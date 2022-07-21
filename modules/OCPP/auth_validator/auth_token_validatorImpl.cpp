// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest
#include "auth_token_validatorImpl.hpp"
#include <ocpp1_6/types.hpp>

namespace module {
namespace auth_validator {

void auth_token_validatorImpl::init() {
}

void auth_token_validatorImpl::ready() {
}

types::auth_token_validator::Result auth_token_validatorImpl::handle_validate_token(std::string& token) {
    auto auth_status = mod->charge_point->authorize_id_tag(ocpp1_6::CiString20Type(token));
    types::auth_token_validator::Result result;
    switch (auth_status) {
    case ocpp1_6::AuthorizationStatus::Accepted:
        result.result = types::auth_token_validator::ValidationResult::Accepted;
        break;
    case ocpp1_6::AuthorizationStatus::Blocked:
        result.result = types::auth_token_validator::ValidationResult::Blocked;
        break;
    case ocpp1_6::AuthorizationStatus::Expired:
        result.result = types::auth_token_validator::ValidationResult::Expired;
        break;
    case ocpp1_6::AuthorizationStatus::Invalid:
        result.result = types::auth_token_validator::ValidationResult::Invalid;
        break;

    default:
        result.result = types::auth_token_validator::ValidationResult::Invalid;
        break;
    }

    result.reason = "Validation by OCPP 1.6 Central System";
    return result;
};

} // namespace auth_validator
} // namespace module
