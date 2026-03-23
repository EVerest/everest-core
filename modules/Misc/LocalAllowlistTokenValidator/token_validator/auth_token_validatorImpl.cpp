// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

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

        std::string line;
        while (std::getline(file, line)) {

            if (line.empty())
                continue;

            std::istringstream iss(line);

            std::string token;
            iss >> token;

            // probably humans are entering the tokens in the allow list, so make sure to compare lower case letters
            // only
            std::transform(token.begin(), token.end(), token.begin(), [](unsigned char c) { return std::tolower(c); });

            if (token != provided_token.id_token.value)
                continue;

            result.authorization_status = types::authorization::AuthorizationStatus::Accepted;

            std::string rest;
            std::getline(iss, rest);

            if (!rest.empty()) {

                std::stringstream ss(rest);
                std::string id;
                result.evse_ids = std::vector<int>{};

                while (std::getline(ss, id, ',')) {

                    std::stringstream trim(id);
                    int value;

                    if (trim >> value) {
                        result.evse_ids->push_back(value);
                    }
                }
                if (!result.evse_ids.has_value()) {
                    result.evse_ids.reset();
                }
            }

            break;
        }

    } catch (const std::ifstream::failure& e) {
        EVLOG_error << "Error opening/reading file " + mod->config.allowlist_file;
    }

    file.close();

    return result;
}

} // namespace token_validator
} // namespace module
