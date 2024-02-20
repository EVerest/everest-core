// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "auth_token_validatorImpl.hpp"

namespace module {
namespace main {

void auth_token_validatorImpl::init() {
}

void auth_token_validatorImpl::ready() {
}

types::authorization::ValidationResult
auth_token_validatorImpl::handle_validate_token(types::authorization::ProvidedIdToken& provided_token) {
    EVLOG_info << "Got validation request for token: " << provided_token.id_token.value;
    types::authorization::ValidationResult ret;
    ret.authorization_status = types::authorization::string_to_authorization_status(config.validation_result);
    ret.reason = config.validation_reason;
    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(config.sleep * 1000)));
    EVLOG_info << "Returning validation status: " << config.validation_result;
    return ret;
}

} // namespace main
} // namespace module
