// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/uk_random_delay/API.hpp>

namespace everest::lib::API::V1_0::types::uk_random_delay {

using json = nlohmann::json;

void to_json(json& j, const CountDown& k) noexcept;
void from_json(const json& j, CountDown& k);

} // namespace everest::lib::API::V1_0::types::uk_random_delay
