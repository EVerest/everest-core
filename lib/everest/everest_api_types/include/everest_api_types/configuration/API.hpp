// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace everest::lib::API::V1_0::types::configuration {

enum class MarkActiveSlotResultEnum {
    Success,
    AlreadyActive,
    DoesNotExist,
    Rejected,
};

enum class DeleteSlotResultEnum {
    Success,
    CannotDeleteActiveSlot,
    DoesNotExist,
    Rejected,
};

enum class ConfigurationParameterUpdateResultEnum {
    Applied,
    WillApplyOnRestart,
    DoesNotExist,
    Rejected,
};

enum class ActiveSlotStatusEnum {
    Running,
    FailedToStart,
    RestartTriggered,
};

enum class ConfigurationParameterDatatype {
    String,
    Decimal,
    Integer,
    Boolean,
    Unknown,
};

enum class ConfigurationParameterMutability {
    ReadOnly,
    ReadWrite,
    WriteOnly,
};

enum class GetConfigurationStatusEnum {
    Success,
    SlotDoesNotExist,
};

struct ConfigMetadata {
    int32_t slot_id;
    std::string last_updated;
    bool is_valid;
    std::optional<std::string> description;
    std::optional<std::string> config_file_path;
};

struct ListSlotIdsResult {
    std::vector<ConfigMetadata> slots;
};

struct GetActiveSlotIdResult {
    std::optional<int32_t> slot_id;
};

struct MarkActiveSlotResult {
    MarkActiveSlotResultEnum result;
};

struct DeleteSlotResult {
    DeleteSlotResultEnum result;
};

struct DuplicateSlotResult {
    bool success;
    std::optional<int32_t> slot_id;
};

struct LoadFromYamlResult {
    bool success;
    std::string error_message;
    std::optional<int32_t> slot_id;
};

struct ConfigurationParameterIdentifier {
    std::string module_id;
    std::string parameter_name;
    std::optional<std::string> implementation_id;
};

struct ConfigurationParameterUpdate {
    ConfigurationParameterIdentifier cfg_param_id;
    std::string value;
};

struct ConfigurationParameterUpdateRequest {
    int32_t slot_id;
    std::vector<ConfigurationParameterUpdate> parameter_updates;
};

struct ConfigurationParameterUpdateRequestResult {
    std::vector<ConfigurationParameterUpdateResultEnum> results;
};

struct OriginOfChange {
    bool external;
    std::optional<std::string> identifier;
};

struct ActiveSlotUpdateNotice {
    std::string tstamp;
    int32_t slot_id;
    ActiveSlotStatusEnum status;
};

struct ConfigurationParameterUpdateResultRecord {
    ConfigurationParameterUpdate update;
    ConfigurationParameterUpdateResultEnum result;
};

struct ConfigurationParameterUpdateNotice {
    std::string tstamp;
    int32_t slot_id;
    std::vector<ConfigurationParameterUpdateResultRecord> update_results;
    //     OriginOfChange origin;  // TODO(CB): Not part of the internal Config Service
};

struct Mapping {
    int32_t evse;
    std::optional<int32_t> connector;
};

struct ImplMapping {
    std::string implementation_id;
    Mapping mapping;
};

struct ModuleTierMappings {
    std::optional<Mapping> module;
    std::vector<ImplMapping> implementations;
};

struct ReqFulfillment {
    std::string module_id;
    std::string implementation_id;
    int32_t index;
};

struct ModuleConnection {
    std::string requirement_id;
    std::vector<ReqFulfillment> fulfillments;
};

struct ConfigurationParameterCharacteristics {
    ConfigurationParameterDatatype datatype;
    ConfigurationParameterMutability mutability;
    std::optional<std::string> unit;
    std::optional<int32_t> min_value;
    std::optional<int32_t> max_value;
};

struct ConfigurationParameter {
    std::string name;
    std::string value;
    ConfigurationParameterCharacteristics characteristics;
};

struct ImplementationConfigurationParameter {
    std::string implementation_id;
    std::vector<ConfigurationParameter> configuration_parameters;
};

struct TelemetryConfig {
    int32_t id;
};

struct ModuleConfigAccess {
    std::string module_id;
    bool allow_read;
    bool allow_write;
    bool allow_set_read_only;
};

struct ConfigAccess {
    bool allow_global_read;
    bool allow_global_write;
    bool allow_set_read_only;
    std::vector<ModuleConfigAccess> module_config_access;
};

struct ConfigAccessControl {
    std::optional<ConfigAccess> config;
};

struct ModuleConfiguration {
    std::string module_id;
    std::string module_name;
    std::vector<ModuleConnection> connections;
    ModuleTierMappings mapping;
    std::vector<ConfigurationParameter> module_configuration_parameters;
    std::vector<ImplementationConfigurationParameter> implementation_configuration_parameters;
    bool standalone;
    bool telemetry_enabled;
    std::optional<TelemetryConfig> telemetry_config;
    ConfigAccessControl config_access;
};

struct GetConfigurationResult {
    GetConfigurationStatusEnum status;
    std::optional<std::vector<ModuleConfiguration>> module_configurations;
};

struct MarkActiveSlotRequest {
    int32_t slot_id;
};

struct DeleteSlotRequest {
    int32_t slot_id;
};

struct DuplicateSlotRequest {
    int32_t slot_id;
    std::optional<std::string> new_description;
};

struct LoadFromYamlRequest {
    std::string raw_yaml;
    std::optional<int32_t> slot_id;
    std::optional<std::string> description;
};

struct GetConfigurationRequest {
    int32_t slot_id;
};

} // namespace everest::lib::API::V1_0::types::configuration
