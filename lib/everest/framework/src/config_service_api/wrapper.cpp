// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2026 Pionix GmbH and Contributors to EVerest

#include <config_service_api/wrapper.hpp>

namespace everest::config::api::types {

namespace {

// TODO(CB): Makes sure all these helpers are actually used and remove the ones that are not needed.

template <class SrcT, class ConvT>
auto srcToTarOpt(std::optional<SrcT> const& src, ConvT const& converter)
    -> std::optional<decltype(converter(src.value()))> {
    if (src) {
        return std::make_optional(converter(src.value()));
    }
    return std::nullopt;
}

template <class SrcT, class ConvT> auto srcToTarVec(std::vector<SrcT> const& src, ConvT const& converter) {
    using TarT = decltype(converter(src[0]));
    std::vector<TarT> result;
    for (SrcT const& elem : src) {
        result.push_back(converter(elem));
    }
    return result;
}

template <class SrcT>
auto optToInternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_internal_api(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return to_internal_api(val); });
}

template <class SrcT>
auto optToExternal(std::optional<SrcT> const& src) -> std::optional<decltype(to_external_api(src.value()))> {
    return srcToTarOpt(src, [](SrcT const& val) { return to_external_api(val); });
}

template <class SrcT> auto vecToExternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return to_external_api(val); });
}

template <class SrcT> auto vecToInternal(std::vector<SrcT> const& src) {
    return srcToTarVec(src, [](SrcT const& val) { return to_internal_api(val); });
}

} // namespace

// TODO(CB): For some types only one direction of conversion is actually needed. We should check which ones are actually
// used and remove the unnecessary ones. (or keep them for round-trip-unit tests?)

MarkActiveSlotResultEnum_Internal to_internal_api(MarkActiveSlotResultEnum_External const& val) {
    using SrcT = MarkActiveSlotResultEnum_External;
    using TarT = MarkActiveSlotResultEnum_Internal;
    switch (val) {
    case SrcT::Success:
        return TarT::Success;
    case SrcT::AlreadyActive:
        return TarT::AlreadyActive;
    case SrcT::DoesNotExist:
        return TarT::DoesNotExist;
    case SrcT::Rejected:
        return TarT::Rejected;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::config_service::MarkActiveSlotResultEnum_External");
}

MarkActiveSlotResultEnum_External to_external_api(MarkActiveSlotResultEnum_Internal const& val) {
    using SrcT = MarkActiveSlotResultEnum_Internal;
    using TarT = MarkActiveSlotResultEnum_External;
    switch (val) {
    case SrcT::Success:
        return TarT::Success;
    case SrcT::AlreadyActive:
        return TarT::AlreadyActive;
    case SrcT::DoesNotExist:
        return TarT::DoesNotExist;
    case SrcT::Rejected:
        return TarT::Rejected;
    }
    throw std::out_of_range("Unexpected value for everest::config::SetActiveSlotStatus");
}

DeleteSlotResultEnum_Internal to_internal_api(DeleteSlotResultEnum_External const& val) {
    using SrcT = DeleteSlotResultEnum_External;
    using TarT = DeleteSlotResultEnum_Internal;
    switch (val) {
    case SrcT::Success:
        return TarT::Success;
    case SrcT::CannotDeleteActiveSlot:
        return TarT::CannotDeleteActiveSlot;
    case SrcT::DoesNotExist:
        return TarT::DoesNotExist;
    case SrcT::Rejected:
        return TarT::Rejected;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::config_service::DeleteSlotResultEnum_External");
}

DeleteSlotResultEnum_External to_external_api(DeleteSlotResultEnum_Internal const& val) {
    using SrcT = DeleteSlotResultEnum_Internal;
    using TarT = DeleteSlotResultEnum_External;
    switch (val) {
    case SrcT::Success:
        return TarT::Success;
    case SrcT::CannotDeleteActiveSlot:
        return TarT::CannotDeleteActiveSlot;
    case SrcT::DoesNotExist:
        return TarT::DoesNotExist;
    case SrcT::Rejected:
        return TarT::Rejected;
    }
    throw std::out_of_range("Unexpected value for everest::config::DeleteSlotStatus");
}

ConfigMetadata_Internal to_internal_api(ConfigMetadata_External const& val) {
    ConfigMetadata_Internal result;
    result.id = val.slot_id;
    result.last_updated = val.last_updated;
    result.is_valid = val.is_valid;
    result.config_file_path = val.config_file_path;
    result.description = val.description;
    return result;
}

ConfigMetadata_External to_external_api(ConfigMetadata_Internal const& val) {
    ConfigMetadata_External result;
    result.slot_id = val.id;
    result.last_updated = val.last_updated;
    result.is_valid = val.is_valid;
    result.config_file_path = val.config_file_path;
    result.description = val.description;
    return result;
}

ListSlotIdsResult_Internal to_internal_api(ListSlotIdsResult_External const& val) {
    ListSlotIdsResult_Internal result;
    for (ConfigMetadata_External const& slot : val.slots) {
        result.push_back(to_internal_api(slot));
    }
    return result;
}

ListSlotIdsResult_External to_external_api(ListSlotIdsResult_Internal const& val) {
    ListSlotIdsResult_External result;
    for (ConfigMetadata_Internal const& slot : val) {
        result.slots.push_back(to_external_api(slot));
    }
    return result;
}

DuplicateSlotResult_Internal to_internal_api(DuplicateSlotResult_External const& val) {
    DuplicateSlotResult_Internal result;
    result.success = val.success;
    result.slot_id = val.slot_id;
    return result;
}

DuplicateSlotResult_External to_external_api(DuplicateSlotResult_Internal const& val) {
    DuplicateSlotResult_External result;
    result.success = val.success;
    result.slot_id = val.slot_id;
    return result;
}

LoadFromYamlResult_Internal to_internal_api(LoadFromYamlResult_External const& val) {
    LoadFromYamlResult_Internal result;
    result.success = val.success;
    result.slot_id = val.slot_id;
    return result;
}

LoadFromYamlResult_External to_external_api(LoadFromYamlResult_Internal const& val) {
    LoadFromYamlResult_External result;
    result.success = val.success;
    result.slot_id = val.slot_id;
    // TODO(CB): The internal type offers an error_message field that is currently not included in the external type.
    return result;
}

ConfigurationParameterUpdateResultEnum_Internal
to_internal_api(ConfigurationParameterUpdateResultEnum_External const& val) {
    using SrcT = ConfigurationParameterUpdateResultEnum_External;
    using TarT = ConfigurationParameterUpdateResultEnum_Internal;
    switch (val) {
    case SrcT::Applied:
        return TarT::Applied;
    case SrcT::WillApplyOnRestart:
        return TarT::WillApplyOnRestart;
    case SrcT::DoesNotExist:
        return TarT::DoesNotExist;
    case SrcT::Rejected:
        return TarT::Rejected;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::config_service::ConfigurationParameterUpdateResultEnum_External");
}

ConfigurationParameterUpdateResultEnum_External
to_external_api(ConfigurationParameterUpdateResultEnum_Internal const& val) {
    using SrcT = ConfigurationParameterUpdateResultEnum_Internal;
    using TarT = ConfigurationParameterUpdateResultEnum_External;
    switch (val) {
    case SrcT::Applied:
        return TarT::Applied;
    case SrcT::WillApplyOnRestart:
        return TarT::WillApplyOnRestart;
    case SrcT::DoesNotExist:
        return TarT::DoesNotExist;
    case SrcT::Rejected:
        return TarT::Rejected;
    }
    throw std::out_of_range(
        "Unexpected value for Everest::config::SetConfigParameterResult");
}

ActiveSlotStatusEnum_Internal to_internal_api(ActiveSlotStatusEnum_External const& val) {
    using SrcT = ActiveSlotStatusEnum_External;
    using TarT = ActiveSlotStatusEnum_Internal;
    switch (val) {
    case SrcT::Running:
        return TarT::Running;
    case SrcT::FailedToStart:
        return TarT::FailedToStart;
    case SrcT::RestartTriggered:
        return TarT::RestartTriggered;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::config_service::ActiveSlotStatusEnum_External");
}

ActiveSlotStatusEnum_External to_external_api(ActiveSlotStatusEnum_Internal const& val) {
    using SrcT = ActiveSlotStatusEnum_Internal;
    using TarT = ActiveSlotStatusEnum_External;
    switch (val) {
    case SrcT::Running:
        return TarT::Running;
    case SrcT::FailedToStart:
        return TarT::FailedToStart;
    case SrcT::RestartTriggered:
        return TarT::RestartTriggered;
    }
    throw std::out_of_range("Unexpected value for Everest::config::ActiveSlotStatus");
}

ConfigurationParameterDatatype_Internal to_internal_api(ConfigurationParameterDatatype_External const& val) {
    using SrcT = ConfigurationParameterDatatype_External;
    using TarT = ConfigurationParameterDatatype_Internal;
    switch (val) {
    case SrcT::Integer:
        return TarT::Integer;
    case SrcT::Decimal:
        return TarT::Decimal;
    case SrcT::String:
        return TarT::String;
    case SrcT::Boolean:
        return TarT::Boolean;
    case SrcT::Unknown:
        return TarT::Unknown;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::config_service::ConfigurationParameterDatatype");
}

ConfigurationParameterDatatype_External to_external_api(ConfigurationParameterDatatype_Internal const& val) {
    using SrcT = ConfigurationParameterDatatype_Internal;
    using TarT = ConfigurationParameterDatatype_External;
    switch (val) {
    case SrcT::Integer:
        return TarT::Integer;
    case SrcT::Decimal:
        return TarT::Decimal;
    case SrcT::String:
        return TarT::String;
    case SrcT::Boolean:
        return TarT::Boolean;
    case SrcT::Unknown:
        return TarT::Unknown;
    }
    throw std::out_of_range("Unexpected value for everest::config::Datatype");
}

ConfigurationParameterMutability_Internal to_internal_api(ConfigurationParameterMutability_External const& val) {
    using SrcT = ConfigurationParameterMutability_External;
    using TarT = ConfigurationParameterMutability_Internal;
    switch (val) {
    case SrcT::ReadOnly:
        return TarT::ReadOnly;
    case SrcT::ReadWrite:
        return TarT::ReadWrite;
    case SrcT::WriteOnly:
        return TarT::WriteOnly;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::config_service::ConfigurationParameterMutability");
}

ConfigurationParameterMutability_External to_external_api(ConfigurationParameterMutability_Internal const& val) {
    using SrcT = ConfigurationParameterMutability_Internal;
    using TarT = ConfigurationParameterMutability_External;
    switch (val) {
    case SrcT::ReadOnly:
        return TarT::ReadOnly;
    case SrcT::ReadWrite:
        return TarT::ReadWrite;
    case SrcT::WriteOnly:
        return TarT::WriteOnly;
    }
    throw std::out_of_range("Unexpected value for everest::config::Mutability");
}

GetConfigurationStatusEnum_Internal to_internal_api(GetConfigurationStatusEnum_External const& val) {
    using SrcT = GetConfigurationStatusEnum_External;
    using TarT = GetConfigurationStatusEnum_Internal;
    switch (val) {
    case SrcT::Success:
        return TarT::Success;
    case SrcT::SlotDoesNotExist:
        return TarT::SlotDoesNotExist;
    }
    throw std::out_of_range(
        "Unexpected value for everest::lib::API::V1_0::types::config_service::GetConfigurationStatusEnum_External");
}

GetConfigurationStatusEnum_External to_external_api(GetConfigurationStatusEnum_Internal const& val) {
    using SrcT = GetConfigurationStatusEnum_Internal;
    using TarT = GetConfigurationStatusEnum_External;
    switch (val) {
    case SrcT::Success:
        return TarT::Success;
    case SrcT::SlotDoesNotExist:
        return TarT::SlotDoesNotExist;
    }
    throw std::out_of_range("Unexpected value for Everest::config::GetConfigurationStatus");
}

ConfigurationParameterIdentifier_Internal to_internal_api(ConfigurationParameterIdentifier_External const& val) {
    ConfigurationParameterIdentifier_Internal result;
    result.module_id = val.module_id;
    result.configuration_parameter_name = val.parameter_name;
    result.module_implementation_id = val.implementation_id;
    return result;
}

ConfigurationParameterIdentifier_External to_external_api(ConfigurationParameterIdentifier_Internal const& val) {
    ConfigurationParameterIdentifier_External result;
    result.module_id = val.module_id;
    result.parameter_name = val.configuration_parameter_name;
    result.implementation_id = val.module_implementation_id;
    return result;
}

ConfigurationParameterUpdate_Internal to_internal_api(ConfigurationParameterUpdate_External const& val) {
    ConfigurationParameterUpdate_Internal result;
    result.identifier = to_internal_api(val.cfg_param_id);
    result.value = val.value;
    return result;
}

ConfigurationParameterUpdate_External to_external_api(ConfigurationParameterUpdate_Internal const& val) {
    ConfigurationParameterUpdate_External result;
    result.cfg_param_id = to_external_api(val.identifier);
    result.value = val.value;
    return result;
}

ActiveSlotUpdateNotice_Internal to_internal_api(ActiveSlotUpdateNotice_External const& val) {
    ActiveSlotUpdateNotice_Internal result;
    result.timestamp = val.tstamp;
    result.slot_id = val.slot_id;
    result.status = to_internal_api(val.status);
    return result;
}

ActiveSlotUpdateNotice_External to_external_api(ActiveSlotUpdateNotice_Internal const& val) {
    ActiveSlotUpdateNotice_External result;
    result.tstamp = val.timestamp;
    result.slot_id = val.slot_id;
    result.status = to_external_api(val.status);
    return result;
}

ConfigurationParameterUpdateResultRecord_Internal
to_internal_api(ConfigurationParameterUpdateResultRecord_External const& val) {
    ConfigurationParameterUpdateResultRecord_Internal result;
    result.identifier = to_internal_api(val.update.cfg_param_id);
    result.value = val.update.value;
    result.result = to_internal_api(val.result);
    return result;
}

ConfigurationParameterUpdateResultRecord_External
to_external_api(ConfigurationParameterUpdateResultRecord_Internal const& val) {
    ConfigurationParameterUpdateResultRecord_External result;
    result.update.cfg_param_id = to_external_api(val.identifier);
    result.update.value = val.value;
    result.result = to_external_api(val.result);
    return result;
}

ConfigurationParameterUpdateNotice_Internal to_internal_api(ConfigurationParameterUpdateNotice_External const& val) {
    ConfigurationParameterUpdateNotice_Internal result;
    result.timestamp = val.tstamp;
    result.slot_id = val.slot_id;
    result.updates = vecToInternal(val.update_results);
    // TODO(CB): Should include an origin field.
    return result;
}

ConfigurationParameterUpdateNotice_External to_external_api(ConfigurationParameterUpdateNotice_Internal const& val) {
    ConfigurationParameterUpdateNotice_External result;
    result.tstamp = val.timestamp;
    result.slot_id = val.slot_id;
    result.update_results = vecToExternal(val.updates);
    // TODO(CB): Should include an origin field.
    return result;
}

Mapping_Internal to_internal_api(Mapping_External const& val) {
    Mapping_Internal result{val.evse};
    result.evse = val.evse;
    if (val.connector) {
        result.connector = val.connector.value();
    } else {
        result.connector = std::nullopt;
    }
    return result;
}

Mapping_External to_external_api(Mapping_Internal const& val) {
    Mapping_External result;
    result.evse = val.evse;
    if (val.connector) {
        result.connector = val.connector.value();
    }
    return result;
}

ModuleTierMappings_Internal to_internal_api(ModuleTierMappings_External const& val) {
    ModuleTierMappings_Internal result;
    result.module = optToInternal(val.module);
    for (auto const& impl_mapping : val.implementations) {
        // TODO(CB): Is it required to set a value for each existing implementation_id?!
        result.implementations.emplace(impl_mapping.implementation_id,
                                       std::optional<Mapping_Internal>{to_internal_api(impl_mapping.mapping)});
    }
    return result;
}

ModuleTierMappings_External to_external_api(ModuleTierMappings_Internal const& val) {
    ModuleTierMappings_External result;
    result.module = optToExternal(val.module);
    for (auto const& [impl_id, mapping] : val.implementations) {
        if (mapping) {
            result.implementations.push_back({impl_id, to_external_api(mapping.value())});
        }
    }
    return result;
}

ReqFulfillment_Internal to_internal_api(ReqFulfillment_External const& val, const std::string& requirement_id) {
    ReqFulfillment_Internal result;
    result.module_id = val.module_id;
    result.implementation_id = val.implementation_id;
    Requirement req{requirement_id, val.index};  // TODO(CB): This is a narrowing conversion; maybe use an appropriate cast or check for overflow?
    result.requirement = req;
    return result;
}

ReqFulfillment_External to_external_api(ReqFulfillment_Internal const& val) {
    ReqFulfillment_External result;
    result.module_id = val.module_id;
    result.implementation_id = val.implementation_id;
    result.index = val.requirement.index;
    return result;
}

ConfigurationParameterCharacteristics_Internal
to_internal_api(ConfigurationParameterCharacteristics_External const& val) {
    ConfigurationParameterCharacteristics_Internal result;
    result.datatype = to_internal_api(val.datatype);
    result.mutability = to_internal_api(val.mutability);
    //    result.activation_policy = to_internal_api(val.activation_policy); // missing in internal type
    result.unit = val.unit;
    result.min_value = val.min_value;
    result.max_value = val.max_value;
    return result;
}

ConfigurationParameterCharacteristics_External
to_external_api(ConfigurationParameterCharacteristics_Internal const& val) {
    ConfigurationParameterCharacteristics_External result;
    result.datatype = to_external_api(val.datatype);
    result.mutability = to_external_api(val.mutability);
    result.activation_policy = ::everest::lib::API::V1_0::types::config_service::ConfigurationActivationPolicy::
        RequiresRestart; // TODO(CB): The activation policy is currently not included in the external type, so we
                         // default to RequiresRestart for now. We might want to add this to the external type in the
                         // future.
    result.unit = val.unit;
    result.min_value = val.min_value;
    result.max_value = val.max_value;
    return result;
}

ConfigurationParameter_Internal to_internal_api(ConfigurationParameter_External const& val) {
    ConfigurationParameter_Internal result;
    result.name = val.name;
    switch (val.characteristics.datatype) {
    case ConfigurationParameterDatatype_External::Integer:
        result.value = std::stoi(val.value);
        break;
    case ConfigurationParameterDatatype_External::Decimal:
        result.value = std::stod(val.value);
        break;
    case ConfigurationParameterDatatype_External::String:
        result.value = val.value;
        break;
    case ConfigurationParameterDatatype_External::Boolean:
        if (val.value == "true") {
            result.value = true;
        } else if (val.value == "false") {
            result.value = false;
        } else {
            throw std::invalid_argument("Invalid value for boolean configuration parameter: " + val.value);
        }
        break;
    default:
        throw std::out_of_range(
            "Unexpected value for everest::lib::API::V1_0::types::config_service::ConfigurationParameterDatatype");
    }
    result.characteristics = to_internal_api(val.characteristics);
    return result;
}

ConfigurationParameter_External to_external_api(ConfigurationParameter_Internal const& val) {
    ConfigurationParameter_External result;
    result.name = val.name;
    result.characteristics = to_external_api(val.characteristics);
    result.value = std::visit(::everest::config::VisitConfigEntry{}, val.value);
    return result;
}

TelemetryConfig_Internal to_internal_api(TelemetryConfig_External const& val) {
    TelemetryConfig_Internal result{val.id};
    return result;
}

TelemetryConfig_External to_external_api(TelemetryConfig_Internal const& val) {
    TelemetryConfig_External result;
    result.id = val.id;
    return result;
}

ModuleConfigAccess_Internal to_internal_api(ModuleConfigAccess_External const& val) {
    ModuleConfigAccess_Internal result;
    result.allow_read = val.allow_read;
    result.allow_write = val.allow_write;
    result.allow_set_read_only = val.allow_set_read_only;
    return result;
}

ModuleConfigAccess_External to_external_api(ModuleConfigAccess_Internal const& val, std::string const& module_id) {
    ModuleConfigAccess_External result;
    result.module_id = module_id;
    result.allow_read = val.allow_read;
    result.allow_write = val.allow_write;
    result.allow_set_read_only = val.allow_set_read_only;
    return result;
}

ConfigAccess_Internal to_internal_api(ConfigAccess_External const& val) {
    ConfigAccess_Internal result;
    result.allow_global_read = val.allow_global_read;
    result.allow_global_write = val.allow_global_write;
    result.allow_set_read_only = val.allow_set_read_only;
    for (auto const& mod_cfg_access : val.module_config_access) {
        result.modules[mod_cfg_access.module_id] = to_internal_api(mod_cfg_access);
    }
    return result;
}

ConfigAccess_External to_external_api(ConfigAccess_Internal const& val) {
    ConfigAccess_External result;
    result.allow_global_read = val.allow_global_read;
    result.allow_global_write = val.allow_global_write;
    result.allow_set_read_only = val.allow_set_read_only;
    for (auto const& [module_id, mod_cfg_access] : val.modules) {
        result.module_config_access.push_back(to_external_api(mod_cfg_access, module_id));
    }
    return result;
}

ConfigAccessControl_Internal to_internal_api(ConfigAccessControl_External const& val) {
    ConfigAccessControl_Internal result;
    result.config = optToInternal(val.config);
    return result;
}

ConfigAccessControl_External to_external_api(ConfigAccessControl_Internal const& val) {
    ConfigAccessControl_External result;
    result.config = optToExternal(val.config);
    return result;
}

std::vector<Fulfillment> fulfillmentVecToInternal(const std::vector<ReqFulfillment_External>& fulfillments_external, const std::string& requirement_id) {
    std::vector<Fulfillment> result;
    for (auto const& fulfillment_external : fulfillments_external) {
        result.push_back(to_internal_api(fulfillment_external, requirement_id));
    }
    return result;
}

ModuleConfiguration_Internal to_internal_api(ModuleConfiguration_External const& val) {
    ModuleConfiguration_Internal result;
    result.module_id = val.module_id;
    result.module_name = val.module_name;
    for (auto const& connection : val.connections) {
        result.connections.emplace(connection.requirement_id, fulfillmentVecToInternal(connection.fulfillments, connection.requirement_id));
    }
    result.mapping = to_internal_api(val.mapping);

    for (auto const& param : val.module_configuration_parameters) {
        const std::string impl_id = "!module"; // TODO(CB): use special implementation_id to identify module
                                               // configuration parameters - correct?!
        result.configuration_parameters[impl_id].push_back(to_internal_api(param));
    }
    for (auto const& impl_config_param : val.implementation_configuration_parameters) {
        result.configuration_parameters[impl_config_param.implementation_id] =
            vecToInternal(impl_config_param.configuration_parameters);
    }

    result.standalone = val.standalone;
    result.telemetry_enabled = val.telemetry_enabled;
    result.telemetry_config = optToInternal(val.telemetry_config);
    result.access = to_internal_api(val.config_access);

    return result;
}

ModuleConfiguration_External to_external_api(ModuleConfiguration_Internal const& val) {
    ModuleConfiguration_External result;
    result.module_id = val.module_id;
    result.module_name = val.module_name;
    for (auto const& [requirement_id, fulfillments] : val.connections) {
        ::everest::lib::API::V1_0::types::config_service::ModuleConnection connection_external;
        connection_external.requirement_id = requirement_id;
        connection_external.fulfillments = vecToExternal(fulfillments);
        result.connections.push_back(connection_external);
    }
    result.mapping = to_external_api(val.mapping);

    for (auto const& [impl_id, config_params] : val.configuration_parameters) {
        if (impl_id == "!module") { // TODO(CB): use special implementation_id to identify module configuration
                                    // parameters - correct?!
            for (auto const& param : config_params) {
                result.module_configuration_parameters.push_back(to_external_api(param));
            }
        } else {
            ::everest::lib::API::V1_0::types::config_service::ImplementationConfigurationParameter
                impl_config_param_external;
            impl_config_param_external.implementation_id = impl_id;
            impl_config_param_external.configuration_parameters = vecToExternal(config_params);
            result.implementation_configuration_parameters.push_back(impl_config_param_external);
        }
    }

    result.standalone = val.standalone;
    result.telemetry_enabled = val.telemetry_enabled;
    result.telemetry_config = optToExternal(val.telemetry_config);
    result.config_access = to_external_api(val.access);

    return result;
}

GetConfigurationResult_Internal to_internal_api(GetConfigurationResult_External const& val) {
    GetConfigurationResult_Internal result;
    result.status = to_internal_api(val.status);
    if (val.module_configurations) {
        result.module_configurations.emplace();
        for (auto const& module_config_external : val.module_configurations.value()) {
            // TODO(CB): This assumes that module_id is unique across the vector, which should be the case but is not
            // explicitly enforced by the type system. We might want to change the external type to already use a map to
            // make this more explicit.
            // TODO(CB): Or maybe we should catch this in the json_codec.cpp (throw if there are duplicate module_ids)
            // to avoid silently dropping modules in case of duplicates?
            result.module_configurations[module_config_external.module_id] = to_internal_api(module_config_external);
        }
    }
    return result;
}

GetConfigurationResult_External to_external_api(GetConfigurationResult_Internal const& val) {
    GetConfigurationResult_External result;
    result.status = to_external_api(val.status);
    if (val.module_configurations.size() > 0) {
        result.module_configurations.emplace();
    }
    for (auto const& [module_id, module_config_internal] : val.module_configurations) {
        result.module_configurations->emplace_back(to_external_api(module_config_internal));
    }
    return result;
}

} // namespace everest::config::api::types