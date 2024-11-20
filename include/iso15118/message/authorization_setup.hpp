// SPDX-License-Identifier: Apache-2.0
// Copyright 2023 Pionix GmbH and Contributors to EVerest
#pragma once

#include <array>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "common_types.hpp"

namespace iso15118::message_20 {

namespace datatypes {
using SupportedProvidersList = std::vector<Name>; // [0 - 128]

struct EIM_ASResAuthorizationMode {};
struct PnC_ASResAuthorizationMode {
    GenChallenge gen_challenge;
    std::optional<datatypes::SupportedProvidersList> supported_providers;
};

} // namespace datatypes

struct AuthorizationSetupRequest {
    Header header;
};

struct AuthorizationSetupResponse {
    Header header;
    datatypes::ResponseCode response_code;

    std::vector<datatypes::Authorization> authorization_services{datatypes::Authorization::EIM};
    bool certificate_installation_service{false};
    std::variant<datatypes::EIM_ASResAuthorizationMode, datatypes::PnC_ASResAuthorizationMode> authorization_mode =
        datatypes::EIM_ASResAuthorizationMode();
};

} // namespace iso15118::message_20
