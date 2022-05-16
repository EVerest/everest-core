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

Object auth_token_validatorImpl::handle_validate_token(std::string& token) {
    auto auth_status = mod->charge_point->authorize_id_tag(ocpp1_6::CiString20Type(token));
    Object result;
    switch (auth_status) {
    case ocpp1_6::AuthorizationStatus::Accepted:
        result["result"] = "Accepted";
        break;
    case ocpp1_6::AuthorizationStatus::Blocked:
        result["result"] = "Blocked";
        break;
    case ocpp1_6::AuthorizationStatus::Expired:
        result["result"] = "Expired";
        break;
    case ocpp1_6::AuthorizationStatus::Invalid:
        result["result"] = "Invalid";
        break;

    default:
        result["result"] = "Invalid";
        break;
    }

    result["reason"] = "Validation by OCPP 1.6 Central System";
    return result;
};

} // namespace auth_validator
} // namespace module
