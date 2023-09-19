
// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef DEVICE_MODEL_OCPP_HPP
#define DEVICE_MODEL_OCPP_HPP

#include <device_model/storage_sqlite.hpp>
#include <ocpp/v201/device_model_storage.hpp>

namespace module {

using VariableMap = std::map<ocpp::v201::Variable, ocpp::v201::VariableMetaData>;
using DeviceModelMap = std::map<ocpp::v201::Component, ocpp::v201::VariableMap>;

/// \brief DeviceModelStorage wrapper that inherits from the ocpp::v201::DeviceModelStorage. It converts to and from
/// ocpp::v201 to Everest types and uses the sqlite storage implementation of libdevicemodel to actually access the
/// device model database.
class DeviceModelStorageOcpp : public ocpp::v201::DeviceModelStorage {

private:
    std::unique_ptr<Everest::DeviceModelStorageSqlite> device_model_storage;

public:
    DeviceModelStorageOcpp(const std::string& device_model_database_path);

    DeviceModelMap get_device_model() override;

    std::optional<ocpp::v201::VariableAttribute>
    get_variable_attribute(const ocpp::v201::Component& component_id, const ocpp::v201::Variable& variable_id,
                           const ocpp::v201::AttributeEnum& attribute_enum) override;
    std::vector<ocpp::v201::VariableAttribute>
    get_variable_attributes(const ocpp::v201::Component& component_id, const ocpp::v201::Variable& variable_id,
                            const std::optional<ocpp::v201::AttributeEnum>& attribute_enum = std::nullopt) override;

    bool set_variable_attribute_value(const ocpp::v201::Component& component_id,
                                      const ocpp::v201::Variable& variable_id,
                                      const ocpp::v201::AttributeEnum& attribute_enum,
                                      const std::string& value) override;
};

namespace conversions {

// enums
Everest::DataEnum from_ocpp(ocpp::v201::DataEnum other);
ocpp::v201::DataEnum to_ocpp(Everest::DataEnum other);
Everest::MonitorEnum from_ocpp(ocpp::v201::MonitorEnum other);
ocpp::v201::MonitorEnum to_ocpp(Everest::MonitorEnum other);
Everest::AttributeEnum from_ocpp(ocpp::v201::AttributeEnum other);
ocpp::v201::AttributeEnum to_ocpp(Everest::AttributeEnum other);
Everest::MutabilityEnum from_ocpp(ocpp::v201::MutabilityEnum other);
ocpp::v201::MutabilityEnum to_ocpp(Everest::MutabilityEnum other);

// structs
Everest::VariableCharacteristics from_ocpp(ocpp::v201::VariableCharacteristics other);
ocpp::v201::VariableCharacteristics to_ocpp(Everest::VariableCharacteristics other);
Everest::VariableMonitoring from_ocpp(ocpp::v201::VariableMonitoring other);
ocpp::v201::VariableMonitoring to_ocpp(Everest::VariableMonitoring other);
Everest::EVSE from_ocpp(ocpp::v201::EVSE other);
ocpp::v201::EVSE to_ocpp(Everest::EVSE other);
Everest::Component from_ocpp(ocpp::v201::Component other);
ocpp::v201::Component to_ocpp(Everest::Component other);
Everest::VariableMetaData from_ocpp(ocpp::v201::VariableMetaData other);
ocpp::v201::VariableMetaData to_ocpp(Everest::VariableMetaData other);
Everest::Variable from_ocpp(ocpp::v201::Variable other);
ocpp::v201::Variable to_ocpp(Everest::Variable other);
Everest::ComponentVariable from_ocpp(ocpp::v201::ComponentVariable other);
ocpp::v201::ComponentVariable to_ocpp(Everest::ComponentVariable other);
Everest::VariableAttribute from_ocpp(ocpp::v201::VariableAttribute other);
ocpp::v201::VariableAttribute to_ocpp(Everest::VariableAttribute other);

} // namespace conversions

} // namespace module

#endif
