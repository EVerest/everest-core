// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <stdexcept>
#include <string>
#include <type_traits>

#include <everest/logging.hpp>

#include <ocpp/common/utils.hpp>
#include <ocpp/v2/device_model_interface.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

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

/// \brief Gets a variable attribute value from the device model
/// \tparam T The target type (must be: std::string, int, double, size_t, DateTime, bool, or uint64_t)
/// \param dm The device model interface
/// \param component_id The component
/// \param variable_id The variable
/// \param attribute_enum The attribute
/// \return The requested value
/// \throws std::runtime_error if the value cannot be retrieved
template <typename T>
T get_value(const DeviceModelInterface& dm, const Component& component_id, const Variable& variable_id,
            const AttributeEnum& attribute_enum = AttributeEnum::Actual) {
    std::string value;
    const auto status = dm.get_variable(component_id, variable_id, attribute_enum, value, true);
    if (status != GetVariableStatusEnum::Accepted) {
        throw std::runtime_error("Required value not available");
    }
    return to_specific_type<T>(value);
}

/// \brief Gets a variable attribute value from the device model
/// \tparam T The target type (must be: std::string, int, double, size_t, DateTime, bool, or uint64_t)
/// \param dm The device model interface
/// \param component_variable The component and variable
/// \param attribute_enum The attribute
/// \return The requested value
/// \throws std::runtime_error if the value cannot be retrieved
template <typename T>
T get_value(const DeviceModelInterface& dm, const ComponentVariable& component_variable,
            const AttributeEnum& attribute_enum = AttributeEnum::Actual) {
    if (!component_variable.variable.has_value()) {
        throw std::runtime_error("ComponentVariable has no variable set");
    }
    return get_value<T>(dm, component_variable.component, component_variable.variable.value(), attribute_enum);
}

/// \brief Gets an optional variable attribute value from the device model
/// \tparam T The target type (must be: std::string, int, double, size_t, DateTime, bool, or uint64_t)
/// \param dm The device model interface
/// \param component_id The component
/// \param variable_id The variable
/// \param attribute_enum The attribute
/// \return The requested value or std::nullopt
template <typename T>
std::optional<T> get_optional_value(const DeviceModelInterface& dm, const Component& component_id,
                                    const Variable& variable_id,
                                    const AttributeEnum& attribute_enum = AttributeEnum::Actual) {
    std::string value;
    const auto status = dm.get_variable(component_id, variable_id, attribute_enum, value, true);
    if (status != GetVariableStatusEnum::Accepted) {
        return std::nullopt;
    }
    try {
        return to_specific_type<T>(value);
    } catch (const std::exception& e) {
        // Conversion failed: the value is present in the DM but cannot be represented as T
        // (malformed integer, malformed bool, out-of-range, etc.). Log at warning level so a
        // caller seeing std::nullopt can distinguish "absent" from "present-but-unparseable".
        EVLOG_warning << "Failed to convert device model value '" << value << "' to requested type for component '"
                      << component_id.name.get() << "', variable '" << variable_id.name.get() << "': " << e.what();
        return std::nullopt;
    }
}

/// \brief Gets an optional variable attribute value from the device model
/// \tparam T The target type (must be: std::string, int, double, size_t, DateTime, bool, or uint64_t)
/// \param dm The device model interface
/// \param component_variable The component and variable
/// \param attribute_enum The attribute
/// \return The requested value or std::nullopt
template <typename T>
std::optional<T> get_optional_value(const DeviceModelInterface& dm, const ComponentVariable& component_variable,
                                    const AttributeEnum& attribute_enum = AttributeEnum::Actual) {
    if (!component_variable.variable.has_value()) {
        return std::nullopt;
    }
    return get_optional_value<T>(dm, component_variable.component, component_variable.variable.value(), attribute_enum);
}

} // namespace v2
} // namespace ocpp
