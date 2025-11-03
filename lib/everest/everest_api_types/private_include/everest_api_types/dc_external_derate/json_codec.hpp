// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/dc_external_derate/API.hpp>

namespace everest::lib::API::V1_0::types::dc_external_derate {

using json = nlohmann::json;

void to_json(json& j, ExternalDerating const& k) noexcept;
void from_json(const json& j, ExternalDerating& k);

} // namespace everest::lib::API::V1_0::types::dc_external_derate
