// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <string>
#include <variant>
#include <vector>

#include "common_types.hpp"

namespace iso15118::message_20 {

namespace datatypes {

enum class AuthStatus {
    Accepted = 0,
    Pending = 1,
    Rejected = 2,
};

struct EIM_ASReqAuthorizationMode {};

struct PnC_ASReqAuthorizationMode {
    std::string id;
    GenChallenge gen_challenge;
    ContractCertificateChain contract_certificate_chain;
};

} // namespace datatypes

struct AuthorizationRequest {
    Header header;
    datatypes::Authorization selected_authorization_service;
    std::variant<datatypes::EIM_ASReqAuthorizationMode, datatypes::PnC_ASReqAuthorizationMode> authorization_mode;
};

struct AuthorizationResponse {
    Header header;
    datatypes::ResponseCode response_code;
    datatypes::Processing evse_processing{datatypes::Processing::Finished};
};

} // namespace iso15118::message_20
