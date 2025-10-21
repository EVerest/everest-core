// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "CanPackets.hpp"
#include "Conversions.hpp"
#include <everest/logging.hpp>

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <sstream>

namespace can_packet_acdc {

namespace {
// Winline CAN ID bit positions (29-bit extended CAN ID)
// Bits 31-29: Not used (always 0)
// Bits 28-20: PROTNO (9 bits) = 0x060
// Bit 19: PTP (1 bit)
// Bits 18-11: DSTADDR (8 bits)
// Bits 10-3: SRCADDR (8 bits)
// Bits 2-0: Group (3 bits)
}

// Winline CAN ID encoding/decoding functions
uint32_t encode_can_id(uint8_t source_address, uint8_t destination_address, uint8_t group_number, bool point_to_point) {
    uint32_t id = 0;

    // Bits 2-0: Group number (3 bits)
    id |= (group_number & WinlineProtocol::GROUP_MASK) << WinlineProtocol::GROUP_SHIFT;

    // Bits 10-3: Source Address (8 bits)
    id |= (source_address & WinlineProtocol::ADDRESS_MASK) << WinlineProtocol::SRCADDR_SHIFT;

    // Bits 18-11: Destination Address (8 bits)
    id |= (destination_address & WinlineProtocol::ADDRESS_MASK) << WinlineProtocol::DSTADDR_SHIFT;

    // Bit 19: Point-to-point flag (1 bit)
    id |= (point_to_point ? 1 : 0) << WinlineProtocol::PTP_SHIFT;

    // Bits 28-20: Protocol number (9 bits) - always 0x060 for Winline
    id |= (WinlineProtocol::PROTNO & WinlineProtocol::PROTNO_MASK) << WinlineProtocol::PROTNO_SHIFT;

    return id;
}

uint8_t destination_address_from_can_id(uint32_t id) {
    return (id >> WinlineProtocol::DSTADDR_SHIFT) & WinlineProtocol::ADDRESS_MASK;
}

uint8_t source_address_from_can_id(uint32_t id) {
    return (id >> WinlineProtocol::SRCADDR_SHIFT) & WinlineProtocol::ADDRESS_MASK;
}

uint8_t group_number_from_can_id(uint32_t id) {
    return (id >> WinlineProtocol::GROUP_SHIFT) & WinlineProtocol::GROUP_MASK;
}

bool point_to_point_from_can_id(uint32_t id) {
    return ((id >> WinlineProtocol::PTP_SHIFT) & WinlineProtocol::PTP_MASK) != 0;
}

uint16_t protocol_number_from_can_id(uint32_t id) {
    return (id >> WinlineProtocol::PROTNO_SHIFT) & WinlineProtocol::PROTNO_MASK;
}

// Command building helpers for Winline protocol
uint32_t build_read_command_id(uint8_t source_address, uint8_t destination_address, uint8_t group_number,
                               bool point_to_point) {
    return encode_can_id(source_address, destination_address, group_number, point_to_point);
}

uint32_t build_set_command_id(uint8_t source_address, uint8_t destination_address, uint8_t group_number,
                              bool point_to_point) {
    return encode_can_id(source_address, destination_address, group_number, point_to_point);
}

// Command frame builders for Winline register-based communication
std::vector<uint8_t> build_read_command(uint16_t register_number) {
    std::vector<uint8_t> data(8, 0); // Initialize 8-byte payload with zeros

    // Byte 0: Function code for READ operation
    data[0] = WinlineProtocol::FUNCTION_READ;

    // Byte 1: Reserved (always 0x00)
    data[1] = 0x00;

    // Bytes 2-3: Register number (big-endian)
    data[2] = (register_number >> 8) & 0xFF;
    data[3] = register_number & 0xFF;

    // Bytes 4-7: Reserved (always 0x00)
    data[4] = 0x00;
    data[5] = 0x00;
    data[6] = 0x00;
    data[7] = 0x00;

    return data;
}

std::vector<uint8_t> build_set_command(uint16_t register_number, const std::vector<uint8_t>& data_payload) {
    std::vector<uint8_t> data(8, 0); // Initialize 8-byte payload with zeros

    // Byte 0: Function code for SET operation
    data[0] = WinlineProtocol::FUNCTION_SET;

    // Byte 1: Reserved (always 0x00)
    data[1] = 0x00;

    // Bytes 2-3: Register number (big-endian)
    data[2] = (register_number >> 8) & 0xFF;
    data[3] = register_number & 0xFF;

    // Bytes 4-7: Data to set (copy from payload, up to 4 bytes)
    size_t copy_size = std::min(data_payload.size(), size_t(4));
    for (size_t i = 0; i < copy_size; ++i) {
        data[4 + i] = data_payload[i];
    }

    return data;
}

std::vector<uint8_t> build_set_command_float(uint16_t register_number, float value) {
    std::vector<uint8_t> float_data;
    to_raw(value, float_data);
    return build_set_command(register_number, float_data);
}

std::vector<uint8_t> build_set_command_integer(uint16_t register_number, uint32_t value) {
    std::vector<uint8_t> int_data;
    to_raw(value, int_data);
    return build_set_command(register_number, int_data);
}

// packet definitions
PowerModuleStatus::PowerModuleStatus() {
}

PowerModuleStatus::PowerModuleStatus(const std::vector<uint8_t>& raw) {
    // Size validation is handled at rx_handler level

    // Winline status parsing based on Chart 2 in protocol document
    // The status is a 32-bit integer returned from register 0x0040
    // We expect the response format: [DataType|ErrorCode|Register|StatusData]
    // For now, assume the raw data contains the 32-bit status value in bytes 4-7

    // Check if we have enough data for standardized response format
    if (raw.size() >= 8) {
        uint8_t data_type = from_raw<uint8_t>(raw, 0);
        uint8_t error_code = from_raw<uint8_t>(raw, 1);

        // Verify this is a valid status response
        if (data_type == WinlineProtocol::DATA_TYPE_INTEGER && error_code == WinlineProtocol::ERROR_NORMAL) {
            // Extract 32-bit status value from bytes 4-7
            uint32_t status_value = from_raw<uint32_t>(raw, 4);

            // Parse Winline status bits according to Chart 2
            module_fault = (status_value & (1U << 0)) != 0;      // Bit 0: Module fault (red indicator)
            module_protection = (status_value & (1U << 1)) != 0; // Bit 1: Module protection (yellow indicator)
            // Bit 2: Reserved
            sci_communication_failure =
                (status_value & (1U << 3)) != 0;                // Bit 3: Module internal SCI communication failure
            input_mode_error = (status_value & (1U << 4)) != 0; // Bit 4: Input mode error/wiring error
            input_mode_mismatch =
                (status_value & (1U << 5)) != 0; // Bit 5: Input mode set by monitor doesn't match actual
            // Bit 6: Reserved
            dcdc_overvoltage = (status_value & (1U << 7)) != 0;     // Bit 7: DCDC overvoltage
            pfc_voltage_abnormal = (status_value & (1U << 8)) != 0; // Bit 8: PFC voltage abnormal
            ac_overvoltage = (status_value & (1U << 9)) != 0;       // Bit 9: AC overvoltage
            // Bits 10-13: Reserved
            ac_undervoltage = (status_value & (1U << 14)) != 0; // Bit 14: AC undervoltage
            // Bit 15: Reserved
            can_communication_failure = (status_value & (1U << 16)) != 0; // Bit 16: CAN communication failure
            module_current_imbalance = (status_value & (1U << 17)) != 0;  // Bit 17: Module current imbalance
            // Bits 18-21: Reserved
            dcdc_on_off_status = (status_value & (1U << 22)) != 0;    // Bit 22: DCDC On/off status (0:On, 1:Off)
            module_power_limiting = (status_value & (1U << 23)) != 0; // Bit 23: Module power limiting
            temperature_derating = (status_value & (1U << 24)) != 0;  // Bit 24: Temperature derating
            ac_power_limiting = (status_value & (1U << 25)) != 0;     // Bit 25: AC power limiting
            // Bit 26: Reserved
            fan_fault = (status_value & (1U << 27)) != 0;          // Bit 27: Fan fault
            dcdc_short_circuit = (status_value & (1U << 28)) != 0; // Bit 28: DCDC short circuit
            // Bit 29: Reserved
            dcdc_over_temperature = (status_value & (1U << 30)) != 0;   // Bit 30: DCDC over temperature
            dcdc_output_overvoltage = (status_value & (1U << 31)) != 0; // Bit 31: DCDC output overvoltage
        } else {
            EVLOG_warning << " Invalid status response - DataType: 0x" << std::hex << static_cast<int>(data_type)
                          << ", ErrorCode: 0x" << static_cast<int>(error_code);
        }
    } else {
        EVLOG_warning << " PowerModuleStatus received insufficient data (size: " << raw.size() << ")";
    }
}

std::ostream& operator<<(std::ostream& out, const PowerModuleStatus& self) {
    out << "PowerModuleStatus: ";

    // Output active status flags with Winline naming
    if (self.module_fault)
        out << "module_fault ";
    if (self.module_protection)
        out << "module_protection ";
    if (self.sci_communication_failure)
        out << "sci_communication_failure ";
    if (self.input_mode_error)
        out << "input_mode_error ";
    if (self.input_mode_mismatch)
        out << "input_mode_mismatch ";
    if (self.dcdc_overvoltage)
        out << "dcdc_overvoltage ";
    if (self.pfc_voltage_abnormal)
        out << "pfc_voltage_abnormal ";
    if (self.ac_overvoltage)
        out << "ac_overvoltage ";
    if (self.ac_undervoltage)
        out << "ac_undervoltage ";
    if (self.can_communication_failure)
        out << "can_communication_failure ";
    if (self.module_current_imbalance)
        out << "module_current_imbalance ";
    if (self.dcdc_on_off_status)
        out << "dcdc_on_off_status ";
    if (self.module_power_limiting)
        out << "module_power_limiting ";
    if (self.temperature_derating)
        out << "temperature_derating ";
    if (self.ac_power_limiting)
        out << "ac_power_limiting ";
    if (self.fan_fault)
        out << "fan_fault ";
    if (self.dcdc_short_circuit)
        out << "dcdc_short_circuit ";
    if (self.dcdc_over_temperature)
        out << "dcdc_over_temperature ";
    if (self.dcdc_output_overvoltage)
        out << "dcdc_output_overvoltage ";

    return out;
}

PowerModuleStatus::operator std::vector<uint8_t>() const {
    // For SET operations, this would create the command payload
    // Status is read-only, so this returns empty vector
    std::vector<uint8_t> data;
    return data;
}

// Winline Register-Based Packet Implementations

// READ operations
ReadVoltage::ReadVoltage() : voltage(0.0f) {
}

ReadVoltage::ReadVoltage(const std::vector<uint8_t>& raw) {
    if (raw.size() >= 8) {
        uint8_t data_type = from_raw<uint8_t>(raw, 0);
        uint8_t error_code = from_raw<uint8_t>(raw, 1);

        if (data_type == WinlineProtocol::DATA_TYPE_FLOAT && error_code == WinlineProtocol::ERROR_NORMAL) {
            voltage = from_raw<float>(raw, 4);
        } else {
            EVLOG_warning << " Invalid voltage response - DataType: 0x" << std::hex << static_cast<int>(data_type)
                          << ", ErrorCode: 0x" << static_cast<int>(error_code);
        }
    }
}

ReadVoltage::operator std::vector<uint8_t>() const {
    return build_read_command(REGISTER);
}

ReadCurrent::ReadCurrent() : current(0.0f) {
}

ReadCurrent::ReadCurrent(const std::vector<uint8_t>& raw) {
    if (raw.size() >= 8) {
        uint8_t data_type = from_raw<uint8_t>(raw, 0);
        uint8_t error_code = from_raw<uint8_t>(raw, 1);

        if (data_type == WinlineProtocol::DATA_TYPE_FLOAT && error_code == WinlineProtocol::ERROR_NORMAL) {
            current = from_raw<float>(raw, 4);
        } else {
            EVLOG_warning << " Invalid current response - DataType: 0x" << std::hex << static_cast<int>(data_type)
                          << ", ErrorCode: 0x" << static_cast<int>(error_code);
        }
    }
}

ReadCurrent::operator std::vector<uint8_t>() const {
    return build_read_command(REGISTER);
}

ReadGroupInfo::ReadGroupInfo() : group_number(0), dip_address(0) {
}

ReadGroupInfo::ReadGroupInfo(const std::vector<uint8_t>& raw) {
    if (raw.size() >= 8) {
        uint8_t data_type = from_raw<uint8_t>(raw, 0);
        uint8_t error_code = from_raw<uint8_t>(raw, 1);

        if (data_type == WinlineProtocol::DATA_TYPE_INTEGER && error_code == WinlineProtocol::ERROR_NORMAL) {
            // Higher 16 bits: group number, Lower 16 bits: DIP address
            uint32_t group_info = from_raw<uint32_t>(raw, 4);
            group_number = (group_info >> 16) & 0xFF;
            dip_address = group_info & 0xFF;
        } else {
            EVLOG_warning << " Invalid group info response - DataType: 0x" << std::hex << static_cast<int>(data_type)
                          << ", ErrorCode: 0x" << static_cast<int>(error_code);
        }
    }
}

ReadGroupInfo::operator std::vector<uint8_t>() const {
    return build_read_command(REGISTER);
}

ReadSerialNumber::ReadSerialNumber() : serial_number("") {
}

ReadSerialNumber::ReadSerialNumber(uint32_t low_bytes, uint32_t high_bytes) {
    // Combine high and low bytes to create serial number string
    std::stringstream ss;
    ss << "SN_" << std::hex << std::setfill('0') << std::setw(8) << high_bytes << std::setw(8) << low_bytes;
    serial_number = ss.str();
}

ReadSerialNumber::operator std::vector<uint8_t>() const {
    // This packet requires reading two registers, so we return empty
    // The caller should handle reading both REGISTER_LOW and REGISTER_HIGH
    return {};
}

ReadRatedOutputPower::ReadRatedOutputPower() : power(0.0f) {
}

ReadRatedOutputPower::ReadRatedOutputPower(const std::vector<uint8_t>& raw) {
    if (raw.size() >= 8) {
        uint8_t data_type = from_raw<uint8_t>(raw, 0);
        uint8_t error_code = from_raw<uint8_t>(raw, 1);

        if (data_type == WinlineProtocol::DATA_TYPE_FLOAT && error_code == WinlineProtocol::ERROR_NORMAL) {
            power = from_raw<float>(raw, 4);
        } else {
            EVLOG_warning << " Invalid power response - DataType: 0x" << std::hex << static_cast<int>(data_type)
                          << ", ErrorCode: 0x" << static_cast<int>(error_code);
        }
    }
}

ReadRatedOutputPower::operator std::vector<uint8_t>() const {
    return build_read_command(REGISTER);
}

ReadRatedOutputCurrent::ReadRatedOutputCurrent() : current(0.0f) {
}

ReadRatedOutputCurrent::ReadRatedOutputCurrent(const std::vector<uint8_t>& raw) {
    if (raw.size() >= 8) {
        uint8_t data_type = from_raw<uint8_t>(raw, 0);
        uint8_t error_code = from_raw<uint8_t>(raw, 1);

        if (data_type == WinlineProtocol::DATA_TYPE_FLOAT && error_code == WinlineProtocol::ERROR_NORMAL) {
            current = from_raw<float>(raw, 4);
        } else {
            EVLOG_warning << " Invalid rated current response - DataType: 0x" << std::hex << static_cast<int>(data_type)
                          << ", ErrorCode: 0x" << static_cast<int>(error_code);
        }
    }
}

ReadRatedOutputCurrent::operator std::vector<uint8_t>() const {
    return build_read_command(REGISTER);
}

// Add missing packet constructors with standardized response parsing
ReadCurrentLimitPoint::ReadCurrentLimitPoint() : limit_point(0.0f) {
}

ReadCurrentLimitPoint::ReadCurrentLimitPoint(const std::vector<uint8_t>& raw) {
    if (raw.size() >= 8) {
        uint8_t data_type = from_raw<uint8_t>(raw, 0);
        uint8_t error_code = from_raw<uint8_t>(raw, 1);

        if (data_type == WinlineProtocol::DATA_TYPE_FLOAT && error_code == WinlineProtocol::ERROR_NORMAL) {
            limit_point = from_raw<float>(raw, 4);
        } else {
            EVLOG_warning << " Invalid current limit point response - DataType: 0x" << std::hex
                          << static_cast<int>(data_type) << ", ErrorCode: 0x" << static_cast<int>(error_code);
        }
    }
}

ReadCurrentLimitPoint::operator std::vector<uint8_t>() const {
    return build_read_command(REGISTER);
}

ReadDCBoardTemperature::ReadDCBoardTemperature() : temperature(0.0f) {
}

ReadDCBoardTemperature::ReadDCBoardTemperature(const std::vector<uint8_t>& raw) {
    if (raw.size() >= 8) {
        uint8_t data_type = from_raw<uint8_t>(raw, 0);
        uint8_t error_code = from_raw<uint8_t>(raw, 1);

        if (data_type == WinlineProtocol::DATA_TYPE_FLOAT && error_code == WinlineProtocol::ERROR_NORMAL) {
            temperature = from_raw<float>(raw, 4);
        } else {
            EVLOG_warning << " Invalid DC board temperature response - DataType: 0x" << std::hex
                          << static_cast<int>(data_type) << ", ErrorCode: 0x" << static_cast<int>(error_code);
        }
    }
}

ReadDCBoardTemperature::operator std::vector<uint8_t>() const {
    return build_read_command(REGISTER);
}

ReadAmbientTemperature::ReadAmbientTemperature() : temperature(0.0f) {
}

ReadAmbientTemperature::ReadAmbientTemperature(const std::vector<uint8_t>& raw) {
    if (raw.size() >= 8) {
        uint8_t data_type = from_raw<uint8_t>(raw, 0);
        uint8_t error_code = from_raw<uint8_t>(raw, 1);

        if (data_type == WinlineProtocol::DATA_TYPE_FLOAT && error_code == WinlineProtocol::ERROR_NORMAL) {
            temperature = from_raw<float>(raw, 4);
        } else {
            EVLOG_warning << " Invalid ambient temperature response - DataType: 0x" << std::hex
                          << static_cast<int>(data_type) << ", ErrorCode: 0x" << static_cast<int>(error_code);
        }
    }
}

ReadAmbientTemperature::operator std::vector<uint8_t>() const {
    return build_read_command(REGISTER);
}

ReadDCDCVersion::ReadDCDCVersion() : version(0) {
}

ReadDCDCVersion::ReadDCDCVersion(const std::vector<uint8_t>& raw) {
    if (raw.size() >= 8) {
        uint8_t data_type = from_raw<uint8_t>(raw, 0);
        uint8_t error_code = from_raw<uint8_t>(raw, 1);

        if (data_type == WinlineProtocol::DATA_TYPE_INTEGER && error_code == WinlineProtocol::ERROR_NORMAL) {
            // Version is in lower 16 bits (bytes 6-7) of response
            uint32_t version_data = from_raw<uint32_t>(raw, 4);
            version = static_cast<uint16_t>(version_data & 0xFFFF);
        } else {
            EVLOG_warning << " Invalid DCDC version response - DataType: 0x" << std::hex << static_cast<int>(data_type)
                          << ", ErrorCode: 0x" << static_cast<int>(error_code);
        }
    }
}

ReadDCDCVersion::operator std::vector<uint8_t>() const {
    return build_read_command(REGISTER);
}

ReadPFCVersion::ReadPFCVersion() : version(0) {
}

ReadPFCVersion::ReadPFCVersion(const std::vector<uint8_t>& raw) {
    if (raw.size() >= 8) {
        uint8_t data_type = from_raw<uint8_t>(raw, 0);
        uint8_t error_code = from_raw<uint8_t>(raw, 1);

        if (data_type == WinlineProtocol::DATA_TYPE_INTEGER && error_code == WinlineProtocol::ERROR_NORMAL) {
            // Version is in lower 16 bits (bytes 6-7) of response
            uint32_t version_data = from_raw<uint32_t>(raw, 4);
            version = static_cast<uint16_t>(version_data & 0xFFFF);
        } else {
            EVLOG_warning << " Invalid PFC version response - DataType: 0x" << std::hex << static_cast<int>(data_type)
                          << ", ErrorCode: 0x" << static_cast<int>(error_code);
        }
    }
}

ReadPFCVersion::operator std::vector<uint8_t>() const {
    return build_read_command(REGISTER);
}

// SET operations
SetVoltage::SetVoltage(float v) : voltage(v) {
}

SetVoltage::operator std::vector<uint8_t>() const {
    return build_set_command_float(REGISTER, voltage);
}

SetCurrent::SetCurrent(float c) : current(c) {
}

SetCurrent::operator std::vector<uint8_t>() const {
    // Winline requires current to be scaled by 1024
    uint32_t scaled_current = static_cast<uint32_t>(current * WinlineProtocol::CURRENT_SCALE_FACTOR);
    return build_set_command_integer(REGISTER, scaled_current);
}

SetCurrentLimitPoint::SetCurrentLimitPoint(float limit_point) : limit_point(limit_point) {
}

SetCurrentLimitPoint::operator std::vector<uint8_t>() const {
    // Current limit point is a float percentage (0.0 to 1.0)
    return build_set_command_float(REGISTER, limit_point);
}

SetVoltageUpperLimit::SetVoltageUpperLimit(float voltage_limit) : voltage_limit(voltage_limit) {
}

SetVoltageUpperLimit::operator std::vector<uint8_t>() const {
    // Voltage upper limit is a direct float value in volts
    return build_set_command_float(REGISTER, voltage_limit);
}

SetPowerControl::SetPowerControl(bool power_on) : power_on(power_on) {
}

SetPowerControl::operator std::vector<uint8_t>() const {
    uint32_t power_value = power_on ? WinlineProtocol::POWER_ON : WinlineProtocol::POWER_OFF;
    return build_set_command_integer(REGISTER, power_value);
}

SetGroupNumber::SetGroupNumber(uint8_t group_num) : group_number(group_num) {
}

SetGroupNumber::operator std::vector<uint8_t>() const {
    // Byte 7 lower 6 bits for group number (range 0~60), other bytes are 0
    uint32_t group_value = group_number & 0x3F; // Ensure only lower 6 bits
    return build_set_command_integer(REGISTER, group_value);
}

SetAltitude::SetAltitude(uint32_t alt) : altitude(alt) {
}

SetAltitude::operator std::vector<uint8_t>() const {
    // Clamp altitude to valid range
    uint32_t clamped_altitude =
        std::max(WinlineProtocol::ALTITUDE_MIN, std::min(altitude, WinlineProtocol::ALTITUDE_MAX));
    return build_set_command_integer(REGISTER, clamped_altitude);
}

SetInputMode::SetInputMode(uint32_t mode) : mode(mode) {
}

SetInputMode::operator std::vector<uint8_t>() const {
    return build_set_command_integer(REGISTER, mode);
}

SetAddressMode::SetAddressMode(uint32_t addr_mode) : mode(addr_mode) {
}

SetAddressMode::operator std::vector<uint8_t>() const {
    return build_set_command_integer(REGISTER, mode);
}

// Error Recovery Operations
SetOvervoltageReset::SetOvervoltageReset(bool enable) : enable(enable) {
}

SetOvervoltageReset::operator std::vector<uint8_t>() const {
    uint32_t reset_value = enable ? WinlineProtocol::RESET_ENABLE : WinlineProtocol::RESET_DISABLE;
    return build_set_command_integer(REGISTER, reset_value);
}

SetOvervoltageProtection::SetOvervoltageProtection(bool enable) : enable(enable) {
}

SetOvervoltageProtection::operator std::vector<uint8_t>() const {
    uint32_t protection_value = enable ? WinlineProtocol::RESET_DISABLE : WinlineProtocol::RESET_ENABLE;
    return build_set_command_integer(REGISTER, protection_value);
}

SetShortCircuitReset::SetShortCircuitReset(bool enable) : enable(enable) {
}

SetShortCircuitReset::operator std::vector<uint8_t>() const {
    uint32_t reset_value = enable ? WinlineProtocol::RESET_ENABLE : WinlineProtocol::RESET_DISABLE;
    return build_set_command_integer(REGISTER, reset_value);
}

} // namespace can_packet_acdc