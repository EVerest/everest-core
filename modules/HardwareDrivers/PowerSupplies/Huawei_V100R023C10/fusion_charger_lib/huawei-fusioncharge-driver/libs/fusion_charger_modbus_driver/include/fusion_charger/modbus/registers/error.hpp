// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#pragma once

#include <functional>
#include <iomanip>
#include <memory>
#include <ostream>
#include <sstream>

#include "modbus-registers/data_provider.hpp"
#include "raw.hpp"

namespace fusion_charger::modbus_driver {

using namespace fusion_charger::modbus_driver::raw_registers;

enum class ErrorCategory : uint16_t {
    PowerUnit = 0,
    ChargingPowerUnit = 1,
    AcBranch = 2,
    AcDcRectifier = 3,
    DcDcChargingModule = 4,
    CoolingSection = 5,
    ErrorSubcategoryPowerDistributionModule = 6,
};

inline std::ostream& operator<<(std::ostream& os, const ErrorCategory& category) {
    switch (category) {
    case ErrorCategory::PowerUnit:
        os << "PowerUnit";
        break;
    case ErrorCategory::ChargingPowerUnit:
        os << "ChargingPowerUnit";
        break;
    case ErrorCategory::AcBranch:
        os << "AcBranch";
        break;
    case ErrorCategory::AcDcRectifier:
        os << "AcDcRectifier";
        break;
    case ErrorCategory::DcDcChargingModule:
        os << "DcDcChargingModule";
        break;
    case ErrorCategory::CoolingSection:
        os << "CoolingSection";
        break;
    case ErrorCategory::ErrorSubcategoryPowerDistributionModule:
        os << "ErrorSubcategoryPowerDistributionModule";
        break;
    }
    return os;
}

enum class ErrorSubcategoryPowerUnit : uint16_t {
    HighVoltageDoorStatusSensor = 0,
    DoorStatusSensor = 1,
    Water = 2,
    Smoke = 3,
    Epo = 4,
};

enum class ErrorSubcategoryChargingPowerUnit : uint16_t {
    UnknownSystemType = 0,
    PowerDetectionException = 1,
    SyncrhonizationCableStatusFaultOfEnergyRoutingBoard = 2,
    SoftStartFault = 3,
    SoftStartModuleCommunicationFailure = 4,
    SoftStartModuleOverloaded = 5,
    SoftStartModuleFault = 6,
    SoftStartModuleOvertemperature = 7,
    SoftStartModuleUndertemperature = 8,
    SoftStartModuleDisconnectionFailure = 9,
    PhaseSequenceAbornmalAlarm = 10,
    PowerDistributionModuleCommunicationFailure = 11,
    FaultOfInsulationResistanceToGround = 12,
    ModbusTcpCertificate = 13
};

enum class ErrorSubcategoryAcBranch : uint16_t {
    AcBranch1 = 0,
    AcBranch2 = 1,
};

enum class ErrorSubcategoryAcDcRectifier : uint16_t {
    rectifier_1 = 0,
    rectifier_2 = 1,
    rectifier_3 = 2,
    rectifier_4 = 3,
    rectifier_5 = 4,
    rectifier_6 = 5,
};
enum class ErrorSubcategoryDcDcChargingModule : uint16_t {
    DcDcModule1 = 0,
    DcDcModule2 = 1,
    DcDcModule3 = 2,
    DcDcModule4 = 3,
    DcDcModule5 = 4,
    DcDcModule6 = 5,
    DcDcModule7 = 6,
    DcDcModule8 = 7,
    DcDcModule9 = 8,
    DcDcModule10 = 9,
    DcDcModule11 = 10,
    DcDcModule12 = 11,
};

enum class ErrorSubcategoryCoolingSection : uint16_t {
    CoolingUnit1 = 0,
};

enum class ErrorSubcategoryPowerDistributionModule : uint16_t {
    PowerDistributionModule1 = 0,
    PowerDistributionModule2 = 1,
    PowerDistributionModule3 = 2,
    PowerDistributionModule4 = 3,
    PowerDistributionModule5 = 4,
};

inline std::ostream& operator<<(std::ostream& os, const ErrorSubcategoryPowerUnit& subcategory) {
    switch (subcategory) {
    case ErrorSubcategoryPowerUnit::HighVoltageDoorStatusSensor:
        os << "HighVoltageDoorStatusSensor";
        break;
    case ErrorSubcategoryPowerUnit::DoorStatusSensor:
        os << "DoorStatusSensor";
        break;
    case ErrorSubcategoryPowerUnit::Water:
        os << "Water";
        break;
    case ErrorSubcategoryPowerUnit::Smoke:
        os << "Smoke";
        break;
    case ErrorSubcategoryPowerUnit::Epo:
        os << "Epo";
        break;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const ErrorSubcategoryChargingPowerUnit& subcategory) {
    switch (subcategory) {
    case ErrorSubcategoryChargingPowerUnit::UnknownSystemType:
        os << "UnknownSystemType";
        break;
    case ErrorSubcategoryChargingPowerUnit::PowerDetectionException:
        os << "PowerDetectionException";
        break;
    case ErrorSubcategoryChargingPowerUnit::SyncrhonizationCableStatusFaultOfEnergyRoutingBoard:
        os << "SyncrhonizationCableStatusFaultOfEnergyRoutingBoard";
        break;
    case ErrorSubcategoryChargingPowerUnit::SoftStartFault:
        os << "SoftStartFault";
        break;
    case ErrorSubcategoryChargingPowerUnit::SoftStartModuleCommunicationFailure:
        os << "SoftStartModuleCommunicationFailure";
        break;
    case ErrorSubcategoryChargingPowerUnit::SoftStartModuleOverloaded:
        os << "SoftStartModuleOverloaded";
        break;
    case ErrorSubcategoryChargingPowerUnit::SoftStartModuleFault:
        os << "SoftStartModuleFault";
        break;
    case ErrorSubcategoryChargingPowerUnit::SoftStartModuleOvertemperature:
        os << "SoftStartModuleOvertemperature";
        break;
    case ErrorSubcategoryChargingPowerUnit::SoftStartModuleUndertemperature:
        os << "SoftStartModuleUndertemperature";
        break;
    case ErrorSubcategoryChargingPowerUnit::SoftStartModuleDisconnectionFailure:
        os << "SoftStartModuleDisconnectionFailure";
        break;
    case ErrorSubcategoryChargingPowerUnit::PhaseSequenceAbornmalAlarm:
        os << "PhaseSequenceAbornmalAlarm";
        break;
    case ErrorSubcategoryChargingPowerUnit::PowerDistributionModuleCommunicationFailure:
        os << "PowerDistributionModuleCommunicationFailure";
        break;
    case ErrorSubcategoryChargingPowerUnit::FaultOfInsulationResistanceToGround:
        os << "FaultOfInsulationResistanceToGround";
        break;
    case ErrorSubcategoryChargingPowerUnit::ModbusTcpCertificate:
        os << "ModbusTcpCertificate";
        break;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const ErrorSubcategoryAcBranch& subcategory) {
    switch (subcategory) {
    case ErrorSubcategoryAcBranch::AcBranch1:
        os << "AcBranch1";
        break;
    case ErrorSubcategoryAcBranch::AcBranch2:
        os << "AcBranch2";
        break;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const ErrorSubcategoryAcDcRectifier& subcategory) {
    switch (subcategory) {
    case ErrorSubcategoryAcDcRectifier::rectifier_1:
        os << "rectifier_1";
        break;
    case ErrorSubcategoryAcDcRectifier::rectifier_2:
        os << "rectifier_2";
        break;
    case ErrorSubcategoryAcDcRectifier::rectifier_3:
        os << "rectifier_3";
        break;
    case ErrorSubcategoryAcDcRectifier::rectifier_4:
        os << "rectifier_4";
        break;
    case ErrorSubcategoryAcDcRectifier::rectifier_5:
        os << "rectifier_5";
        break;
    case ErrorSubcategoryAcDcRectifier::rectifier_6:
        os << "rectifier_6";
        break;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const ErrorSubcategoryDcDcChargingModule& subcategory) {
    switch (subcategory) {
    case ErrorSubcategoryDcDcChargingModule::DcDcModule1:
        os << "DcDcModule1";
        break;
    case ErrorSubcategoryDcDcChargingModule::DcDcModule2:
        os << "DcDcModule2";
        break;
    case ErrorSubcategoryDcDcChargingModule::DcDcModule3:
        os << "DcDcModule3";
        break;
    case ErrorSubcategoryDcDcChargingModule::DcDcModule4:
        os << "DcDcModule4";
        break;
    case ErrorSubcategoryDcDcChargingModule::DcDcModule5:
        os << "DcDcModule5";
        break;
    case ErrorSubcategoryDcDcChargingModule::DcDcModule6:
        os << "DcDcModule6";
        break;
    case ErrorSubcategoryDcDcChargingModule::DcDcModule7:
        os << "DcDcModule7";
        break;
    case ErrorSubcategoryDcDcChargingModule::DcDcModule8:
        os << "DcDcModule8";
        break;
    case ErrorSubcategoryDcDcChargingModule::DcDcModule9:
        os << "DcDcModule9";
        break;
    case ErrorSubcategoryDcDcChargingModule::DcDcModule10:
        os << "DcDcModule10";
        break;
    case ErrorSubcategoryDcDcChargingModule::DcDcModule11:
        os << "DcDcModule11";
        break;
    case ErrorSubcategoryDcDcChargingModule::DcDcModule12:
        os << "DcDcModule12";
        break;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const ErrorSubcategoryCoolingSection& subcategory) {
    switch (subcategory) {
    case ErrorSubcategoryCoolingSection::CoolingUnit1:
        os << "CoolingUnit1";
        break;
    }
    return os;
}

inline std::ostream& operator<<(std::ostream& os, const ErrorSubcategoryPowerDistributionModule& subcategory) {
    switch (subcategory) {
    case ErrorSubcategoryPowerDistributionModule::PowerDistributionModule1:
        os << "PowerDistributionModule1";
        break;
    case ErrorSubcategoryPowerDistributionModule::PowerDistributionModule2:
        os << "PowerDistributionModule2";
        break;
    case ErrorSubcategoryPowerDistributionModule::PowerDistributionModule3:
        os << "PowerDistributionModule3";
        break;
    case ErrorSubcategoryPowerDistributionModule::PowerDistributionModule4:
        os << "PowerDistributionModule4";
        break;
    case ErrorSubcategoryPowerDistributionModule::PowerDistributionModule5:
        os << "PowerDistributionModule5";
        break;
    }
    return os;
}

union ErrorSubcategory {
    ErrorSubcategoryPowerUnit power_unit;
    ErrorSubcategoryChargingPowerUnit charging_power_unit;
    ErrorSubcategoryAcBranch ac_branch;
    ErrorSubcategoryAcDcRectifier ac_dc_rectifier;
    ErrorSubcategoryDcDcChargingModule dc_dc_charging_module;
    ErrorSubcategoryCoolingSection cooling_section;
    ErrorSubcategoryPowerDistributionModule power_distribution_module;
    uint16_t raw;

    bool operator<(const ErrorSubcategory& rhs) const {
        return raw < rhs.raw;
    }
    bool operator==(const ErrorSubcategory& rhs) const {
        return raw == rhs.raw;
    }
    bool operator!=(const ErrorSubcategory& rhs) const {
        return raw != rhs.raw;
    }
};

union ErrorPayload {
    uint32_t error_flags;
    AlarmStatus alarm;
    uint32_t raw;

    bool is_error() const {
        return raw != 0;
    }
    bool operator==(const ErrorPayload& rhs) const {
        return raw == rhs.raw;
    }
    bool operator!=(const ErrorPayload& rhs) const {
        return raw != rhs.raw;
    }
};

struct ErrorEvent {
    ErrorCategory error_category;
    ErrorSubcategory error_subcategory;
    ErrorPayload payload;

    bool operator==(const ErrorEvent& rhs) const {
        return error_category == rhs.error_category && error_subcategory.raw == rhs.error_subcategory.raw &&
               payload.raw == rhs.payload.raw;
    }
    bool operator!=(const ErrorEvent& rhs) const {
        return !(*this == rhs);
    }

    friend std::ostream& operator<<(std::ostream& os, const ErrorEvent& errorEvent) {
        os << "Category: " << static_cast<uint16_t>(errorEvent.error_category)
           << "; Subcategory: " << static_cast<uint16_t>(errorEvent.error_subcategory.charging_power_unit)
           << "; Flags: " << errorEvent.payload.error_flags << std::endl;

        return os;
    }

    std::string to_everest_subtype() const {
        std::stringstream oss;

        switch (error_category) {
        case ErrorCategory::PowerUnit: {
            oss << "PowerUnit"
                << "/";
            ErrorSubcategoryPowerUnit subcategory = error_subcategory.power_unit;
            oss << subcategory;
            return oss.str();
        }
        case ErrorCategory::ChargingPowerUnit: {
            oss << "ChargingPowerUnit"
                << "/";
            auto subcategory = error_subcategory.charging_power_unit;
            oss << subcategory;
            return oss.str();
        }
        case ErrorCategory::AcBranch: {
            oss << "AcBranch"
                << "/";
            auto subcategory = error_subcategory.ac_branch;
            oss << subcategory;
            return oss.str();
        }
        case ErrorCategory::AcDcRectifier: {
            oss << "AcDcRectifier"
                << "/";
            auto subcategory = error_subcategory.ac_dc_rectifier;
            oss << subcategory;
            return oss.str();
        }
        case ErrorCategory::DcDcChargingModule: {
            oss << "DcDcChargingModule"
                << "/";
            auto subcategory = error_subcategory.dc_dc_charging_module;
            oss << subcategory;
            return oss.str();
        }
        case ErrorCategory::CoolingSection: {
            oss << "CoolingSection"
                << "/";
            auto subcategory = error_subcategory.cooling_section;
            oss << subcategory;
            return oss.str();
        }
        case ErrorCategory::ErrorSubcategoryPowerDistributionModule: {
            oss << "PowerDistributionModule"
                << "/";
            auto subcategory = error_subcategory.power_distribution_module;
            oss << subcategory;
            return oss.str();
        }
        }

        return oss.str();
    }

    std::string to_error_log_string() const {
        std::stringstream oss;

        oss << "Category: " << error_category << "; ";

        switch (error_category) {
        case ErrorCategory::PowerUnit: {
            ErrorSubcategoryPowerUnit subcategory = error_subcategory.power_unit;
            oss << "Subcategory: " << subcategory << "; ";
            oss << "AlarmState: " << payload.alarm;
            break;
        }
        case ErrorCategory::ChargingPowerUnit: {
            auto subcategory = error_subcategory.charging_power_unit;

            switch (subcategory) {
            case ErrorSubcategoryChargingPowerUnit::UnknownSystemType:
            case ErrorSubcategoryChargingPowerUnit::PowerDetectionException:
            case ErrorSubcategoryChargingPowerUnit::SyncrhonizationCableStatusFaultOfEnergyRoutingBoard:
            case ErrorSubcategoryChargingPowerUnit::SoftStartFault:
            case ErrorSubcategoryChargingPowerUnit::SoftStartModuleCommunicationFailure:
            case ErrorSubcategoryChargingPowerUnit::SoftStartModuleOverloaded:
            case ErrorSubcategoryChargingPowerUnit::SoftStartModuleFault:
            case ErrorSubcategoryChargingPowerUnit::SoftStartModuleOvertemperature:
            case ErrorSubcategoryChargingPowerUnit::SoftStartModuleUndertemperature:
            case ErrorSubcategoryChargingPowerUnit::SoftStartModuleDisconnectionFailure:
            case ErrorSubcategoryChargingPowerUnit::PhaseSequenceAbornmalAlarm:
            case ErrorSubcategoryChargingPowerUnit::PowerDistributionModuleCommunicationFailure:
            case ErrorSubcategoryChargingPowerUnit::FaultOfInsulationResistanceToGround:
                oss << "Subcategory: " << subcategory << "; ";
                oss << "AlarmState: " << payload.alarm;

                break;
            case ErrorSubcategoryChargingPowerUnit::ModbusTcpCertificate:
                oss << "Subcategory: " << subcategory << "; ";
                oss << "Flags: 0x";
                oss << std::hex << std::setw(4) << std::setfill('0') << payload.error_flags;
                break;
            }

        } break;
        case ErrorCategory::AcBranch: {
            auto subcategory = error_subcategory.ac_branch;
            oss << "Subcategory: " << subcategory << "; ";
            oss << "Flags: 0x";
            oss << std::hex << std::setw(8) << std::setfill('0') << payload.error_flags;
            break;
        }
        case ErrorCategory::AcDcRectifier: {
            auto subcategory = error_subcategory.ac_dc_rectifier;
            oss << "Subcategory: " << subcategory << "; ";
            oss << "Flags: 0x";
            oss << std::hex << std::setw(8) << std::setfill('0') << payload.error_flags;
            break;
        }
        case ErrorCategory::DcDcChargingModule: {
            auto subcategory = error_subcategory.dc_dc_charging_module;
            oss << "Subcategory: " << subcategory << "; ";
            oss << "Flags: 0x";
            oss << std::hex << std::setw(8) << std::setfill('0') << payload.error_flags;
            break;
        }
        case ErrorCategory::CoolingSection: {
            auto subcategory = error_subcategory.cooling_section;
            oss << "Subcategory: " << subcategory << "; ";
            oss << "Flags: 0x";
            oss << std::hex << std::setw(8) << std::setfill('0') << payload.error_flags;
            break;
        }
        case ErrorCategory::ErrorSubcategoryPowerDistributionModule: {
            auto subcategory = error_subcategory.power_distribution_module;
            oss << "Subcategory: " << subcategory << "; ";
            oss << "Flags: 0x";
            oss << std::hex << std::setw(8) << std::setfill('0') << payload.error_flags;
            break;
        }
        };

        return oss.str();
    }

    bool operator<(const ErrorEvent& rhs) const {
        if (error_category != rhs.error_category) {
            return error_category < rhs.error_category;
        }

        return error_subcategory.raw < rhs.error_subcategory.raw;
    }
};

struct ErrorEventComparator {
    bool operator()(const ErrorEvent& a, const ErrorEvent& b) const {
        return a < b;
    }
};

class ErrorRegisters {
public:
    ErrorRegisters() {
    }

    void add_to_registry(modbus::registers::registry::ComplexRegisterRegistry& registry) {
        raw_registers::AlarmPowerUnitRegisters::DataProviders alarm_power_unit_providers{
            .high_voltage_door_status_sensor = power_unit.high_voltage_door_status_sensor,
            .door_status_sensor = power_unit.door_status_sensor,
            .water = power_unit.water,
            .smoke = power_unit.smoke,
            .epo = power_unit.epo,
        };
        registry.add(std::make_unique<raw_registers::AlarmPowerUnitRegisters>(alarm_power_unit_providers));

        raw_registers::AlarmChargingPowerUnitRegisters::DataProviders alarm_charging_power_unit_providers{
            .unknown_system_type = charging_power_unit.unknown_system_type,
            .power_detection_exception = charging_power_unit.power_detection_exception,
            .syncrhonization_cable_status_fault_of_energy_routing_board =
                charging_power_unit.syncrhonization_cable_status_fault_of_energy_routing_board,
            .soft_start_fault = charging_power_unit.soft_start_fault,
            .soft_start_module_communication_failure = charging_power_unit.soft_start_module_communication_failure,
            .soft_start_module_overloaded = charging_power_unit.soft_start_module_overloaded,
            .soft_start_module_fault = charging_power_unit.soft_start_module_fault,
            .soft_start_module_overtemperature = charging_power_unit.soft_start_module_overtemperature,
            .soft_start_module_undertemperature = charging_power_unit.soft_start_module_undertemperature,
            .soft_start_module_disconnection_failure = charging_power_unit.soft_start_module_disconnection_failure,
            .phase_sequence_abornmal_alarm = charging_power_unit.phase_sequence_abornmal_alarm,
            .power_distribution_module_communication_failure =
                charging_power_unit.power_distribution_module_communication_failure,
            .fault_of_insulation_resistance_to_ground = charging_power_unit.fault_of_insulation_resistance_to_ground,
            .modbus_tcp_certificate = charging_power_unit.modbus_tcp_certificate,
        };
        registry.add(
            std::make_unique<raw_registers::AlarmChargingPowerUnitRegisters>(alarm_charging_power_unit_providers));

        raw_registers::AlarmAcBranchRegisters::DataProviders alarm_ac_branch_providers{
            .ac_branch_1 = ac_branch.ac_branch_1,
            .ac_branch_2 = ac_branch.ac_branch_2,
        };
        registry.add(std::make_unique<raw_registers::AlarmAcBranchRegisters>(alarm_ac_branch_providers));

        raw_registers::AlarmAcDcRectifierRegisters::DataProviders ac_dc_rectifier_providers{
            .rectifier_1 = ac_dc_rectifier.rectifier_1,
            .rectifier_2 = ac_dc_rectifier.rectifier_2,
            .rectifier_3 = ac_dc_rectifier.rectifier_3,
            .rectifier_4 = ac_dc_rectifier.rectifier_4,
            .rectifier_5 = ac_dc_rectifier.rectifier_5,
            .rectifier_6 = ac_dc_rectifier.rectifier_6,
        };
        registry.add(std::make_unique<raw_registers::AlarmAcDcRectifierRegisters>(ac_dc_rectifier_providers));

        raw_registers::DcDcChargingModuleRegisters::DataProviders dc_dc_charging_module_providers{
            .dc_dc_charging_module_1 = dc_dc_charging_module.dc_dc_module_1,
            .dc_dc_charging_module_2 = dc_dc_charging_module.dc_dc_module_2,
            .dc_dc_charging_module_3 = dc_dc_charging_module.dc_dc_module_3,
            .dc_dc_charging_module_4 = dc_dc_charging_module.dc_dc_module_4,
            .dc_dc_charging_module_5 = dc_dc_charging_module.dc_dc_module_5,
            .dc_dc_charging_module_6 = dc_dc_charging_module.dc_dc_module_6,
            .dc_dc_charging_module_7 = dc_dc_charging_module.dc_dc_module_7,
            .dc_dc_charging_module_8 = dc_dc_charging_module.dc_dc_module_8,
            .dc_dc_charging_module_9 = dc_dc_charging_module.dc_dc_module_9,
            .dc_dc_charging_module_10 = dc_dc_charging_module.dc_dc_module_10,
            .dc_dc_charging_module_11 = dc_dc_charging_module.dc_dc_module_11,
            .dc_dc_charging_module_12 = dc_dc_charging_module.dc_dc_module_12,
        };
        registry.add(std::make_unique<raw_registers::DcDcChargingModuleRegisters>(dc_dc_charging_module_providers));

        raw_registers::CoolingSectionRegisters::DataProviders cooling_section_providers{
            .cooling_unit_1 = cooling_section.cooling_unit_1,
        };
        registry.add(std::make_unique<raw_registers::CoolingSectionRegisters>(cooling_section_providers));

        raw_registers::PowerDistributionModuleRegisters::DataProviders power_distribution_module_providers{
            .power_distribution_module_1 = power_distribution_module.power_distribution_module_1,
            .power_distribution_module_2 = power_distribution_module.power_distribution_module_2,
            .power_distribution_module_3 = power_distribution_module.power_distribution_module_3,
            .power_distribution_module_4 = power_distribution_module.power_distribution_module_4,
            .power_distribution_module_5 = power_distribution_module.power_distribution_module_5,
        };
        registry.add(
            std::make_unique<raw_registers::PowerDistributionModuleRegisters>(power_distribution_module_providers));
    }

    void add_callback(std::function<void(ErrorEvent)> callback) {
        add_callback_to_alarm_power_unit_registers(callback);
        add_callback_to_alarm_charging_power_unit_registers(callback);
        add_callback_to_ac_branch_registers(callback);
        add_callback_to_ac_dc_rectifier_registers(callback);
        add_callback_to_dc_dc_charging_module_registers(callback);
        add_callback_to_cooling_section_registers(callback);
        add_callback_to_power_distribution_module_registers(callback);
    }

private:
    struct {
        DataProviderHolding<AlarmStatus> high_voltage_door_status_sensor =
            DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> door_status_sensor = DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> water = DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> smoke = DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> epo = DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
    } power_unit;

    struct {
        DataProviderHolding<AlarmStatus> unknown_system_type = DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> power_detection_exception =
            DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> syncrhonization_cable_status_fault_of_energy_routing_board =
            DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> soft_start_fault = DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> soft_start_module_communication_failure =
            DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> soft_start_module_overloaded =
            DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> soft_start_module_fault =
            DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> soft_start_module_overtemperature =
            DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> soft_start_module_undertemperature =
            DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> soft_start_module_disconnection_failure =
            DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> phase_sequence_abornmal_alarm =
            DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> power_distribution_module_communication_failure =
            DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<AlarmStatus> fault_of_insulation_resistance_to_ground =
            DataProviderHolding<AlarmStatus>(AlarmStatus::NORMAL);
        DataProviderHolding<uint16_t> modbus_tcp_certificate = DataProviderHolding<uint16_t>(0);
    } charging_power_unit;

    struct {
        DataProviderHolding<uint32_t> ac_branch_1 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> ac_branch_2 = DataProviderHolding<uint32_t>(0);
    } ac_branch;

    struct {
        DataProviderHolding<uint32_t> rectifier_1 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> rectifier_2 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> rectifier_3 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> rectifier_4 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> rectifier_5 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> rectifier_6 = DataProviderHolding<uint32_t>(0);
    } ac_dc_rectifier;

    struct {
        DataProviderHolding<uint32_t> dc_dc_module_1 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> dc_dc_module_2 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> dc_dc_module_3 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> dc_dc_module_4 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> dc_dc_module_5 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> dc_dc_module_6 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> dc_dc_module_7 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> dc_dc_module_8 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> dc_dc_module_9 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> dc_dc_module_10 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> dc_dc_module_11 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> dc_dc_module_12 = DataProviderHolding<uint32_t>(0);
    } dc_dc_charging_module;

    struct {
        DataProviderHolding<uint32_t> cooling_unit_1 = DataProviderHolding<uint32_t>(0);
    } cooling_section;

    struct {
        DataProviderHolding<uint32_t> power_distribution_module_1 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> power_distribution_module_2 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> power_distribution_module_3 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> power_distribution_module_4 = DataProviderHolding<uint32_t>(0);
        DataProviderHolding<uint32_t> power_distribution_module_5 = DataProviderHolding<uint32_t>(0);
    } power_distribution_module;

#define ERROR_ALARM_CALLBACK(CATEGORY, SUBCATEGORY)                                                                    \
    [callback](AlarmStatus register_value) {                                                                           \
        struct ErrorEvent event = {                                                                                    \
            .error_category = CATEGORY,                                                                                \
            .error_subcategory = SUBCATEGORY,                                                                          \
            .payload = ErrorPayload{.alarm = register_value},                                                          \
        };                                                                                                             \
        callback(event);                                                                                               \
    }

#define ERROR_BITFLAGS_CALLBACK(CATEGORY, SUBCATEGORY)                                                                 \
    [callback](uint32_t register_value) {                                                                              \
        struct ErrorEvent event = {                                                                                    \
            .error_category = CATEGORY,                                                                                \
            .error_subcategory = SUBCATEGORY,                                                                          \
            .payload = ErrorPayload{.error_flags = register_value},                                                    \
        };                                                                                                             \
        callback(event);                                                                                               \
    }

#define POWER_UNIT_ALARM_CALLBACK(SUBCATEGORY)                                                                         \
    ERROR_ALARM_CALLBACK(ErrorCategory::PowerUnit, ErrorSubcategory{.power_unit = SUBCATEGORY})

#define CHARGING_POWER_UNIT_ALARM_CALLBACK(SUBCATEGORY)                                                                \
    ERROR_ALARM_CALLBACK(ErrorCategory::ChargingPowerUnit, ErrorSubcategory{.charging_power_unit = SUBCATEGORY})

#define AC_BRANCH_BITFLAGS_CALLBACK(SUBCATEGORY)                                                                       \
    ERROR_BITFLAGS_CALLBACK(ErrorCategory::AcBranch, ErrorSubcategory{.ac_branch = SUBCATEGORY})

#define AC_DC_RECTIFIER_BITFLAGS_CALLBACK(SUBCATEGORY)                                                                 \
    ERROR_BITFLAGS_CALLBACK(ErrorCategory::AcDcRectifier, ErrorSubcategory{.ac_dc_rectifier = SUBCATEGORY})

#define DC_DC_CHARGING_BITFLAGS_CALLBACK(SUBCATEGORY)                                                                  \
    ERROR_BITFLAGS_CALLBACK(ErrorCategory::DcDcChargingModule, ErrorSubcategory{.dc_dc_charging_module = SUBCATEGORY})

#define COOLING_SECTION_BITFLAGS_CALLBACK(SUBCATEGORY)                                                                 \
    ERROR_BITFLAGS_CALLBACK(ErrorCategory::CoolingSection, ErrorSubcategory{.cooling_section = SUBCATEGORY})

#define POWER_DISTRIBUTION_BITFLAGS_CALLBACK(SUBCATEGORY)                                                              \
    ERROR_BITFLAGS_CALLBACK(ErrorCategory::ErrorSubcategoryPowerDistributionModule,                                    \
                            ErrorSubcategory{.power_distribution_module = SUBCATEGORY})

    void add_callback_to_alarm_power_unit_registers(std::function<void(ErrorEvent)> callback) {
        power_unit.high_voltage_door_status_sensor.add_write_callback(
            POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryPowerUnit::HighVoltageDoorStatusSensor));

        power_unit.door_status_sensor.add_write_callback(
            POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryPowerUnit::DoorStatusSensor));

        power_unit.water.add_write_callback(POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryPowerUnit::Water));

        power_unit.smoke.add_write_callback(POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryPowerUnit::Smoke));

        power_unit.epo.add_write_callback(POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryPowerUnit::Epo));
    }

    void add_callback_to_alarm_charging_power_unit_registers(std::function<void(ErrorEvent)> callback) {
        charging_power_unit.unknown_system_type.add_write_callback(
            CHARGING_POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryChargingPowerUnit::UnknownSystemType));

        charging_power_unit.power_detection_exception.add_write_callback(
            CHARGING_POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryChargingPowerUnit::PowerDetectionException));

        charging_power_unit.syncrhonization_cable_status_fault_of_energy_routing_board.add_write_callback(
            CHARGING_POWER_UNIT_ALARM_CALLBACK(
                ErrorSubcategoryChargingPowerUnit::SyncrhonizationCableStatusFaultOfEnergyRoutingBoard));

        charging_power_unit.soft_start_fault.add_write_callback(
            CHARGING_POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryChargingPowerUnit::SoftStartFault));

        charging_power_unit.soft_start_module_communication_failure.add_write_callback(
            CHARGING_POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryChargingPowerUnit::SoftStartModuleCommunicationFailure));

        charging_power_unit.soft_start_module_overloaded.add_write_callback(
            CHARGING_POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryChargingPowerUnit::SoftStartModuleOverloaded));

        charging_power_unit.soft_start_module_fault.add_write_callback(
            CHARGING_POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryChargingPowerUnit::SoftStartModuleFault));

        charging_power_unit.soft_start_module_overtemperature.add_write_callback(
            CHARGING_POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryChargingPowerUnit::SoftStartModuleOvertemperature));

        charging_power_unit.soft_start_module_undertemperature.add_write_callback(
            CHARGING_POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryChargingPowerUnit::SoftStartModuleUndertemperature));

        charging_power_unit.soft_start_module_disconnection_failure.add_write_callback(
            CHARGING_POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryChargingPowerUnit::SoftStartModuleDisconnectionFailure));

        charging_power_unit.phase_sequence_abornmal_alarm.add_write_callback(
            CHARGING_POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryChargingPowerUnit::PhaseSequenceAbornmalAlarm));

        charging_power_unit.power_distribution_module_communication_failure.add_write_callback(
            CHARGING_POWER_UNIT_ALARM_CALLBACK(
                ErrorSubcategoryChargingPowerUnit::PowerDistributionModuleCommunicationFailure));

        charging_power_unit.fault_of_insulation_resistance_to_ground.add_write_callback(
            CHARGING_POWER_UNIT_ALARM_CALLBACK(ErrorSubcategoryChargingPowerUnit::FaultOfInsulationResistanceToGround));

        // This one is special, because it is the only uint16_t flags register
        charging_power_unit.modbus_tcp_certificate.add_write_callback([callback](uint16_t register_value) {
            struct ErrorEvent event = {
                .error_category = ErrorCategory::ChargingPowerUnit,
                .error_subcategory =
                    ErrorSubcategory{.charging_power_unit = ErrorSubcategoryChargingPowerUnit::ModbusTcpCertificate},
                .payload = ErrorPayload{.error_flags = register_value},
            };
            callback(event);
        });
    }

    void add_callback_to_ac_branch_registers(std::function<void(ErrorEvent)> callback) {
        ac_branch.ac_branch_1.add_write_callback(AC_BRANCH_BITFLAGS_CALLBACK(ErrorSubcategoryAcBranch::AcBranch1));

        ac_branch.ac_branch_2.add_write_callback(AC_BRANCH_BITFLAGS_CALLBACK(ErrorSubcategoryAcBranch::AcBranch2));
    }

    void add_callback_to_ac_dc_rectifier_registers(std::function<void(ErrorEvent)> callback) {
        ac_dc_rectifier.rectifier_1.add_write_callback(
            AC_DC_RECTIFIER_BITFLAGS_CALLBACK(ErrorSubcategoryAcDcRectifier::rectifier_1));

        ac_dc_rectifier.rectifier_2.add_write_callback(
            AC_DC_RECTIFIER_BITFLAGS_CALLBACK(ErrorSubcategoryAcDcRectifier::rectifier_2));

        ac_dc_rectifier.rectifier_3.add_write_callback(
            AC_DC_RECTIFIER_BITFLAGS_CALLBACK(ErrorSubcategoryAcDcRectifier::rectifier_3));

        ac_dc_rectifier.rectifier_4.add_write_callback(
            AC_DC_RECTIFIER_BITFLAGS_CALLBACK(ErrorSubcategoryAcDcRectifier::rectifier_4));

        ac_dc_rectifier.rectifier_5.add_write_callback(
            AC_DC_RECTIFIER_BITFLAGS_CALLBACK(ErrorSubcategoryAcDcRectifier::rectifier_5));

        ac_dc_rectifier.rectifier_6.add_write_callback(
            AC_DC_RECTIFIER_BITFLAGS_CALLBACK(ErrorSubcategoryAcDcRectifier::rectifier_6));
    }

    void add_callback_to_dc_dc_charging_module_registers(std::function<void(ErrorEvent)> callback) {
        dc_dc_charging_module.dc_dc_module_1.add_write_callback(
            DC_DC_CHARGING_BITFLAGS_CALLBACK(ErrorSubcategoryDcDcChargingModule::DcDcModule1));

        dc_dc_charging_module.dc_dc_module_2.add_write_callback(
            DC_DC_CHARGING_BITFLAGS_CALLBACK(ErrorSubcategoryDcDcChargingModule::DcDcModule2));

        dc_dc_charging_module.dc_dc_module_3.add_write_callback(
            DC_DC_CHARGING_BITFLAGS_CALLBACK(ErrorSubcategoryDcDcChargingModule::DcDcModule3));

        dc_dc_charging_module.dc_dc_module_4.add_write_callback(
            DC_DC_CHARGING_BITFLAGS_CALLBACK(ErrorSubcategoryDcDcChargingModule::DcDcModule4));

        dc_dc_charging_module.dc_dc_module_5.add_write_callback(
            DC_DC_CHARGING_BITFLAGS_CALLBACK(ErrorSubcategoryDcDcChargingModule::DcDcModule5));

        dc_dc_charging_module.dc_dc_module_6.add_write_callback(
            DC_DC_CHARGING_BITFLAGS_CALLBACK(ErrorSubcategoryDcDcChargingModule::DcDcModule6));

        dc_dc_charging_module.dc_dc_module_7.add_write_callback(
            DC_DC_CHARGING_BITFLAGS_CALLBACK(ErrorSubcategoryDcDcChargingModule::DcDcModule7));

        dc_dc_charging_module.dc_dc_module_8.add_write_callback(
            DC_DC_CHARGING_BITFLAGS_CALLBACK(ErrorSubcategoryDcDcChargingModule::DcDcModule8));

        dc_dc_charging_module.dc_dc_module_9.add_write_callback(
            DC_DC_CHARGING_BITFLAGS_CALLBACK(ErrorSubcategoryDcDcChargingModule::DcDcModule9));

        dc_dc_charging_module.dc_dc_module_10.add_write_callback(
            DC_DC_CHARGING_BITFLAGS_CALLBACK(ErrorSubcategoryDcDcChargingModule::DcDcModule10));

        dc_dc_charging_module.dc_dc_module_11.add_write_callback(
            DC_DC_CHARGING_BITFLAGS_CALLBACK(ErrorSubcategoryDcDcChargingModule::DcDcModule11));

        dc_dc_charging_module.dc_dc_module_12.add_write_callback(
            DC_DC_CHARGING_BITFLAGS_CALLBACK(ErrorSubcategoryDcDcChargingModule::DcDcModule12));
    }

    void add_callback_to_cooling_section_registers(std::function<void(ErrorEvent)> callback) {
        cooling_section.cooling_unit_1.add_write_callback(
            COOLING_SECTION_BITFLAGS_CALLBACK(ErrorSubcategoryCoolingSection::CoolingUnit1));
    }

    void add_callback_to_power_distribution_module_registers(std::function<void(ErrorEvent)> callback) {
        power_distribution_module.power_distribution_module_1.add_write_callback(
            POWER_DISTRIBUTION_BITFLAGS_CALLBACK(ErrorSubcategoryPowerDistributionModule::PowerDistributionModule1));

        power_distribution_module.power_distribution_module_2.add_write_callback(
            POWER_DISTRIBUTION_BITFLAGS_CALLBACK(ErrorSubcategoryPowerDistributionModule::PowerDistributionModule2));

        power_distribution_module.power_distribution_module_3.add_write_callback(
            POWER_DISTRIBUTION_BITFLAGS_CALLBACK(ErrorSubcategoryPowerDistributionModule::PowerDistributionModule3));

        power_distribution_module.power_distribution_module_4.add_write_callback(
            POWER_DISTRIBUTION_BITFLAGS_CALLBACK(ErrorSubcategoryPowerDistributionModule::PowerDistributionModule4));

        power_distribution_module.power_distribution_module_5.add_write_callback(
            POWER_DISTRIBUTION_BITFLAGS_CALLBACK(ErrorSubcategoryPowerDistributionModule::PowerDistributionModule5));
    }

#undef ERROR_ALARM_CALLBACK
#undef ERROR_BITFLAGS_CALLBACK
#undef POWER_UNIT_ALARM_CALLBACK
#undef CHARGING_POWER_UNIT_ALARM_CALLBACK
#undef AC_BRANCH_BITFLAGS_CALLBACK
#undef AC_DC_RECTIFIER_BITFLAGS_CALLBACK
#undef DC_DC_CHARGING_BITFLAGS_CALLBACK
#undef COOLING_SECTION_BITFLAGS_CALLBACK
#undef POWER_DISTRIBUTION_BITFLAGS_CALLBACK
};

} // namespace fusion_charger::modbus_driver
