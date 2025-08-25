// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <everest/can/protocol/dpm1000.hpp>

#include <algorithm>
#include <cstring>
#include <type_traits>

#include <endian.h>

namespace can::protocol::dpm1000 {

static inline void clear_frame_data(struct can_frame& frame) {
    memset(frame.data, 0, sizeof(frame.data));
}

template <typename EnumType> static inline auto to_underlying(EnumType value) {
    return static_cast<std::underlying_type_t<EnumType>>(value);
}

static void set_msg_type(def::MessageType msg_type, struct can_frame& frame) {
    frame.data[0] = to_underlying(msg_type);
}

void set_header(struct can_frame& frame, uint8_t source, uint8_t destination) {
    const auto ptp_value = (destination != 0xFF);
    frame.can_id = (0b1UL << 31UL) | (def::MESSAGE_HEADER << def::MESSAGE_HEADER_BIT_SHIFT) |
                   (ptp_value << def::MESSAGE_HEADER_PTP_BIT_SHIFT) |
                   (destination << def::MESSAGE_HEADER_DSTADDR_BIT_SHIFT) |
                   (source << def::MESSAGE_HEADER_SRCADDR_BIT_SHIFT) | (0 << def::MESSAGE_HEADER_CNT_BIT_SHIFT) |
                   0b11; // bits 0 and 1 default to 1 ...
    frame.data[1] = to_underlying(def::ErrorType::NO_ERROR);
}

void power_on(struct can_frame& frame, bool switch_on, bool close_input_relay) {
    clear_frame_data(frame);
    frame.data[0] = to_underlying(def::MessageType::SET_DATA_REQUEST);

    frame.data[2] |= (((switch_on) ? 0 : 1) << def::SET_DATA_REQUEST_POWER_BIT_SHIFT);
    frame.data[2] |= (((close_input_relay) ? 0 : 1) << def::SET_DATA_REQUEST_INPUT_RELAY_BIT_SHIFT);
    frame.data[3] = 0x80; // FIXME (aw): literal
    frame.can_dlc = sizeof(frame.data);
}

void request_data(struct can_frame& frame, def::ReadValueType value_type) {
    clear_frame_data(frame);
    frame.data[0] = to_underlying(def::MessageType::REQUEST_DATA_BYTE);

    const auto value_type_raw = htobe16(to_underlying(value_type));
    memcpy(&frame.data[2], &value_type_raw, sizeof(value_type_raw));

    frame.can_dlc = sizeof(frame.data);
}

void set_data(struct can_frame& frame, def::SetValueType value_type, const std::vector<uint8_t>& payload) {
    clear_frame_data(frame);
    frame.data[0] = to_underlying(def::MessageType::SET_DATA);

    const auto value_type_raw = htobe16(to_underlying(value_type));
    memcpy(&frame.data[2], &value_type_raw, sizeof(value_type_raw));

    constexpr std::size_t MAX_PAYLOAD_SIZE = 4;
    const auto payload_size = std::min(payload.size(), MAX_PAYLOAD_SIZE);

    memcpy(&frame.data[4], payload.data(), payload_size);

    frame.can_dlc = sizeof(frame.data) - MAX_PAYLOAD_SIZE + payload_size;
}

uint8_t parse_source(const struct can_frame& frame) {
    return ((frame.can_id >> def::MESSAGE_HEADER_SRCADDR_BIT_SHIFT) & 0xFF);
}

uint16_t parse_msg_type(const struct can_frame& frame) {
    uint16_t retval;
    memcpy(&retval, &frame.data[2], sizeof(retval));

    return be16toh(retval);
}

} // namespace can::protocol::dpm1000
