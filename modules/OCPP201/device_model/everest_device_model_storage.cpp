// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <device_model/everest_device_model_storage.hpp>

namespace module::device_model {
ocpp::v2::DeviceModelMap EverestDeviceModelStorage::get_device_model() {
    DeviceModelMap everest_dm;

    const auto auth_config = this->r_auth_config->get_all_configs();
    const auto iso15118_config = this->r_iso15118_config->get_all_configs();

    // for (const auto config_item : auth_config) {
        // if config_item.key == ConnectionTimeout:
            // everest.dm.auth_ctrlr.connection_timeout = ...
    }

    // for (auto extension : r_device_model_extension) {
    //  everest_dm.merge(extension_get_device_model());
    // }

    everest_dm.merge(libocpp_dm);
    return everest_dm;
}

std::optional<ocpp::v2::VariableAttribute>
EverestDeviceModelStorage::get_variable_attribute(const ocpp::v2::Component& /*component_id*/,
                                                  const ocpp::v2::Variable& /*variable_id*/,
                                                  const ocpp::v2::AttributeEnum& /*attribute_enum*/) {
    if (get_variable_source(component_id, variable_id) == EVEREST_STANDARDIZED) {
        // request from requirement configs 
    } else {
        // request from r_device_model_extension
    }          
    return std::nullopt;
}

std::vector<ocpp::v2::VariableAttribute>
EverestDeviceModelStorage::get_variable_attributes(const ocpp::v2::Component& /*component_id*/,
                                                   const ocpp::v2::Variable& /*variable_id*/,
                                                   const std::optional<ocpp::v2::AttributeEnum>& /*attribute_enum*/) {
    return {};
}

bool EverestDeviceModelStorage::set_variable_attribute_value(const ocpp::v2::Component& component_id,
                                                             const ocpp::v2::Variable& variable_id,
                                                             const ocpp::v2::AttributeEnum& attribute_enum,
                                                             const std::string& value,
                                                             const std::string& source) {
    // if component == AuthCtlrlr and variable == EVConnectionTimeout:
        // return this->r_auth_config->set_connection_timeout(value, source);
    // else if (component == ISO15118Ctrlr and variable == ContractValidationAllowed):
        // return this->r_iso15118_config->set_contract_validation_allowed(value, source);
    // else if (component == CustomCtrlr and variable == CustomVariable):
        // return this->r_ocpp_device_model->set_variable(...);
    
    return false;
}

std::optional<ocpp::v2::VariableMonitoringMeta>
EverestDeviceModelStorage::set_monitoring_data(const ocpp::v2::SetMonitoringData& /*data*/,
                                               const ocpp::v2::VariableMonitorType /*type*/) {
    return std::nullopt;
}

bool EverestDeviceModelStorage::update_monitoring_reference(const int32_t /*monitor_id*/,
                                                            const std::string& /*reference_value*/) {
    return false;
}

std::vector<ocpp::v2::VariableMonitoringMeta>
EverestDeviceModelStorage::get_monitoring_data(const std::vector<ocpp::v2::MonitoringCriterionEnum>& /*criteria*/,
                                               const ocpp::v2::Component& /*component_id*/,
                                               const ocpp::v2::Variable& /*variable_id*/) {
    return {};
}

ocpp::v2::ClearMonitoringStatusEnum EverestDeviceModelStorage::clear_variable_monitor(int /*monitor_id*/,
                                                                                      bool /*allow_protected*/) {
    return ocpp::v2::ClearMonitoringStatusEnum::NotFound;
}

int32_t EverestDeviceModelStorage::clear_custom_variable_monitors() {
    return 0;
}

void EverestDeviceModelStorage::check_integrity() {
}
} // namespace module::device_model
