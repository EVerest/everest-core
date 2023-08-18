// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <device_model_ocpp.hpp>

namespace module {

// constructor der device_model_sqlite initialisiert
DeviceModelStorageOcpp::DeviceModelStorageOcpp(const std::string& device_model_database_path) {
    this->device_model_storage = std::make_unique<Everest::DeviceModelStorageSqlite>(device_model_database_path);
}

DeviceModelMap DeviceModelStorageOcpp::get_device_model() {
    const auto everest_device_model_map = this->device_model_storage->get_device_model();
    DeviceModelMap ocpp_device_model_map;
    for (const auto& [everest_component, everest_variable_map] : everest_device_model_map) {
        VariableMap ocpp_variable_map;
        ocpp::v201::Component ocpp_component = conversions::to_ocpp(everest_component);
        for (const auto& [everest_variable, everest_variable_meta_data] : everest_variable_map) {
            ocpp::v201::Variable ocpp_variable = conversions::to_ocpp(everest_variable);
            ocpp_variable_map[ocpp_variable] = conversions::to_ocpp(everest_variable_meta_data);
        }
        ocpp_device_model_map[ocpp_component] = ocpp_variable_map;
    }
    return ocpp_device_model_map;
}

std::optional<ocpp::v201::VariableAttribute>
DeviceModelStorageOcpp::get_variable_attribute(const ocpp::v201::Component& component_id,
                                               const ocpp::v201::Variable& variable_id,
                                               const ocpp::v201::AttributeEnum& attribute_enum) {
    Everest::Component component = conversions::from_ocpp(component_id);
    Everest::Variable variable = conversions::from_ocpp(variable_id);
    Everest::AttributeEnum _attribute_enum = conversions::from_ocpp(attribute_enum);
    const auto variable_attribute =
        this->device_model_storage->get_variable_attribute(component, variable, _attribute_enum);
    if (!variable_attribute.has_value()) {
        return std::nullopt;
    }
    return conversions::to_ocpp(variable_attribute.value());
}
std::vector<ocpp::v201::VariableAttribute>
DeviceModelStorageOcpp::get_variable_attributes(const ocpp::v201::Component& component_id,
                                                const ocpp::v201::Variable& variable_id,
                                                const std::optional<ocpp::v201::AttributeEnum>& attribute_enum) {
    std::vector<ocpp::v201::VariableAttribute> result;
    Everest::Component component = conversions::from_ocpp(component_id);
    Everest::Variable variable = conversions::from_ocpp(variable_id);
    std::optional<Everest::AttributeEnum> _attribute_enum = std::nullopt;
    if (attribute_enum.has_value()) {
        _attribute_enum = conversions::from_ocpp(attribute_enum.value());
    }
    const auto v = this->device_model_storage->get_variable_attributes(component, variable, _attribute_enum);
    for (const auto& e : v) {
        result.push_back(conversions::to_ocpp(e));
    }
    return result;
}

bool DeviceModelStorageOcpp::set_variable_attribute_value(const ocpp::v201::Component& component_id,
                                                          const ocpp::v201::Variable& variable_id,
                                                          const ocpp::v201::AttributeEnum& attribute_enum,
                                                          const std::string& value) {
    Everest::Component component = conversions::from_ocpp(component_id);
    Everest::Variable variable = conversions::from_ocpp(variable_id);
    Everest::AttributeEnum _attribute_enum = conversions::from_ocpp(attribute_enum);
    return this->device_model_storage->set_variable_attribute_value(component, variable, _attribute_enum, value);
}

namespace conversions {

Everest::DataEnum from_ocpp(ocpp::v201::DataEnum other) {
    switch (other) {
    case ocpp::v201::DataEnum::string:
        return Everest::DataEnum::string;
    case ocpp::v201::DataEnum::decimal:
        return Everest::DataEnum::decimal;
    case ocpp::v201::DataEnum::integer:
        return Everest::DataEnum::integer;
    case ocpp::v201::DataEnum::dateTime:
        return Everest::DataEnum::dateTime;
    case ocpp::v201::DataEnum::boolean:
        return Everest::DataEnum::boolean;
    case ocpp::v201::DataEnum::OptionList:
        return Everest::DataEnum::OptionList;
    case ocpp::v201::DataEnum::SequenceList:
        return Everest::DataEnum::SequenceList;
    case ocpp::v201::DataEnum::MemberList:
        return Everest::DataEnum::MemberList;
    }
    throw std::runtime_error("Could not convert ocpp::v201::DataEnum to DataEnum");
}

ocpp::v201::DataEnum to_ocpp(Everest::DataEnum other) {
    switch (other) {
    case Everest::DataEnum::string:
        return ocpp::v201::DataEnum::string;
    case Everest::DataEnum::decimal:
        return ocpp::v201::DataEnum::decimal;
    case Everest::DataEnum::integer:
        return ocpp::v201::DataEnum::integer;
    case Everest::DataEnum::dateTime:
        return ocpp::v201::DataEnum::dateTime;
    case Everest::DataEnum::boolean:
        return ocpp::v201::DataEnum::boolean;
    case Everest::DataEnum::OptionList:
        return ocpp::v201::DataEnum::OptionList;
    case Everest::DataEnum::SequenceList:
        return ocpp::v201::DataEnum::SequenceList;
    case Everest::DataEnum::MemberList:
        return ocpp::v201::DataEnum::MemberList;
    }
    throw std::runtime_error("Could not convert DataEnum to ocpp::v201::DataEnum");
}

Everest::MonitorEnum from_ocpp(ocpp::v201::MonitorEnum other) {
    switch (other) {
    case ocpp::v201::MonitorEnum::UpperThreshold:
        return Everest::MonitorEnum::UpperThreshold;
    case ocpp::v201::MonitorEnum::LowerThreshold:
        return Everest::MonitorEnum::LowerThreshold;
    case ocpp::v201::MonitorEnum::Delta:
        return Everest::MonitorEnum::Delta;
    case ocpp::v201::MonitorEnum::Periodic:
        return Everest::MonitorEnum::Periodic;
    case ocpp::v201::MonitorEnum::PeriodicClockAligned:
        return Everest::MonitorEnum::PeriodicClockAligned;
    }
    throw std::runtime_error("Could not convert ocpp::v201::MonitorEnum to MonitorEnum");
}

ocpp::v201::MonitorEnum to_ocpp(Everest::MonitorEnum other) {
    switch (other) {
    case Everest::MonitorEnum::UpperThreshold:
        return ocpp::v201::MonitorEnum::UpperThreshold;
    case Everest::MonitorEnum::LowerThreshold:
        return ocpp::v201::MonitorEnum::LowerThreshold;
    case Everest::MonitorEnum::Delta:
        return ocpp::v201::MonitorEnum::Delta;
    case Everest::MonitorEnum::Periodic:
        return ocpp::v201::MonitorEnum::Periodic;
    case Everest::MonitorEnum::PeriodicClockAligned:
        return ocpp::v201::MonitorEnum::PeriodicClockAligned;
    }
    throw std::runtime_error("Could not convert MonitorEnum to ocpp::v201::MonitorEnum");
}

Everest::AttributeEnum from_ocpp(ocpp::v201::AttributeEnum other) {
    switch (other) {
    case ocpp::v201::AttributeEnum::Actual:
        return Everest::AttributeEnum::Actual;
    case ocpp::v201::AttributeEnum::Target:
        return Everest::AttributeEnum::Target;
    case ocpp::v201::AttributeEnum::MinSet:
        return Everest::AttributeEnum::MinSet;
    case ocpp::v201::AttributeEnum::MaxSet:
        return Everest::AttributeEnum::MaxSet;
    }
    throw std::runtime_error("Could not convert ocpp::v201::AttributeEnum to AttributeEnum");
}

ocpp::v201::AttributeEnum to_ocpp(Everest::AttributeEnum other) {
    switch (other) {
    case Everest::AttributeEnum::Actual:
        return ocpp::v201::AttributeEnum::Actual;
    case Everest::AttributeEnum::Target:
        return ocpp::v201::AttributeEnum::Target;
    case Everest::AttributeEnum::MinSet:
        return ocpp::v201::AttributeEnum::MinSet;
    case Everest::AttributeEnum::MaxSet:
        return ocpp::v201::AttributeEnum::MaxSet;
    }
    throw std::runtime_error("Could not convert AttributeEnum to ocpp::v201::AttributeEnum");
}

Everest::MutabilityEnum from_ocpp(ocpp::v201::MutabilityEnum other) {
    switch (other) {
    case ocpp::v201::MutabilityEnum::ReadOnly:
        return Everest::MutabilityEnum::ReadOnly;
    case ocpp::v201::MutabilityEnum::WriteOnly:
        return Everest::MutabilityEnum::WriteOnly;
    case ocpp::v201::MutabilityEnum::ReadWrite:
        return Everest::MutabilityEnum::ReadWrite;
    }
    throw std::runtime_error("Could not convert ocpp::v201::MutabilityEnum to MutabilityEnum");
}

ocpp::v201::MutabilityEnum to_ocpp(Everest::MutabilityEnum other) {
    switch (other) {
    case Everest::MutabilityEnum::ReadOnly:
        return ocpp::v201::MutabilityEnum::ReadOnly;
    case Everest::MutabilityEnum::WriteOnly:
        return ocpp::v201::MutabilityEnum::WriteOnly;
    case Everest::MutabilityEnum::ReadWrite:
        return ocpp::v201::MutabilityEnum::ReadWrite;
    }
    throw std::runtime_error("Could not convert MutabilityEnum to ocpp::v201::MutabilityEnum");
}

Everest::VariableCharacteristics from_ocpp(ocpp::v201::VariableCharacteristics other) {
    Everest::VariableCharacteristics lhs;
    lhs.data_type = from_ocpp(other.dataType);
    lhs.supports_monitoring = other.supportsMonitoring;
    lhs.min_limit = other.minLimit;
    lhs.max_limit = other.maxLimit;
    if (other.unit.has_value()) {
        lhs.unit = other.unit.value();
    }
    if (other.valuesList.has_value()) {
        lhs.values_list = other.valuesList.value();
    }
    return lhs;
}

ocpp::v201::VariableCharacteristics to_ocpp(Everest::VariableCharacteristics other) {
    ocpp::v201::VariableCharacteristics lhs;
    lhs.dataType = to_ocpp(other.data_type);
    lhs.supportsMonitoring = other.supports_monitoring;
    lhs.minLimit = other.min_limit;
    lhs.maxLimit = other.max_limit;
    if (other.unit.has_value()) {
        lhs.unit = other.unit.value();
    }
    if (other.values_list.has_value()) {
        lhs.valuesList = other.values_list.value();
    }
    return lhs;
}

Everest::VariableMonitoring from_ocpp(ocpp::v201::VariableMonitoring other) {
    Everest::VariableMonitoring lhs;
    lhs.id = other.id;
    lhs.transaction = other.transaction;
    lhs.value = other.value;
    lhs.type = from_ocpp(other.type);
    lhs.severity = other.severity;
    return lhs;
}

ocpp::v201::VariableMonitoring to_ocpp(Everest::VariableMonitoring other) {
    ocpp::v201::VariableMonitoring lhs;
    lhs.id = other.id;
    lhs.transaction = other.transaction;
    lhs.value = other.value;
    lhs.type = to_ocpp(other.type);
    lhs.severity = other.severity;
    return lhs;
}

Everest::EVSE from_ocpp(ocpp::v201::EVSE other) {
    Everest::EVSE lhs;
    lhs.id = other.id;
    if (other.connectorId.has_value()) {
        lhs.connector_id = other.connectorId.value();
    }
    return lhs;
}

ocpp::v201::EVSE to_ocpp(Everest::EVSE other) {
    ocpp::v201::EVSE lhs;
    lhs.id = other.id;
    if (other.connector_id.has_value()) {
        lhs.connectorId = other.connector_id.value();
    }
    return lhs;
}

Everest::Component from_ocpp(ocpp::v201::Component other) {
    Everest::Component lhs;
    lhs.name = other.name;
    if (other.evse.has_value()) {
        lhs.evse = from_ocpp(other.evse.value());
    }
    if (other.instance.has_value()) {
        lhs.instance = other.instance.value();
    }
    return lhs;
}

ocpp::v201::Component to_ocpp(Everest::Component other) {
    ocpp::v201::Component lhs;
    lhs.name = other.name;
    if (other.evse.has_value()) {
        lhs.evse = to_ocpp(other.evse.value());
    }
    if (other.instance.has_value()) {
        lhs.instance = other.instance.value();
    }
    return lhs;
}

Everest::VariableMetaData from_ocpp(ocpp::v201::VariableMetaData other) {
    Everest::VariableMetaData lhs;
    lhs.characteristics = from_ocpp(other.characteristics);
    for (const auto& e : other.monitors) {
        lhs.monitors.push_back(from_ocpp(e));
    }
    return lhs;
}

ocpp::v201::VariableMetaData to_ocpp(Everest::VariableMetaData other) {
    ocpp::v201::VariableMetaData lhs;
    lhs.characteristics = to_ocpp(other.characteristics);
    for (const auto& e : other.monitors) {
        lhs.monitors.push_back(to_ocpp(e));
    }
    return lhs;
}

Everest::Variable from_ocpp(ocpp::v201::Variable other) {
    Everest::Variable lhs;
    lhs.name = other.name;
    if (other.instance.has_value()) {
        lhs.instance = other.instance.value();
    }
    return lhs;
}

ocpp::v201::Variable to_ocpp(Everest::Variable other) {
    ocpp::v201::Variable lhs;
    lhs.name = other.name;
    if (other.instance.has_value()) {
        lhs.instance = other.instance.value();
    }
    return lhs;
}

Everest::ComponentVariable from_ocpp(ocpp::v201::ComponentVariable other) {
    Everest::ComponentVariable lhs;
    lhs.component = from_ocpp(other.component);
    if (other.variable.has_value()) {
        lhs.variable = from_ocpp(other.variable.value());
    }
    return lhs;
}

ocpp::v201::ComponentVariable to_ocpp(Everest::ComponentVariable other) {
    ocpp::v201::ComponentVariable lhs;
    lhs.component = to_ocpp(other.component);
    if (other.variable.has_value()) {
        lhs.variable = to_ocpp(other.variable.value());
    }
    return lhs;
}

Everest::VariableAttribute from_ocpp(ocpp::v201::VariableAttribute other) {
    Everest::VariableAttribute lhs;
    if (other.type.has_value()) {
        lhs.type = from_ocpp(other.type.value());
    }
    if (other.value.has_value()) {
        lhs.value = other.value.value();
    }
    if (other.mutability.has_value()) {
        lhs.mutability = from_ocpp(other.mutability.value());
    }
    if (other.persistent.has_value()) {
        lhs.persistent = other.persistent.value();
    }
    if (other.constant.has_value()) {
        lhs.constant = other.constant.value();
    }
    return lhs;
}

ocpp::v201::VariableAttribute to_ocpp(Everest::VariableAttribute other) {
    ocpp::v201::VariableAttribute lhs;
    if (other.type.has_value()) {
        lhs.type = to_ocpp(other.type.value());
    }
    if (other.value.has_value()) {
        lhs.value = other.value.value();
    }
    if (other.mutability.has_value()) {
        lhs.mutability = to_ocpp(other.mutability.value());
    }
    if (other.persistent.has_value()) {
        lhs.persistent = other.persistent.value();
    }
    if (other.constant.has_value()) {
        lhs.constant = other.constant.value();
    }
    return lhs;
}

} // namespace conversions

} // namespace module