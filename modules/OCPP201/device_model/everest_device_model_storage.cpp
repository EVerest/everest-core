// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <everest/logging.hpp>

#include <device_model/definitions.hpp>
#include <device_model/everest_device_model_storage.hpp>
#include <ocpp/v2/init_device_model_db.hpp>
#include <ocpp/v2/ocpp_types.hpp>

using ocpp::v2::Component;
using ocpp::v2::DataEnum;
using ocpp::v2::EVSE;
using ocpp::v2::Variable;
using ocpp::v2::VariableAttribute;
using ocpp::v2::VariableCharacteristics;
using ocpp::v2::VariableMap;
using ocpp::v2::VariableMetaData;

static constexpr auto VARIABLE_SOURCE_EVEREST = "EVEREST";

using ocpp::v2::ComponentKey;
using ocpp::v2::DbVariableAttribute;
using ocpp::v2::DeviceModelVariable;

namespace module::device_model {
ocpp::v2::DataEnum to_ocpp_data_enum(const everest::config::Datatype& data_type) {
    switch (data_type) {
    case everest::config::Datatype::Unknown:
        throw std::out_of_range("Could not convert Datatype::Unknown to DataEnum");
    case everest::config::Datatype::String:
        return ocpp::v2::DataEnum::string;
    case everest::config::Datatype::Decimal:
        return ocpp::v2::DataEnum::decimal;
    case everest::config::Datatype::Integer:
        return ocpp::v2::DataEnum::integer;
    case everest::config::Datatype::Boolean:
        return ocpp::v2::DataEnum::boolean;
    }
    throw std::out_of_range("Could not convert Datatype to DataEnum");
}

ocpp::v2::MutabilityEnum to_ocpp_mutability_enum(const everest::config::Mutability& mutability) {
    switch (mutability) {
    case everest::config::Mutability::ReadOnly:
        return ocpp::v2::MutabilityEnum::ReadOnly;
    case everest::config::Mutability::ReadWrite:
        return ocpp::v2::MutabilityEnum::ReadWrite;
    case everest::config::Mutability::WriteOnly:
        return ocpp::v2::MutabilityEnum::WriteOnly;
    }
    throw std::out_of_range("Could not convert Mutability to MutabilityEnum");
}

namespace {

Component get_evse_component(const int32_t evse_id) {
    Component component;
    component.name = "EVSE";
    component.evse = EvseDefinitions::get_evse(evse_id);
    return component;
}

Component get_connector_component(const int32_t evse_id, const int32_t connector_id) {
    Component component;
    component.name = "Connector";
    component.evse = EvseDefinitions::get_evse(evse_id, connector_id);
    return component;
}

ComponentKey get_evse_component_key(const int32_t evse_id) {
    ComponentKey component;
    component.name = "EVSE";
    component.evse_id = evse_id;
    return component;
}

ComponentKey get_connector_component_key(const int32_t evse_id, const int32_t connector_id) {
    ComponentKey component;
    component.name = "Connector";
    component.evse_id = evse_id;
    component.connector_id = connector_id;
    return component;
}

ocpp::v2::Component get_component(const ComponentKey& component_key) {
    ocpp::v2::Component component;
    component.name = ocpp::CiString<50>(component_key.name, ocpp::StringTooLarge::Truncate);
    if (component_key.instance.has_value()) {
        component.instance = ocpp::CiString<50>(component_key.instance.value(), ocpp::StringTooLarge::Truncate);
    }
    if (component_key.evse_id.has_value()) {
        ocpp::v2::EVSE evse;
        evse.id = component_key.evse_id.value();
        evse.connectorId = component_key.connector_id;
        component.evse = evse;
    }
    return component;
}

ocpp::v2::Variable get_variable(const DeviceModelVariable& dm_variable) {
    ocpp::v2::Variable variable;
    variable.name = ocpp::CiString<50>(dm_variable.name, ocpp::StringTooLarge::Truncate);
    if (dm_variable.instance.has_value()) {
        variable.instance = ocpp::CiString<50>(dm_variable.instance.value(), ocpp::StringTooLarge::Truncate);
    }
    return variable;
}

// Helper function to construct DeviceModelVariable with common structure
DeviceModelVariable make_variable(const std::string& name, const ocpp::v2::VariableCharacteristics& characteristics,
                                  const std::string& value = "",
                                  ocpp::v2::MutabilityEnum mutability = ocpp::v2::MutabilityEnum::ReadOnly) {
    DeviceModelVariable var_data;
    var_data.name = name;
    var_data.characteristics = characteristics;
    var_data.source = VARIABLE_SOURCE_EVEREST;

    DbVariableAttribute db_attr;
    VariableAttribute attr;
    attr.type = ocpp::v2::AttributeEnum::Actual;
    attr.value = value;
    attr.mutability = mutability;
    db_attr.variable_attribute = attr;

    var_data.attributes.push_back(db_attr);
    return var_data;
}

// Populates EVSE variables
std::vector<DeviceModelVariable> build_evse_variables(const float max_power) {
    std::vector<DeviceModelVariable> variables;

    auto evse_power_characteristics = EvseDefinitions::Characteristics::EVSEPower;
    evse_power_characteristics.maxLimit = max_power;

    return {make_variable(ocpp::v2::EvseComponentVariables::Available.name, EvseDefinitions::Characteristics::Available,
                          "true"),
            make_variable(ocpp::v2::EvseComponentVariables::AvailabilityState.name,
                          EvseDefinitions::Characteristics::AvailabilityState, "Available"),
            make_variable(ocpp::v2::EvseComponentVariables::Power.name, evse_power_characteristics),
            make_variable(ocpp::v2::EvseComponentVariables::SupplyPhases.name,
                          EvseDefinitions::Characteristics::SupplyPhases),
            make_variable(ocpp::v2::EvseComponentVariables::AllowReset.name,
                          EvseDefinitions::Characteristics::AllowReset, "false"),
            make_variable(ocpp::v2::EvseComponentVariables::ISO15118EvseId.name,
                          EvseDefinitions::Characteristics::ISO15118EvseId, "DEFAULT_EVSE_ID")};
}

// Populates Connector variables
std::vector<DeviceModelVariable> build_connector_variables() {

    return {make_variable(ocpp::v2::ConnectorComponentVariables::Available.name,
                          ConnectorDefinitions::Characteristics::Available, "true"),
            make_variable(ocpp::v2::ConnectorComponentVariables::AvailabilityState.name,
                          ConnectorDefinitions::Characteristics::AvailabilityState, "Available"),
            make_variable(ocpp::v2::ConnectorComponentVariables::Type.name,
                          ConnectorDefinitions::Characteristics::ConnectorType),
            make_variable(ocpp::v2::ConnectorComponentVariables::SupplyPhases.name,
                          ConnectorDefinitions::Characteristics::SupplyPhases)};
}

std::string get_everest_config_value(const everest::config::ModuleConfigurationParameters& module_config,
                                     std::string impl, std::string config_key) {
    const auto& config = module_config.at(impl);
    for (const auto& config_param : config) {
        if (config_param.name == config_key) {
            return everest::config::config_entry_to_string(config_param.value);
        }
    }
    throw std::out_of_range("Could not find requested config key: " + config_key);
}

} // anonymous namespace

EverestDeviceModelStorage::EverestDeviceModelStorage(
    const std::vector<std::unique_ptr<evse_managerIntf>>& r_evse_manager,
    const std::map<int32_t, types::evse_board_support::HardwareCapabilities>& evse_hardware_capabilities_map,
    const std::filesystem::path& db_path, const std::filesystem::path& migration_files_path,
    std::unique_ptr<VariableMapping> variable_mapping,
    std::shared_ptr<Everest::config::ConfigServiceClient> config_service_client) :
    r_evse_manager(r_evse_manager),
    config_service_client(config_service_client),
    variable_mapping(std::move(variable_mapping)) {

    this->module_configs = config_service_client->get_module_configs();
    this->mappings = config_service_client->get_mappings();
    std::map<ComponentKey, std::vector<DeviceModelVariable>> component_configs;

    for (const auto& evse_manager : r_evse_manager) {
        const auto evse_info = evse_manager->call_get_evse();
        const auto& hw_capabilities = evse_hardware_capabilities_map.at(evse_info.id);

        ComponentKey evse_component_key = get_evse_component_key(evse_info.id);
        const auto max_power = hw_capabilities.max_current_A_import * 230.0f * hw_capabilities.max_phase_count_import;
        component_configs[evse_component_key] = build_evse_variables(max_power);

        for (const auto& connector : evse_info.connectors) {
            ComponentKey connector_component_key = get_connector_component_key(evse_info.id, connector.id);
            component_configs[connector_component_key] = build_connector_variables();
        }
    }

    // build OCPP2.x device model components from EVerest config    // This is our mapping strategy:
    // Component.name = module_type
    // Component.instance = module_id
    // Component.evse.id/connector = mapping of module
    // impl mappings are not taken into account at the moment
    for (const auto& [module_id_type, module_config] : this->module_configs) {
        ComponentKey component_key;
        component_key.name = module_id_type.module_type;
        component_key.instance = module_id_type.module_id;
        const auto& mapping = this->mappings.at(module_id_type.module_id);
        if (mapping.module.has_value()) {
            const auto& module_mapping = mapping.module.value();
            // in OCPP2.x the id and connectorId of the EVSEType must be > 0
            if (module_mapping.evse > 0) {
                component_key.evse_id = module_mapping.evse;
                if (module_mapping.connector.has_value()) {
                    const auto connector_id = module_mapping.connector.value();
                    if (connector_id > 0) {
                        component_key.connector_id = module_mapping.connector.value();
                    }
                }
            }
        }

        Component component = get_component(component_key);
        component_configs[component_key] =
            build_everest_config_variables(module_config, module_id_type.module_id, component);
    }

    ocpp::v2::InitDeviceModelDb init_device_model_db(db_path, migration_files_path);
    init_device_model_db.initialize_database(component_configs, false);
    init_device_model_db.close_connection();
    this->device_model_storage = std::make_unique<ocpp::v2::DeviceModelStorageSqlite>(db_path);

    this->init_hw_capabilities(evse_hardware_capabilities_map);
}

std::vector<DeviceModelVariable> EverestDeviceModelStorage::build_everest_config_variables(
    const everest::config::ModuleConfigurationParameters& module_config, const std::string& module_id,
    const ocpp::v2::Component& component) {
    std::vector<DeviceModelVariable> component_config;
    for (const auto& [impl, config_params] : module_config) {
        std::string prefix;
        if (impl != Everest::config::MODULE_IMPLEMENTATION_ID) {
            // prefix variable name with impl + .
            prefix = impl + ".";
        }
        for (const auto& config_param : config_params) {
            try {
                const auto variable_name = prefix + config_param.name;
                ocpp::v2::VariableCharacteristics characteristics;
                characteristics.dataType = to_ocpp_data_enum(config_param.characteristics.datatype);
                characteristics.supportsMonitoring = false; // TODO: can we enable monitoring support?
                // TODO: add unit if/once available?

                auto device_model_variable = make_variable(
                    variable_name, characteristics, get_everest_config_value(module_config, impl, config_param.name),
                    to_ocpp_mutability_enum(config_param.characteristics.mutability));
                component_config.push_back(device_model_variable);

                everest::config::ConfigurationParameterIdentifier config_param_id;
                config_param_id.module_id = module_id;
                config_param_id.configuration_parameter_name = config_param.name;
                config_param_id.module_implementation_id = impl;

                const auto ocpp_cv_opt = this->variable_mapping->get_ocpp_cv(config_param_id);

                if (ocpp_cv_opt.has_value()) {
                    const auto& ocpp_cv = ocpp_cv_opt.value();
                    ocpp::v2::ComponentVariable everest_cv = {component, get_variable(device_model_variable)};
                    // store mapping from EVerest OCPP ComponentVariable and the OCPP representation of it
                    this->variable_mapping->add_cv_mapping(everest_cv, ocpp_cv);
                    this->stored_in_everest_config_service.insert(ocpp_cv);
                }

                ocpp::v2::ComponentVariable component_variable{component, get_variable(device_model_variable)};
                this->stored_in_everest_config_service.insert(component_variable);
            } catch (const std::exception& e) {
                EVLOG_error << "Could not add EVerest config entry to OCPP device model: " << e.what();
            }
        }
    }

    return component_config;
}

ocpp::v2::DeviceModelMap EverestDeviceModelStorage::apply_mappings(const ocpp::v2::DeviceModelMap& device_model_map) {
    ocpp::v2::DeviceModelMap mapped_device_model;

    for (const auto& [component, variable_map] : device_model_map) {
        for (const auto& [variable, variable_meta_data] : variable_map) {
            ocpp::v2::ComponentVariable everest_component_variable{component, variable};
            const auto& ocpp_component_variable_opt = this->variable_mapping->get_ocpp_cv(everest_component_variable);
            if (ocpp_component_variable_opt.has_value()) {
                const auto& ocpp_component_variable = ocpp_component_variable_opt.value();
                EVLOG_info << "Mapping identified for component variable: " << ocpp_component_variable;

                if (!ocpp_component_variable.variable.has_value()) {
                    EVLOG_warning << "No variable defined for component variable: " << ocpp_component_variable;
                    continue;
                }

                ocpp::v2::VariableMetaData ocpp_variable_meta_data;
                ocpp_variable_meta_data.characteristics = variable_meta_data.characteristics;
                ocpp_variable_meta_data.monitors = variable_meta_data.monitors;
                ocpp_variable_meta_data.source = variable_meta_data.source;

                mapped_device_model[ocpp_component_variable.component][ocpp_component_variable.variable.value()] =
                    ocpp_variable_meta_data;
            }
            mapped_device_model[component][variable] = variable_meta_data;
        }
    }

    return mapped_device_model;
}

void EverestDeviceModelStorage::init_hw_capabilities(
    const std::map<int32_t, types::evse_board_support::HardwareCapabilities>& evse_hardware_capabilities_map) {
    for (const auto& evse_manager : r_evse_manager) {
        const auto evse_info = evse_manager->call_get_evse();
        Component evse_component = get_evse_component(evse_info.id);

        if (evse_hardware_capabilities_map.find(evse_info.id) != evse_hardware_capabilities_map.end()) {
            this->update_hw_capabilities(evse_component, evse_hardware_capabilities_map.at(evse_info.id));
        } else {
            EVLOG_error << "No hardware capabilities found for EVSE with ID " << evse_info.id;
        }

        evse_manager->subscribe_hw_capabilities(
            [this, evse_component](const types::evse_board_support::HardwareCapabilities hw_capabilities) {
                this->update_hw_capabilities(evse_component, hw_capabilities);
            });

        for (const auto& connector : evse_info.connectors) {
            if (connector.type.has_value()) {
                const auto component = get_connector_component(evse_info.id, connector.id);
                std::lock_guard<std::mutex> lock(device_model_mutex);
                this->device_model_storage->set_variable_attribute_value(
                    component, ocpp::v2::ConnectorComponentVariables::Type, ocpp::v2::AttributeEnum::Actual,
                    types::evse_manager::connector_type_enum_to_string(connector.type.value()),
                    VARIABLE_SOURCE_EVEREST);
            }
        }
    }
}

void EverestDeviceModelStorage::update_hw_capabilities(
    const Component& evse_component, const types::evse_board_support::HardwareCapabilities& hw_capabilities) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    this->device_model_storage->set_variable_attribute_value(
        evse_component, ocpp::v2::EvseComponentVariables::SupplyPhases, ocpp::v2::AttributeEnum::Actual,
        std::to_string(hw_capabilities.max_phase_count_import), VARIABLE_SOURCE_EVEREST);
    // TODO: update EVSE.Power maxLimit value once device model storage interface supports it
}

void EverestDeviceModelStorage::update_power(const int32_t evse_id, const float total_power_active_import) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    Component evse_component = get_evse_component(evse_id);
    this->device_model_storage->set_variable_attribute_value(
        evse_component, ocpp::v2::EvseComponentVariables::Power, ocpp::v2::AttributeEnum::Actual,
        std::to_string(total_power_active_import), VARIABLE_SOURCE_EVEREST);
}

ocpp::v2::DeviceModelMap EverestDeviceModelStorage::get_device_model() {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    // Get the device model from the storage and apply variable mappings
    auto device_model = this->device_model_storage->get_device_model();
    // The device model in the storage does not contain the mappings, so we apply the defined mappings to it
    return this->apply_mappings(device_model);
}

std::optional<ocpp::v2::VariableAttribute>
EverestDeviceModelStorage::get_variable_attribute(const ocpp::v2::Component& component_id,
                                                  const ocpp::v2::Variable& variable_id,
                                                  const ocpp::v2::AttributeEnum& attribute_enum) {
    std::lock_guard<std::mutex> lock(device_model_mutex);

    // check if a mapping exists
    ocpp::v2::ComponentVariable component_variable{component_id, variable_id};
    const auto everest_component_variable_opt = this->variable_mapping->get_everest_cv(component_variable);
    if (everest_component_variable_opt.has_value()) {
        const auto& everest_component_variable = everest_component_variable_opt.value();

        if (!everest_component_variable.variable.has_value()) {
            EVLOG_warning << "No variable defined for mapped component variable: " << everest_component_variable;
            return std::nullopt;
        }

        return this->device_model_storage->get_variable_attribute(
            everest_component_variable.component, everest_component_variable.variable.value(), attribute_enum);
    } else {
        return this->device_model_storage->get_variable_attribute(component_id, variable_id, attribute_enum);
    }
}

std::vector<ocpp::v2::VariableAttribute>
EverestDeviceModelStorage::get_variable_attributes(const ocpp::v2::Component& component_id,
                                                   const ocpp::v2::Variable& variable_id,
                                                   const std::optional<ocpp::v2::AttributeEnum>& attribute_enum) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    // check if a mapping exists
    ocpp::v2::ComponentVariable component_variable{component_id, variable_id};
    const auto everest_component_variable_opt = this->variable_mapping->get_everest_cv(component_variable);
    if (everest_component_variable_opt.has_value()) {
        const auto& everest_component_variable = everest_component_variable_opt.value();

        if (!everest_component_variable.variable.has_value()) {
            EVLOG_warning << "No variable defined for mapped component variable: " << everest_component_variable;
            return {};
        }

        return this->device_model_storage->get_variable_attributes(
            everest_component_variable.component, everest_component_variable.variable.value(), attribute_enum);
    } else {
        return this->device_model_storage->get_variable_attributes(component_id, variable_id, attribute_enum);
    }
}

ocpp::v2::SetVariableStatusEnum EverestDeviceModelStorage::set_variable_attribute_value(
    const ocpp::v2::Component& component_id, const ocpp::v2::Variable& variable_id,
    const ocpp::v2::AttributeEnum& attribute_enum, const std::string& value, const std::string& source) {

    std::lock_guard<std::mutex> lock(device_model_mutex);

    ocpp::v2::ComponentVariable component_variable;
    component_variable.component = component_id;
    component_variable.variable = variable_id;

    const auto everest_component_variable_opt = this->variable_mapping->get_everest_cv(component_variable);
    if (everest_component_variable_opt.has_value()) {
        // there is a mapping, so the incoming component variable is not present in the storage, we need to map it to
        // the EVerest representation of it and set this
        component_variable = everest_component_variable_opt.value();
    }

    auto stored_in_everest_config_service_it = this->stored_in_everest_config_service.find(component_variable);
    if (stored_in_everest_config_service_it != this->stored_in_everest_config_service.end()) {
        if (attribute_enum != ocpp::v2::AttributeEnum::Actual) {
            return ocpp::v2::SetVariableStatusEnum::Rejected;
        }
        if (not component_variable.component.instance.has_value()) {
            return ocpp::v2::SetVariableStatusEnum::Rejected;
        }
        const auto module_id = component_variable.component.instance.value();
        everest::config::ConfigurationParameterIdentifier identifier;
        identifier.module_id = module_id;
        const std::string variable_name = component_variable.variable.value().name;
        const auto strpos = variable_name.find(".");
        if (strpos != std::string::npos) {
            identifier.module_implementation_id = variable_name.substr(0, strpos);
            identifier.configuration_parameter_name = variable_name.substr(strpos + 1, variable_name.length());
        } else {
            identifier.module_implementation_id = Everest::config::MODULE_IMPLEMENTATION_ID;
            identifier.configuration_parameter_name = variable_name;
        }

        const auto result = this->config_service_client->set_config_value(identifier, value);

        if (result.set_status == everest::config::SetConfigStatus::Accepted) {
            // immediately set it in the libocpp device model as well
            const auto everest_dm_result = this->device_model_storage->set_variable_attribute_value(
                component_variable.component, component_variable.variable.value(), attribute_enum, value, source);
            if (everest_dm_result != ocpp::v2::SetVariableStatusEnum::Accepted) {
                EVLOG_error << "Device model set variable results disagree";
            }
            return everest_dm_result; // FIXME: what to return, libocpp or EVerest result?
        } else if (result.set_status == everest::config::SetConfigStatus::Rejected) {
            return ocpp::v2::SetVariableStatusEnum::Rejected;
        } else if (result.set_status == everest::config::SetConfigStatus::RebootRequired) {
            return ocpp::v2::SetVariableStatusEnum::RebootRequired;
        }
    }

    // FIXME: device_model_storage->set_variable_attribute_value does only return accepted or rejected, no other
    // checks
    // are performed. Since libocpp contains the full device model in memory and does these checks independently,
    // it's currently only a minor issue.
    return this->device_model_storage->set_variable_attribute_value(component_id, variable_id, attribute_enum, value,
                                                                    source);
}

std::optional<ocpp::v2::VariableMonitoringMeta>
EverestDeviceModelStorage::set_monitoring_data(const ocpp::v2::SetMonitoringData& data,
                                               const ocpp::v2::VariableMonitorType type) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->set_monitoring_data(data, type);
}

bool EverestDeviceModelStorage::update_monitoring_reference(const int32_t monitor_id,
                                                            const std::string& reference_value) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->update_monitoring_reference(monitor_id, reference_value);
}

std::vector<ocpp::v2::VariableMonitoringMeta>
EverestDeviceModelStorage::get_monitoring_data(const std::vector<ocpp::v2::MonitoringCriterionEnum>& criteria,
                                               const ocpp::v2::Component& component_id,
                                               const ocpp::v2::Variable& variable_id) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->get_monitoring_data(criteria, component_id, variable_id);
}

ocpp::v2::ClearMonitoringStatusEnum EverestDeviceModelStorage::clear_variable_monitor(int monitor_id,
                                                                                      bool allow_protected) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->clear_variable_monitor(monitor_id, allow_protected);
}

int32_t EverestDeviceModelStorage::clear_custom_variable_monitors() {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->clear_custom_variable_monitors();
}

void EverestDeviceModelStorage::check_integrity() {
}
} // namespace module::device_model
