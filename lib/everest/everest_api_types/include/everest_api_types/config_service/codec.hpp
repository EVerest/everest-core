// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include "API.hpp"
#include <optional>
#include <string>

namespace everest::lib::API::V1_0::types::config_service {

std::string serialize(MarkActiveSlotResultEnum val) noexcept;
std::string serialize(StopModulesResultEnum val) noexcept;
std::string serialize(StartModulesResultEnum val) noexcept;
std::string serialize(DeleteSlotResultEnum val) noexcept;
std::string serialize(ConfigurationParameterUpdateResultEnum val) noexcept;
std::string serialize(ActiveSlotStatusEnum val) noexcept;
std::string serialize(ExecutionStatusEnum val) noexcept;
std::string serialize(ConfigurationParameterDatatype val) noexcept;
std::string serialize(ConfigurationParameterMutability val) noexcept;
std::string serialize(ConfigurationActivationPolicy val) noexcept;
std::string serialize(GetConfigurationStatusEnum val) noexcept;
std::string serialize(ApplyMode val) noexcept;

std::string serialize(ConfigMetadata const& val) noexcept;
std::string serialize(ListSlotIdsResult const& val) noexcept;
std::string serialize(GetActiveSlotIdResult const& val) noexcept;
std::string serialize(MarkActiveSlotResult const& val) noexcept;
std::string serialize(StopModulesResult const& val) noexcept;
std::string serialize(StartModulesResult const& val) noexcept;
std::string serialize(DeleteSlotResult const& val) noexcept;
std::string serialize(DuplicateSlotResult const& val) noexcept;
std::string serialize(LoadFromYamlResult const& val) noexcept;
std::string serialize(ConfigurationParameterIdentifier const& val) noexcept;
std::string serialize(ConfigurationParameterUpdate const& val) noexcept;
std::string serialize(ConfigurationParameterUpdateRequest const& val) noexcept;
std::string serialize(ConfigurationParameterUpdateRequestResult const& val) noexcept;
std::string serialize(OriginOfChange const& val) noexcept;
std::string serialize(ActiveSlotUpdateNotice const& val) noexcept;
std::string serialize(ConfigurationParameterUpdateResultRecord const& val) noexcept;
std::string serialize(ConfigurationParameterUpdateResult const& val) noexcept;
std::string serialize(ConfigurationParameterUpdateNotice const& val) noexcept;
std::string serialize(ExecutionStatusUpdateNotice const& val) noexcept;
std::string serialize(Mapping const& val) noexcept;
std::string serialize(ImplMapping const& val) noexcept;
std::string serialize(ModuleTierMappings const& val) noexcept;
std::string serialize(ReqFulfillment const& val) noexcept;
std::string serialize(ModuleConnection const& val) noexcept;
std::string serialize(ConfigurationParameterCharacteristics const& val) noexcept;
std::string serialize(ConfigurationParameter const& val) noexcept;
std::string serialize(ImplementationConfigurationParameter const& val) noexcept;
std::string serialize(ModuleConfiguration const& val) noexcept;
std::string serialize(GetConfigurationResult const& val) noexcept;
std::string serialize(MarkActiveSlotRequest const& val) noexcept;
std::string serialize(DeleteSlotRequest const& val) noexcept;
std::string serialize(DuplicateSlotRequest const& val) noexcept;
std::string serialize(LoadFromYamlRequest const& val) noexcept;
std::string serialize(GetConfigurationRequest const& val) noexcept;

std::ostream& operator<<(std::ostream& os, MarkActiveSlotResultEnum const& val);
std::ostream& operator<<(std::ostream& os, StopModulesResultEnum const& val);
std::ostream& operator<<(std::ostream& os, StartModulesResultEnum const& val);
std::ostream& operator<<(std::ostream& os, DeleteSlotResultEnum const& val);
std::ostream& operator<<(std::ostream& os, ConfigurationParameterUpdateResultEnum const& val);
std::ostream& operator<<(std::ostream& os, ActiveSlotStatusEnum const& val);
std::ostream& operator<<(std::ostream& os, ExecutionStatusEnum const& val);
std::ostream& operator<<(std::ostream& os, ConfigurationParameterDatatype const& val);
std::ostream& operator<<(std::ostream& os, ConfigurationParameterMutability const& val);
std::ostream& operator<<(std::ostream& os, ConfigurationActivationPolicy const& val);
std::ostream& operator<<(std::ostream& os, GetConfigurationStatusEnum const& val);
std::ostream& operator<<(std::ostream& os, ApplyMode const& val);

std::ostream& operator<<(std::ostream& os, ConfigMetadata const& val);
std::ostream& operator<<(std::ostream& os, ListSlotIdsResult const& val);
std::ostream& operator<<(std::ostream& os, GetActiveSlotIdResult const& val);
std::ostream& operator<<(std::ostream& os, MarkActiveSlotResult const& val);
std::ostream& operator<<(std::ostream& os, StopModulesResult const& val);
std::ostream& operator<<(std::ostream& os, StartModulesResult const& val);
std::ostream& operator<<(std::ostream& os, DeleteSlotResult const& val);
std::ostream& operator<<(std::ostream& os, DuplicateSlotResult const& val);
std::ostream& operator<<(std::ostream& os, LoadFromYamlResult const& val);
std::ostream& operator<<(std::ostream& os, ConfigurationParameterIdentifier const& val);
std::ostream& operator<<(std::ostream& os, ConfigurationParameterUpdate const& val);
std::ostream& operator<<(std::ostream& os, ConfigurationParameterUpdateRequest const& val);
std::ostream& operator<<(std::ostream& os, ConfigurationParameterUpdateRequestResult const& val);
std::ostream& operator<<(std::ostream& os, OriginOfChange const& val);
std::ostream& operator<<(std::ostream& os, ActiveSlotUpdateNotice const& val);
std::ostream& operator<<(std::ostream& os, ConfigurationParameterUpdateResultRecord const& val);
std::ostream& operator<<(std::ostream& os, ConfigurationParameterUpdateResult const& val);
std::ostream& operator<<(std::ostream& os, ConfigurationParameterUpdateNotice const& val);
std::ostream& operator<<(std::ostream& os, ExecutionStatusUpdateNotice const& val);
std::ostream& operator<<(std::ostream& os, Mapping const& val);
std::ostream& operator<<(std::ostream& os, ImplMapping const& val);
std::ostream& operator<<(std::ostream& os, ModuleTierMappings const& val);
std::ostream& operator<<(std::ostream& os, ReqFulfillment const& val);
std::ostream& operator<<(std::ostream& os, ModuleConnection const& val);
std::ostream& operator<<(std::ostream& os, ConfigurationParameterCharacteristics const& val);
std::ostream& operator<<(std::ostream& os, ConfigurationParameter const& val);
std::ostream& operator<<(std::ostream& os, ImplementationConfigurationParameter const& val);
std::ostream& operator<<(std::ostream& os, ModuleConfiguration const& val);
std::ostream& operator<<(std::ostream& os, GetConfigurationResult const& val);
std::ostream& operator<<(std::ostream& os, MarkActiveSlotRequest const& val);
std::ostream& operator<<(std::ostream& os, DeleteSlotRequest const& val);
std::ostream& operator<<(std::ostream& os, DuplicateSlotRequest const& val);
std::ostream& operator<<(std::ostream& os, LoadFromYamlRequest const& val);
std::ostream& operator<<(std::ostream& os, GetConfigurationRequest const& val);


template <class T> T deserialize(std::string const& val);
template <class T> std::optional<T> try_deserialize(std::string const& val) {
    try {
        return deserialize<T>(val);
    } catch (...) {
        return std::nullopt;
    }
}
template <class T> bool adl_deserialize(std::string const& json_data, T& obj) {
    auto opt = try_deserialize<T>(json_data);
    if (opt) {
        obj = opt.value();
        return true;
    }
    return false;
}

} // namespace everest::lib::API::V1_0::types::config_service
