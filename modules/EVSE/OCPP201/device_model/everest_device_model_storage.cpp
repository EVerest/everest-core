// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <algorithm>
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
static constexpr auto NUMBER_OF_CONNECTED_EV_PROTOCOLS = 20;

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

Component get_v2x_component(const int32_t evse_id) {
    Component component;
    component.name = "V2XChargingCtrlr";
    component.evse = EvseDefinitions::get_evse(evse_id);
    return component;
}

Component get_iso15118_component(const int32_t evse_id) {
    Component component;
    component.name = "ISO15118Ctrlr";
    component.evse = EvseDefinitions::get_evse(evse_id);
    return component;
}

Component get_connected_ev_component(const int32_t evse_id) {
    Component component;
    component.name = "ConnectedEV";
    component.evse = EvseDefinitions::get_evse(evse_id);
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

ComponentKey get_v2x_component_key(const int32_t evse_id) {
    ComponentKey component;
    component.name = "V2XChargingCtrlr";
    component.evse_id = evse_id;
    return component;
}

ComponentKey get_iso15118_component_key(const int32_t evse_id) {
    ComponentKey component;
    component.name = "ISO15118Ctrlr";
    component.evse_id = evse_id;
    return component;
}

ComponentKey get_connected_ev_component_key(const int32_t evse_id) {
    ComponentKey component;
    component.name = "ConnectedEV";
    component.evse_id = evse_id;
    return component;
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

// Helper function to construct DeviceModelVariable with common structure
DeviceModelVariable
make_variable_with_instance(const std::string& name, const std::string& instance,
                            const ocpp::v2::VariableCharacteristics& characteristics, const std::string& value = "",
                            ocpp::v2::MutabilityEnum mutability = ocpp::v2::MutabilityEnum::ReadOnly) {
    DeviceModelVariable var_data;
    var_data.name = name;
    var_data.instance = instance;
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

// Populates V2X variables
std::vector<DeviceModelVariable> build_v2x_variables(const bool v2x_supported,
                                                     const std::string& supported_energy_transfers,
                                                     const std::string& supported_operation_modes) {
    std::string v2x_supported_string = v2x_supported ? "true" : "false";
    return {make_variable(ocpp::v2::V2xComponentVariables::Available.name, V2XDefinitions::Characteristics::Available,
                          v2x_supported_string),
            make_variable(ocpp::v2::V2xComponentVariables::Enabled.name, V2XDefinitions::Characteristics::Enabled,
                          v2x_supported_string),
            make_variable(ocpp::v2::V2xComponentVariables::SupportedEnergyTransferModes.name,
                          V2XDefinitions::Characteristics::SupportedEnergyTransferModes, supported_energy_transfers),
            make_variable(ocpp::v2::V2xComponentVariables::SupportedOperationModes.name,
                          V2XDefinitions::Characteristics::SupportedOperationModes, supported_operation_modes)};
}

// Populates ISO15118 variables
std::vector<DeviceModelVariable> build_iso15118_variables(const bool iso_supported,
                                                          const bool service_renegotiation_supported,
                                                          const std::string& supported_protocols) {
    return {make_variable(ocpp::v2::ISO15118ComponentVariables::Enabled.name,
                          ISO15118Definitions::Characteristics::Enabled, iso_supported ? "true" : "false"),
            make_variable(ocpp::v2::ISO15118ComponentVariables::ServiceRenegotiationSupport.name,
                          ISO15118Definitions::Characteristics::ServiceRenegotiationSupport,
                          service_renegotiation_supported ? "true" : "false"),
            make_variable(ocpp::v2::ISO15118ComponentVariables::ProtocolSupported.name,
                          ISO15118Definitions::Characteristics::ProtocolSupported, supported_protocols)};
}

// Populates ConnectedEV variables
std::vector<DeviceModelVariable> build_connected_ev_variables() {
    std::vector<DeviceModelVariable> connected_ev_variables{
        make_variable(ocpp::v2::ConnectedEvComponentVariables::Available.name,
                      ConnectedEVDefinitions::Characteristics::Available, "false"),
        make_variable(ocpp::v2::ConnectedEvComponentVariables::VehicleId.name,
                      ConnectedEVDefinitions::Characteristics::VehicleId, ""),
        make_variable(ocpp::v2::ConnectedEvComponentVariables::ProtocolAgreed.name,
                      ConnectedEVDefinitions::Characteristics::ProtocolAgreed, ""),
        make_variable(ocpp::v2::ConnectedEvComponentVariables::VehicleCertificateLeaf.name,
                      ConnectedEVDefinitions::Characteristics::VehicleCertificateLeaf, ""),
        make_variable(ocpp::v2::ConnectedEvComponentVariables::VehicleCertificateSubCa1.name,
                      ConnectedEVDefinitions::Characteristics::VehicleCertificateSubCa1, ""),
        make_variable(ocpp::v2::ConnectedEvComponentVariables::VehicleCertificateSubCa2.name,
                      ConnectedEVDefinitions::Characteristics::VehicleCertificateSubCa2, ""),
        make_variable(ocpp::v2::ConnectedEvComponentVariables::VehicleCertificateRoot.name,
                      ConnectedEVDefinitions::Characteristics::VehicleCertificateRoot, "")};
    const int number_of_variables = NUMBER_OF_CONNECTED_EV_PROTOCOLS + connected_ev_variables.size();
    connected_ev_variables.resize(number_of_variables);

    std::string variable_name = ocpp::v2::ConnectedEvComponentVariables::get_protocol_supported_by_ev(1).name;
    for (int i = 1; i <= NUMBER_OF_CONNECTED_EV_PROTOCOLS; ++i) {
        connected_ev_variables.emplace_back(make_variable_with_instance(
            variable_name, std::to_string(i), ConnectedEVDefinitions::Characteristics::ProtocolSupportedByEV));
    }
    return connected_ev_variables;
}

std::string get_everest_config_value(const everest::config::ModuleConfigurationParameters& module_config,
                                     const std::string& impl, const std::string& config_key) {
    const auto& config = module_config.at(impl);
    for (const auto& config_param : config) {
        if (config_param.name == config_key) {
            return everest::config::config_entry_to_string(config_param.value);
        }
    }
    throw std::out_of_range("Could not find requested config key: " + config_key);
}

// Populate EVerest module config variables
std::vector<DeviceModelVariable>
build_everest_config_variables(const everest::config::ModuleConfigurationParameters& module_config) {
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
            } catch (const std::exception& e) {
                EVLOG_error << "Could not add EVerest config entry to OCPP device model: " << e.what();
            }
        }
    }

    return component_config;
}

std::string supported_energy_transfer_modes_vector_to_string(
    const std::vector<types::iso15118::EnergyTransferMode>& evse_supported_energy_transfers) {
    std::string supported_string{};
    for (const auto& supported_transfer : evse_supported_energy_transfers) {
        supported_string += types::iso15118::energy_transfer_mode_to_string(supported_transfer) + ",";
    }
    if (!supported_string.empty()) {
        supported_string.pop_back();
    }
    return supported_string;
}

std::string supported_operation_modes_vector_to_string(
    const std::vector<ocpp::v2::OperationModeEnum>& evse_supported_operation_modes) {
    std::string supported_string{};
    for (const auto& operation_mode : evse_supported_operation_modes) {
        supported_string += ocpp::v2::conversions::operation_mode_enum_to_string(operation_mode) + ",";
    }
    if (!supported_string.empty()) {
        supported_string.pop_back();
    }
    return supported_string;
}

std::string build_supported_protocol_string(const std::string& uri, const int32_t major, const int32_t minor) {
    return uri + ',' + std::to_string(major) + ',' + std::to_string(minor);
}

} // anonymous namespace

EverestDeviceModelStorage::EverestDeviceModelStorage(
    const std::vector<std::unique_ptr<evse_managerIntf>>& r_evse_manager,
    const std::vector<std::unique_ptr<iso15118_extensionsIntf>>& r_extensions_15118,
    const std::map<int32_t, types::evse_board_support::HardwareCapabilities>& evse_hardware_capabilities_map,
    const std::map<int32_t, std::vector<types::iso15118::EnergyTransferMode>>& evse_supported_energy_transfers,
    const std::map<int32_t, bool>& evse_service_renegotiation_supported, const std::filesystem::path& db_path,
    const std::filesystem::path& migration_files_path,
    std::shared_ptr<Everest::config::ConfigServiceClient> config_service_client) :
    r_evse_manager(r_evse_manager),
    r_extensions_15118(r_extensions_15118),
    config_service_client(config_service_client) {
    this->module_configs = config_service_client->get_module_configs();
    this->mappings = config_service_client->get_mappings();
    std::map<ComponentKey, std::vector<DeviceModelVariable>> component_configs;

    for (const auto& evse_manager : r_evse_manager) {
        const auto evse_info = evse_manager->call_get_evse();
        const auto& hw_capabilities = evse_hardware_capabilities_map.at(evse_info.id);

        ComponentKey evse_component_key = get_evse_component_key(evse_info.id);
        const auto max_power = hw_capabilities.max_current_A_import * 230.0F * hw_capabilities.max_phase_count_import;
        component_configs[evse_component_key] = build_evse_variables(max_power);

        for (const auto& connector : evse_info.connectors) {
            ComponentKey connector_component_key = get_connector_component_key(evse_info.id, connector.id);
            component_configs[connector_component_key] = build_connector_variables();
        }

        const auto v2x_component_key = get_v2x_component_key(evse_info.id);
        const auto& supported_energy_transfer_modes = evse_supported_energy_transfers.at(evse_info.id);
        // TODO(mlitre): Update dynamically operation mode, depends on future implementation
        const bool supports_v2x =
            std::find_if(supported_energy_transfer_modes.cbegin(), supported_energy_transfer_modes.cend(),
                         [](const types::iso15118::EnergyTransferMode& mode) {
                             return mode == types::iso15118::EnergyTransferMode::AC_BPT or
                                    mode == types::iso15118::EnergyTransferMode::AC_BPT_DER or
                                    mode == types::iso15118::EnergyTransferMode::DC_BPT or
                                    mode == types::iso15118::EnergyTransferMode::DC_ACDP_BPT;
                         }) != supported_energy_transfer_modes.cend();
        component_configs[v2x_component_key] = build_v2x_variables(
            supports_v2x, supported_energy_transfer_modes_vector_to_string(supported_energy_transfer_modes),
            supported_operation_modes_vector_to_string(
                std::vector<ocpp::v2::OperationModeEnum>{ocpp::v2::OperationModeEnum::ChargingOnly}));

        const auto connected_ev_component_key = get_connected_ev_component_key(evse_info.id);
        component_configs[connected_ev_component_key] = build_connected_ev_variables();
    }
    for (const auto& extension : r_extensions_15118) {
        auto mapping = extension->get_mapping();
        if (!mapping.has_value()) {
            continue;
        }
        int evse_id = mapping->evse;
        // TODO(mlitre): Correctly fill iso supported protocols
        const auto iso15118_component_key = get_iso15118_component_key(evse_id);
        component_configs[iso15118_component_key] =
            build_iso15118_variables(true, evse_service_renegotiation_supported.at(evse_id), "");
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
                        component_key.connector_id = module_mapping.connector;
                    }
                }
            }
        }

        component_configs[component_key] = build_everest_config_variables(module_config);
    }

    ocpp::v2::InitDeviceModelDb init_device_model_db(db_path, migration_files_path);
    init_device_model_db.initialize_database(component_configs, false);
    init_device_model_db.close_connection();
    this->device_model_storage = std::make_unique<ocpp::v2::DeviceModelStorageSqlite>(db_path);

    this->init_evse_components_and_variables(evse_hardware_capabilities_map, evse_supported_energy_transfers);
    this->init_everest_config();
}

void EverestDeviceModelStorage::init_evse_components_and_variables(
    const std::map<int32_t, types::evse_board_support::HardwareCapabilities>& evse_hardware_capabilities_map,
    const std::map<int32_t, std::vector<types::iso15118::EnergyTransferMode>>& evse_supported_energy_transfers) {
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

        Component v2x_component = get_v2x_component(evse_info.id);

        if (evse_supported_energy_transfers.find(evse_info.id) != evse_supported_energy_transfers.end()) {
            this->update_supported_energy_transfers(v2x_component, evse_supported_energy_transfers.at(evse_info.id));
        } else {
            EVLOG_error << "No supported energy transfer modes found for EVSE with ID " << evse_info.id;
        }

        evse_manager->subscribe_supported_energy_transfer_modes(
            [this,
             v2x_component](const std::vector<types::iso15118::EnergyTransferMode>& supported_energy_transfer_modes) {
                this->update_supported_energy_transfers(v2x_component, supported_energy_transfer_modes);
            });
        // TODO(mlitre): Dynamic update of OperationModeEnum via energy manger conf or subscribed var

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
    for (const auto& extension : r_extensions_15118) {
        const auto mapping = extension->get_mapping();
        if (!mapping.has_value()) {
            continue;
        }
        const auto evse_id = mapping->evse;
        Component iso15118_component = get_iso15118_component(evse_id);
        extension->subscribe_service_renegotiation_supported(
            [this, iso15118_component](const bool service_renegotiation_supported) {
                this->update_service_renegotiation_supported(iso15118_component, service_renegotiation_supported);
            });

        const auto connected_ev_component = get_connected_ev_component(evse_id);
        extension->subscribe_ev_info(
            [this, connected_ev_component](const types::iso15118::EvInformation& ev_information) {
                this->update_connected_ev_information(connected_ev_component, ev_information);
            });
    }
}

void EverestDeviceModelStorage::init_everest_config() {
    for (const auto& [module_id_type, module_config] : this->module_configs) {
        for (const auto& [impl, config_params] : module_config) {
            std::string prefix;
            if (impl != Everest::config::MODULE_IMPLEMENTATION_ID) {
                // prefix variable name with impl + .
                prefix = impl + ".";
            }
            for (const auto& config_param : config_params) {
                try {
                    const auto variable_name = prefix + config_param.name;

                    Component component;
                    component.name = module_id_type.module_type;
                    component.instance = module_id_type.module_id;
                    Variable variable;
                    variable.name = variable_name;
                    ocpp::v2::ComponentVariable component_variable;
                    component_variable.component = component;
                    component_variable.variable = variable;
                    // allows to differentiate variables backed by the EVerest config from other device model variables
                    this->stored_in_everest_config_service.insert(component_variable);

                    std::lock_guard<std::mutex> lock(device_model_mutex);
                    this->device_model_storage->set_variable_attribute_value(
                        component, variable, ocpp::v2::AttributeEnum::Actual,
                        get_everest_config_value(module_config, impl, config_param.name), VARIABLE_SOURCE_EVEREST);
                } catch (const std::exception& e) {
                    EVLOG_error << "Could not initialize EVerest config entry in OCPP device model: " << e.what();
                }
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

void EverestDeviceModelStorage::update_supported_energy_transfers(
    const ocpp::v2::Component& evse_component,
    const std::vector<types::iso15118::EnergyTransferMode>& evse_supported_energy_transfers) {
    std::string supported_string = supported_energy_transfer_modes_vector_to_string(evse_supported_energy_transfers);
    std::lock_guard<std::mutex> lock(device_model_mutex);
    this->device_model_storage->set_variable_attribute_value(
        evse_component, ocpp::v2::V2xComponentVariables::SupportedEnergyTransferModes, ocpp::v2::AttributeEnum::Actual,
        supported_string, VARIABLE_SOURCE_EVEREST);
}

void EverestDeviceModelStorage::update_supported_operation_modes(
    const ocpp::v2::Component& evse_component,
    const std::vector<ocpp::v2::OperationModeEnum>& evse_supported_operation_modes) {
    std::string supported_string = supported_operation_modes_vector_to_string(evse_supported_operation_modes);
    std::lock_guard<std::mutex> lock(device_model_mutex);
    this->device_model_storage->set_variable_attribute_value(
        evse_component, ocpp::v2::V2xComponentVariables::SupportedOperationModes, ocpp::v2::AttributeEnum::Actual,
        supported_string, VARIABLE_SOURCE_EVEREST);
}

void EverestDeviceModelStorage::update_service_renegotiation_supported(const ocpp::v2::Component& iso15118_component,
                                                                       const bool& service_renegotiation_supported) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    this->device_model_storage->set_variable_attribute_value(
        iso15118_component, ocpp::v2::ISO15118ComponentVariables::ServiceRenegotiationSupport,
        ocpp::v2::AttributeEnum::Actual, service_renegotiation_supported ? "true" : "false", VARIABLE_SOURCE_EVEREST);
}

void EverestDeviceModelStorage::update_connected_ev_information(const ocpp::v2::Component& connected_ev_component,
                                                                const types::iso15118::EvInformation& ev_information) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    // We update to true even though it should already be true as a precaution
    this->device_model_storage->set_variable_attribute_value(
        connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::Available, ocpp::v2::AttributeEnum::Actual,
        "true", VARIABLE_SOURCE_EVEREST);
    this->device_model_storage->set_variable_attribute_value(
        connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::VehicleId, ocpp::v2::AttributeEnum::Actual,
        ev_information.evcc_id, VARIABLE_SOURCE_EVEREST);
    this->device_model_storage->set_variable_attribute_value(
        connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::ProtocolAgreed,
        ocpp::v2::AttributeEnum::Actual,
        build_supported_protocol_string(ev_information.selected_protocol.protocol_namespace,
                                        ev_information.selected_protocol.version_number_major,
                                        ev_information.selected_protocol.version_number_minor),
        VARIABLE_SOURCE_EVEREST);
    this->device_model_storage->set_variable_attribute_value(
        connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::VehicleCertificateLeaf,
        ocpp::v2::AttributeEnum::Actual,
        ev_information.tls_leaf_certificate.has_value() ? ev_information.tls_leaf_certificate.value() : "",
        VARIABLE_SOURCE_EVEREST);
    this->device_model_storage->set_variable_attribute_value(
        connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::VehicleCertificateSubCa1,
        ocpp::v2::AttributeEnum::Actual,
        ev_information.tls_sub_ca_1_certificate.has_value() ? ev_information.tls_sub_ca_1_certificate.value() : "",
        VARIABLE_SOURCE_EVEREST);
    this->device_model_storage->set_variable_attribute_value(
        connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::VehicleCertificateSubCa2,
        ocpp::v2::AttributeEnum::Actual,
        ev_information.tls_sub_ca_2_certificate.has_value() ? ev_information.tls_sub_ca_2_certificate.value() : "",
        VARIABLE_SOURCE_EVEREST);
    this->device_model_storage->set_variable_attribute_value(
        connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::VehicleCertificateRoot,
        ocpp::v2::AttributeEnum::Actual,
        ev_information.tls_root_certificate.has_value() ? ev_information.tls_root_certificate.value() : "",
        VARIABLE_SOURCE_EVEREST);
    for (const auto& protocol : ev_information.supported_protocols.Protocols) {
        this->device_model_storage->set_variable_attribute_value(
            connected_ev_component,
            ocpp::v2::ConnectedEvComponentVariables::get_protocol_supported_by_ev(protocol.priority),
            ocpp::v2::AttributeEnum::Actual,
            build_supported_protocol_string(protocol.protocol_namespace, protocol.version_number_major,
                                            protocol.version_number_minor),
            VARIABLE_SOURCE_EVEREST);
    }
}

void EverestDeviceModelStorage::update_connected_ev_available(const int32_t evse_id, const bool connected) {
    const auto connected_ev_component = get_connected_ev_component(evse_id);
    std::lock_guard<std::mutex> lock(device_model_mutex);
    this->device_model_storage->set_variable_attribute_value(
        connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::Available, ocpp::v2::AttributeEnum::Actual,
        connected ? "true" : "false", VARIABLE_SOURCE_EVEREST);
    if (!connected) {
        this->device_model_storage->set_variable_attribute_value(
            connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::VehicleId, ocpp::v2::AttributeEnum::Actual,
            "", VARIABLE_SOURCE_EVEREST);
        this->device_model_storage->set_variable_attribute_value(
            connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::ProtocolAgreed,
            ocpp::v2::AttributeEnum::Actual, "", VARIABLE_SOURCE_EVEREST);
        this->device_model_storage->set_variable_attribute_value(
            connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::VehicleCertificateLeaf,
            ocpp::v2::AttributeEnum::Actual, "", VARIABLE_SOURCE_EVEREST);
        this->device_model_storage->set_variable_attribute_value(
            connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::VehicleCertificateSubCa1,
            ocpp::v2::AttributeEnum::Actual, "", VARIABLE_SOURCE_EVEREST);
        this->device_model_storage->set_variable_attribute_value(
            connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::VehicleCertificateSubCa2,
            ocpp::v2::AttributeEnum::Actual, "", VARIABLE_SOURCE_EVEREST);
        this->device_model_storage->set_variable_attribute_value(
            connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::VehicleCertificateRoot,
            ocpp::v2::AttributeEnum::Actual, "", VARIABLE_SOURCE_EVEREST);
        for (int i = 1; i <= NUMBER_OF_CONNECTED_EV_PROTOCOLS; ++i) {
            this->device_model_storage->set_variable_attribute_value(
                connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::get_protocol_supported_by_ev(i),
                ocpp::v2::AttributeEnum::Actual, "", VARIABLE_SOURCE_EVEREST);
        }
    }
}

void EverestDeviceModelStorage::update_connected_ev_vehicle_id(const int32_t evse_id, const std::string& vehicle_id) {
    const auto connected_ev_component = get_connected_ev_component(evse_id);
    std::lock_guard<std::mutex> lock(device_model_mutex);
    this->device_model_storage->set_variable_attribute_value(
        connected_ev_component, ocpp::v2::ConnectedEvComponentVariables::VehicleId, ocpp::v2::AttributeEnum::Actual,
        vehicle_id, VARIABLE_SOURCE_EVEREST);
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
    return this->device_model_storage->get_device_model();
}

std::optional<ocpp::v2::VariableAttribute>
EverestDeviceModelStorage::get_variable_attribute(const ocpp::v2::Component& component_id,
                                                  const ocpp::v2::Variable& variable_id,
                                                  const ocpp::v2::AttributeEnum& attribute_enum) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->get_variable_attribute(component_id, variable_id, attribute_enum);
}

std::vector<ocpp::v2::VariableAttribute>
EverestDeviceModelStorage::get_variable_attributes(const ocpp::v2::Component& component_id,
                                                   const ocpp::v2::Variable& variable_id,
                                                   const std::optional<ocpp::v2::AttributeEnum>& attribute_enum) {
    std::lock_guard<std::mutex> lock(device_model_mutex);
    return this->device_model_storage->get_variable_attributes(component_id, variable_id, attribute_enum);
}

ocpp::v2::SetVariableStatusEnum EverestDeviceModelStorage::set_variable_attribute_value(
    const ocpp::v2::Component& component_id, const ocpp::v2::Variable& variable_id,
    const ocpp::v2::AttributeEnum& attribute_enum, const std::string& value, const std::string& source) {

    std::lock_guard<std::mutex> lock(device_model_mutex);

    int evse_id = 0;
    if (component_id.evse.has_value()) {
        evse_id = component_id.evse.value().id;
    }

    ocpp::v2::ComponentVariable component_variable;
    component_variable.component = component_id;
    component_variable.variable = variable_id;
    auto stored_in_everest_config_service_it = this->stored_in_everest_config_service.find(component_variable);
    if (stored_in_everest_config_service_it != this->stored_in_everest_config_service.end()) {
        if (attribute_enum != ocpp::v2::AttributeEnum::Actual) {
            return ocpp::v2::SetVariableStatusEnum::Rejected;
        }
        if (not component_id.instance.has_value()) {
            return ocpp::v2::SetVariableStatusEnum::Rejected;
        }
        const auto module_id = component_id.instance.value();
        everest::config::ConfigurationParameterIdentifier identifier;
        identifier.module_id = module_id;
        const std::string variable_name = variable_id.name;
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
            const auto libocpp_result = this->device_model_storage->set_variable_attribute_value(
                component_id, variable_id, attribute_enum, value, source);
            if (libocpp_result != ocpp::v2::SetVariableStatusEnum::Accepted) {
                EVLOG_error << "Device model set variable results disagree";
            }
            return libocpp_result; // FIXME: what to return, libocpp or EVerest result?
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
