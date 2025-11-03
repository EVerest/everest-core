// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <fstream>

#include "auth_token_validatorImpl.hpp"

namespace module {
namespace token_validator {

void auth_token_validatorImpl::init() {
}

void auth_token_validatorImpl::ready() {
}

types::authorization::ValidationResult
auth_token_validatorImpl::handle_validate_token(types::authorization::ProvidedIdToken& provided_token) {
    types::authorization::ValidationResult result;
    result.authorization_status = types::authorization::AuthorizationStatus::Invalid;

    // load file each time we validate so that EVerest requires no restart when the file is changed
    std::ifstream file;

    try {
        file.open(mod->config.allowlist_file);
        while (!file.eof()) {
            std::string token;
            getline(file, token);
            if (token == provided_token.id_token.value) {
                result.authorization_status = types::authorization::AuthorizationStatus::Accepted;
                break;
            }
        }
    } catch (std::ifstream::failure e) {
        EVLOG_error << "Error opening/reading file " + mod->config.allowlist_file;
    }

    file.close();

    return result;
}

} // namespace token_validator
} // namespace module
