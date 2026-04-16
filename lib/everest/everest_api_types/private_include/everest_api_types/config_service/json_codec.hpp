// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include "nlohmann/json_fwd.hpp"
#include <everest_api_types/config_service/API.hpp>

namespace everest::lib::API::V1_0::types::config_service {

using json = nlohmann::json;

void to_json(json& j, MarkActiveSlotResultEnum const& k) noexcept;
void from_json(const json& j, MarkActiveSlotResultEnum& k);

void to_json(json& j, StopModulesResultEnum const& k) noexcept;
void from_json(const json& j, StopModulesResultEnum& k);

void to_json(json& j, StartModulesResultEnum const& k) noexcept;
void from_json(const json& j, StartModulesResultEnum& k);

void to_json(json& j, DeleteSlotResultEnum const& k) noexcept;
void from_json(const json& j, DeleteSlotResultEnum& k);

void to_json(json& j, ConfigurationParameterUpdateResultEnum const& k) noexcept;
void from_json(const json& j, ConfigurationParameterUpdateResultEnum& k);

void to_json(json& j, ActiveSlotStatusEnum const& k) noexcept;
void from_json(const json& j, ActiveSlotStatusEnum& k);

void to_json(json& j, ExecutionStatusEnum const& k) noexcept;
void from_json(const json& j, ExecutionStatusEnum& k);

void to_json(json& j, ConfigurationParameterDatatype const& k) noexcept;
void from_json(const json& j, ConfigurationParameterDatatype& k);

void to_json(json& j, ConfigurationParameterMutability const& k) noexcept;
void from_json(const json& j, ConfigurationParameterMutability& k);

void to_json(json& j, ConfigurationActivationPolicy const& k) noexcept;
void from_json(const json& j, ConfigurationActivationPolicy& k);

void to_json(json& j, GetConfigurationStatusEnum const& k) noexcept;
void from_json(const json& j, GetConfigurationStatusEnum& k);

void to_json(json& j, ApplyMode const& k) noexcept;
void from_json(const json& j, ApplyMode& k);

void to_json(json& j, ConfigMetadata const& k) noexcept;
void from_json(const json& j, ConfigMetadata& k);

void to_json(json& j, ListSlotIdsResult const& k) noexcept;
void from_json(const json& j, ListSlotIdsResult& k);

void to_json(json& j, GetActiveSlotIdResult const& k) noexcept;
void from_json(const json& j, GetActiveSlotIdResult& k);

void to_json(json& j, MarkActiveSlotResult const& k) noexcept;
void from_json(const json& j, MarkActiveSlotResult& k);

void to_json(json& j, StopModulesResult const& k) noexcept;
void from_json(const json& j, StopModulesResult& k);

void to_json(json& j, StartModulesResult const& k) noexcept;
void from_json(const json& j, StartModulesResult& k);

void to_json(json& j, DeleteSlotResult const& k) noexcept;
void from_json(const json& j, DeleteSlotResult& k);

void to_json(json& j, DuplicateSlotResult const& k) noexcept;
void from_json(const json& j, DuplicateSlotResult& k);

void to_json(json& j, LoadFromYamlResult const& k) noexcept;
void from_json(const json& j, LoadFromYamlResult& k);

void to_json(json& j, ConfigurationParameterIdentifier const& k) noexcept;
void from_json(const json& j, ConfigurationParameterIdentifier& k);

void to_json(json& j, ConfigurationParameterUpdate const& k) noexcept;
void from_json(const json& j, ConfigurationParameterUpdate& k);

void to_json(json& j, ConfigurationParameterUpdateRequest const& k) noexcept;
void from_json(const json& j, ConfigurationParameterUpdateRequest& k);

void to_json(json& j, ConfigurationParameterUpdateRequestResult const& k) noexcept;
void from_json(const json& j, ConfigurationParameterUpdateRequestResult& k);

void to_json(json& j, OriginOfChange const& k) noexcept;
void from_json(const json& j, OriginOfChange& k);

void to_json(json& j, ActiveSlotUpdateNotice const& k) noexcept;
void from_json(const json& j, ActiveSlotUpdateNotice& k);

void to_json(json& j, ConfigurationParameterUpdateResultRecord const& k) noexcept;
void from_json(const json& j, ConfigurationParameterUpdateResultRecord& k);

void to_json(json& j, ConfigurationParameterUpdateResult const& k) noexcept;
void from_json(const json& j, ConfigurationParameterUpdateResult& k);

void to_json(json& j, ConfigurationParameterUpdateNotice const& k) noexcept;
void from_json(const json& j, ConfigurationParameterUpdateNotice& k);

void to_json(json& j, ExecutionStatusUpdateNotice const& k) noexcept;
void from_json(const json& j, ExecutionStatusUpdateNotice& k);

void to_json(json& j, Mapping const& k) noexcept;
void from_json(const json& j, Mapping& k);

void to_json(json& j, ImplMapping const& k) noexcept;
void from_json(const json& j, ImplMapping& k);

void to_json(json& j, ModuleTierMappings const& k) noexcept;
void from_json(const json& j, ModuleTierMappings& k);

void to_json(json& j, ReqFulfillment const& k) noexcept;
void from_json(const json& j, ReqFulfillment& k);

void to_json(json& j, ModuleConnection const& k) noexcept;
void from_json(const json& j, ModuleConnection& k);

void to_json(json& j, ModuleConnections const& k) noexcept;
void from_json(const json& j, ModuleConnections& k);

void to_json(json& j, ConfigurationParameterCharacteristics const& k) noexcept;
void from_json(const json& j, ConfigurationParameterCharacteristics& k);

void to_json(json& j, ConfigurationParameter const& k) noexcept;
void from_json(const json& j, ConfigurationParameter& k);

void to_json(json& j, ImplementationConfigurationParameter const& k) noexcept;
void from_json(const json& j, ImplementationConfigurationParameter& k);

void to_json(json& j, ModuleConfiguration const& k) noexcept;
void from_json(const json& j, ModuleConfiguration& k);

void to_json(json& j, GetConfigurationResult const& k) noexcept;
void from_json(const json& j, GetConfigurationResult& k);

void to_json(json& j, MarkActiveSlotRequest const& k) noexcept;
void from_json(const json& j, MarkActiveSlotRequest& k);

void to_json(json& j, DeleteSlotRequest const& k) noexcept;
void from_json(const json& j, DeleteSlotRequest& k);

void to_json(json& j, DuplicateSlotRequest const& k) noexcept;
void from_json(const json& j, DuplicateSlotRequest& k);

void to_json(json& j, LoadFromYamlRequest const& k) noexcept;
void from_json(const json& j, LoadFromYamlRequest& k);

void to_json(json& j, GetConfigurationRequest const& k) noexcept;
void from_json(const json& j, GetConfigurationRequest& k);

} // namespace everest::lib::API::V1_0::types::config_service