// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include "config_service/json_codec.hpp"
#include "config_service/API.hpp"
#include "config_service/codec.hpp"

#include "nlohmann/json.hpp"
#include <stdexcept>
#include <unordered_set>

namespace everest::lib::API::V1_0::types::config_service {

void to_json(json& j, MarkActiveSlotResultEnum const& k) noexcept {
    switch (k) {
    case MarkActiveSlotResultEnum::Success:
        j = "Success";
        return;
    case MarkActiveSlotResultEnum::AlreadyActive:
        j = "AlreadyActive";
        return;
    case MarkActiveSlotResultEnum::DoesNotExist:
        j = "DoesNotExist";
        return;
    case MarkActiveSlotResultEnum::Rejected:
        j = "Rejected";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::config_service::MarkActiveSlotResultEnum";
}

void from_json(const json& j, MarkActiveSlotResultEnum& k) {
    std::string s = j;
    if (s == "Success") {
        k = MarkActiveSlotResultEnum::Success;
        return;
    }
    if (s == "AlreadyActive") {
        k = MarkActiveSlotResultEnum::AlreadyActive;
        return;
    }
    if (s == "DoesNotExist") {
        k = MarkActiveSlotResultEnum::DoesNotExist;
        return;
    }
    if (s == "Rejected") {
        k = MarkActiveSlotResultEnum::Rejected;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type "
                            "everest::lib::API::V1_0::types::config_service::MarkActiveSlotResultEnum");
}

void to_json(json& j, DeleteSlotResultEnum const& k) noexcept {
    switch (k) {
    case DeleteSlotResultEnum::Success:
        j = "Success";
        return;
    case DeleteSlotResultEnum::CannotDeleteActiveSlot:
        j = "CannotDeleteActiveSlot";
        return;
    case DeleteSlotResultEnum::DoesNotExist:
        j = "DoesNotExist";
        return;
    case DeleteSlotResultEnum::Rejected:
        j = "Rejected";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::config_service::DeleteSlotResultEnum";
}

void from_json(const json& j, DeleteSlotResultEnum& k) {
    std::string s = j;
    if (s == "Success") {
        k = DeleteSlotResultEnum::Success;
        return;
    }
    if (s == "CannotDeleteActiveSlot") {
        k = DeleteSlotResultEnum::CannotDeleteActiveSlot;
        return;
    }
    if (s == "DoesNotExist") {
        k = DeleteSlotResultEnum::DoesNotExist;
        return;
    }
    if (s == "Rejected") {
        k = DeleteSlotResultEnum::Rejected;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::config_service::DeleteSlotResultEnum");
}

void to_json(json& j, ConfigurationParameterUpdateResultEnum const& k) noexcept {
    switch (k) {
    case ConfigurationParameterUpdateResultEnum::Applied:
        j = "Applied";
        return;
    case ConfigurationParameterUpdateResultEnum::WillApplyOnRestart:
        j = "WillApplyOnRestart";
        return;
    case ConfigurationParameterUpdateResultEnum::DoesNotExist:
        j = "DoesNotExist";
        return;
    case ConfigurationParameterUpdateResultEnum::Rejected:
        j = "Rejected";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::config_service::ConfigurationParameterUpdateResultEnum";
}

void from_json(const json& j, ConfigurationParameterUpdateResultEnum& k) {
    std::string s = j;
    if (s == "Applied") {
        k = ConfigurationParameterUpdateResultEnum::Applied;
        return;
    }
    if (s == "WillApplyOnRestart") {
        k = ConfigurationParameterUpdateResultEnum::WillApplyOnRestart;
        return;
    }
    if (s == "DoesNotExist") {
        k = ConfigurationParameterUpdateResultEnum::DoesNotExist;
        return;
    }
    if (s == "Rejected") {
        k = ConfigurationParameterUpdateResultEnum::Rejected;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type "
                            "everest::lib::API::V1_0::types::config_service::ConfigurationParameterUpdateResultEnum");
}

void to_json(json& j, ActiveSlotStatusEnum const& k) noexcept {
    switch (k) {
    case ActiveSlotStatusEnum::Running:
        j = "Running";
        return;
    case ActiveSlotStatusEnum::FailedToStart:
        j = "FailedToStart";
        return;
    case ActiveSlotStatusEnum::RestartTriggered:
        j = "RestartTriggered";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::config_service::ActiveSlotStatusEnum";
}

void from_json(const json& j, ActiveSlotStatusEnum& k) {
    std::string s = j;
    if (s == "Running") {
        k = ActiveSlotStatusEnum::Running;
        return;
    }
    if (s == "FailedToStart") {
        k = ActiveSlotStatusEnum::FailedToStart;
        return;
    }
    if (s == "RestartTriggered") {
        k = ActiveSlotStatusEnum::RestartTriggered;
        return;
    }

    throw std::out_of_range(
        "Provided string " + s +
        " could not be converted to enum of type everest::lib::API::V1_0::types::config_service::ActiveSlotStatusEnum");
}

void to_json(json& j, ConfigurationParameterDatatype const& k) noexcept {
    switch (k) {
    case ConfigurationParameterDatatype::String:
        j = "String";
        return;
    case ConfigurationParameterDatatype::Decimal:
        j = "Decimal";
        return;
    case ConfigurationParameterDatatype::Integer:
        j = "Integer";
        return;
    case ConfigurationParameterDatatype::Boolean:
        j = "Boolean";
        return;
    case ConfigurationParameterDatatype::Unknown:
        j = "Unknown";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::config_service::ConfigurationParameterDatatype";
}

void from_json(const json& j, ConfigurationParameterDatatype& k) {
    std::string s = j;
    if (s == "String") {
        k = ConfigurationParameterDatatype::String;
        return;
    }
    if (s == "Decimal") {
        k = ConfigurationParameterDatatype::Decimal;
        return;
    }
    if (s == "Integer") {
        k = ConfigurationParameterDatatype::Integer;
        return;
    }
    if (s == "Boolean") {
        k = ConfigurationParameterDatatype::Boolean;
        return;
    }
    if (s == "Unknown") {
        k = ConfigurationParameterDatatype::Unknown;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type "
                            "everest::lib::API::V1_0::types::config_service::ConfigurationParameterDatatype");
}

void to_json(json& j, ConfigurationParameterMutability const& k) noexcept {
    switch (k) {
    case ConfigurationParameterMutability::ReadOnly:
        j = "ReadOnly";
        return;
    case ConfigurationParameterMutability::ReadWrite:
        j = "ReadWrite";
        return;
    case ConfigurationParameterMutability::WriteOnly:
        j = "WriteOnly";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::config_service::ConfigurationParameterMutability";
}

void from_json(const json& j, ConfigurationParameterMutability& k) {
    std::string s = j;
    if (s == "ReadOnly") {
        k = ConfigurationParameterMutability::ReadOnly;
        return;
    }
    if (s == "ReadWrite") {
        k = ConfigurationParameterMutability::ReadWrite;
        return;
    }
    if (s == "WriteOnly") {
        k = ConfigurationParameterMutability::WriteOnly;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type "
                            "everest::lib::API::V1_0::types::config_service::ConfigurationParameterMutability");
}

void to_json(json& j, GetConfigurationStatusEnum const& k) noexcept {
    switch (k) {
    case GetConfigurationStatusEnum::Success:
        j = "Success";
        return;
    case GetConfigurationStatusEnum::SlotDoesNotExist:
        j = "SlotDoesNotExist";
        return;
    }

    j = "INVALID_VALUE__everest::lib::API::V1_0::types::config_service::GetConfigurationStatusEnum";
}

void from_json(const json& j, GetConfigurationStatusEnum& k) {
    std::string s = j;
    if (s == "Success") {
        k = GetConfigurationStatusEnum::Success;
        return;
    }
    if (s == "SlotDoesNotExist") {
        k = GetConfigurationStatusEnum::SlotDoesNotExist;
        return;
    }

    throw std::out_of_range("Provided string " + s +
                            " could not be converted to enum of type "
                            "everest::lib::API::V1_0::types::config_service::GetConfigurationStatusEnum");
}

void to_json(json& j, ConfigMetadata const& k) noexcept {
    j = json{{"slot_id", k.slot_id}, {"last_updated", k.last_updated}, {"is_valid", k.is_valid}};
    if (k.description) {
        j["description"] = *k.description;
    }
    if (k.config_file_path) {
        j["config_file_path"] = *k.config_file_path;
    }
}

void from_json(const json& j, ConfigMetadata& k) {
    k.slot_id = j.at("slot_id");
    k.last_updated = j.at("last_updated");
    k.is_valid = j.at("is_valid");
    if (j.contains("description")) {
        k.description = j.at("description");
    }
    if (j.contains("config_file_path")) {
        k.config_file_path = j.at("config_file_path");
    }
}

void to_json(json& j, ListSlotIdsResult const& k) noexcept {
    j = json{{"slots", k.slots}};
}

void from_json(const json& j, ListSlotIdsResult& k) {
    for (auto& item : j.at("slots")) {
        k.slots.push_back(item);
    }
}

void to_json(json& j, GetActiveSlotIdResult const& k) noexcept {
    j = json{};
    if (k.slot_id) {
        j["slot_id"] = *k.slot_id;
    }
}

void from_json(const json& j, GetActiveSlotIdResult& k) {
    if (j.contains("slot_id")) {
        k.slot_id = j.at("slot_id");
    }
}

void to_json(json& j, MarkActiveSlotResult const& k) noexcept {
    j = json{{"result", k.result}};
}

void from_json(const json& j, MarkActiveSlotResult& k) {
    k.result = j.at("result");
}

void to_json(json& j, DeleteSlotResult const& k) noexcept {
    j = json{{"result", k.result}};
}

void from_json(const json& j, DeleteSlotResult& k) {
    k.result = j.at("result");
}

void to_json(json& j, DuplicateSlotResult const& k) noexcept {
    j = json{{"success", k.success}};
    if (k.slot_id) {
        j["slot_id"] = *k.slot_id;
    }
}

void from_json(const json& j, DuplicateSlotResult& k) {
    k.success = j.at("success");
    if (j.contains("slot_id")) {
        k.slot_id = j.at("slot_id");
    }
}

void to_json(json& j, LoadFromYamlResult const& k) noexcept {
    j = json{{"success", k.success}, {"error_message", k.error_message}};
    if (k.slot_id) {
        j["slot_id"] = *k.slot_id;
    }
}

void from_json(const json& j, LoadFromYamlResult& k) {
    k.success = j.at("success");
    k.error_message = j.at("error_message");
    if (j.contains("slot_id")) {
        k.slot_id = j.at("slot_id");
    }
}

void to_json(json& j, ConfigurationParameterIdentifier const& k) noexcept {
    j = json{{"module_id", k.module_id}, {"parameter_name", k.parameter_name}};
    if (k.implementation_id) {
        j["implementation_id"] = *k.implementation_id;
    }
}

void from_json(const json& j, ConfigurationParameterIdentifier& k) {
    k.module_id = j.at("module_id");
    k.parameter_name = j.at("parameter_name");
    if (j.contains("implementation_id")) {
        k.implementation_id = j.at("implementation_id");
    }
}

void to_json(json& j, ConfigurationParameterUpdate const& k) noexcept {
    j = json{{"cfg_param_id", k.cfg_param_id}, {"value", k.value}};
}

void from_json(const json& j, ConfigurationParameterUpdate& k) {
    k.cfg_param_id = j.at("cfg_param_id");
    k.value = j.at("value");
}

void to_json(json& j, ConfigurationParameterUpdateRequest const& k) noexcept {
    j = json{{"slot_id", k.slot_id}, {"parameter_updates", k.parameter_updates}};
}

void from_json(const json& j, ConfigurationParameterUpdateRequest& k) {
    k.slot_id = j.at("slot_id");
    for (auto& item : j.at("parameter_updates")) {
        k.parameter_updates.push_back(item);
    }
}

void to_json(json& j, ConfigurationParameterUpdateRequestResult const& k) noexcept {
    j = json{{"results", k.results}};
}

void from_json(const json& j, ConfigurationParameterUpdateRequestResult& k) {
    for (auto& item : j.at("results")) {
        k.results.push_back(item);
    }
}

void to_json(json& j, OriginOfChange const& k) noexcept {
    j = json{{"external", k.external}};
    if (k.identifier) {
        j["identifier"] = *k.identifier;
    }
}

void from_json(const json& j, OriginOfChange& k) {
    k.external = j.at("external");
    if (j.contains("identifier")) {
        k.identifier = j.at("identifier");
    }
}

void to_json(json& j, ActiveSlotUpdateNotice const& k) noexcept {
    j = json{{"tstamp", k.tstamp}, {"slot_id", k.slot_id}, {"status", k.status}};
}

void from_json(const json& j, ActiveSlotUpdateNotice& k) {
    k.tstamp = j.at("tstamp");
    k.slot_id = j.at("slot_id");
    k.status = j.at("status");
}

void to_json(json& j, ConfigurationParameterUpdateResultRecord const& k) noexcept {
    j = json{{"update", k.update}, {"result", k.result}};
}

void from_json(const json& j, ConfigurationParameterUpdateResultRecord& k) {
    k.update = j.at("update");
    k.result = j.at("result");
}

void to_json(json& j, ConfigurationParameterUpdateNotice const& k) noexcept {
    j = json{{"tstamp", k.tstamp},
             {"slot_id", k.slot_id},
             {"update_results", k.update_results}}; // TODO(CB) : Missing internally:, {"origin", k.origin}};
}

void from_json(const json& j, ConfigurationParameterUpdateNotice& k) {
    k.tstamp = j.at("tstamp");
    k.slot_id = j.at("slot_id");
    for (auto& item : j.at("update_results")) {
        k.update_results.push_back(item);
    }
    // TODO(CB) : Missing internally: k.origin = j.at("origin");
}

void to_json(json& j, Mapping const& k) noexcept {
    j = json{{"evse", k.evse}};
    if (k.connector) {
        j["connector"] = *k.connector;
    }
}

void from_json(const json& j, Mapping& k) {
    k.evse = j.at("evse");
    if (j.contains("connector")) {
        k.connector = j.at("connector");
    }
}

void to_json(json& j, ImplMapping const& k) noexcept {
    j = json{{"implementation_id", k.implementation_id}, {"mapping", k.mapping}};
}

void from_json(const json& j, ImplMapping& k) {
    k.implementation_id = j.at("implementation_id");
    k.mapping = j.at("mapping");
}

void to_json(json& j, ModuleTierMappings const& k) noexcept {
    if (k.module) {
        j["module"] = *k.module;
    }
    j["implementations"] = k.implementations;
}

void from_json(const json& j, ModuleTierMappings& k) {
    if (j.contains("module")) {
        k.module = j.at("module");
    }
    for (auto& item : j.at("implementations")) {
        k.implementations.push_back(item);
    }
}

void to_json(json& j, ReqFulfillment const& k) noexcept {
    j = json{{"module_id", k.module_id}, {"implementation_id", k.implementation_id}, {"index", k.index}};
}

void from_json(const json& j, ReqFulfillment& k) {
    k.module_id = j.at("module_id");
    k.implementation_id = j.at("implementation_id");
    k.index = j.at("index");
}

void to_json(json& j, ModuleConnection const& k) noexcept {
    j = json{{"requirement_id", k.requirement_id}, {"fulfillments", k.fulfillments}};
}

void from_json(const json& j, ModuleConnection& k) {
    k.requirement_id = j.at("requirement_id");
    for (auto& item : j.at("fulfillments")) {
        k.fulfillments.push_back(item);
    }
}

void to_json(json& j, ConfigurationParameterCharacteristics const& k) noexcept {
    j = json{{"datatype", k.datatype}, {"mutability", k.mutability}};
    if (k.unit) {
        j["unit"] = *k.unit;
    }
    if (k.min_value) {
        j["min_value"] = *k.min_value;
    }
    if (k.max_value) {
        j["max_value"] = *k.max_value;
    }
}

void from_json(const json& j, ConfigurationParameterCharacteristics& k) {
    k.datatype = j.at("datatype");
    k.mutability = j.at("mutability");
    if (j.contains("unit")) {
        k.unit = j.at("unit");
    }
    if (j.contains("min_value")) {
        k.min_value = j.at("min_value");
    }
    if (j.contains("max_value")) {
        k.max_value = j.at("max_value");
    }
}

void to_json(json& j, ConfigurationParameter const& k) noexcept {
    j = json{{"name", k.name}, {"value", k.value}, {"characteristics", k.characteristics}};
}

void from_json(const json& j, ConfigurationParameter& k) {
    k.name = j.at("name");
    k.value = j.at("value");
    k.characteristics = j.at("characteristics");
}

void to_json(json& j, ImplementationConfigurationParameter const& k) noexcept {
    j = json{{"implementation_id", k.implementation_id}, {"configuration_parameters", k.configuration_parameters}};
}

void from_json(const json& j, ImplementationConfigurationParameter& k) {
    k.implementation_id = j.at("implementation_id");
    for (auto& item : j.at("configuration_parameters")) {
        k.configuration_parameters.push_back(item);
    }
}

void to_json(json& j, TelemetryConfig const& k) noexcept {
    j = json{{"id", k.id}};
}

void from_json(const json& j, TelemetryConfig& k) {
    k.id = j.at("id");
}

void to_json(json& j, ModuleConfigAccess const& k) noexcept {
    j = json{{"module_id", k.module_id},
             {"allow_read", k.allow_read},
             {"allow_write", k.allow_write},
             {"allow_set_read_only", k.allow_set_read_only}};
}

void from_json(const json& j, ModuleConfigAccess& k) {
    k.module_id = j.at("module_id");
    k.allow_read = j.at("allow_read");
    k.allow_write = j.at("allow_write");
    k.allow_set_read_only = j.at("allow_set_read_only");
}

void to_json(json& j, ConfigAccess const& k) noexcept {
    j = json{{"allow_global_read", k.allow_global_read},
             {"allow_global_write", k.allow_global_write},
             {"allow_set_read_only", k.allow_set_read_only},
             {"module_config_access", k.module_config_access}};
}

void from_json(const json& j, ConfigAccess& k) {
    k.allow_global_read = j.at("allow_global_read");
    k.allow_global_write = j.at("allow_global_write");
    k.allow_set_read_only = j.at("allow_set_read_only");
    for (auto& item : j.at("module_config_access")) {
        k.module_config_access.push_back(item);
    }
}

void to_json(json& j, ConfigAccessControl const& k) noexcept {
    j = json{};
    if (k.config) {
        j["config"] = *k.config;
    }
}

void from_json(const json& j, ConfigAccessControl& k) {
    if (j.contains("config")) {
        k.config = j.at("config");
    } else {
        k.config = std::nullopt;
    }
}

void to_json(json& j, ModuleConfiguration const& k) noexcept {
    j = json{{"module_id", k.module_id},
             {"module_name", k.module_name},
             {"connections", k.connections},
             {"mapping", k.mapping},
             {"module_configuration_parameters", k.module_configuration_parameters},
             {"implementation_configuration_parameters", k.implementation_configuration_parameters},
             {"standalone", k.standalone},
             {"telemetry_enabled", k.telemetry_enabled}};
    if (k.telemetry_config) {
        j["telemetry_config"] = *k.telemetry_config;
    }
    j["config_access"] = k.config_access;
}

void from_json(const json& j, ModuleConfiguration& k) {
    k.module_id = j.at("module_id");
    k.module_name = j.at("module_name");
    k.connections = j.at("connections");
    k.mapping = j.at("mapping");
    for (auto& item : j.at("module_configuration_parameters")) {
        k.module_configuration_parameters.push_back(item);
    }
    for (auto& item : j.at("implementation_configuration_parameters")) {
        k.implementation_configuration_parameters.push_back(item);
    }
    k.standalone = j.at("standalone");
    k.telemetry_enabled = j.at("telemetry_enabled");
    if (j.contains("telemetry_config")) {
        k.telemetry_config = j.at("telemetry_config");
    }
    k.config_access = j["config_access"];
}

void to_json(json& j, GetConfigurationResult const& k) noexcept {
    j = json{};
    j["status"] = k.status;
    if (k.module_configurations) {
        j["module_configurations"] = *k.module_configurations;
    }
}

void from_json(const json& j, GetConfigurationResult& k) {
    k.status = j.at("status");
    if (j.contains("module_configurations")) {
        k.module_configurations.emplace<std::vector<ModuleConfiguration>>({});
        std::unordered_set<std::string> seen_ids;
        for (auto& item : j.at("module_configurations")) {
            ModuleConfiguration mc = item;
            if (!seen_ids.insert(mc.module_id).second) {
                throw std::invalid_argument("Duplicate module_id in module_configurations: " + mc.module_id);
            }
            k.module_configurations->push_back(std::move(mc));
        }
    }
}

void to_json(json& j, MarkActiveSlotRequest const& k) noexcept {
    j = json{{"slot_id", k.slot_id}};
}

void from_json(const json& j, MarkActiveSlotRequest& k) {
    k.slot_id = j.at("slot_id");
}

void to_json(json& j, DeleteSlotRequest const& k) noexcept {
    j = json{{"slot_id", k.slot_id}};
}

void from_json(const json& j, DeleteSlotRequest& k) {
    k.slot_id = j.at("slot_id");
}

void to_json(json& j, DuplicateSlotRequest const& k) noexcept {
    j = json{{"slot_id", k.slot_id}};
    if (k.new_description) {
        j["new_description"] = *k.new_description;
    }
}

void from_json(const json& j, DuplicateSlotRequest& k) {
    k.slot_id = j.at("slot_id");
    if (j.contains("new_description")) {
        k.new_description = j.at("new_description");
    }
}

void to_json(json& j, LoadFromYamlRequest const& k) noexcept {
    j = json{{"raw_yaml", k.raw_yaml}};
    if (k.slot_id) {
        j["slot_id"] = *k.slot_id;
    }
    if (k.description) {
        j["description"] = *k.description;
    }
}

void from_json(const json& j, LoadFromYamlRequest& k) {
    k.raw_yaml = j.at("raw_yaml");
    if (j.contains("slot_id")) {
        k.slot_id = j.at("slot_id");
    }
    if (j.contains("description")) {
        k.description = j.at("description");
    }
}

void to_json(json& j, GetConfigurationRequest const& k) noexcept {
    j = json{{"slot_id", k.slot_id}};
}

void from_json(const json& j, GetConfigurationRequest& k) {
    k.slot_id = j.at("slot_id");
}

} // namespace everest::lib::API::V1_0::types::config_service
