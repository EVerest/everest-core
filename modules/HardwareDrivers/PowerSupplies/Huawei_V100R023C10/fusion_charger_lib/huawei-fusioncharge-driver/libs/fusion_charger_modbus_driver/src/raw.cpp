// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#include <fusion_charger/modbus/registers/raw.hpp>

std::string fusion_charger::modbus_driver::raw_registers::working_status_to_string(const WorkingStatus& status) {
    switch (status) {
    case WorkingStatus::STANDBY:
        return "STANDBY";
    case WorkingStatus::STANDBY_WITH_CONNECTOR_INSERTED:
        return "STANDBY_WITH_CONNECTOR_INSERTED";
    case WorkingStatus::CHARGING:
        return "CHARGING";
    case WorkingStatus::CHARGING_COMPLETE:
        return "CHARGING_COMPLETE";
    case WorkingStatus::FAULT:
        return "FAULT";
    case WorkingStatus::DISPENSER_UPGRADE:
        return "DISPENSER_UPGRADE";
    case WorkingStatus::CHARGING_STARTING:
        return "CHARGING_STARTING";
    default:
        return "UNKNOWN";
    }
}

std::string fusion_charger::modbus_driver::raw_registers::psu_output_port_availability_to_string(
    const PsuOutputPortAvailability& availability) {
    switch (availability) {
    case PsuOutputPortAvailability::AVAILABLE:
        return "AVAILABLE";
    case PsuOutputPortAvailability::NOT_AVAILABLE:
        return "NOT_AVAILABLE";
    }

    return "UNKNOWN";
}

std::string fusion_charger::modbus_driver::raw_registers::SettingPowerUnitRegisters::psu_running_mode_to_string(
    const PSURunningMode& mode) {
    switch (mode) {
    case PSURunningMode::STARTING_UP:
        return "STARTING_UP";
    case PSURunningMode::RUNNING:
        return "RUNNING";
    case PSURunningMode::FAULTY:
        return "FAULTY";
    case PSURunningMode::SLEEPING:
        return "SLEEPING";
    case PSURunningMode::UPGRADING:
        return "UPGRADING";
    default:
        return "UNKNOWN";
    }
}
