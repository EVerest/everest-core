// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef DEVICE_MODEL_HPP
#define DEVICE_MODEL_HPP

#include <type_traits>

#include <everest/logging.hpp>

#include <ocpp/v2/ctrlr_component_variables.hpp>
#include <ocpp/v2/device_model_storage_interface.hpp>

namespace ocpp {
namespace v2 {

/// \brief Response to requesting a value from the device model
/// \tparam T
template <typename T> struct RequestDeviceModelResponse {
    GetVariableStatusEnum status;
    std::optional<T> value;
};

/// \brief Converts the given \p value to the specific type based on the template parameter
/// \tparam T
/// \param value
/// \return
template <typename T> T to_specific_type(const std::string& value) {
    static_assert(std::is_same<T, std::string>::value || std::is_same<T, int>::value ||
                      std::is_same<T, double>::value || std::is_same<T, size_t>::value ||
                      std::is_same<T, DateTime>::value || std::is_same<T, bool>::value ||
                      std::is_same<T, uint64_t>::value,
                  "Requested unknown datatype");

    if constexpr (std::is_same<T, std::string>::value) {
        return value;
    } else if constexpr (std::is_same<T, int>::value) {
        return std::stoi(value);
    } else if constexpr (std::is_same<T, double>::value) {
        return std::stod(value);
    } else if constexpr (std::is_same<T, size_t>::value) {
        size_t res = std::stoul(value);
        return res;
    } else if constexpr (std::is_same<T, DateTime>::value) {
        return DateTime(value);
    } else if constexpr (std::is_same<T, bool>::value) {
        return ocpp::conversions::string_to_bool(value);
    } else if constexpr (std::is_same<T, uint64_t>::value) {
        return std::stoull(value);
    }
}

template <DataEnum T> auto to_specific_type_auto(const std::string& value) {
    static_assert(T == DataEnum::string || T == DataEnum::integer || T == DataEnum::decimal ||
                      T == DataEnum::dateTime || T == DataEnum::boolean,
                  "Requested unknown datatype");

    if constexpr (T == DataEnum::string) {
        return to_specific_type<std::string>(value);
    } else if constexpr (T == DataEnum::integer) {
        return to_specific_type<int>(value);
    } else if constexpr (T == DataEnum::decimal) {
        return to_specific_type<double>(value);
    } else if constexpr (T == DataEnum::dateTime) {
        return to_specific_type<DateTime>(value);
    } else if constexpr (T == DataEnum::boolean) {
        return to_specific_type<bool>(value);
    }
}

template <DataEnum T> bool is_type_numeric() {
    static_assert(T == DataEnum::string || T == DataEnum::integer || T == DataEnum::decimal ||
                      T == DataEnum::dateTime || T == DataEnum::boolean || T == DataEnum::OptionList ||
                      T == DataEnum::SequenceList || T == DataEnum::MemberList,
                  "Requested unknown datatype");

    if constexpr (T == DataEnum::integer || T == DataEnum::decimal) {
        return true;
    } else {
        return false;
    }
}

typedef std::function<void(const std::unordered_map<int64_t, VariableMonitoringMeta>& monitors,
                           const Component& component, const Variable& variable,
                           const VariableCharacteristics& characteristics, const VariableAttribute& attribute,
                           const std::string& value_previous, const std::string& value_current)>
    on_variable_changed;

typedef std::function<void(const VariableMonitoringMeta& updated_monitor, const Component& component,
                           const Variable& variable, const VariableCharacteristics& characteristics,
                           const VariableAttribute& attribute, const std::string& current_value)>
    on_monitor_updated;

/// \brief This class manages access to the device model representation and to the device model interface and provides
/// functionality to support the use cases defined in the functional block Provisioning
class DeviceModel {

private:
    DeviceModelMap device_model_map;
    std::unique_ptr<DeviceModelStorageInterface> device_model;

    /// \brief Listener for the internal change of a variable
    on_variable_changed variable_listener;
    /// \brief Listener for the internal update of a monitor
    on_monitor_updated monitor_update_listener;

    /// \brief Private helper method that does some checks with the device model representation in memory to evaluate if
    /// a value for the given parameters can be requested. If it can be requested it will be retrieved from the device
    /// model interface and the given \p value will be set to the value that was retrieved
    /// \param component_id
    /// \param variable_id
    /// \param attribute_enum
    /// \param value string reference to value: will be set to requested value if value is present
    /// \param allow_write_only true to allow a writeOnly value to be read.
    /// \return GetVariableStatusEnum that indicates the result of the request
    GetVariableStatusEnum request_value_internal(const Component& component_id, const Variable& variable_id,
                                                 const AttributeEnum& attribute_enum, std::string& value,
                                                 bool allow_write_only) const;

    /// \brief Iterates over the given \p component_criteria and converts this to the variable names
    /// (Active,Available,Enabled,Problem). If any of the variables can not be found as part of a component this
    /// function returns false. If any of those variable's value is true, this function returns true (except for
    /// criteria problem). If all variable's value are false, this function returns false
    ///  \param component_id
    ///  \param /// component_criteria
    ///  \return
    bool component_criteria_match(const Component& component_id,
                                  const std::vector<ComponentCriterionEnum>& component_criteria);

    /// @brief Iterates over the given \p component_variables and filters them according to the requirement conditions.
    /// @param component_variables
    /// @param component_ current component
    /// @param variable_ current variable
    /// @return true if the component is found according to any of the requirement conditions.
    bool component_variables_match(const std::vector<ComponentVariable>& component_variables,
                                   const ocpp::v2::Component& component_, const struct ocpp::v2::Variable& variable_);

    ///
    /// \brief Helper function to check if a variable has a value.
    /// \param component_variable   Component variable to check.
    /// \param attribute            Attribute to check.
    ///
    /// \throws DeviceModelError if variable has no value or value is an empty string.
    ///
    void check_variable_has_value(const ComponentVariable& component_variable,
                                  const AttributeEnum attribute = AttributeEnum::Actual);

    ///
    /// \brief Helper function to check if a required variable has a value.
    /// \param required_variable    Required component variable to check.
    /// \param supported_versions   The current supported ocpp versions.
    /// \throws DeviceModelError if variable has no value or value is an empty string.
    ///
    void check_required_variable(const RequiredComponentVariable& required_variable,
                                 const std::vector<OcppProtocolVersion>& supported_versions);

    ///
    /// \brief Loop over all required variables to check if they have a value.
    ///
    /// This will check for all required variables from `ctrlr_component_variables.cpp` `required_variables`.
    /// It will also check for specific required variables that belong to a specific controller. If a controller is not
    /// available, the 'required' variables of that component are not required at this point.
    ///
    /// \throws DeviceModelError if one of the variables does not have a value or value is an empty string.
    ///
    void check_required_variables();

public:
    /// \brief Constructor for the device model
    /// \param device_model_storage_interface pointer to a device model interface class
    explicit DeviceModel(std::unique_ptr<DeviceModelStorageInterface> device_model_storage_interface);

    /// \brief Direct access to value of a VariableAttribute for the given component, variable and attribute_enum. This
    /// should only be called for variables that have a role standardized in the OCPP2.0.1 specification.
    /// \tparam T datatype of the value that is requested
    /// \param component_variable Combination of Component and Variable that identifies the Variable
    /// \param attribute_enum defaults to AttributeEnum::Actual
    /// \return the requested value from the device model interface
    template <typename T>
    T get_value(const RequiredComponentVariable& component_variable,
                const AttributeEnum& attribute_enum = AttributeEnum::Actual) const {
        std::string value;
        auto response = GetVariableStatusEnum::UnknownVariable;
        if (component_variable.variable.has_value()) {
            response = this->request_value_internal(component_variable.component, component_variable.variable.value(),
                                                    attribute_enum, value, true);
        }
        if (response == GetVariableStatusEnum::Accepted) {
            return to_specific_type<T>(value);
        } else {
            EVLOG_critical << "Directly requested value for ComponentVariable that doesn't exist in the device model: "
                           << component_variable;
            EVLOG_AND_THROW(std::runtime_error(
                "Directly requested value for ComponentVariable that doesn't exist in the device model."));
        }
    }

    /// \brief  Access to std::optional of a VariableAttribute for the given component, variable and attribute_enum.
    /// \tparam T Type of the value that is requested
    /// \param component_variable Combination of Component and Variable that identifies the Variable
    /// \param attribute_enum
    /// \return std::optional<T> if the combination of \p component_variable and \p attribute_enum could successfully
    /// requested from the storage and a value is present for this combination, else std::nullopt .
    template <typename T>
    std::optional<T> get_optional_value(const ComponentVariable& component_variable,
                                        const AttributeEnum& attribute_enum = AttributeEnum::Actual) const {
        std::string value;
        auto response = GetVariableStatusEnum::UnknownVariable;
        if (component_variable.variable.has_value()) {
            response = this->request_value_internal(component_variable.component, component_variable.variable.value(),
                                                    attribute_enum, value, true);
        }
        if (response == GetVariableStatusEnum::Accepted) {
            return to_specific_type<T>(value);
        } else {
            return std::nullopt;
        }
    }

    /// \brief Requests a value of a VariableAttribute specified by combination of \p component_id and \p variable_id
    /// from the device model
    /// \tparam T datatype of the value that is requested
    /// \param component_id
    /// \param variable_id
    /// \param attribute_enum
    /// \return Response to request that contains status of the request and the requested value as std::optional<T> .
    /// The value is present if the status is GetVariableStatusEnum::Accepted
    template <typename T>
    RequestDeviceModelResponse<T> request_value(const Component& component_id, const Variable& variable_id,
                                                const AttributeEnum& attribute_enum) {
        std::string value;
        const auto req_status = this->request_value_internal(component_id, variable_id, attribute_enum, value, false);

        if (req_status == GetVariableStatusEnum::Accepted) {
            return {GetVariableStatusEnum::Accepted, to_specific_type<T>(value)};
        } else {
            return {req_status};
        }
    }

    /// \brief Get the mutability for the given component, variable and attribute_enum
    /// \param component_id
    /// \param variable_id
    /// \param attribute_enum
    /// \return The mutability of the given component variable, else std::nullopt
    std::optional<MutabilityEnum> get_mutability(const Component& component_id, const Variable& variable,
                                                 const AttributeEnum& attribute_enum);

    /// \brief Sets the variable_id attribute \p value specified by \p component_id , \p variable_id and \p
    /// attribute_enum
    /// \param component_id
    /// \param variable_id
    /// \param attribute_enum
    /// \param value
    /// \param source           The source of the value (for example 'csms' or 'default').
    /// \param allow_read_only If this is true, read-only variables can be changed,
    ///                        otherwise only non read-only variables can be changed. Defaults to false
    /// \return Result of the requested operation
    SetVariableStatusEnum set_value(const Component& component_id, const Variable& variable_id,
                                    const AttributeEnum& attribute_enum, const std::string& value,
                                    const std::string& source, const bool allow_read_only = false);
    /// \brief Sets the variable_id attribute \p value specified by \p component_id , \p variable_id and \p
    /// attribute_enum for read only variables only. Only works on certain allowed components.
    /// \param component_id
    /// \param variable_id
    /// \param attribute_enum
    /// \param value
    /// \param source           The source of the value (for example 'csms' or 'default').
    /// \return Result of the requested operation
    SetVariableStatusEnum set_read_only_value(const Component& component_id, const Variable& variable_id,
                                              const AttributeEnum& attribute_enum, const std::string& value,
                                              const std::string& source);

    /// \brief Gets the VariableMetaData for the given \p component_id and \p variable_id
    /// \param component_id
    /// \param variable_id
    /// \return VariableMetaData or std::nullopt if \p component_id or \p variable_id not present
    std::optional<VariableMetaData> get_variable_meta_data(const Component& component_id, const Variable& variable_id);

    /// \brief Gets the ReportData for the specifed filter \p report_base \p component_variables and \p
    /// component_criteria
    /// \param report_base
    /// \param component_variables
    /// \param component_criteria
    /// \return
    std::vector<ReportData> get_base_report_data(const ReportBaseEnum& report_base);

    /// \brief Gets the ReportData for the specifed filter \p component_variables and \p
    /// component_criteria
    /// \param report_base
    /// \param component_variables
    /// \param component_criteria
    /// \return
    std::vector<ReportData>
    get_custom_report_data(const std::optional<std::vector<ComponentVariable>>& component_variables = std::nullopt,
                           const std::optional<std::vector<ComponentCriterionEnum>>& component_criteria = std::nullopt);

    void register_variable_listener(on_variable_changed&& listener) {
        variable_listener = std::move(listener);
    }

    void register_monitor_listener(on_monitor_updated&& listener) {
        monitor_update_listener = std::move(listener);
    }

    /// \brief Sets the given monitor \p requests in the device model
    /// \param request
    /// \param type The type of the set monitors. HardWiredMonitor - used for OEM specific monitors,
    /// PreconfiguredMonitor - monitors that were manually defined in the component config,
    /// CustomMonitor - used for monitors that are set by the CSMS,
    /// \return List of results of the requested operation
    std::vector<SetMonitoringResult> set_monitors(const std::vector<SetMonitoringData>& requests,
                                                  const VariableMonitorType type = VariableMonitorType::CustomMonitor);

    bool update_monitor_reference(int32_t monitor_id, const std::string& reference_value);

    std::vector<VariableMonitoringPeriodic> get_periodic_monitors();

    /// \brief Gets the Monitoring data for the request \p criteria and \p component_variables
    /// \param criteria
    /// \param component_variables
    /// \return List of results of the requested monitors
    std::vector<MonitoringData> get_monitors(const std::vector<MonitoringCriterionEnum>& criteria,
                                             const std::vector<ComponentVariable>& component_variables);

    /// \brief Clears the given \p request_ids from the registered monitors if request_id is present
    /// \param request_ids
    /// \param allow_protected if we should delete the non-custom monitors, defaults to false when
    /// this operation is requested by the CSMS
    /// \return List of results of the requested operation
    std::vector<ClearMonitoringResult> clear_monitors(const std::vector<int>& request_ids,
                                                      bool allow_protected = false);

    /// \brief Clears all the custom monitors (set by the CSMS) present in the database
    /// \return count of monitors deleted, can be 0 if no custom monitors were present in the db
    int32_t clear_custom_monitors();

    /// \brief Check data integrity of the device model provided by the device model data storage:
    /// For "required" variables, assert values exist. Checks might be extended in the future.
    void check_integrity(const std::map<int32_t, int32_t>& evse_connector_structure);
};

} // namespace v2
} // namespace ocpp

#endif // DEVICE_MODEL_HPP
