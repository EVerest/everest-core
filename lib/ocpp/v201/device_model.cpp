// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/database/database_exceptions.hpp>
#include <ocpp/common/utils.hpp>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/device_model_storage_sqlite.hpp>

namespace ocpp {

namespace v201 {

using DatabaseException = ocpp::common::DatabaseException;

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
        EVLOG_debug << "unknown component in " << component_id.name << "." << variable_id.name;
        return GetVariableStatusEnum::UnknownComponent;
    }

    const auto& component = component_it->second;
    const auto& variable_it = component.find(variable_id);

    if (variable_it == component.end()) {
        EVLOG_debug << "unknown variable in " << component_id.name << "." << variable_id.name;
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
                                             const std::string& source, bool allow_read_only) {

    if (this->device_model.find(component) == this->device_model.end()) {
        return SetVariableStatusEnum::UnknownComponent;
    }

    auto variable_map = this->device_model[component];

    if (variable_map.find(variable) == variable_map.end()) {
        return SetVariableStatusEnum::UnknownVariable;
    }

    const auto& characteristics = variable_map[variable].characteristics;
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

    const auto success =
        this->storage->set_variable_attribute_value(component, variable, attribute_enum, value, source);

    // Only trigger for actual values
    if ((attribute_enum == AttributeEnum::Actual) && success && variable_listener) {
        const auto& monitors = variable_map[variable].monitors;

        // If we had a variable value change, trigger the listener
        if (!monitors.empty()) {
            static const std::string EMPTY_VALUE{};

            const std::string& value_previous = attribute.value().value.value_or(EMPTY_VALUE);
            const std::string& value_current = value;

            if (value_previous != value_current) {
                variable_listener(monitors, component, variable, characteristics, attribute.value(), value_previous,
                                  value_current);
            }
        }
    }

    return success ? SetVariableStatusEnum::Accepted : SetVariableStatusEnum::Rejected;
};

DeviceModel::DeviceModel(std::unique_ptr<DeviceModelStorage> device_model_storage) :
    storage{std::move(device_model_storage)} {
    this->device_model = this->storage->get_device_model();
}

SetVariableStatusEnum DeviceModel::set_read_only_value(const Component& component, const Variable& variable,
                                                       const AttributeEnum& attribute_enum, const std::string& value,
                                                       const std::string& source) {

    if (allow_set_read_only_value(component, variable, attribute_enum)) {
        return this->set_value(component, variable, attribute_enum, value, source, true);
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

bool DeviceModel::update_monitor_reference(int32_t monitor_id, const std::string& reference_value) {
    bool found_monitor = false;
    VariableMonitoringMeta* monitor_meta = nullptr;

    // See if this is a trivial delta monitor and that it exists
    for (auto& [component, variable_map] : this->device_model) {
        bool found_monitor_id = false;

        for (auto& [variable, variable_meta_data] : variable_map) {
            auto it = variable_meta_data.monitors.find(monitor_id);
            if (it != std::end(variable_meta_data.monitors)) {
                auto& characteristics = variable_meta_data.characteristics;

                if ((characteristics.dataType == DataEnum::boolean) || (characteristics.dataType == DataEnum::string) ||
                    (characteristics.dataType == DataEnum::dateTime) ||
                    (characteristics.dataType == DataEnum::OptionList) ||
                    (characteristics.dataType == DataEnum::MemberList) ||
                    (characteristics.dataType == DataEnum::SequenceList) &&
                        (it->second.monitor.type == MonitorEnum::Delta)) {
                    monitor_meta = &it->second;
                    found_monitor = true;
                } else {
                    found_monitor = false;
                }

                found_monitor_id = true;
                break; // Break inner loop
            }
        }

        if (found_monitor_id) {
            break; // Break outer loop
        }
    }

    if (found_monitor) {
        try {
            if (this->storage->update_monitoring_reference(monitor_id, reference_value)) {
                // Update value in-memory too
                monitor_meta->reference_value = reference_value;
                return true;
            } else {
                EVLOG_warning << "Could not update in DB trivial delta monitor with ID: " << monitor_id
                              << ". Reference value not updated!";
            }
        } catch (const DatabaseException& e) {
            EVLOG_error << "Exception while updating trivial delta monitor reference with ID: " << monitor_id;
            throw DeviceModelStorageError(e.what());
        }
    } else {
        EVLOG_warning << "Could not find trivial delta monitor with ID: " << monitor_id
                      << ". Reference value not updated!";
    }

    return false;
}

std::vector<SetMonitoringResult> DeviceModel::set_monitors(const std::vector<SetMonitoringData>& requests,
                                                           const VariableMonitorType type) {
    if (requests.empty()) {
        return {};
    }

    std::vector<SetMonitoringResult> set_monitors_res;

    for (auto& request : requests) {
        SetMonitoringResult result;

        // Set the com/var based on the request since it's required in a response
        // even if it is 'UnknownComoponent/Variable'
        result.component = request.component;
        result.variable = request.variable;
        result.severity = request.severity;
        result.type = request.type;
        result.id = request.id;

        // N04.FR.16 - If we find a monitor with this ID, and there's a comp/var mismatch, send a rejected result
        // N04.FR.13 - If we receive an ID but we can't find a monitor with this ID send a rejected result
        bool request_has_id = request.id.has_value();
        bool id_found = false;

        if (request_has_id) {
            // Search through all the ID's
            for (const auto& [component, variable_map] : this->device_model) {
                for (const auto& [variable, variable_meta] : variable_map) {
                    if (variable_meta.monitors.find(request.id.value()) != std::end(variable_meta.monitors)) {
                        id_found = true;
                        break;
                    }
                }

                if (id_found) {
                    break;
                }
            }

            if (!id_found) {
                result.status = SetMonitoringStatusEnum::Rejected;
                set_monitors_res.push_back(result);
                continue;
            }
        }

        if (this->device_model.find(request.component) == this->device_model.end()) {
            // N04.FR.16
            if (request_has_id && id_found) {
                result.status = SetMonitoringStatusEnum::Rejected;
            } else {
                result.status = SetMonitoringStatusEnum::UnknownComponent;
            }

            set_monitors_res.push_back(result);
            continue;
        }

        auto& variable_map = this->device_model[request.component];

        auto variable_it = variable_map.find(request.variable);
        if (variable_it == variable_map.end()) {
            // N04.FR.16
            if (request_has_id && id_found) {
                result.status = SetMonitoringStatusEnum::Rejected;
            } else {
                result.status = SetMonitoringStatusEnum::UnknownVariable;
            }

            set_monitors_res.push_back(result);
            continue;
        }

        // Validate the data we want to set based on the characteristics and
        // see if it is out of range or out of the variable list
        const auto& characteristics = variable_it->second.characteristics;
        bool valid_value = true;

        if (characteristics.supportsMonitoring) {
            EVLOG_debug << "Validating monitor request of type: [" << request << "] and characteristics: ["
                        << characteristics << "]"
                        << " and value: " << request.value;

            // If the monitor is of type 'delta' (or periodic) and it is of a non-numeric type
            // the value is ignored since all values will be reported (excepting negative values)
            if (request.type == MonitorEnum::Delta && std::signbit(request.value)) {
                // N04.FR.14
                valid_value = false;
            } else if (request.type == MonitorEnum::Delta && (characteristics.dataType != DataEnum::decimal &&
                                                              characteristics.dataType != DataEnum::integer)) {
                valid_value = true;
            } else if (request.type == MonitorEnum::Periodic || request.type == MonitorEnum::PeriodicClockAligned) {
                valid_value = true;
            } else {
                try {
                    valid_value = validate_value(characteristics, std::to_string(request.value),
                                                 allow_zero(request.component, request.variable));
                } catch (const std::exception& e) {
                    EVLOG_warning << "Could not validate monitor value: " << request.value
                                  << " for component: " << request.component << " and variable: " << request.variable;
                    valid_value = false;
                }
            }
        } else {
            valid_value = false;
        }

        if (!valid_value) {
            result.status = SetMonitoringStatusEnum::Rejected;
            set_monitors_res.push_back(result);
            continue;
        }

        // 3.77 Duplicate - A monitor already exists for the given type/severity combination.
        bool duplicate_value = false;

        // Only test for duplicates if we do not receive an explicit monitor ID
        if (!request_has_id) {
            for (const auto& [id, monitor_meta] : variable_it->second.monitors) {
                if (monitor_meta.monitor.type == request.type && monitor_meta.monitor.severity == request.severity) {
                    duplicate_value = true;
                    break;
                }
            }
        }

        if (duplicate_value) {
            result.status = SetMonitoringStatusEnum::Duplicate;
            set_monitors_res.push_back(result);
            continue;
        }

        try {
            auto monitor_meta = this->storage->set_monitoring_data(request, type);

            if (monitor_meta.has_value()) {
                // If we had a successful insert, add/replace it to the variable monitor map
                variable_it->second.monitors[monitor_meta.value().monitor.id] = std::move(monitor_meta.value());

                result.id = monitor_meta.value().monitor.id;
                result.status = SetMonitoringStatusEnum::Accepted;
            } else {
                result.status = SetMonitoringStatusEnum::Rejected;
            }
        } catch (const DatabaseException& e) {
            EVLOG_error << "Set monitors failed:" << e.what();
            throw DeviceModelStorageError(e.what());
        }

        set_monitors_res.push_back(result);
    }

    return set_monitors_res;
}

std::vector<VariableMonitoringPeriodic> DeviceModel::get_periodic_monitors() {
    std::vector<VariableMonitoringPeriodic> periodics;

    for (const auto& [component, variable_map] : this->device_model) {
        for (const auto& [variable, variable_metadata] : variable_map) {
            std::vector<VariableMonitoringMeta> monitors;

            for (const auto& [id, monitor_meta] : variable_metadata.monitors) {
                if (monitor_meta.monitor.type == MonitorEnum::Periodic ||
                    monitor_meta.monitor.type == MonitorEnum::PeriodicClockAligned) {
                    monitors.push_back(monitor_meta);
                }
            }

            if (!monitors.empty()) {
                periodics.push_back({component, variable, monitors});
            }
        }
    }

    return periodics;
}

std::vector<MonitoringData> DeviceModel::get_monitors(const std::vector<MonitoringCriterionEnum>& criteria,
                                                      const std::vector<ComponentVariable>& component_variables) {
    std::vector<MonitoringData> get_monitors_res{};

    if (!component_variables.empty()) {
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

                    if (!monitor_data.variableMonitoring.empty()) {
                        get_monitors_res.push_back(std::move(monitor_data));
                    }
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

                if (!monitor_data.variableMonitoring.empty()) {
                    get_monitors_res.push_back(std::move(monitor_data));
                }
            }
        }
    } else {
        // N02.FR.11 - if criteria and component_variables are empty, return all existing monitors
        for (const auto& [component, variable_map] : this->device_model) {
            for (const auto& [variable, variable_metadata] : variable_map) {
                std::vector<VariableMonitoring> monitors;

                for (const auto& [id, monitor_meta] : variable_metadata.monitors) {
                    // Also handles the case when the criteria is empty,
                    // since in that case N02.FR.11 applies (all monitors pass)
                    if (filter_criteria_monitor(criteria, monitor_meta)) {
                        monitors.push_back(monitor_meta.monitor);
                    }
                }

                if (!monitors.empty()) {
                    get_monitors_res.push_back({component, variable, monitors, std::nullopt});
                }
            }
        }
    }

    return get_monitors_res;
}
std::vector<ClearMonitoringResult> DeviceModel::clear_monitors(const std::vector<int>& request_ids,
                                                               bool allow_protected) {
    if (request_ids.size() <= 0) {
        return {};
    }

    std::vector<ClearMonitoringResult> clear_monitors_vec;

    for (auto& id : request_ids) {
        ClearMonitoringResult clear_monitor_res;
        clear_monitor_res.id = id;

        try {
            auto clear_result = this->storage->clear_variable_monitor(id, allow_protected);
            if (clear_result == ClearMonitoringStatusEnum::Accepted) {
                // Clear from memory too
                for (auto& [component, variable_map] : this->device_model) {
                    for (auto& [variable, variable_metadata] : variable_map) {
                        variable_metadata.monitors.erase(static_cast<int64_t>(id));
                    }
                }
            }

            clear_monitor_res.status = clear_result;
        } catch (const DatabaseException& e) {
            EVLOG_error << "Clear monitors failed:" << e.what();
            throw DeviceModelStorageError(e.what());
        }

        clear_monitors_vec.push_back(clear_monitor_res);
    }

    return clear_monitors_vec;
}

int32_t DeviceModel::clear_custom_monitors() {
    try {
        int32_t deleted = this->storage->clear_custom_variable_monitors();

        // Clear from memory too
        for (auto& [component, variable_map] : this->device_model) {
            for (auto& [variable, variable_metadata] : variable_map) {
                // Delete while iterating all custom monitors
                for (auto it = variable_metadata.monitors.begin(); it != variable_metadata.monitors.end();) {
                    if (it->second.type == VariableMonitorType::CustomMonitor) {
                        it = variable_metadata.monitors.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }

        return deleted;
    } catch (const DatabaseException& e) {
        EVLOG_error << "Clear custom monitors failed:" << e.what();
        throw DeviceModelStorageError(e.what());
    }

    return 0;
}

} // namespace v201
} // namespace ocpp
