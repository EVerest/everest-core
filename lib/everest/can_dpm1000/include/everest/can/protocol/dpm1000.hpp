// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef CAN_PROTOCOL_DPM1000_HPP
#define CAN_PROTOCOL_DPM1000_HPP

#include <cstdint>
#include <vector>

#include <linux/can.h>

namespace can::protocol::dpm1000 {
namespace def {

enum class ErrorType : uint8_t {
    NO_ERROR = 0xf0,
    INVALID_NODE_ADDRESS = 0xf1,
    INVALID_COMMAND = 0xf2,
    DATA_VERIFICATION_FAULT = 0xf3,
    ADDRESS_RECOGNITION = 0xf4,
};

enum class MessageType : uint8_t {
    SET_DATA_REQUEST = 0x00,
    REQUEST_DATA_BYTE = 0x01,
    RESPONSE_REQUEST = 0x41,
    SET_DATA = 0x03,
    RESPONSE_CONFIGURATION = 0x43,
};

constexpr auto SET_DATA_REQUEST_INPUT_RELAY_BIT_SHIFT = 2;
constexpr auto SET_DATA_REQUEST_POWER_BIT_SHIFT = 7;

enum class ReadValueType : uint16_t {
    VOLTAGE = 0x0001,
    CURRENT_REAL_PART = 0x0002,
    CURRENT_LIMIT = 0x0003,
    DCDC_TEMPERATURE = 0x0004,
    AC_VOLTAGE = 0x0005,
    VOLTAGE_LIMIT = 0x0006,
    CURRENT = 0x0007, // how is this different from the real part?
    PFC0_VOLTAGE = 0x0008,
    PFC1_VOLTAGE = 0x000A,
    ENV_TEMPERATURE = 0x000B,
    AC_VOLTAGE_PHASE_A = 0x000C,
    AC_VOLTAGE_PHASE_B = 0x000D,
    AC_VOLTAGE_PHASE_C = 0x000E,
    PFC_TEMPERATURE = 0x0010,
    POWER_LIMIT = 0x0014,
    ALARM = 0x0040,
    DEFAULT_CURRENT_LIMIT = 0x0019,
};

enum class SetValueType : uint16_t {
    DEFAULT_CURRENT_LIMIT = 0x0019,
    POWER_LIMIT = 0x0020,
    VOLTAGE = 0x0021,
    CURRENT_LIMIT = 0x0022,
    SERIES_PARALLEL_MODE = 0x0023,
    DEFAULT_OUTPUT_VOLTAGE = 0x0024,
    SWITCH_ON_OFF_SETTING = 0x0030,
    INPUT_RELAY_POWER_OFF_SETTING = 0x0035,
};

enum class Alarm : uint8_t {
    FUSE_BURN_OUT = 2,
    PFC_DCDC_COMMUNICATION_ERROR = 3,
    UNBALANCED_BUS_VOLTAGE = 6,
    BUS_OVER_VOLTAGE = 7,
    BUS_ABNORMAL_VOLTAGE = 8,
    PHASE_OVER_VOLTAGE = 9,
    ID_NUMBER_REPETITION = 10,
    BUS_UNDER_VOLTAGE = 11,
    PHASE_LOSS = 12,
    PHASE_UNDER_VOLTAGE = 14,
    CAN_COMMUNICATION_FAULT = 16,
    DCDC_UNEVEN_CURRENT_SHARING = 17,
    PFC_POWER_OFF = 19,
    FAN_FULL_SPEED = 21,
    DCDC_POWER_OFF = 22,
    POWER_LIMITING = 23,
    TEMPERATURE_POWER_LIMITING = 24,
    AC_POWER_LIMITING = 25,
    DCDC_EEPROM_FAULTS = 26,
    FAN_FAULTS = 27,
    DCDC_SHORT_CIRCUIT = 28,
    PFC_EEPROM_FAULTS = 29,
    DCDC_OVER_TEMPERATURE = 30,
    DCDC_OUTPUT_OVER_VOLTAGE = 31,
};

constexpr uint16_t MESSAGE_HEADER = 0b001100000;
constexpr uint16_t MESSAGE_HEADER_MASK = 0b111111111;
constexpr auto MESSAGE_HEADER_BIT_SHIFT = 20;
constexpr auto MESSAGE_HEADER_PTP_BIT_SHIFT = 19;
constexpr auto MESSAGE_HEADER_DSTADDR_BIT_SHIFT = 11;
constexpr auto MESSAGE_HEADER_SRCADDR_BIT_SHIFT = 3;
constexpr auto MESSAGE_HEADER_CNT_BIT_SHIFT = 2;

constexpr auto ERROR_FLAG_BIT_SHIFT = 7;

// FIXME (aw): unknown ValueTypes
// CURRENT_ALARM_STATUS = 0x0040 (is this get or set?)
// MODULE_GROUPING_SETTINGS = 0x0048 (is this get or set?)

} // namespace def

int dumb_function();

void set_header(struct can_frame&, uint8_t source, uint8_t destination = 0xFF);

void power_on(struct can_frame&, bool switch_on, bool close_input_relay);

void request_data(struct can_frame&, def::ReadValueType);

void set_data(struct can_frame&, def::SetValueType, const std::vector<uint8_t>& payload);

uint8_t parse_source(const struct can_frame&);
uint16_t parse_msg_type(const struct can_frame&);

inline bool is_error_flag_set(const struct can_frame& frame) {
    return (frame.data[0] >> def::ERROR_FLAG_BIT_SHIFT);
}

} // namespace can::protocol::dpm1000

#endif // CAN_PROTOCOL_DPM1000_HPP
