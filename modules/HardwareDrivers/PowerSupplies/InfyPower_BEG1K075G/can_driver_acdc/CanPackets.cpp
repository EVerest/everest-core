// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "CanPackets.hpp"
#include "Conversions.hpp"
#include <iostream>

namespace can_packet_acdc {

// helper functions
uint32_t encode_can_id(uint8_t source_address, uint8_t destination_address, uint8_t command_number,
                       uint8_t device_number, uint8_t error_code) {
    uint32_t id = source_address;
    id |= destination_address << 8;
    command_number &= 0x3F;
    id |= command_number << 16;
    device_number &= 0x0F;
    id |= device_number << 22;
    error_code &= 0x07;
    id |= error_code << 26;
    return id;
}

/*
 destination_address is either 0x3F (broadcast) or group address
*/
uint32_t encode_can_id(uint8_t destination_address, uint8_t command_number) {
    uint8_t device_number;
    if (destination_address == 0x3F) {
        device_number = 0x0A;
    } else {
        device_number = 0x0B;
    }

    return encode_can_id(0xF0, destination_address, command_number, device_number, 0);
}

uint8_t destination_address_from_can_id(uint32_t id) {
    return (id >> 8) & 0xFF;
}
uint8_t source_address_from_can_id(uint32_t id) {
    return id & 0xFF;
}

uint8_t command_number_from_can_id(uint32_t id) {
    return (id >> 16) & 0x3F;
}

uint8_t error_code_from_can_id(uint32_t id) {
    return (id >> 26) & 0x07;
}

// packet definitions

// GenericSetting

GenericSetting::GenericSetting(const std::vector<uint8_t> raw) {
    byte0 = from_raw<uint8_t>(raw, 0);
    byte1 = from_raw<uint8_t>(raw, 1);
    value = from_raw<uint32_t>(raw, 4);
}

GenericSetting::GenericSetting(uint8_t _byte0, uint8_t _byte1, uint32_t _value) :
    byte0(_byte0), byte1(_byte1), value(_value) {
}

std::ostream& operator<<(std::ostream& out, const GenericSetting& self) {
    out << "GenericSetting: byte0: " << std::to_string(self.byte0) << " byte1: " << std::to_string(self.byte0)
        << " Value: " << std::to_string(self.value);
    return out;
}

GenericSetting::operator std::vector<uint8_t>() {
    std::vector<uint8_t> data;
    to_raw(byte0, data);
    to_raw(byte1, data);
    to_raw(static_cast<uint16_t>(0x00), data);
    to_raw(value, data);

    return data;
}

// SystemDCVoltage

SystemDCVoltage::SystemDCVoltage(const std::vector<uint8_t> raw) {
    volt = (float)from_raw<uint32_t>(raw, 4) / 1000.;
}

SystemDCVoltage::SystemDCVoltage(float _volt) : volt(_volt) {
}

SystemDCVoltage::SystemDCVoltage() : volt(0.) {
}

std::ostream& operator<<(std::ostream& out, const SystemDCVoltage& self) {
    out << "SystemDCVoltage: " << std::to_string(self.volt);
    return out;
}

SystemDCVoltage::operator std::vector<uint8_t>() {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint8_t>(0x10), data);
    to_raw(static_cast<uint8_t>(0x01), data);
    to_raw(static_cast<uint16_t>(0x00), data);
    to_raw(static_cast<uint32_t>(volt * 1000), data);

    return data;
}

// SystemDCCurrent

SystemDCCurrent::SystemDCCurrent(const std::vector<uint8_t> raw) {
    ampere = (float)from_raw<uint32_t>(raw, 4) / 1000.;
}

SystemDCCurrent::SystemDCCurrent(float _ampere) : ampere(_ampere) {
}

SystemDCCurrent::SystemDCCurrent() : ampere(0.) {
}

std::ostream& operator<<(std::ostream& out, const SystemDCCurrent& self) {
    out << "SystemDCCurrent: " << std::to_string(self.ampere);
    return out;
}

SystemDCCurrent::operator std::vector<uint8_t>() {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint8_t>(0x10), data);
    to_raw(static_cast<uint8_t>(0x02), data);
    to_raw(static_cast<uint16_t>(0x00), data);
    to_raw(static_cast<uint32_t>(ampere * 1000), data);

    return data;
}

// PowerModuleNumber

PowerModuleNumber::PowerModuleNumber(const std::vector<uint8_t> raw) {
    number = from_raw<uint16_t>(raw, 6);
}

PowerModuleNumber::PowerModuleNumber() : number(0) {
}

std::ostream& operator<<(std::ostream& out, const PowerModuleNumber& self) {
    out << "PowerModuleNumber: " << std::to_string(self.number);
    return out;
}

PowerModuleNumber::operator std::vector<uint8_t>() {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint8_t>(0x10), data);
    to_raw(static_cast<uint8_t>(0x10), data);
    to_raw(static_cast<uint16_t>(0x00), data);
    to_raw(static_cast<uint32_t>(0x00), data);

    return data;
}

// PowerModuleStatus

PowerModuleStatus::PowerModuleStatus() {
}

PowerModuleStatus::PowerModuleStatus(const std::vector<uint8_t> raw) {
    uint8_t status0 = from_raw<uint8_t>(raw, 7);
    uint8_t status1 = from_raw<uint8_t>(raw, 6);
    uint8_t status2 = from_raw<uint8_t>(raw, 5);

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

PowerModuleStatus::operator std::vector<uint8_t>() {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint8_t>(0x11), data);
    to_raw(static_cast<uint8_t>(0x10), data);
    to_raw(static_cast<uint16_t>(0x00), data);
    to_raw(static_cast<uint32_t>(0x00), data);

    return data;
}

// InverterStatus

InverterStatus::InverterStatus(const std::vector<uint8_t> raw) {
    uint8_t status0 = from_raw<uint8_t>(raw, 7);

    invert_mode = status0 & (1 << 0);
}

InverterStatus::InverterStatus() : invert_mode{false} {
}

std::ostream& operator<<(std::ostream& out, const InverterStatus& self) {
    out << "InverterStatus: " << (self.invert_mode ? "inverter" : "rectifier");
    return out;
}

InverterStatus::operator std::vector<uint8_t>() {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint8_t>(0x11), data);
    to_raw(static_cast<uint8_t>(0x11), data);
    to_raw(static_cast<uint16_t>(0x00), data);
    to_raw(static_cast<uint32_t>(0), data);

    return data;
}

// OnOff

OnOff::OnOff(bool o) : on{o} {};

OnOff::operator std::vector<uint8_t>() {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint8_t>(0x11), data);
    to_raw(static_cast<uint8_t>(0x10), data);
    to_raw(static_cast<uint16_t>(0x00), data);
    to_raw(static_cast<uint16_t>(0x00), data);
    to_raw(static_cast<uint16_t>(on ? 0xA0 : 0xA1), data);

    return data;
}

// Walk in enable

WalkInEnable::WalkInEnable(bool e) : enabled{e} {};

WalkInEnable::operator std::vector<uint8_t>() {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint8_t>(0x11), data);
    to_raw(static_cast<uint8_t>(0x22), data);
    to_raw(static_cast<uint16_t>(0x00), data);
    to_raw(static_cast<uint16_t>(0x00), data);
    to_raw(static_cast<uint16_t>(enabled ? 0xA1 : 0xA0), data);

    return data;
}

// WorkingMode

WorkingMode::WorkingMode(bool o) : inverter{o} {};

WorkingMode::operator std::vector<uint8_t>() {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint8_t>(0x21), data);
    to_raw(static_cast<uint8_t>(0x10), data);
    to_raw(static_cast<uint16_t>(0x00), data);
    to_raw(static_cast<uint16_t>(0x00), data);
    to_raw(static_cast<uint16_t>(inverter ? 0xA1 : 0xA0), data);

    return data;
}

// PowerFactorAdjust

PowerFactorAdjust::PowerFactorAdjust(float p) : pf{p} {};

PowerFactorAdjust::operator std::vector<uint8_t>() {
    std::vector<uint8_t> data;
    to_raw(static_cast<uint8_t>(0x21), data);
    to_raw(static_cast<uint8_t>(0x05), data);
    to_raw(static_cast<uint16_t>(0x00), data);
    to_raw(static_cast<int32_t>(pf * 1000.), data);

    return data;
}

} // namespace can_packet_acdc
