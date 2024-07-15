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

/// \brief Helper struct that holds database only values that don't have spec coverage
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
    std::unordered_map<int64_t, VariableMonitoringMeta> monitors;
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
    /// \param source           The source of the value.
    /// \return true if the value could be set in the storage, else false
    virtual bool set_variable_attribute_value(const Component& component_id, const Variable& variable_id,
                                              const AttributeEnum& attribute_enum, const std::string& value,
                                              const std::string& source) = 0;

    /// \brief Inserts or replaces a variable monitor in the database
    /// \param data Monitor data to set
    /// \return true if the value could be inserted, or valse otherwise
    virtual std::optional<VariableMonitoringMeta> set_monitoring_data(const SetMonitoringData& data,
                                                                      const VariableMonitorType type) = 0;

    /// \brief Updates the reference value for a monitor. The reference values is used for the
    /// delta monitors to detect a trigger, and must be updated when a trigger is detected
    /// \param monitor_id Id of the monitor that requires the reference changed
    /// \param reference_value Value to replace the reference with
    /// \return true if the reference value could be updated, false otherwise
    virtual bool update_monitoring_reference(const int32_t monitor_id, const std::string& reference_value) = 0;

    /// \brief Returns all the monitors currently in the database based
    /// on the provided filtering criteria
    /// \param criteria
    /// \param component_id
    /// \param variable_id
    /// \return the monitoring data if it could be found or an empty vector
    virtual std::vector<VariableMonitoringMeta>
    get_monitoring_data(const std::vector<MonitoringCriterionEnum>& criteria, const Component& component_id,
                        const Variable& variable_id) = 0;

    /// \brief Clears a single monitor based on the ID from the database
    /// \param monitor_id Monitor ID
    /// \param allow_protected If we are allowed to delete non-custom monitors
    /// \return if not Accepted, NotFound if the monitor could not be
    /// found, or Rejected if it is a protected monitor
    virtual ClearMonitoringStatusEnum clear_variable_monitor(int monitor_id, bool allow_protected) = 0;

    /// \brief Clears all custom monitors (that were added by the CSMS)
    /// from the database
    /// \return count of monitors deleted, or 0 if none were deleted
    virtual int32_t clear_custom_variable_monitors() = 0;

    /// \brief Check data integrity of the stored data:
    /// For "required" variables, assert values exist. Checks might be extended in the future.
    virtual void check_integrity() = 0;
};

} // namespace v201
} // namespace ocpp

#endif // OCPP_V201_DEVICE_MODEL_STORAGE_HPP
