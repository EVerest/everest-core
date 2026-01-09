// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <everest/logging.hpp>

#include <ocpp/common/utils.hpp>
#include <ocpp/v2/comparators.hpp>
#include <ocpp/v2/device_model_interface.hpp>
#include <ocpp/v2/ocpp_types.hpp>

namespace ocpp {
namespace v2 {

/// \brief Response to requesting a value from the device model
/// \tparam T
template <typename T> struct RequestDeviceModelResponse {
    GetVariableStatusEnum status;
    std::optional<T> value;
};

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

/// \brief Base class for device model implementations, provides templated getters for variable attribute values
class DeviceModelBase {

public:
    // ============================================================================
    // Value getters
    // ============================================================================

    /// \brief Retrieves a variable attribute value
    /// \tparam T The target type (must be: std::string, int, double, size_t, DateTime, bool, or uint64_t)
    /// \param component_id The component
    /// \param variable_id The variable
    /// \param attribute_enum The attribute
    /// \return The requested value
    template <typename T>
    T get_value(const Component& component_id, const Variable& variable_id,
                const AttributeEnum& attribute_enum = AttributeEnum::Actual) const {
        std::string value;
        const auto status = request_value_internal(component_id, variable_id, attribute_enum, value, true);

        if (status != GetVariableStatusEnum::Accepted) {
            EVLOG_critical << "Required value " << variable_id.name << " of component " << component_id.name
                           << " could not be retrieved";
            EVLOG_AND_THROW(std::runtime_error("Required value not available"));
        }

        return to_specific_type<T>(value);
    }

    /// \brief Retrieves a variable attribute value
    /// \tparam T The target type (must be: std::string, int, double, size_t, DateTime, bool, or uint64_t)
    /// \param component_variable The component and variable
    /// \param attribute_enum The attribute
    /// \return The requested value
    template <typename T>
    T get_value(const ComponentVariable& component_variable,
                const AttributeEnum& attribute_enum = AttributeEnum::Actual) const {
        if (!component_variable.variable.has_value()) {
            EVLOG_critical << "ComponentVariable has no variable set for component: " << component_variable.component;
            EVLOG_AND_THROW(std::runtime_error("ComponentVariable has no variable set"));
        }
        return get_value<T>(component_variable.component, component_variable.variable.value(), attribute_enum);
    }

    /// \brief Retrieves an optional variable attribute value
    /// \tparam T The target type (must be: std::string, int, double, size_t, DateTime, bool, or uint64_t)
    /// \param component_id The component
    /// \param variable_id The variable
    /// \param attribute_enum The attribute
    /// \return The requested value or std::nullopt
    template <typename T>
    std::optional<T> get_optional_value(const Component& component_id, const Variable& variable_id,
                                        const AttributeEnum& attribute_enum = AttributeEnum::Actual) const {
        std::string value;
        const auto status = request_value_internal(component_id, variable_id, attribute_enum, value, true);

        if (status != GetVariableStatusEnum::Accepted) {
            return std::nullopt;
        }

        try {
            return to_specific_type<T>(value);
        } catch (const std::exception&) {
            return std::nullopt;
        }
    }

    /// \brief Retrieves an optional variable attribute value
    /// \tparam T The target type (must be: std::string, int, double, size_t, DateTime, bool, or uint64_t)
    /// \param component_variable The component and variable
    /// \param attribute_enum The attribute
    /// \return The requested value or std::nullopt
    template <typename T>
    std::optional<T> get_optional_value(const ComponentVariable& component_variable,
                                        const AttributeEnum& attribute_enum = AttributeEnum::Actual) const {
        if (!component_variable.variable.has_value()) {
            return std::nullopt;
        }
        return get_optional_value<T>(component_variable.component, component_variable.variable.value(), attribute_enum);
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
                                                const AttributeEnum& attribute_enum) const {
        std::string value;
        const auto status = request_value_internal(component_id, variable_id, attribute_enum, value, false);

        if (status != GetVariableStatusEnum::Accepted) {
            return {status, std::nullopt};
        }

        try {
            return {status, to_specific_type<T>(value)};
        } catch (const std::exception&) {
            return {GetVariableStatusEnum::Rejected, std::nullopt};
        }
    }

private:
    virtual GetVariableStatusEnum request_value_internal(const Component& component_id, const Variable& variable_id,
                                                         const AttributeEnum& attribute_enum, std::string& value,
                                                         bool allow_write_only = false) const = 0;
};

} // namespace v2
} // namespace ocpp
