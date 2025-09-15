// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef CAN_PACKETS_HPP
#define CAN_PACKETS_HPP

#include <linux/can.h>
#include <ostream>
#include <stdint.h>
#include <vector>

namespace InfyProtocol {
// CAN Protocol Constants
constexpr uint8_t DEVICE_SINGLE_MODULE = 0x0A;
constexpr uint8_t DEVICE_GROUP_MODULE = 0x0B;
constexpr uint32_t CAN_EXTENDED_FLAG = 0x80000000U;

// Bit Masks for CAN ID encoding
constexpr uint8_t COMMAND_MASK = 0x3F;       // 6-bit mask for command number
constexpr uint8_t DEVICE_NUMBER_MASK = 0x0F; // 4-bit mask for device number
constexpr uint8_t ERROR_CODE_MASK = 0x07;    // 3-bit mask for error code

// Unit Conversion Constants
constexpr uint32_t VOLTAGE_TO_MV = 1000U; // Volts to millivolts (V * 1000 = mV)
constexpr uint32_t CURRENT_TO_MA = 1000U; // Amperes to milliamperes (A * 1000 = mA)

// Scaling Factors for Raw Data Conversion
constexpr float SCALING_FACTOR_0_1 = 0.1f;   // 0.1 scaling factor for voltage/current raw data
constexpr float SCALING_FACTOR_1_0 = 1.0f;   // 1.0 scaling factor for voltage raw data
constexpr float SCALING_FACTOR_10_0 = 10.0f; // 10.0 scaling factor for power raw data
} // namespace InfyProtocol

namespace can_packet_acdc {

uint32_t encode_can_id(uint8_t source_address, uint8_t destination_address, uint8_t command_number,
                       uint8_t device_number, uint8_t error_code);

uint8_t destination_address_from_can_id(uint32_t id);
uint8_t source_address_from_can_id(uint32_t id);
uint8_t command_number_from_can_id(uint32_t id);
uint8_t error_code_from_can_id(uint32_t id);

struct PowerModuleStatus {
    static constexpr uint8_t CMD_ID = 0x04;

    PowerModuleStatus();
    PowerModuleStatus(const std::vector<uint8_t>& raw);
    friend std::ostream& operator<<(std::ostream& out, const PowerModuleStatus& self);
    operator std::vector<uint8_t>() const;

    bool output_short_current{false};      // Error if all modules have this state
    bool sleeping{false};                  // Status
    bool discharge_abnormal{false};        // Vendor Warning
    bool dc_side_off{false};               // Status
    bool fault_alarm{false};               // Error if all modules have this state
    bool protection_alarm{false};          // Vendor Warning
    bool fan_fault_alarm{false};           // Vendor Warning
    bool over_temperature_alarm{false};    // Vendor Warning
    bool output_over_voltage_alarm{false}; // Vendor Warning
    bool walk_in_enable{false};            // Status
    bool communication_interrupt_alarm{false};
    bool power_limit_status{false};            // Status
    bool id_repeat_alarm{false};               // Status
    bool load_sharing_alarm{false};            // Vendor Warning
    bool input_phase_lost_alarm{false};        // Vendor Warning
    bool input_unbalanced_alarm{false};        // Vendor Warning
    bool input_low_voltage_alarm{false};       // Vendor Warning
    bool input_over_voltage_protection{false}; // Vendor Warning
    bool pfc_side_off{false};                  // Status
};

// New packet classes for V1.13 protocol

struct ReadModuleCount {
    static constexpr uint8_t CMD_ID = 0x02;

    ReadModuleCount();
    ReadModuleCount(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    uint8_t count{0};
};

struct ReadModuleVI {
    static constexpr uint8_t CMD_ID = 0x03;

    ReadModuleVI(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    float voltage{0.0f};
    float current{0.0f};
};

struct ReadModuleCapabilities {
    static constexpr uint8_t CMD_ID = 0x0A;

    ReadModuleCapabilities(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    float max_voltage{0.0f};
    float min_voltage{0.0f};
    float max_current{0.0f};
    float rated_power{0.0f};
};

struct ReadModuleVIAfterDiode {
    static constexpr uint8_t CMD_ID = 0x0C;

    ReadModuleVIAfterDiode(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    float v_ext{0.0f};
    float i_avail{0.0f};
};

struct ReadModuleBarcode {
    static constexpr uint8_t CMD_ID = 0x0B;

    ReadModuleBarcode(const std::vector<uint8_t>& raw);
    operator std::vector<uint8_t>() const;
    std::string serial_number; // Full barcode string (e.g., "081807123451V1704")
};

struct SetModuleVI {
    static constexpr uint8_t CMD_ID = 0x1C;

    SetModuleVI(float voltage, float current);
    operator std::vector<uint8_t>() const;
    float voltage{0.0f};
    float current{0.0f};
};

struct SetModuleOnOff {
    static constexpr uint8_t CMD_ID = 0x1A;

    SetModuleOnOff(bool on);
    operator std::vector<uint8_t>() const;
    bool on{false};
};

} // namespace can_packet_acdc

#endif // CAN_PACKETS_HPP