// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/text_message/API.hpp>

namespace everest::lib::API::V1_0::types::text_message {

using json = nlohmann::json;

void to_json(json& j, MessageFormat const& k) noexcept;
void from_json(const json& j, MessageFormat& k);

void to_json(json& j, MessageContent const& k) noexcept;
void from_json(const json& j, MessageContent& k);

} // namespace everest::lib::API::V1_0::types::text_message
