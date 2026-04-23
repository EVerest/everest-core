// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest_api_types/execution/API.hpp>

#include <utils/config_service_interface.hpp>

namespace Everest::api::types::execution {

using StopModulesResultEnum_Internal = ::Everest::config::StopModulesResult;
using StopModulesResultEnum_External = ::everest::lib::API::V1_0::types::execution::StopModulesResultEnum;

StopModulesResultEnum_Internal to_internal_api(StopModulesResultEnum_External const& val);
StopModulesResultEnum_External to_external_api(StopModulesResultEnum_Internal const& val);

using StartModulesResultEnum_Internal = ::Everest::config::RestartModulesResult;
using StartModulesResultEnum_External = ::everest::lib::API::V1_0::types::execution::StartModulesResultEnum;

StartModulesResultEnum_Internal to_internal_api(StartModulesResultEnum_External const& val);
StartModulesResultEnum_External to_external_api(StartModulesResultEnum_Internal const& val);

} // namespace Everest::api::types::execution
