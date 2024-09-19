// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <array>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "common.hpp"

namespace iso15118::message_20 {

struct AuthorizationSetupRequest {
    Header header;
};

struct AuthorizationSetupResponse {

    static constexpr auto GEN_CHALLENGE_LENGTH = 16;

    AuthorizationSetupResponse() :
        authorization_services({Authorization::EIM}),
        certificate_installation_service(false),
        authorization_mode(std::in_place_type<AuthorizationSetupResponse::EIM_ASResAuthorizationMode>){};

    Header header;
    ResponseCode response_code;

    struct EIM_ASResAuthorizationMode {};
    struct PnC_ASResAuthorizationMode {
        std::array<uint8_t, GEN_CHALLENGE_LENGTH> gen_challenge;
        std::optional<std::vector<std::string>> supported_providers;
    };

    std::vector<Authorization> authorization_services;
    bool certificate_installation_service;
    std::variant<EIM_ASResAuthorizationMode, PnC_ASResAuthorizationMode> authorization_mode;
};

} // namespace iso15118::message_20
