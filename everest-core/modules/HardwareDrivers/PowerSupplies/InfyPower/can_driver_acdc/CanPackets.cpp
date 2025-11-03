// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "CanPackets.hpp"
#include "Conversions.hpp"
#include <cstring>
#include <everest/logging.hpp>
#include <iomanip>
#include <iostream>
#include <sstream>

namespace can_packet_acdc {

namespace {
// CAN ID bit positions (as per InfyPower V1.13 protocol)
constexpr uint8_t CAN_ID_DESTINATION_ADDRESS_SHIFT = 8; // Bits 15-8
constexpr uint8_t CAN_ID_COMMAND_NUMBER_SHIFT = 16;     // Bits 21-16
constexpr uint8_t CAN_ID_DEVICE_NUMBER_SHIFT = 22;      // Bits 25-22
constexpr uint8_t CAN_ID_ERROR_CODE_SHIFT = 26;         // Bits 28-26
} // namespace

// helper functions
uint32_t encode_can_id(uint8_t source_address, uint8_t destination_address, uint8_t command_number,
                       uint8_t device_number, uint8_t error_code) {
    // Bits 28-26: Error code (0x00 for normal)
    // Bits 25-22: Device No (4 bits)
    // Bits 21-16: Command No (6 bits)
    // Bits 15-8: Destination Address (8 bits)
    // Bits 7-0: Source Address (8 bits) - configurable controller address (default 0xF0)
    uint32_t id = source_address;
    id |= destination_address << CAN_ID_DESTINATION_ADDRESS_SHIFT;
    command_number &= InfyProtocol::COMMAND_MASK;
    id |= command_number << CAN_ID_COMMAND_NUMBER_SHIFT;
    device_number &= InfyProtocol::DEVICE_NUMBER_MASK;
    id |= device_number << CAN_ID_DEVICE_NUMBER_SHIFT;
    error_code &= InfyProtocol::ERROR_CODE_MASK;
    id |= error_code << CAN_ID_ERROR_CODE_SHIFT;
    return id;
}

uint8_t destination_address_from_can_id(uint32_t id) {
    return (id >> CAN_ID_DESTINATION_ADDRESS_SHIFT) & 0xFF;
}
uint8_t source_address_from_can_id(uint32_t id) {
    return id & 0xFF;
}

uint8_t command_number_from_can_id(uint32_t id) {
    return (id >> CAN_ID_COMMAND_NUMBER_SHIFT) & InfyProtocol::COMMAND_MASK;
}

uint8_t error_code_from_can_id(uint32_t id) {
    return (id >> CAN_ID_ERROR_CODE_SHIFT) & InfyProtocol::ERROR_CODE_MASK;
}

// packet definitions
PowerModuleStatus::PowerModuleStatus() {
}

PowerModuleStatus::PowerModuleStatus(const std::vector<uint8_t>& raw) {
    // Size validation is handled at rx_handler level
    uint8_t status0 = from_raw<uint8_t>(raw, 7);
    uint8_t status1 = from_raw<uint8_t>(raw, 6);
    uint8_t status2 = from_raw<uint8_t>(raw, 5);

    // PowerModuleStatus bit mapping per InfyPower V1.13 protocol documentation
    // status0 (byte 7): Bit0=output_short_current, Bit4=sleeping, Bit5=discharge_abnormal
    // status1 (byte 6): Bit0=dc_side_off, Bit1=fault_alarm, Bit2=protection_alarm, Bit3=fan_fault_alarm,
    //                   Bit4=over_temperature_alarm, Bit5=output_over_voltage_alarm, Bit6=walk_in_enable,
    //                   Bit7=communication_interrupt_alarm
    // status2 (byte 5): Bit0=power_limit_status, Bit1=id_repeat_alarm, Bit2=load_sharing_alarm,
    // Bit3=input_phase_lost_alarm,
    //                   Bit4=input_unbalanced_alarm, Bit5=input_low_voltage_alarm, Bit6=input_over_voltage_protection,
    //                   Bit7=pfc_side_off

    output_short_current = status0 & (1 << 0);
    sleeping = status0 & (1 << 4);
    discharge_abnormal = status0 & (1 << 5);
    dc_side_off = status1 & (1 << 0);
    fault_alarm = status1 & (1 << 1);
    protection_alarm = status1 & (1 << 2);
    fan_fault_alarm = status1 & (1 << 3);
    over_temperature_alarm = status1 & (1 << 4);
    output_over_voltage_alarm = status1 & (1 << 5);
    walk_in_enable = status1 & (1 << 6);
    communication_interrupt_alarm = status1 & (1 << 7);
    power_limit_status = status2 & (1 << 0);
    id_repeat_alarm = status2 & (1 << 1);
    load_sharing_alarm = status2 & (1 << 2);
    input_phase_lost_alarm = status2 & (1 << 3);
    input_unbalanced_alarm = status2 & (1 << 4);
    input_low_voltage_alarm = status2 & (1 << 5);
    input_over_voltage_protection = status2 & (1 << 6);
    pfc_side_off = status2 & (1 << 7);
}

std::ostream& operator<<(std::ostream& out, const PowerModuleStatus& self) {
    out << "PowerModuleStatus: " << (self.output_short_current ? "output_short_current " : "")
        << (self.sleeping ? "sleeping " : "") << (self.discharge_abnormal ? "discharge_abnormal " : "")
        << (self.dc_side_off ? "dc_side_off " : "") << (self.fault_alarm ? "fault_alarm " : "")
        << (self.protection_alarm ? "protection_alarm " : "") << (self.fan_fault_alarm ? "fan_fault_alarm " : "")
        << (self.over_temperature_alarm ? "over_temperature_alarm " : "")
        << (self.output_over_voltage_alarm ? "output_over_voltage_alarm " : "")
        << (self.walk_in_enable ? "walk_in_enable " : "")
        << (self.communication_interrupt_alarm ? "communication_interrupt_alarm " : "")
        << (self.power_limit_status ? "power_limit_status " : "") << (self.id_repeat_alarm ? "id_repeat_alarm " : "")
        << (self.load_sharing_alarm ? "load_sharing_alarm " : "")
        << (self.input_phase_lost_alarm ? "input_phase_lost_alarm " : "")
        << (self.input_unbalanced_alarm ? "input_unbalanced_alarm " : "")
        << (self.input_low_voltage_alarm ? "input_low_voltage_alarm " : "")
        << (self.input_over_voltage_protection ? "input_over_voltage_protection " : "")
        << (self.pfc_side_off ? "pfc_side_off " : "");
    return out;
}

PowerModuleStatus::operator std::vector<uint8_t>() const {
    std::vector<uint8_t> data;
    return data;
}

// New packet class implementations for V1.13 protocol

ReadModuleCount::ReadModuleCount() : count(0) {
}

ReadModuleCount::ReadModuleCount(const std::vector<uint8_t>& raw) {
    if (raw.size() < 3) {
        EVLOG_warning << "Received invalid ReadModuleCount packet with size " << raw.size();
        return;
    }
    count = from_raw<uint8_t>(raw, 2);
}

ReadModuleCount::operator std::vector<uint8_t>() const {
    std::vector<uint8_t> data(8);
    return data;
}

ReadModuleVI::ReadModuleVI(const std::vector<uint8_t>& raw) {
    // Size validation is handled at rx_handler level
    voltage = from_raw<float>(raw, 0);
    current = from_raw<float>(raw, 4);
}

ReadModuleVI::operator std::vector<uint8_t>() const {
    return {};
}

ReadModuleCapabilities::ReadModuleCapabilities(const std::vector<uint8_t>& raw) {
    // Size validation is handled at rx_handler level
    max_voltage = from_raw<uint16_t>(raw, 0) * InfyProtocol::SCALING_FACTOR_1_0;
    min_voltage = from_raw<uint16_t>(raw, 2) * InfyProtocol::SCALING_FACTOR_1_0;
    max_current = from_raw<uint16_t>(raw, 4) * InfyProtocol::SCALING_FACTOR_0_1;
    rated_power = from_raw<uint16_t>(raw, 6) * InfyProtocol::SCALING_FACTOR_10_0;
}

ReadModuleCapabilities::operator std::vector<uint8_t>() const {
    return {};
}

ReadModuleVIAfterDiode::ReadModuleVIAfterDiode(const std::vector<uint8_t>& raw) {
    // Size validation is handled at rx_handler level
    v_ext = from_raw<uint16_t>(raw, 0) * InfyProtocol::SCALING_FACTOR_0_1;
    i_avail = from_raw<uint16_t>(raw, 2) * InfyProtocol::SCALING_FACTOR_0_1;
}

ReadModuleVIAfterDiode::operator std::vector<uint8_t>() const {
    return {};
}

ReadModuleBarcode::ReadModuleBarcode(const std::vector<uint8_t>& raw) {
    // Size validation is handled at rx_handler level

    // According to protocol documentation for Command 0x0B:
    // Example: Barcode "081807123451V1704" -> Response: 56 13 0C 15 A3 FB 06 A8
    // Byte 0: 13th character (ASCII) - 0x56 = 'V'
    // Bytes 1-7: Encoded first 12 characters + last 4 characters in complex HEX format

    // For now, we'll extract what we can and create a readable serial number
    char thirteenth_char = static_cast<char>(from_raw<uint8_t>(raw, 0));

    // Extract the remaining bytes as hex values for debugging/identification
    std::stringstream ss;
    ss << "SN_" << std::hex << std::setfill('0');
    for (size_t i = 1; i < 8 && i < raw.size(); ++i) {
        ss << std::setw(2) << static_cast<unsigned>(from_raw<uint8_t>(raw, i));
    }
    ss << "_" << thirteenth_char;

    serial_number = ss.str();
}

ReadModuleBarcode::operator std::vector<uint8_t>() const {
    return {};
}

SetModuleVI::SetModuleVI(float v, float c) : voltage(v), current(c) {
}

SetModuleVI::operator std::vector<uint8_t>() const {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint32_t>(voltage * InfyProtocol::VOLTAGE_TO_MV), data); // we need to transform V to mV
    to_raw(static_cast<uint32_t>(current * InfyProtocol::CURRENT_TO_MA), data); // we need to transform A to mA
    return data;
}

SetModuleOnOff::SetModuleOnOff(bool o) : on(o) {
}

SetModuleOnOff::operator std::vector<uint8_t>() const {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint8_t>(on ? 0x00 : 0x01), data);
    // The rest of the payload is reserved and should be 0
    for (int i = 0; i < 7; ++i) {
        to_raw(static_cast<uint8_t>(0), data);
    }
    return data;
}

} // namespace can_packet_acdc