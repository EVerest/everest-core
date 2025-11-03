// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef CAN_PACKETS_HPP
#define CAN_PACKETS_HPP

#include <linux/can.h>
#include <ostream>
#include <stdint.h>
#include <vector>

namespace UU {

uint32_t encode_can_id(uint8_t destination_address);

inline uint8_t module_address_from_can_id(uint32_t id) {
    return (id >> 14) & 0x7F;
};

inline uint8_t monitor_adress_from_can_id(uint32_t id) {
    return (id >> 21) & 0x0F;
};

inline uint8_t production_date_from_can_id(uint32_t id) {
    return (id >> 9) & 0x1F;
};

inline uint8_t serial_num_low_from_can_id(uint32_t id) {
    return id & 0x1FF;
};

// Commands
const uint8_t CMD_READ = 0x23;
const uint8_t CMD_WRITE = 0x24;

// Addresses
const uint8_t ADDR_BROADCAST = 0x00;

// RX and TX packet definitions

enum class MessageType : uint8_t {
    SetData = 0,
    SetDataResponse = 1,
    ReadData = 2,
    ReadDataResponse = 3,
    ReadSerialNumberResponse = 4,
    AllSetData = 11,
    AllSetDataResponse = 12
};

// Note: naming in this enum is questionable, but follows the Can protocol documentation from UUGreenPower
enum class CommandType : uint8_t {
    Vout = 0,
    Iout_slow = 1,
    VoutReference = 2,
    IoutLimit = 3,
    ShutdownDCDC = 4,
    ReadSN = 5,
    ModuleStatus = 8,
    Vab = 20,
    Vbc = 21,
    Vca = 22,
    VfanReference = 26,
    Tin = 30,
    Iout_fastest = 47,
    FanSilentLevel = 62,
    GroupAddress = 89,
    HiMode_LoMode_Selection = 95,
    HiMode_LoMode_Status = 96,
    Vout_fast = 98,
    TrueHiLo_Status = 101,
    CurrentCapability = 104,
    CurrentAndCapability = 114
};

struct Packet {
    Packet(MessageType _message_type, CommandType _command_type, uint32_t _data = 0x00, uint8_t _group_address = 0x00) :
        message_type(_message_type), command_type(_command_type), data(_data), group_address(_group_address){};

    Packet(const std::vector<uint8_t> raw);
    friend std::ostream& operator<<(std::ostream& out, const Packet& self);
    operator std::vector<uint8_t>();

    uint8_t group_address{0};
    MessageType message_type{MessageType::ReadData};
    CommandType command_type{CommandType::ReadSN};
    uint16_t bytes23{0};
    uint32_t data{0};
};

} // namespace UU

#endif // CAN_PACKETS_HPP
