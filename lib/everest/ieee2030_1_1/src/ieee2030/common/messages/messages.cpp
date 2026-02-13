// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2026 Pionix GmbH and Contributors to EVerest
#include <iostream>

#include <ieee2030/common/detail/conversions.hpp>
#include <ieee2030/common/messages/messages.hpp>

namespace ieee2030::messages {
EV100::EV100(const std::vector<uint8_t> raw) {
    max_battery_voltage = static_cast<float>(from_raw<uint16_t>(raw, 4));
    charged_rate = from_raw<uint8_t>(raw, 6);
}

std::ostream& operator<<(std::ostream& out, const EV100& self) {
    out << "Maximum Battery Voltage: " << self.max_battery_voltage << "Charged Rate: " << self.charged_rate;
    return out;
}

EV100::operator std::vector<uint8_t>() {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint8_t>(0), data);
    to_raw(static_cast<uint8_t>(0), data);
    to_raw(static_cast<uint8_t>(0), data);
    to_raw(static_cast<uint8_t>(0), data);
    to_raw(static_cast<uint16_t>(max_battery_voltage), data);
    to_raw(static_cast<uint8_t>(charged_rate), data);
    to_raw(static_cast<uint8_t>(0), data);
    return data;
}

EV101::EV101(const std::vector<uint8_t> raw) {
    max_charging_time_10s = from_raw<uint8_t>(raw, 1);
    max_charging_time_1min = from_raw<uint8_t>(raw, 2);
    estimated_charging_time_1min = from_raw<uint8_t>(raw, 3);
    float capacity = static_cast<float>(from_raw<uint16_t>(raw, 5));
    if (capacity > 0) {
        total_capacity = capacity;
    }
}

std::ostream& operator<<(std::ostream& out, const EV101& self) {
    out << "EV Msg 101: Maximum Charging Time (10s): " << self.max_charging_time_10s
        << "Maximum Charging Time (1min): " << self.max_charging_time_1min
        << "Estimated Charging Time: " << self.estimated_charging_time_1min
        << "Total Capacity: " << self.total_capacity.value_or(0);
    return out;
}

EV101::operator std::vector<uint8_t>() {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint8_t>(0), data);
    to_raw(static_cast<uint8_t>(max_charging_time_10s), data);
    to_raw(static_cast<uint8_t>(max_charging_time_1min), data);
    to_raw(static_cast<uint8_t>(estimated_charging_time_1min), data);
    to_raw(static_cast<uint8_t>(0), data);
    to_raw(static_cast<uint16_t>(total_capacity.value_or(0)), data);
    to_raw(static_cast<uint8_t>(0), data);

    return data;
}

EV102::EV102(const std::vector<uint8_t> raw) {
    protocol = static_cast<defs::ProtocolNumber>(from_raw<uint8_t>(raw, 0));
    target_voltage = static_cast<float>(from_raw<uint16_t>(raw, 1));
    target_current = static_cast<float>(from_raw<uint8_t>(raw, 3));

    uint8_t fault = from_raw<uint8_t>(raw, 4);
    fault_battery_over_voltage = fault & (1 << 0);
    fault_battery_under_voltage = fault & (1 << 1);
    fault_battery_current_deviation_error = fault & (1 << 2);
    fault_high_battery_temperature = fault & (1 << 3);
    fault_battery_voltage_deviation_error = fault & (1 << 4);

    uint8_t status = from_raw<uint8_t>(raw, 5);
    status_charging_enabled = status & (1 << 0);
    status_shift_position = status & (1 << 1);
    status_system_fault = status & (1 << 2);
    status_vehicle_status = status & (1 << 3);
    status_stop_request = status & (1 << 4);

    soc = from_raw<uint8_t>(raw, 6);
}

std::ostream& operator<<(std::ostream& out, const EV102& self) {
    out << "EV Msg 102: Protocol: " << static_cast<uint8_t>(self.protocol)
        << "Target Voltage [V]: " << self.target_voltage << "Target Current [A]: " << self.target_current
        << "SoC [%]: " << self.soc << "Fault battery over voltage: " << self.fault_battery_under_voltage
        << "Fault battery under voltage: " << self.fault_battery_under_voltage
        << "Fault battery current deviation error: " << self.fault_battery_current_deviation_error
        << "Fault high battery temp: " << self.fault_high_battery_temperature
        << "Fault battery voltage deviation error: " << self.fault_battery_voltage_deviation_error
        << "Status charging enabled: " << self.status_charging_enabled
        << "Status shift position: " << self.status_shift_position
        << "Status system fault: " << self.status_system_fault
        << "Status vehicle status: " << self.status_vehicle_status
        << "Status stop request: " << self.status_stop_request;
    return out;
}

EV102::operator std::vector<uint8_t, std::allocator<uint8_t>>() {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint8_t>(protocol), data);
    to_raw(static_cast<uint16_t>(target_voltage), data);
    to_raw(static_cast<uint8_t>(target_current), data);

    uint8_t fault = fault_battery_voltage_deviation_error << 4 | fault_high_battery_temperature << 3 |
                    fault_battery_current_deviation_error << 2 | fault_battery_under_voltage << 1 |
                    fault_battery_over_voltage;

    to_raw(static_cast<uint8_t>(fault), data);

    uint8_t status = status_stop_request << 4 | status_vehicle_status << 3 | status_system_fault << 2 |
                     status_shift_position << 1 | status_charging_enabled;

    to_raw(static_cast<uint8_t>(status), data);
    to_raw(static_cast<uint8_t>(soc), data);
    to_raw(static_cast<uint8_t>(0), data);

    return data;
}

Charger108::Charger108(const std::vector<uint8_t> raw) {
    identifier_welding_detection = from_raw<uint8_t>(raw, 0);
    available_voltage = static_cast<float>(from_raw<uint16_t>(raw, 1));
    available_current = static_cast<float>(from_raw<uint8_t>(raw, 3));
    threshold_voltage = static_cast<float>(from_raw<uint16_t>(raw, 4));
}

std::ostream& operator<<(std::ostream& out, const Charger108& self) {
    out << "Charger Msg 108: Identifier welding detection: " << self.identifier_welding_detection
        << "Available Voltage [V]: " << self.available_voltage << "Available Current [A]: " << self.available_current
        << "Threshold voltage [V]: " << self.threshold_voltage;
    return out;
}

Charger108::operator std::vector<uint8_t>() {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint8_t>(identifier_welding_detection), data);
    to_raw(static_cast<uint16_t>(available_voltage), data);
    to_raw(static_cast<uint8_t>(available_current), data);
    to_raw(static_cast<uint16_t>(threshold_voltage), data);
    to_raw(static_cast<uint16_t>(0), data);
    return data;
}

Charger109::Charger109(const std::vector<uint8_t> raw) {
    protocol = static_cast<defs::ProtocolNumber>(from_raw<uint8_t>(raw, 0));
    present_voltage = static_cast<float>(from_raw<uint16_t>(raw, 1));
    present_current = static_cast<float>(from_raw<uint8_t>(raw, 3));

    uint8_t status = from_raw<uint8_t>(raw, 5);
    charger_status = status & (1 << 0);
    charger_malfunction = status & (1 << 1);
    connector_lock = status & (1 << 2);
    battery_incompatibility = status & (1 << 3);
    system_malfunction = status & (1 << 4);
    stop_control = status & (1 << 5);

    reamining_time_10s = from_raw<uint8_t>(raw, 6);
    reamining_time_1min = from_raw<uint8_t>(raw, 7);
}

std::ostream& operator<<(std::ostream& out, const Charger109& self) {
    out << "Charger Msg 109: Protocol: " << static_cast<uint8_t>(self.protocol)
        << "Present Voltage [V]: " << self.present_voltage << "Present Current [A]: " << self.present_current
        << "Reamining time [10s]: " << self.reamining_time_10s << "Reamining time [1min]: " << self.reamining_time_1min
        << "Charger status: " << self.charger_status << "Charger malfunction: " << self.charger_malfunction
        << "Connector lock: " << self.connector_lock << "Battery incompatibility: " << self.battery_incompatibility
        << "System malfunction: " << self.system_malfunction << "Stop control: " << self.stop_control;
    return out;
}

Charger109::operator std::vector<uint8_t>() {
    std::vector<uint8_t> data;

    to_raw(static_cast<uint8_t>(protocol), data);
    to_raw(static_cast<uint16_t>(present_voltage), data);
    to_raw(static_cast<uint8_t>(present_current), data);
    to_raw(static_cast<uint8_t>(0), data);

    uint8_t status = stop_control << 5 | system_malfunction << 4 | battery_incompatibility << 3 | connector_lock << 2 |
                     charger_malfunction << 1 | charger_status;

    to_raw(static_cast<uint8_t>(status), data);
    to_raw(static_cast<uint8_t>(reamining_time_10s), data);
    to_raw(static_cast<uint8_t>(reamining_time_1min), data);

    return data;
}

} // namespace ieee2030::messages
