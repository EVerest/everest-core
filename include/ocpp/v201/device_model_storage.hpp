// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_V201_DEVICE_MODEL_STORAGE_HPP
#define OCPP_V201_DEVICE_MODEL_STORAGE_HPP

#include <map>
#include <memory>
#include <ocpp/common/support_older_cpp_versions.hpp>
#include <ocpp/v201/comparators.hpp>
#include <optional>

#include <ocpp/v201/ocpp_types.hpp>

namespace ocpp {
namespace v201 {

/// \brief Helper struct that combines VariableCharacteristics and VariableMonitoring
struct VariableMetaData {
    VariableCharacteristics characteristics;
    std::vector<VariableMonitoring> monitors;
};

using VariableMap = std::map<Variable, VariableMetaData>;
using DeviceModelMap = std::map<Component, VariableMap>;

class DeviceModelStorageError : public std::exception {
public:
    [[nodiscard]] const char* what() const noexcept override {
        return this->reason.c_str();
    }
    explicit DeviceModelStorageError(std::string msg) {
        this->reason = std::move(msg);
    }
    explicit DeviceModelStorageError(const char* msg) {
        this->reason = std::string(msg);
    }

private:
    std::string reason;
};

/// \brief Abstract base class for device model storage. This class provides an interface for accessing and modifying
/// device model data. Implementations of this class should provide concrete implementations for the virtual methods
/// declared here.
class DeviceModelStorage {

public:
    virtual ~DeviceModelStorage() = default;

    /// \brief Gets the device model from the device model storage
    /// \return std::map<Component, std::map<Variable, VariableMetaData>> that will contain a full representation of the
    /// device model except for the VariableAttribute(s) of each Variable.
    virtual DeviceModelMap get_device_model() = 0;

    /// \brief Gets a VariableAttribute from the storage if present
    /// \param component_id
    /// \param variable_id
    /// \param attribute_enum
    /// \return VariableAttribute or std::nullopt if not present in the storage
    virtual std::optional<VariableAttribute> get_variable_attribute(const Component& component_id,
                                                                    const Variable& variable_id,
                                                                    const AttributeEnum& attribute_enum) = 0;
    /// \brief Gets a std::vector<VariableAttribute> from the storage.
    /// \param component_id
    /// \param variable_id
    /// \param attribute_enum optionally specifies an AttributeEnum. In this case this std::vector will contain max. one
    /// element
    /// \return std::vector<VariableAttribute> with maximum size of 4 elements (Actual, Target, MinSet, MaxSet) and
    /// minum size of 0 elements (if no VariableAttribute is present for the requested parameters)
    virtual std::vector<VariableAttribute>
    get_variable_attributes(const Component& component_id, const Variable& variable_id,
                            const std::optional<AttributeEnum>& attribute_enum = std::nullopt) = 0;

    /// \brief Sets the value of an VariableAttribute if present
    /// \param component_id
    /// \param variable_id
    /// \param attribute_enum
    /// \param value
    /// \return true if the value could be set in the storage, else false
    virtual bool set_variable_attribute_value(const Component& component_id, const Variable& variable_id,
                                              const AttributeEnum& attribute_enum, const std::string& value) = 0;

    /// \brief Check data integrity of the stored data:
    /// For "required" variables, assert values exist. Checks might be extended in the future.
    virtual void check_integrity() = 0;
};

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_DEVICE_MODEL_STORAGE_HPP
