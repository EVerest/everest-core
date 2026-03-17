// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef DEVICE_MODEL_INTERFACE_HPP
#define DEVICE_MODEL_INTERFACE_HPP

#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include <everest/logging.hpp>
#include <ocpp/common/utils.hpp>
#include <ocpp/v2/enums.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Convert a raw string value from the device model to a typed value
/// \tparam T Target type (std::string, int, double, size_t, DateTime, bool, or uint64_t)
/// \param value Raw string value
/// \return Typed value
/// \throws std::invalid_argument if the value cannot be converted
template <typename T> T to_specific_type(const std::string& value) {
    static_assert(std::is_same_v<T, std::string> || std::is_same_v<T, int> || std::is_same_v<T, double> ||
                      std::is_same_v<T, size_t> || std::is_same_v<T, DateTime> || std::is_same_v<T, bool> ||
                      std::is_same_v<T, std::uint64_t>,
                  "Requested unknown datatype");

    if constexpr (std::is_same_v<T, std::string>) {
        return value;
    } else if constexpr (std::is_same_v<T, int>) {
        return std::stoi(value);
    } else if constexpr (std::is_same_v<T, double>) {
        return std::stod(value);
    } else if constexpr (std::is_same_v<T, std::size_t>) {
        const std::size_t res = std::stoul(value);
        return res;
    } else if constexpr (std::is_same_v<T, DateTime>) {
        return DateTime(value);
    } else if constexpr (std::is_same_v<T, bool>) {
        if (!is_boolean(value)) {
            throw std::invalid_argument("Invalid boolean value: " + value);
        }
        return ocpp::conversions::string_to_bool(value);
    } else if constexpr (std::is_same_v<T, std::uint64_t>) {
        return std::stoull(value);
    }
}

/// \brief Convert a raw string value to a typed value, selecting the type at compile time via DataEnum
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

/// \brief Returns whether the given DataEnum type is numeric
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

/// \brief Helper struct that holds values that don't have spec coverage
struct VariableMonitoringMeta {
    VariableMonitoring monitor;
    VariableMonitorType type;
    std::optional<std::string> reference_value;
};

/// \brief Helper struct that contains all monitors related to a variable that are of a periodic type
struct VariableMonitoringPeriodic {
    Component component;
    Variable variable;
    std::vector<VariableMonitoringMeta> monitors;
};

/// \brief Helper struct that combines VariableCharacteristics and VariableMonitoring
struct VariableMetaData {
    VariableCharacteristics characteristics;
    std::unordered_map<std::int64_t, VariableMonitoringMeta> monitors;
    std::optional<std::string> source;
};

using VariableMap = std::map<Variable, VariableMetaData>;
using DeviceModelMap = std::map<Component, VariableMap>;

class DeviceModelError : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override {
        return this->reason.c_str();
    }
    explicit DeviceModelError(std::string msg) : reason(std::move(msg)) {
    }
    explicit DeviceModelError(const char* msg) : reason(std::string(msg)) {
    }

private:
    std::string reason;
};

/// \brief Pure abstract interface for device model access.
class DeviceModelInterface {
public:
    virtual ~DeviceModelInterface() = default;

    // ============================================================================
    // Value getters
    // ============================================================================

    /// \brief Gets a variable attribute value
    /// \param component_id The component
    /// \param variable_id The variable
    /// \param attribute_enum The attribute
    /// \param value The value to get (as string)
    /// \param allow_write_only If true, write-only variables can be accessed
    /// \return Result of the requested operation
    virtual GetVariableStatusEnum get_variable(const Component& component_id, const Variable& variable_id,
                                               const AttributeEnum& attribute_enum, std::string& value,
                                               bool allow_write_only = false) const = 0;

    // ============================================================================
    // Value setters
    // ============================================================================

    /// \brief Sets a variable attribute value
    /// \param component_id The component
    /// \param variable_id The variable
    /// \param attribute_enum The attribute
    /// \param value The value to set (as string)
    /// \param source The source of the value (e.g., 'csms', 'default', 'internal')
    /// \param allow_read_only If true, read-only variables can be changed
    /// \return Result of the requested operation
    virtual SetVariableStatusEnum set_value(const Component& component_id, const Variable& variable_id,
                                            const AttributeEnum& attribute_enum, const std::string& value,
                                            const std::string& source, bool allow_read_only = false) = 0;

    /// \brief Sets a read-only variable attribute value (only works on certain allowed components)
    /// \param component_id The component
    /// \param variable_id The variable
    /// \param attribute_enum The attribute
    /// \param value The value to set (as string)
    /// \param source The source of the value (e.g., 'csms', 'default', 'internal')
    /// \return Result of the requested operation
    virtual SetVariableStatusEnum set_read_only_value(const Component& component_id, const Variable& variable_id,
                                                      const AttributeEnum& attribute_enum, const std::string& value,
                                                      const std::string& source) = 0;

    // ============================================================================
    // Metadata and monitoring
    // ============================================================================

    /// \brief Get the mutability for the given component, variable and attribute
    /// \param component_id The component
    /// \param variable_id The variable
    /// \param attribute_enum The attribute
    /// \return The mutability of the given component variable, or std::nullopt
    virtual std::optional<MutabilityEnum> get_mutability(const Component& component_id, const Variable& variable_id,
                                                         const AttributeEnum& attribute_enum) = 0;

    /// \brief Gets the VariableMetaData for the given component and variable
    /// \param component_id The component
    /// \param variable_id The variable
    /// \return VariableMetaData or std::nullopt if not present
    virtual std::optional<VariableMetaData> get_variable_meta_data(const Component& component_id,
                                                                   const Variable& variable_id) = 0;

    /// \brief Gets the ReportData for the specified report base
    /// \param report_base The report base filter
    /// \return Vector of ReportData
    virtual std::vector<ReportData> get_base_report_data(const ReportBaseEnum& report_base) = 0;

    /// \brief Gets the ReportData for the specified filters
    /// \param component_variables Optional component variables filter
    /// \param component_criteria Optional component criteria filter
    /// \return Vector of ReportData
    virtual std::vector<ReportData> get_custom_report_data(
        const std::optional<std::vector<ComponentVariable>>& component_variables = std::nullopt,
        const std::optional<std::vector<ComponentCriterionEnum>>& component_criteria = std::nullopt) = 0;

    // ============================================================================
    // Monitoring operations
    // ============================================================================

    /// \brief Sets the given monitor requests in the device model
    /// \param requests The monitoring requests
    /// \param type The type of monitors (HardWiredMonitor, PreconfiguredMonitor, CustomMonitor)
    /// \return List of results of the requested operation
    virtual std::vector<SetMonitoringResult>
    set_monitors(const std::vector<SetMonitoringData>& requests,
                 const VariableMonitorType type = VariableMonitorType::CustomMonitor) = 0;

    /// \brief Updates the reference value for a monitor
    /// \param monitor_id The monitor ID
    /// \param reference_value The new reference value
    /// \return true if successful, false otherwise
    virtual bool update_monitor_reference(std::int32_t monitor_id, const std::string& reference_value) = 0;

    /// \brief Gets all periodic monitors
    /// \return Vector of periodic monitors
    virtual std::vector<VariableMonitoringPeriodic> get_periodic_monitors() = 0;

    /// \brief Gets the monitoring data for the requested criteria and component variables
    /// \param criteria The monitoring criteria
    /// \param component_variables The component variables to get monitors for
    /// \return List of monitoring data
    virtual std::vector<MonitoringData> get_monitors(const std::vector<MonitoringCriterionEnum>& criteria,
                                                     const std::vector<ComponentVariable>& component_variables) = 0;

    /// \brief Clears the given monitor IDs from the registered monitors
    /// \param request_ids The monitor IDs to clear
    /// \param allow_protected If true, non-custom monitors can be deleted
    /// \return List of results of the requested operation
    virtual std::vector<ClearMonitoringResult> clear_monitors(const std::vector<int>& request_ids,
                                                              bool allow_protected = false) = 0;

    /// \brief Clears all custom monitors (set by the CSMS)
    /// \return Count of monitors deleted
    virtual std::int32_t clear_custom_monitors() = 0;

    /// \brief Registers a listener for variable changes
    /// \param listener The listener function to register
    virtual void register_variable_listener(
        std::function<void(const std::unordered_map<std::int64_t, VariableMonitoringMeta>& monitors,
                           const Component& component, const Variable& variable,
                           const VariableCharacteristics& characteristics, const VariableAttribute& attribute,
                           const std::string& value_previous, const std::string& value_current)>&& listener) = 0;

    /// \brief Registers a listener for monitor updates
    /// \param listener The listener function to register
    virtual void register_monitor_listener(
        std::function<void(const VariableMonitoringMeta& updated_monitor, const Component& component,
                           const Variable& variable, const VariableCharacteristics& characteristics,
                           const VariableAttribute& attribute, const std::string& current_value)>&& listener) = 0;

    // ============================================================================
    // Integrity check
    // ============================================================================

    /// \brief Check data integrity of the device model
    /// \param evse_connector_structure The EVSE connector structure
    virtual void check_integrity(const std::map<std::int32_t, std::int32_t>& evse_connector_structure) = 0;

    // ============================================================================
    // Typed value getters (implemented in terms of get_variable)
    // ============================================================================

    /// \brief Retrieves a variable attribute value as a typed value
    /// \tparam T Target type (std::string, int, double, size_t, DateTime, bool, or uint64_t)
    /// \param component_id The component
    /// \param variable_id The variable
    /// \param attribute_enum The attribute
    /// \return Typed value; throws std::runtime_error if the variable is not found
    template <typename T>
    T get_value(const Component& component_id, const Variable& variable_id,
                const AttributeEnum& attribute_enum = AttributeEnum::Actual) const {
        std::string value;
        const auto status = get_variable(component_id, variable_id, attribute_enum, value, true);
        if (status != GetVariableStatusEnum::Accepted) {
            EVLOG_critical << "Required value " << variable_id.name << " of component " << component_id.name
                           << " could not be retrieved";
            throw std::runtime_error("Required device model value not available: " + variable_id.name.get());
        }
        return to_specific_type<T>(value);
    }

    /// \brief Retrieves a variable attribute value as a typed value
    /// \tparam T Target type (std::string, int, double, size_t, DateTime, bool, or uint64_t)
    /// \param component_variable The component and variable
    /// \param attribute_enum The attribute
    /// \return Typed value; throws std::runtime_error if the variable is not found
    template <typename T>
    T get_value(const ComponentVariable& component_variable,
                const AttributeEnum& attribute_enum = AttributeEnum::Actual) const {
        if (!component_variable.variable.has_value()) {
            throw std::runtime_error("ComponentVariable has no variable set");
        }
        return get_value<T>(component_variable.component, component_variable.variable.value(), attribute_enum);
    }

    /// \brief Retrieves an optional variable attribute value as a typed value
    /// \tparam T Target type (std::string, int, double, size_t, DateTime, bool, or uint64_t)
    /// \param component_id The component
    /// \param variable_id The variable
    /// \param attribute_enum The attribute
    /// \return Typed value or std::nullopt if the variable is not found or cannot be converted
    template <typename T>
    std::optional<T> get_optional_value(const Component& component_id, const Variable& variable_id,
                                        const AttributeEnum& attribute_enum = AttributeEnum::Actual) const {
        std::string value;
        const auto status = get_variable(component_id, variable_id, attribute_enum, value, true);
        if (status != GetVariableStatusEnum::Accepted) {
            return std::nullopt;
        }
        try {
            return to_specific_type<T>(value);
        } catch (const std::exception&) {
            return std::nullopt;
        }
    }

    /// \brief Retrieves an optional variable attribute value as a typed value
    /// \tparam T Target type (std::string, int, double, size_t, DateTime, bool, or uint64_t)
    /// \param component_variable The component and variable
    /// \param attribute_enum The attribute
    /// \return Typed value or std::nullopt if the variable is not found or cannot be converted
    template <typename T>
    std::optional<T> get_optional_value(const ComponentVariable& component_variable,
                                        const AttributeEnum& attribute_enum = AttributeEnum::Actual) const {
        if (!component_variable.variable.has_value()) {
            return std::nullopt;
        }
        return get_optional_value<T>(component_variable.component, component_variable.variable.value(), attribute_enum);
    }
};

namespace NetworkConfigurationComponentVariables {

/// \brief Read a NetworkConnectionProfile from device model variables for the given slot.
/// Returns std::nullopt if OcppCsmsUrl is absent/empty (slot not configured).
std::optional<NetworkConnectionProfile> read_profile_from_device_model(DeviceModelInterface& dm, int32_t slot);

/// \brief Write a NetworkConnectionProfile to device model variables for the given slot.
/// Returns true on success.
bool write_profile_to_device_model(DeviceModelInterface& dm, int32_t slot, const NetworkConnectionProfile& profile,
                                   const std::string& source);

/// \brief One-time migration: if the NetworkConnectionProfiles blob is non-empty and no
/// NetworkConfiguration DM slot has a configured OcppCsmsUrl, import profiles from the blob.
/// Clears the blob after migration (or if DM is already populated) so this is idempotent.
void migrate_from_blob_if_needed(DeviceModelInterface& dm);

/// \brief Clear all NetworkConfiguration device model variables for the given slot.
/// After this call, read_profile_from_device_model returns std::nullopt for the slot.
void clear_slot_in_device_model(DeviceModelInterface& dm, int32_t slot);

} // namespace NetworkConfigurationComponentVariables

} // namespace v2
} // namespace ocpp

#endif // DEVICE_MODEL_INTERFACE_HPP
