// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/execution/API.hpp>

namespace everest::lib::API::V1_0::types::execution {

using json = nlohmann::json;

void to_json(json& j, StopModulesResultEnum const& k) noexcept;
void from_json(const json& j, StopModulesResultEnum& k);

void to_json(json& j, StartModulesResultEnum const& k) noexcept;
void from_json(const json& j, StartModulesResultEnum& k);

void to_json(json& j, ModuleExecutionStatusEnum const& k) noexcept;
void from_json(const json& j, ModuleExecutionStatusEnum& k);

void to_json(json& j, StopModulesResult const& k) noexcept;
void from_json(const json& j, StopModulesResult& k);

void to_json(json& j, StartModulesResult const& k) noexcept;
void from_json(const json& j, StartModulesResult& k);

void to_json(json& j, ExecutionStatusUpdateNotice const& k) noexcept;
void from_json(const json& j, ExecutionStatusUpdateNotice& k);

} // namespace everest::lib::API::V1_0::types::execution