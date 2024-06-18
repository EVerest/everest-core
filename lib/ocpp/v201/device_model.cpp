// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/utils.hpp>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/device_model_storage_sqlite.hpp>

namespace ocpp {

namespace v201 {

/// \brief For AlignedDataInterval, SampledDataTxUpdatedInterval and SampledDataTxEndedInterval, zero is allowed
static bool allow_zero(const Component& component, const Variable& variable) {
    ComponentVariable component_variable = {component, std::nullopt, variable};
    return component_variable == ControllerComponentVariables::AlignedDataInterval or
           component_variable == ControllerComponentVariables::SampledDataTxUpdatedInterval or
           component_variable == ControllerComponentVariables::SampledDataTxEndedInterval;
}

bool allow_set_read_only_value(const Component& component, const Variable& variable,
                               const AttributeEnum attribute_enum) {
    if (attribute_enum != AttributeEnum::Actual) {
        return false;
    }

    return component == ControllerComponents::AuthCacheCtrlr or component == ControllerComponents::LocalAuthListCtrlr or
           component == ControllerComponents::OCPPCommCtrlr or component == ControllerComponents::SecurityCtrlr or
           variable == EvseComponentVariables::AvailabilityState or variable == EvseComponentVariables::Power or
           variable == ConnectorComponentVariables::AvailabilityState;
}

bool filter_criteria_monitor(const std::vector<MonitoringCriterionEnum>& criteria,
                             const VariableMonitoringMeta& monitor) {
    // N02.FR.11 - if no criteria is provided we have a match
    if (criteria.empty()) {
        return true;
    }

    auto type = monitor.monitor.type;
    bool any_filter_match = false;

    for (auto& criterion : criteria) {
        switch (criterion) {
        case MonitoringCriterionEnum::DeltaMonitoring:
            any_filter_match = (type == MonitorEnum::Delta);
            break;
        case MonitoringCriterionEnum::ThresholdMonitoring:
            any_filter_match = (type == MonitorEnum::LowerThreshold || type == MonitorEnum::UpperThreshold);
            break;
        case MonitoringCriterionEnum::PeriodicMonitoring:
            any_filter_match = (type == MonitorEnum::Periodic || type == MonitorEnum::PeriodicClockAligned);
            break;
        }

        if (any_filter_match) {
            break;
        }
    }

    return any_filter_match;
}

void filter_criteria_monitors(const std::vector<MonitoringCriterionEnum>& criteria,
                              std::vector<VariableMonitoringMeta>& monitors) {
    // N02.FR.11 - if no criteria is provided, all monitors are left
    if (criteria.empty()) {
        return;
    }

    for (auto it = std::begin(monitors); it != std::end(monitors);) {
        bool any_filter_match = filter_criteria_monitor(criteria, *it);

        if (any_filter_match == false) {
            it = monitors.erase(it);
        } else {
            ++it;
        }
    }
}

bool DeviceModel::component_criteria_match(const Component& component,
                                           const std::vector<ComponentCriterionEnum>& component_criteria) {
    if (component_criteria.empty()) {
        return false;
    }
    for (const auto& criteria : component_criteria) {
        const Variable variable = {conversions::component_criterion_enum_to_string(criteria)};

        const auto response = this->request_value<bool>(component, variable, AttributeEnum::Actual);
        auto value = response.value;
        if (response.status == GetVariableStatusEnum::Accepted and value.has_value() and value.value()) {
            return true;
        }
        // also send true if the component crietria isn't part of the component except "problem"
        else if (!value.has_value() and (variable.name != "Problem")) {
            return true;
        }
    }
    return false;
}
bool DeviceModel::component_variables_match(const std::vector<ComponentVariable>& component_variables,
                                            const ocpp::v201::Component& component,
                                            const ocpp::v201::Variable& variable) {

    return std::find_if(
               component_variables.begin(), component_variables.end(), [component, variable](ComponentVariable v) {
                   return (component == v.component and !v.variable.has_value()) or // if component has no variable
                          (component == v.component and v.variable.has_value() and
                           variable == v.variable.value()) or // if component has variables
                          (component == v.component and v.variable.has_value() and
                           !v.variable.value().instance.has_value() and
                           variable.name == v.variable.value().name) or // if component has no variable instances
                          (!v.component.evse.has_value() and (component.name == v.component.name) and
                           (component.instance == v.component.instance) and (variable == v.variable)); // B08.FR.23
               }) != component_variables.end();
}
bool validate_value(const VariableCharacteristics& characteristics, const std::string& value, bool allow_zero) {
    switch (characteristics.dataType) {
    case DataEnum::string:
        if (characteristics.minLimit.has_value() and value.size() < characteristics.minLimit.value()) {
            return false;
        }
        if (characteristics.maxLimit.has_value() and value.size() > characteristics.maxLimit.value()) {
            return false;
        }
        return true;
    case DataEnum::decimal: {
        if (!is_decimal_number(value)) {
            return false;
        }
        float f = std::stof(value);

        if (allow_zero and f == 0) {
            return true;
        }
        if (characteristics.minLimit.has_value() and f < characteristics.minLimit.value()) {
            return false;
        }
        if (characteristics.maxLimit.has_value() and f > characteristics.maxLimit.value()) {
            return false;
        }
        return true;
    }
    case DataEnum::integer: {
        if (!is_integer(value)) {
            return false;
        }

        int i = std::stoi(value);

        if (allow_zero and i == 0) {
            return true;
        }
        if (characteristics.minLimit.has_value() and i < characteristics.minLimit.value()) {
            return false;
        }
        if (characteristics.maxLimit.has_value() and i > characteristics.maxLimit.value()) {
            return false;
        }
        return true;
    }
    case DataEnum::dateTime: {
        return is_rfc3339_datetime(value);
    }
    case DataEnum::boolean:
        return (value == "true" or value == "false");
    case DataEnum::OptionList: {
        // OptionList: The (Actual) Variable value must be a single value from the reported (CSV) enumeration list.
        if (!characteristics.valuesList.has_value()) {
            return true;
        }
        const auto values_list = ocpp::get_vector_from_csv(characteristics.valuesList.value().get());
        return std::find(values_list.begin(), values_list.end(), value) != values_list.end();
    }
    default: // same validation for MemberList or SequenceList
        // MemberList: The (Actual) Variable value may be an (unordered) (sub-)set of the reported (CSV) valid
        // values list. SequenceList: The (Actual) Variable value may be an ordered (priority, etc) (sub-)set of the
        // reported (CSV) valid values.
        {
            if (!characteristics.valuesList.has_value()) {
                return true;
            }
            const auto values_list = ocpp::get_vector_from_csv(characteristics.valuesList.value().get());
            const auto value_csv = get_vector_from_csv(value);
            for (const auto& v : value_csv) {
                if (std::find(values_list.begin(), values_list.end(), v) == values_list.end()) {
                    return false;
                }
            }
            return true;
        }
    }
}

GetVariableStatusEnum DeviceModel::request_value_internal(const Component& component_id, const Variable& variable_id,
                                                          const AttributeEnum& attribute_enum, std::string& value,
                                                          bool allow_write_only) {
    const auto component_it = this->device_model.find(component_id);
    if (component_it == this->device_model.end()) {
        EVLOG_warning << "unknown component in " << component_id.name << "." << variable_id.name;
        return GetVariableStatusEnum::UnknownComponent;
    }

    const auto& component = component_it->second;
    const auto& variable_it = component.find(variable_id);

    if (variable_it == component.end()) {
        EVLOG_warning << "unknown variable in " << component_id.name << "." << variable_id.name;
        return GetVariableStatusEnum::UnknownVariable;
    }

    const auto attribute_opt = this->storage->get_variable_attribute(component_id, variable_id, attribute_enum);

    if ((not attribute_opt) or (not attribute_opt->value)) {
        return GetVariableStatusEnum::NotSupportedAttributeType;
    }

    // only internal functions can access WriteOnly variables
    if (!allow_write_only and attribute_opt.value().mutability.has_value() and
        attribute_opt.value().mutability.value() == MutabilityEnum::WriteOnly) {
        return GetVariableStatusEnum::Rejected;
    }

    value = attribute_opt->value->get();
    return GetVariableStatusEnum::Accepted;
}

SetVariableStatusEnum DeviceModel::set_value(const Component& component, const Variable& variable,
                                             const AttributeEnum& attribute_enum, const std::string& value,
                                             bool allow_read_only) {

    if (this->device_model.find(component) == this->device_model.end()) {
        return SetVariableStatusEnum::UnknownComponent;
    }

    auto variable_map = this->device_model[component];

    if (variable_map.find(variable) == variable_map.end()) {
        return SetVariableStatusEnum::UnknownVariable;
    }

    const auto characteristics = variable_map[variable].characteristics;
    try {
        if (!validate_value(characteristics, value, allow_zero(component, variable))) {
            return SetVariableStatusEnum::Rejected;
        }
    } catch (const std::exception& e) {
        EVLOG_warning << "Could not validate value: " << value << " for component: " << component
                      << " and variable: " << variable;
        return SetVariableStatusEnum::Rejected;
    }

    const auto attribute = this->storage->get_variable_attribute(component, variable, attribute_enum);

    if (!attribute.has_value()) {
        return SetVariableStatusEnum::NotSupportedAttributeType;
    }

    // If allow_read_only is false, don't allow read only
    if (!attribute.value().mutability.has_value() or
        ((attribute.value().mutability.value() == MutabilityEnum::ReadOnly) and !allow_read_only)) {
        return SetVariableStatusEnum::Rejected;
    }

    const auto success = this->storage->set_variable_attribute_value(component, variable, attribute_enum, value);
    return success ? SetVariableStatusEnum::Accepted : SetVariableStatusEnum::Rejected;
};

DeviceModel::DeviceModel(std::unique_ptr<DeviceModelStorage> device_model_storage) :
    storage{std::move(device_model_storage)} {
    this->device_model = this->storage->get_device_model();
}

SetVariableStatusEnum DeviceModel::set_read_only_value(const Component& component, const Variable& variable,
                                                       const AttributeEnum& attribute_enum, const std::string& value) {

    if (allow_set_read_only_value(component, variable, attribute_enum)) {
        return this->set_value(component, variable, attribute_enum, value, true);
    }
    throw std::invalid_argument("Not allowed to set read only value for component " + component.name.get() +
                                " and variable " + variable.name.get());
}

std::optional<VariableMetaData> DeviceModel::get_variable_meta_data(const Component& component,
                                                                    const Variable& variable) {
    if (this->device_model.count(component) and this->device_model.at(component).count(variable)) {
        return this->device_model.at(component).at(variable);
    } else {
        return std::nullopt;
    }
}

std::vector<ReportData> DeviceModel::get_base_report_data(const ReportBaseEnum& report_base) {
    std::vector<ReportData> report_data_vec;

    for (auto const& [component, variable_map] : this->device_model) {
        for (auto const& [variable, variable_meta_data] : variable_map) {

            ReportData report_data;
            report_data.component = component;
            report_data.variable = variable;

            // request the variable attribute from the device model storage
            const auto variable_attributes = this->storage->get_variable_attributes(component, variable);

            // iterate over possibly (Actual, Target, MinSet, MaxSet)
            for (const auto& variable_attribute : variable_attributes) {
                // FIXME(piet): Right now this reports only FullInventory (ReadOnly,
                // ReadWrite or WriteOnly) and ConfigurationInventory (ReadWrite or WriteOnly) correctly
                // TODO(piet): SummaryInventory
                if (report_base == ReportBaseEnum::FullInventory or
                    (report_base == ReportBaseEnum::ConfigurationInventory and
                     (variable_attribute.mutability == MutabilityEnum::ReadWrite or
                      variable_attribute.mutability == MutabilityEnum::WriteOnly))) {
                    report_data.variableAttribute.push_back(variable_attribute);
                    // scrub WriteOnly value from report
                    if (variable_attribute.mutability == MutabilityEnum::WriteOnly) {
                        report_data.variableAttribute.back().value.reset();
                    }
                    report_data.variableCharacteristics = variable_map.at(variable).characteristics;
                }
            }
            if (!report_data.variableAttribute.empty()) {
                report_data_vec.push_back(report_data);
            }
        }
    }
    return report_data_vec;
}

std::vector<ReportData>
DeviceModel::get_custom_report_data(const std::optional<std::vector<ComponentVariable>>& component_variables,
                                    const std::optional<std::vector<ComponentCriterionEnum>>& component_criteria) {
    std::vector<ReportData> report_data_vec;

    for (auto const& [component, variable_map] : this->device_model) {
        if (!component_criteria.has_value() or component_criteria_match(component, component_criteria.value())) {

            for (auto const& [variable, variable_meta_data] : variable_map) {
                if (!component_variables.has_value() or
                    component_variables_match(component_variables.value(), component, variable)) {
                    ReportData report_data;
                    report_data.component = component;
                    report_data.variable = variable;

                    //  request the variable attribute from the device model storage
                    const auto variable_attributes = this->storage->get_variable_attributes(component, variable);

                    for (const auto& variable_attribute : variable_attributes) {
                        report_data.variableAttribute.push_back(variable_attribute);
                        report_data.variableCharacteristics = variable_map.at(variable).characteristics;
                    }

                    if (!report_data.variableAttribute.empty()) {
                        report_data_vec.push_back(report_data);
                    }
                }
            }
        }
    }
    return report_data_vec;
}

void DeviceModel::check_integrity(const std::map<int32_t, int32_t>& evse_connector_structure) {
    EVLOG_debug << "Checking integrity of device model in storage";
    try {
        this->storage->check_integrity();

        int32_t nr_evse_components = 0;
        std::map<int32_t, int32_t> evse_id_nr_connector_components;

        for (const auto& [component, variable_map] : this->device_model) {
            if (component.name == "EVSE") {
                nr_evse_components++;
            } else if (component.name == "Connector") {
                if (evse_id_nr_connector_components.count(component.evse.value().id)) {
                    evse_id_nr_connector_components[component.evse.value().id] += 1;
                } else {
                    evse_id_nr_connector_components[component.evse.value().id] = 1;
                }
            }
        }

        // check if number of EVSE in the device model matches the configured number
        if (nr_evse_components != evse_connector_structure.size()) {
            throw DeviceModelStorageError("Number of EVSE configured in device model is incompatible with number of "
                                          "configured EVSEs of the ChargePoint");
        }

        for (const auto [evse_id, nr_of_connectors] : evse_connector_structure) {
            // check if number of Cpnnectors for this EVSE in the device model matches the configured number
            if (evse_id_nr_connector_components[evse_id] != nr_of_connectors) {
                throw DeviceModelStorageError(
                    "Number of Connectors configured in device model is incompatible with number "
                    "of configured Connectors of the ChargePoint");
            }

            // check if all relevant EVSE and Connector components can be found
            EVSE evse = {evse_id};
            Component evse_component = {"EVSE", std::nullopt, evse};
            if (!this->device_model.count(evse_component)) {
                throw DeviceModelStorageError("Could not find required EVSE component in device model");
            }
            for (size_t connector_id = 1; connector_id <= nr_of_connectors; connector_id++) {
                evse_component.name = "Connector";
                evse_component.evse.value().connectorId = connector_id;
                if (!this->device_model.count(evse_component)) {
                    throw DeviceModelStorageError("Could not find required Connector component in device model");
                }
            }
        }
    } catch (const DeviceModelStorageError& e) {
        EVLOG_error << "Integrity check in Device Model storage failed:" << e.what();
        throw e;
    }
}

std::vector<SetMonitoringResult> DeviceModel::set_monitors(const std::vector<SetMonitoringData>& requests,
                                                           const VariableMonitorType type) {
    if (requests.empty()) {
        return {};
    }

    std::vector<SetMonitoringResult> set_monitors_res;

    for (auto& request : requests) {
        SetMonitoringResult result;

        if (this->device_model.find(request.component) == this->device_model.end()) {
            result.status = SetMonitoringStatusEnum::UnknownComponent;
            set_monitors_res.push_back(result);
            continue;
        }

        auto& variable_map = this->device_model[request.component];
        auto variable_it = variable_map.find(request.variable);

        if (variable_it == variable_map.end()) {
            result.status = SetMonitoringStatusEnum::UnknownVariable;
            set_monitors_res.push_back(result);
            continue;
        }

        // TODO (ioan): see how we should handle the 'Duplicate' data
        try {
            auto monitor_meta = this->storage->set_monitoring_data(request, type);

            if (monitor_meta.has_value()) {
                // If we had a successful insert, add it to the variable monitor map
                variable_it->second.monitors.insert(
                    std::pair{monitor_meta.value().monitor.id, std::move(monitor_meta.value())});
                result.status = SetMonitoringStatusEnum::Accepted;
            } else {
                result.status = SetMonitoringStatusEnum::Rejected;
            }
        } catch (const ocpp::common::QueryExecutionException& e) {
            EVLOG_error << "Set monitors failed:" << e.what();
            throw e;
        }

        set_monitors_res.push_back(result);
    }

    return set_monitors_res;
}
std::vector<MonitoringData> DeviceModel::get_monitors(const std::vector<MonitoringCriterionEnum>& criteria,
                                                      const std::vector<ComponentVariable>& component_variables) {
    std::vector<MonitoringData> get_monitors_res{};

    // N02.FR.11 - if criteria and component_variables are empty, return all existing monitors
    if (criteria.empty() && component_variables.empty()) {
        for (const auto& [component, variable_map] : this->device_model) {
            for (const auto& [variable, variable_metadata] : variable_map) {
                std::vector<VariableMonitoring> monitors;

                for (const auto& [id, monitor_meta] : variable_metadata.monitors) {
                    monitors.push_back(monitor_meta.monitor);
                }

                get_monitors_res.push_back({component, variable, monitors, std::nullopt});
            }
        }

        return get_monitors_res;
    } else {
        for (auto& component_variable : component_variables) {
            // Case not handled by spec, skipping
            if (this->device_model.find(component_variable.component) == this->device_model.end()) {
                continue;
            }

            auto& variable_map = this->device_model[component_variable.component];

            // N02.FR.16 - if variable is missing, report all existing variables inside that component
            if (component_variable.variable.has_value() == false) {
                for (const auto& [variable, variable_meta] : variable_map) {
                    MonitoringData monitor_data;

                    monitor_data.component = component_variable.component;
                    monitor_data.variable = variable;

                    for (const auto& [id, monitor_meta] : variable_meta.monitors) {
                        if (filter_criteria_monitor(criteria, monitor_meta)) {
                            monitor_data.variableMonitoring.push_back(monitor_meta.monitor);
                        }
                    }

                    get_monitors_res.push_back(std::move(monitor_data));
                }
            } else {
                auto variable_it = variable_map.find(component_variable.variable.value());

                // Case not handled by spec, skipping
                if (variable_it == variable_map.end()) {
                    continue;
                }

                MonitoringData monitor_data;

                monitor_data.component = component_variable.component;
                monitor_data.variable = variable_it->first;

                auto& variable_meta = variable_it->second;

                for (const auto& [id, monitor_meta] : variable_meta.monitors) {
                    if (filter_criteria_monitor(criteria, monitor_meta)) {
                        monitor_data.variableMonitoring.push_back(monitor_meta.monitor);
                    }
                }

                get_monitors_res.push_back(std::move(monitor_data));
            }
        }
    }

    return get_monitors_res;
}
std::vector<ClearMonitoringResult> DeviceModel::clear_monitors(const std::vector<int>& request_ids) {
    if (request_ids.size() <= 0) {
        return {};
    }

    std::vector<ClearMonitoringResult> clear_monitors_vec;

    for (auto& id : request_ids) {
        ClearMonitoringResult clear_monitor_res;
        clear_monitor_res.id = id;

        if (this->storage->clear_variable_monitor(id)) {
            // Clear from memory too
            for (auto& [component, variable_map] : this->device_model) {
                for (auto& [variable, variable_metadata] : variable_map) {
                    variable_metadata.monitors.erase(static_cast<int64_t>(id));
                }
            }

            clear_monitor_res.status = ClearMonitoringStatusEnum::Accepted;
        } else {
            clear_monitor_res.status = ClearMonitoringStatusEnum::NotFound;
        }

        clear_monitors_vec.push_back(clear_monitor_res);
    }

    return clear_monitors_vec;
}

} // namespace v201
} // namespace ocpp
