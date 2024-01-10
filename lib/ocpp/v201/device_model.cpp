// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <ocpp/common/utils.hpp>
#include <ocpp/v201/ctrlr_component_variables.hpp>
#include <ocpp/v201/device_model.hpp>
#include <ocpp/v201/device_model_storage_sqlite.hpp>

namespace ocpp {

namespace v201 {

bool DeviceModel::component_criteria_match(const Component& component,
                                           const std::vector<ComponentCriterionEnum>& component_criteria) {
    if (component_criteria.empty()) {
        return true;
    }
    for (const auto& criteria : component_criteria) {
        const Variable variable = {conversions::component_criterion_enum_to_string(criteria)};
        // B08.FR.07
        // B08.FR.08
        // B08.FR.09
        // B08.FR.10
        if (!this->device_model.at(component).count(variable)) {
            return true;
        } else {
            const auto response = this->request_value<bool>(component, variable, AttributeEnum::Actual);
            auto value = response.value;
            if (response.status == GetVariableStatusEnum::Accepted and value.has_value() and value.value()) {
                return true;
            }
        }
    }
    return false;
}

bool DeviceModel::component_criteria_match_custom(const Component& component,
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
                           variable == v.variable.value()) or                       // if component has variables
                          (component == v.component and v.variable.has_value() and
                           !v.variable.value().instance.has_value() and
                           variable.name == v.variable.value().name) or // if component has no variable instances
                          (!v.component.evse.has_value() and (component.name == v.component.name) and
                           (component.instance == v.component.instance) and (variable == v.variable)); // B08.FR.23
               }) != component_variables.end();
}
bool validate_value(const VariableCharacteristics& characteristics, const std::string& value) {
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

SetVariableStatusEnum DeviceModel::set_value_internal(const Component& component, const Variable& variable,
                                                      const AttributeEnum& attribute_enum, const std::string& value,
                                                      bool force_read_only) {

    if (this->device_model.find(component) == this->device_model.end()) {
        return SetVariableStatusEnum::UnknownComponent;
    }

    auto variable_map = this->device_model[component];

    if (variable_map.find(variable) == variable_map.end()) {
        return SetVariableStatusEnum::UnknownVariable;
    }

    const auto characteristics = variable_map[variable].characteristics;
    try {
        if (!validate_value(characteristics, value)) {
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

    // If force_read_only is false, don't allow read only
    // If force_read_only is true, only allow read only
    if (!attribute.value().mutability.has_value() or
        ((attribute.value().mutability.value() == MutabilityEnum::ReadOnly) and !force_read_only) or
        ((attribute.value().mutability.value() != MutabilityEnum::ReadOnly) and force_read_only)) {
        return SetVariableStatusEnum::Rejected;
    }

    const auto success = this->storage->set_variable_attribute_value(component, variable, attribute_enum, value);
    return success ? SetVariableStatusEnum::Accepted : SetVariableStatusEnum::Rejected;
};

DeviceModel::DeviceModel(std::unique_ptr<DeviceModelStorage> device_model_storage) :
    storage{std::move(device_model_storage)} {
    this->device_model = this->storage->get_device_model();
}

SetVariableStatusEnum DeviceModel::set_value(const Component& component, const Variable& variable,
                                             const AttributeEnum& attribute_enum, const std::string& value) {
    return this->set_value_internal(component, variable, attribute_enum, value, false);
}

SetVariableStatusEnum DeviceModel::set_read_only_value(const Component& component, const Variable& variable,
                                                       const AttributeEnum& attribute_enum, const std::string& value) {

    if (component == ControllerComponents::AuthCacheCtrlr or component == ControllerComponents::LocalAuthListCtrlr or
        component == ControllerComponents::SecurityCtrlr) {
        return this->set_value_internal(component, variable, attribute_enum, value, true);
    }
    throw std::invalid_argument("Not allowed to set read only value for component " + component.name.get());
}

std::optional<VariableMetaData> DeviceModel::get_variable_meta_data(const Component& component,
                                                                    const Variable& variable) {
    if (this->device_model.count(component) and this->device_model.at(component).count(variable)) {
        return this->device_model.at(component).at(variable);
    } else {
        return std::nullopt;
    }
}

std::vector<ReportData>
DeviceModel::get_report_data(const std::optional<ReportBaseEnum>& report_base,
                             const std::optional<std::vector<ComponentVariable>>& component_variables,
                             const std::optional<std::vector<ComponentCriterionEnum>>& component_criteria) {
    std::vector<ReportData> report_data_vec;

    for (auto const& [component, variable_map] : this->device_model) {
        // check if this component should be reported based on the component criteria
        if (!component_criteria.has_value() or component_criteria_match(component, component_criteria.value())) {
            for (auto const& [variable, variable_meta_data] : variable_map) {
                // check if this variable should be reported based on the given component_variables
                auto variable_ = variable;
                auto component_ = component;
                if (!component_variables.has_value() or
                    std::find_if(component_variables.value().begin(), component_variables.value().end(),
                                 [variable_, component_](ComponentVariable v) {
                                     return component_ == v.component and v.variable.has_value() and
                                            variable_ == v.variable.value();
                                 }) != component_variables.value().end()) {
                    ReportData report_data;
                    report_data.component = component;
                    report_data.variable = variable;

                    // request the variable attribute from the device model storage
                    const auto variable_attributes = this->storage->get_variable_attributes(component, variable);

                    // iterate over possibly (Actual, Target, MinSet, MaxSet)
                    for (const auto& variable_attribute : variable_attributes) {
                        // FIXME(piet): Right now this reports only FullInventory and ConfigurationInventory (ReadWrite
                        // or WriteOnly) correctly
                        // TODO(piet): SummaryInventory
                        if (report_base == ReportBaseEnum::FullInventory or
                            variable_attribute.mutability == MutabilityEnum::ReadWrite or
                            variable_attribute.mutability == MutabilityEnum::WriteOnly) {
                            report_data.variableAttribute.push_back(variable_attribute);
                            report_data.variableCharacteristics = variable_map.at(variable).characteristics;
                        }
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

std::vector<ReportData>
DeviceModel::get_custom_report_data(const std::optional<std::vector<ComponentVariable>>& component_variables,
                                    const std::optional<std::vector<ComponentCriterionEnum>>& component_criteria) {
    std::vector<ReportData> report_data_vec;

    for (auto const& [component, variable_map] : this->device_model) {
        if (!component_criteria.has_value() or component_criteria_match_custom(component, component_criteria.value())) {

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

void DeviceModel::check_integrity() {
    EVLOG_debug << "Checking integrity of device model in storage";
    try {
        this->storage->check_integrity();
    } catch (const DeviceModelStorageError& e) {
        EVLOG_error << "Integrity check in Device Model storage failed:" << e.what();
        throw e;
    }
}

} // namespace v201
} // namespace ocpp
