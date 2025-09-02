// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/slac/API.hpp>

namespace everest::lib::API::V1_0::types::slac {

using json = nlohmann::json;

void to_json(json& j, State const& k) noexcept;
void from_json(json const& j, State& k);

} // namespace everest::lib::API::V1_0::types::slac
