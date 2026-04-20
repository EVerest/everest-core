// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "config_service/codec.hpp"
#include <string>
#include "nlohmann/json.hpp"
#include "config_service/API.hpp"
#include "config_service/json_codec.hpp"
#include "utilities/constants.hpp"

namespace everest::lib::API::V1_0::types::config_service {

std::string serialize(MarkActiveSlotResultEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(DeleteSlotResultEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigurationParameterUpdateResultEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ActiveSlotStatusEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigurationParameterDatatype val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigurationParameterMutability val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigurationActivationPolicy val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(GetConfigurationStatusEnum val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigMetadata const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ListSlotIdsResult const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(GetActiveSlotIdResult const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(MarkActiveSlotResult const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(DeleteSlotResult const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(DuplicateSlotResult const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(LoadFromYamlResult const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigurationParameterIdentifier const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigurationParameterUpdate const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigurationParameterUpdateRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigurationParameterUpdateRequestResult const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(OriginOfChange const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ActiveSlotUpdateNotice const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigurationParameterUpdateResultRecord const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigurationParameterUpdateNotice const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(Mapping const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ImplMapping const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ModuleTierMappings const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ReqFulfillment const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ModuleConnection const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigurationParameterCharacteristics const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigurationParameter const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ImplementationConfigurationParameter const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(TelemetryConfig const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ModuleConfigAccess const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigAccess const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ConfigAccessControl const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(ModuleConfiguration const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(GetConfigurationResult const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(MarkActiveSlotRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(DeleteSlotRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(DuplicateSlotRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(LoadFromYamlRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::string serialize(GetConfigurationRequest const& val) noexcept {
    json result = val;
    return result.dump(json_indent);
}

std::ostream& operator<<(std::ostream& os, MarkActiveSlotResultEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, DeleteSlotResultEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigurationParameterUpdateResultEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ActiveSlotStatusEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigurationParameterDatatype const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigurationParameterMutability const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigurationActivationPolicy const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, GetConfigurationStatusEnum const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigMetadata const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ListSlotIdsResult const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, GetActiveSlotIdResult const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, MarkActiveSlotResult const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, DeleteSlotResult const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, DuplicateSlotResult const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, LoadFromYamlResult const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigurationParameterIdentifier const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigurationParameterUpdate const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigurationParameterUpdateRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigurationParameterUpdateRequestResult const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, OriginOfChange const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ActiveSlotUpdateNotice const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigurationParameterUpdateResultRecord const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigurationParameterUpdateNotice const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, Mapping const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ImplMapping const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ModuleTierMappings const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ReqFulfillment const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ModuleConnection const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigurationParameterCharacteristics const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigurationParameter const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ImplementationConfigurationParameter const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, TelemetryConfig const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ModuleConfigAccess const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigAccess const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ConfigAccessControl const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, ModuleConfiguration const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, GetConfigurationResult const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, MarkActiveSlotRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, DeleteSlotRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, DuplicateSlotRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, LoadFromYamlRequest const& val) {
    os << serialize(val);
    return os;
}

std::ostream& operator<<(std::ostream& os, GetConfigurationRequest const& val) {
    os << serialize(val);
    return os;
}

template <> MarkActiveSlotResultEnum deserialize(std::string const& val) {
    auto data = json::parse(val);
    MarkActiveSlotResultEnum obj = data;
    return obj;
}

template <> DeleteSlotResultEnum deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    DeleteSlotResultEnum obj = data;
    return obj;
}

template <> ConfigurationParameterUpdateResultEnum deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigurationParameterUpdateResultEnum obj = data;
    return obj;
}

template <> ActiveSlotStatusEnum deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ActiveSlotStatusEnum obj = data;
    return obj;
}

template <> ConfigurationParameterDatatype deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigurationParameterDatatype obj = data;
    return obj;
}

template <> ConfigurationParameterMutability deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigurationParameterMutability obj = data;
    return obj;
}

template <> ConfigurationActivationPolicy deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigurationActivationPolicy obj = data;
    return obj;
}

template <> GetConfigurationStatusEnum deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    GetConfigurationStatusEnum obj = data;
    return obj;
}

template <> ConfigMetadata deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigMetadata obj = data;
    return obj;
}

template <> ListSlotIdsResult deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ListSlotIdsResult obj = data;
    return obj;
}

template <> GetActiveSlotIdResult deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    GetActiveSlotIdResult obj = data;
    return obj;
}

template <> MarkActiveSlotResult deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    MarkActiveSlotResult obj = data;
    return obj;
}

template <> DeleteSlotResult deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    DeleteSlotResult obj = data;
    return obj;
}

template <> DuplicateSlotResult deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    DuplicateSlotResult obj = data;
    return obj;
}

template <> LoadFromYamlResult deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    LoadFromYamlResult obj = data;
    return obj;
}

template <> ConfigurationParameterIdentifier deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigurationParameterIdentifier obj = data;
    return obj;
}

template <> ConfigurationParameterUpdate deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigurationParameterUpdate obj = data;
    return obj;
}

template <> ConfigurationParameterUpdateRequest deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigurationParameterUpdateRequest obj = data;
    return obj;
}

template <> ConfigurationParameterUpdateRequestResult deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigurationParameterUpdateRequestResult obj = data;
    return obj;
}

template <> OriginOfChange deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    OriginOfChange obj = data;
    return obj;
}

template <> ActiveSlotUpdateNotice deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ActiveSlotUpdateNotice obj = data;
    return obj;
}

template <> ConfigurationParameterUpdateResultRecord deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigurationParameterUpdateResultRecord obj = data;
    return obj;
}

template <> ConfigurationParameterUpdateNotice deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigurationParameterUpdateNotice obj = data;
    return obj;
}

template <> Mapping deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    Mapping obj = data;
    return obj;
}

template <> ImplMapping deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ImplMapping obj = data;
    return obj;
}

template <> ModuleTierMappings deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ModuleTierMappings obj = data;
    return obj;
}

template <> ReqFulfillment deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ReqFulfillment obj = data;
    return obj;
}

template <> ModuleConnection deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ModuleConnection obj = data;
    return obj;
}

template <> ConfigurationParameterCharacteristics deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigurationParameterCharacteristics obj = data;
    return obj;
}

template <> ConfigurationParameter deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigurationParameter obj = data;
    return obj;
}

template <> ImplementationConfigurationParameter deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ImplementationConfigurationParameter obj = data;
    return obj;
}

template <> TelemetryConfig deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    TelemetryConfig obj = data;
    return obj;
}

template <> ModuleConfigAccess deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ModuleConfigAccess obj = data;
    return obj;
}

template <> ConfigAccess deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigAccess obj = data;
    return obj;
}

template <> ConfigAccessControl deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ConfigAccessControl obj = data;
    return obj;
}

template <> ModuleConfiguration deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    ModuleConfiguration obj = data;
    return obj;
}

template <> GetConfigurationResult deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    GetConfigurationResult obj = data;
    return obj;
}

template <> MarkActiveSlotRequest deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    MarkActiveSlotRequest obj = data;
    return obj;
}

template <> DeleteSlotRequest deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    DeleteSlotRequest obj = data;
    return obj;
}

template <> DuplicateSlotRequest deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    DuplicateSlotRequest obj = data;
    return obj;
}

template <> LoadFromYamlRequest deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    LoadFromYamlRequest obj = data;
    return obj;
}

template <> GetConfigurationRequest deserialize<>(std::string const& val) {
    auto data = json::parse(val);
    GetConfigurationRequest obj = data;
    return obj;
}

} // namespace everest::lib::API::V1_0::types::config_service
