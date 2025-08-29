// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "CanPackets.hpp"
#include "Conversions.hpp"
#include <iostream>

namespace UU {

static std::string to_string(CommandType m) {
    switch (m) {
    case CommandType::Vout:
        return "Vout";
    case CommandType::Iout_slow:
        return "Iout_slow";
    case CommandType::VoutReference:
        return "VoutReference";
    case CommandType::IoutLimit:
        return "IoutLimit";
    case CommandType::ShutdownDCDC:
        return "ShutdownDCDC";
    case CommandType::ReadSN:
        return "ReadSN";
    case CommandType::ModuleStatus:
        return "ModuleStatus";
    case CommandType::Vab:
        return "Vab";
    case CommandType::Vbc:
        return "Vbc";
    case CommandType::VfanReference:
        return "VfanReference";
    case CommandType::Tin:
        return "Tin";
    case CommandType::Iout_fastest:
        return "Iout_fastest";
    case CommandType::FanSilentLevel:
        return "FanSilentLevel";
    case CommandType::GroupAddress:
        return "GroupAddress";
    case CommandType::HiMode_LoMode_Selection:
        return "HiMode_LoMode_Selection";
    case CommandType::HiMode_LoMode_Status:
        return "HiMode_LoMode_Status";
    case CommandType::Vout_fast:
        return "Vout_fast";
    case CommandType::TrueHiLo_Status:
        return "TrueHiLo_Status";
    case CommandType::CurrentCapability:
        return "CurrentCapability";
    case CommandType::CurrentAndCapability:
        return "CurrentAndCapability";
    }
    return "Unknown";
}

static std::string to_string(MessageType m) {
    switch (m) {
    case MessageType::SetData:
        return "SetData";
    case MessageType::SetDataResponse:
        return "SetDataResponse";
    case MessageType::ReadData:
        return "ReadData";
    case MessageType::ReadDataResponse:
        return "ReadDataResponse";
    case MessageType::ReadSerialNumberResponse:
        return "ReadSerialNumberResponse";
    case MessageType::AllSetData:
        return "AllSetData";
    case MessageType::AllSetDataResponse:
        return "AllSetDataResponse";
    }
    return "Unknown";
}

// helper functions
uint32_t encode_can_id(uint8_t module_address) {
    uint32_t id = (module_address & 0x7F)
                  << 14; // Module address is bits 20:14 (7 bits). Use 0x00 for broadcast to all modules
    id |= (1 << 25);     // Protocol version 0x01 in bits 28:25 (4 bits)
    id |= (1 << 21);     // Monitor address is 0x01 in bits 24:21 (4 bits)
    // Production date and SerialNumber is not set on our outgoing monitor frames
    return id;
}

std::ostream& operator<<(std::ostream& out, const Packet& self) {
    out << "UUPacket: grp: " << std::to_string(self.group_address) << " MsgType: " << to_string(self.message_type)
        << " CmdType: " << to_string(self.command_type) << " Data: " << std::to_string(self.data);
    return out;
}

Packet::operator std::vector<uint8_t>() {
    std::vector<uint8_t> out;
    to_raw(static_cast<uint8_t>(((group_address & 0x0F) << 4) |
                                static_cast<std::underlying_type<MessageType>::type>(message_type) & 0x0F),
           out); // 4 bits group address, 4 bits message type
    to_raw(static_cast<uint8_t>(static_cast<std::underlying_type<MessageType>::type>(command_type) & 0x7F),
           out);                              // 7 bits of command type
    to_raw(static_cast<uint16_t>(0x00), out); // 2 reserved bytes
    to_raw(data, out);                        // 4 bytes of actual data

    return out;
}

Packet::Packet(const std::vector<uint8_t> raw) {
    uint8_t grp_msg_type = from_raw<uint8_t>(raw, 0);
    group_address = grp_msg_type >> 4;
    message_type = static_cast<MessageType>(grp_msg_type & 0x0F);
    command_type = static_cast<CommandType>(from_raw<uint8_t>(raw, 1));
    bytes23 = from_raw<uint16_t>(raw, 2);
    data = from_raw<uint32_t>(raw, 4);
}

} // namespace UU
