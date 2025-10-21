// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef CAN_PACKETS_HPP
#define CAN_PACKETS_HPP

#include <linux/can.h>
#include <ostream>
#include <stdint.h>
#include <vector>

namespace WinlineProtocol {
// CAN Frame Constants
constexpr uint32_t CAN_EXTENDED_FLAG = 0x80000000U;

// Winline Protocol Constants
constexpr uint16_t PROTNO = 0x060;      // Protocol number (9 bits) - fixed for Winline
constexpr uint8_t FUNCTION_SET = 0x03;  // SET operation function code
constexpr uint8_t FUNCTION_READ = 0x10; // READ operation function code

// Address Constants
constexpr uint8_t MODULE_ADDRESS_MIN = 0x00;   // Minimum module address
constexpr uint8_t MODULE_ADDRESS_MAX = 0x3F;   // Maximum module address (63 modules)
constexpr uint8_t CONTROLLER_ADDRESS = 0xF0;   // Default controller address
constexpr uint8_t BROADCAST_ADDR = 0xFF;       // Individual broadcast address
constexpr uint8_t GROUP_BROADCAST_ADDR = 0xFE; // Group broadcast address

// Group Constants
constexpr uint8_t GROUP_MIN = 0x00; // Minimum group number
constexpr uint8_t GROUP_MAX = 0x07; // Maximum group number (3 bits)

// Response Error Codes (Winline Protocol Specification)
constexpr uint8_t ERROR_NORMAL = 0xF0; // Normal response - only valid response code
constexpr uint8_t ERROR_FAULT = 0xF2;  // Fault response (example from protocol doc)
// Note: Per Winline spec - "F0: Normal, Others: Fault, discard frame"
// Any error code != 0xF0 indicates a fault and frame should be discarded

// Error Recovery Operations
constexpr uint32_t RESET_ENABLE = 0x00010000;  // Enable reset command value
constexpr uint32_t RESET_DISABLE = 0x00000000; // Disable reset command value

// Response Data Types
constexpr uint8_t DATA_TYPE_FLOAT = 0x41;   // Float point data type indicator
constexpr uint8_t DATA_TYPE_INTEGER = 0x42; // Integer data type indicator

// Bit Masks for CAN ID encoding (Winline format)
constexpr uint32_t PROTNO_MASK = 0x1FF; // 9-bit mask for protocol number
constexpr uint8_t PTP_MASK = 0x01;      // 1-bit mask for point-to-point flag
constexpr uint8_t GROUP_MASK = 0x07;    // 3-bit mask for group number
constexpr uint8_t ADDRESS_MASK = 0xFF;  // 8-bit mask for addresses

// Bit Positions for CAN ID encoding (Winline format)
constexpr uint8_t SRCADDR_SHIFT = 3;  // Bits 10-3: Source address (8 bits)
constexpr uint8_t DSTADDR_SHIFT = 11; // Bits 18-11: Destination address (8 bits)
constexpr uint8_t GROUP_SHIFT = 0;    // Bits 2-0: Group number (3 bits)
constexpr uint8_t PTP_SHIFT = 19;     // Bit 19: Point-to-point flag
constexpr uint8_t PROTNO_SHIFT = 20;  // Bits 28-20: Protocol number

// Winline Register Definitions
namespace Registers {
// Read-only registers (used with FUNCTION_READ)
constexpr uint16_t VOLTAGE = 0x0001;               // Module output voltage (float)
constexpr uint16_t CURRENT = 0x0002;               // Module output current (float)
constexpr uint16_t CURRENT_LIMIT_POINT = 0x0003;   // Module current limit point (float)
constexpr uint16_t DC_BOARD_TEMPERATURE = 0x0004;  // Module DC board temperature (float)
constexpr uint16_t INPUT_VOLTAGE = 0x0005;         // Module input voltage (float)
constexpr uint16_t PFC_POSITIVE_VOLTAGE = 0x0008;  // PFC positive half bus voltage (float)
constexpr uint16_t PFC_NEGATIVE_VOLTAGE = 0x000A;  // PFC negative half bus voltage (float)
constexpr uint16_t AMBIENT_TEMPERATURE = 0x000B;   // Panel ambient temperature (float)
constexpr uint16_t AC_PHASE_A_VOLTAGE = 0x000C;    // AC phase A voltage (float)
constexpr uint16_t AC_PHASE_B_VOLTAGE = 0x000D;    // AC phase B voltage (float)
constexpr uint16_t AC_PHASE_C_VOLTAGE = 0x000E;    // AC phase C voltage (float)
constexpr uint16_t PFC_BOARD_TEMPERATURE = 0x0010; // PFC board temperature (float)
constexpr uint16_t RATED_OUTPUT_POWER = 0x0011;    // Module rated output power (float)
constexpr uint16_t RATED_OUTPUT_CURRENT = 0x0012;  // Module rated output current (float)
constexpr uint16_t STATUS = 0x0040;                // Current alarm/status (integer)
constexpr uint16_t GROUP_INFO = 0x0043;            // Group number & DIP switch address (integer)
constexpr uint16_t INPUT_POWER = 0x0048;           // Input power (integer, unit: 1W)
constexpr uint16_t CURRENT_ALTITUDE = 0x004A;      // Current set altitude (integer, unit: m)
constexpr uint16_t INPUT_WORKING_MODE = 0x004B;    // Current input working mode (integer)
constexpr uint16_t SERIAL_NUMBER_LOW = 0x0054;     // Node serial number low bytes (integer)
constexpr uint16_t SERIAL_NUMBER_HIGH = 0x0055;    // Node serial number high bytes (integer)
constexpr uint16_t DCDC_VERSION = 0x0056;          // DCDC version (integer)
constexpr uint16_t PFC_VERSION = 0x0057;           // PFC version (integer)

// Read/Write registers (used with FUNCTION_SET)
constexpr uint16_t SET_ALTITUDE = 0x0017;               // Set working altitude (integer, 1000-5000m)
constexpr uint16_t SET_OUTPUT_CURRENT = 0x001B;         // Set output current (integer, value*1024)
constexpr uint16_t SET_GROUP_NUMBER = 0x001E;           // Set group number (integer)
constexpr uint16_t SET_ADDRESS_MODE = 0x001F;           // Set address assignment mode (integer)
constexpr uint16_t SET_OUTPUT_VOLTAGE = 0x0021;         // Set output voltage (float)
constexpr uint16_t SET_CURRENT_LIMIT_POINT = 0x0022;    // Set current limit point (float)
constexpr uint16_t SET_VOLTAGE_UPPER_LIMIT = 0x0023;    // Set voltage upper limit (float)
constexpr uint16_t POWER_CONTROL = 0x0030;              // Power on/off control (integer)
constexpr uint16_t SET_OVERVOLTAGE_RESET = 0x0031;      // Set overvoltage reset (integer)
constexpr uint16_t SET_OVERVOLTAGE_PROTECTION = 0x003E; // Set overvoltage protection permission (integer)
constexpr uint16_t SET_SHORT_CIRCUIT_RESET = 0x0044;    // Set short circuit reset (integer)
constexpr uint16_t SET_INPUT_MODE = 0x0046;             // Set input mode (integer)
} // namespace Registers

// Power Control Values (for POWER_CONTROL register)
constexpr uint32_t POWER_ON = 0x00000000;  // Power on value
constexpr uint32_t POWER_OFF = 0x00010000; // Power off value

// Input Mode Values (for SET_INPUT_MODE register)
constexpr uint32_t INPUT_MODE_AC = 0x00000001; // AC input mode (default)
constexpr uint32_t INPUT_MODE_DC = 0x00000002; // DC input mode

// Address Assignment Mode Values (for SET_ADDRESS_MODE register)
constexpr uint32_t ADDRESS_AUTO = 0x00000000; // Automatically assigned
constexpr uint32_t ADDRESS_DIP = 0x00010000;  // Set by DIP switch (default)

// Current Scaling Factor (for SET_OUTPUT_CURRENT register)
constexpr uint32_t CURRENT_SCALE_FACTOR = 1024; // Current value = actual_current * 1024

// Altitude Limits (for SET_ALTITUDE register)
constexpr uint32_t ALTITUDE_MIN = 1000;     // Minimum altitude setting (meters)
constexpr uint32_t ALTITUDE_MAX = 5000;     // Maximum altitude setting (meters)
constexpr uint32_t ALTITUDE_DEFAULT = 1000; // Default altitude setting (meters)

// Unit Conversion Constants (legacy, may be useful for some conversions)
constexpr uint32_t VOLTAGE_TO_MV = 1000U; // Volts to millivolts (V * 1000 = mV)
constexpr uint32_t CURRENT_TO_MA = 1000U; // Amperes to milliamperes (A * 1000 = mA)
} // namespace WinlineProtocol

namespace can_packet_acdc {

// Winline CAN ID encoding/decoding functions
uint32_t encode_can_id(uint8_t source_address, uint8_t destination_address, uint8_t group_number, bool point_to_point);

uint8_t destination_address_from_can_id(uint32_t id);
uint8_t source_address_from_can_id(uint32_t id);
uint8_t group_number_from_can_id(uint32_t id);
bool point_to_point_from_can_id(uint32_t id);
uint16_t protocol_number_from_can_id(uint32_t id);

// Command building helpers for Winline protocol
uint32_t build_read_command_id(uint8_t source_address, uint8_t destination_address, uint8_t group_number,
                               bool point_to_point);
uint32_t build_set_command_id(uint8_t source_address, uint8_t destination_address, uint8_t group_number,
                              bool point_to_point);

// Command frame builders for Winline register-based communication
std::vector<uint8_t> build_read_command(uint16_t register_number);
std::vector<uint8_t> build_set_command(uint16_t register_number, const std::vector<uint8_t>& data);
std::vector<uint8_t> build_set_command_float(uint16_t register_number, float value);
std::vector<uint8_t> build_set_command_integer(uint16_t register_number, uint32_t value);

struct PowerModuleStatus {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::STATUS;

    PowerModuleStatus();
    PowerModuleStatus(const std::vector<uint8_t>& raw);
    friend std::ostream& operator<<(std::ostream& out, const PowerModuleStatus& self);
    operator std::vector<uint8_t>() const;

    // Winline status bits (based on Chart 2 in protocol document)
    bool module_fault{false};              // Bit 0: Module fault (red indicator)
    bool module_protection{false};         // Bit 1: Module protection (yellow indicator)
    bool sci_communication_failure{false}; // Bit 3: Module internal SCI communication failure
    bool input_mode_error{false};          // Bit 4: Input mode error/wiring error
    bool input_mode_mismatch{false};       // Bit 5: Input mode set by monitor doesn't match actual
    bool dcdc_overvoltage{false};          // Bit 7: DCDC overvoltage
    bool pfc_voltage_abnormal{false};      // Bit 8: PFC voltage abnormal (imbalance/over/under)
    bool ac_overvoltage{false};            // Bit 9: AC overvoltage
    bool ac_undervoltage{false};           // Bit 14: AC undervoltage
    bool can_communication_failure{false}; // Bit 16: CAN communication failure
    bool module_current_imbalance{false};  // Bit 17: Module current imbalance
    bool dcdc_on_off_status{false};        // Bit 22: DCDC On/off status (0:On, 1:Off)
    bool module_power_limiting{false};     // Bit 23: Module power limiting
    bool temperature_derating{false};      // Bit 24: Temperature derating
    bool ac_power_limiting{false};         // Bit 25: AC power limiting
    bool fan_fault{false};                 // Bit 27: Fan fault
    bool dcdc_short_circuit{false};        // Bit 28: DCDC short circuit
    bool dcdc_over_temperature{false};     // Bit 30: DCDC over temperature
    bool dcdc_output_overvoltage{false};   // Bit 31: DCDC output overvoltage
};

// Winline Register-Based Packet Structures

// READ operations (Function 0x10 + Register)
struct ReadVoltage {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::VOLTAGE;

    ReadVoltage();
    ReadVoltage(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    float voltage{0.0f};
};

struct ReadCurrent {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::CURRENT;

    ReadCurrent();
    ReadCurrent(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    float current{0.0f};
};

struct ReadCurrentLimitPoint {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::CURRENT_LIMIT_POINT;

    ReadCurrentLimitPoint();
    ReadCurrentLimitPoint(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    float limit_point{0.0f};
};

struct ReadDCBoardTemperature {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::DC_BOARD_TEMPERATURE;

    ReadDCBoardTemperature();
    ReadDCBoardTemperature(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    float temperature{0.0f};
};

struct ReadAmbientTemperature {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::AMBIENT_TEMPERATURE;

    ReadAmbientTemperature();
    ReadAmbientTemperature(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    float temperature{0.0f};
};

struct ReadRatedOutputPower {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::RATED_OUTPUT_POWER;

    ReadRatedOutputPower();
    ReadRatedOutputPower(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    float power{0.0f};
};

struct ReadRatedOutputCurrent {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::RATED_OUTPUT_CURRENT;

    ReadRatedOutputCurrent();
    ReadRatedOutputCurrent(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    float current{0.0f};
};

struct ReadGroupInfo {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::GROUP_INFO;

    ReadGroupInfo();
    ReadGroupInfo(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    uint8_t group_number{0};
    uint8_t dip_address{0};
};

struct ReadSerialNumber {
    static constexpr uint16_t REGISTER_LOW = WinlineProtocol::Registers::SERIAL_NUMBER_LOW;
    static constexpr uint16_t REGISTER_HIGH = WinlineProtocol::Registers::SERIAL_NUMBER_HIGH;

    ReadSerialNumber();
    ReadSerialNumber(uint32_t low_bytes, uint32_t high_bytes);
    operator std::vector<uint8_t>() const;
    std::string serial_number;
};

struct ReadDCDCVersion {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::DCDC_VERSION;

    ReadDCDCVersion();
    ReadDCDCVersion(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    uint16_t version{0};
};

struct ReadPFCVersion {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::PFC_VERSION;

    ReadPFCVersion();
    ReadPFCVersion(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    uint16_t version{0};
};

// SET operations (Function 0x03 + Register)
struct SetVoltage {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::SET_OUTPUT_VOLTAGE;

    SetVoltage(float voltage);
    operator std::vector<uint8_t>() const;
    float voltage{0.0f};
};

struct SetCurrent {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::SET_OUTPUT_CURRENT;

    SetCurrent(float current);
    operator std::vector<uint8_t>() const;
    float current{0.0f};
};

struct SetCurrentLimitPoint {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::SET_CURRENT_LIMIT_POINT;

    SetCurrentLimitPoint(float limit_point);
    operator std::vector<uint8_t>() const;
    float limit_point{1.0f}; // Default to 100% (no limiting)
};

struct SetVoltageUpperLimit {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::SET_VOLTAGE_UPPER_LIMIT;

    SetVoltageUpperLimit(float voltage_limit);
    operator std::vector<uint8_t>() const;
    float voltage_limit{0.0f};
};

struct SetPowerControl {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::POWER_CONTROL;

    SetPowerControl(bool power_on);
    operator std::vector<uint8_t>() const;
    bool power_on{false};
};

struct SetGroupNumber {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::SET_GROUP_NUMBER;

    SetGroupNumber(uint8_t group_number);
    operator std::vector<uint8_t>() const;
    uint8_t group_number{0};
};

struct SetAltitude {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::SET_ALTITUDE;

    SetAltitude(uint32_t altitude);
    operator std::vector<uint8_t>() const;
    uint32_t altitude{WinlineProtocol::ALTITUDE_DEFAULT};
};

struct SetInputMode {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::SET_INPUT_MODE;

    SetInputMode(uint32_t mode);
    operator std::vector<uint8_t>() const;
    uint32_t mode{WinlineProtocol::INPUT_MODE_AC};
};

struct SetAddressMode {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::SET_ADDRESS_MODE;

    SetAddressMode(uint32_t mode);
    operator std::vector<uint8_t>() const;
    uint32_t mode{WinlineProtocol::ADDRESS_DIP};
};

// Error Recovery Operations
struct SetOvervoltageReset {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::SET_OVERVOLTAGE_RESET;

    SetOvervoltageReset(bool enable);
    operator std::vector<uint8_t>() const;
    bool enable{false};
};

struct SetOvervoltageProtection {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::SET_OVERVOLTAGE_PROTECTION;

    SetOvervoltageProtection(bool enable);
    operator std::vector<uint8_t>() const;
    bool enable{false};
};

struct SetShortCircuitReset {
    static constexpr uint16_t REGISTER = WinlineProtocol::Registers::SET_SHORT_CIRCUIT_RESET;

    SetShortCircuitReset(bool enable);
    operator std::vector<uint8_t>() const;
    bool enable{false};
};

} // namespace can_packet_acdc

#endif // CAN_PACKETS_HPP