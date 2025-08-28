// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#include "CanPackets.hpp"
#include "Conversions.hpp"
#include <iostream>

namespace Huawei {

static std::string to_string(MessageId m) {
    switch (m) {

    case MessageId::Invalid:
        return "Invalid";
    case MessageId::ControlCommand:
        return "ControlCommand";
    case MessageId::ConfigurationCommand:
        return "ConfigurationCommand";
    case MessageId::QueryCommand:
        return "QueryCommand";
    case MessageId::ModuleReport:
        return "ModuleReport";
    case MessageId::QueryAllRealtimeData:
        return "QueryAllRealtimeData";
    case MessageId::QueryInherentModuleInformation:
        return "QueryInherentModuleInformation";
    case MessageId::ReceiveELabelFromMonitoringUnit:
        return "ReceiveELabelFromMonitoringUnit";
    case MessageId::ReturnELabelToMonitoringUnit:
        return "ReturnELabelToMonitoringUnit";
    case MessageId::OnlineLoadingStartup1:
        return "OnlineLoadingStartup1";
    case MessageId::OnlineLoadingStartup2:
        return "OnlineLoadingStartup2";
    case MessageId::FrameTransmission1:
        return "FrameTransmission1";
    case MessageId::FrameTransmission2:
        return "FrameTransmission2";
    case MessageId::FrameConfirmation1:
        return "FrameConfirmation1";
    case MessageId::FrameConfirmation2:
        return "FrameConfirmation2";
    case MessageId::DownloadEnd1:
        return "DownloadEnd1";
    case MessageId::DownloadEnd2:
        return "DownloadEnd2";
    case MessageId::LoadingFailure1:
        return "LoadingFailure1";
    case MessageId::LoadingFailure2:
        return "LoadingFailure2";
    case MessageId::CommonBlackBoxDataQuery:
        return "CommonBlackBoxDataQuery";
    }
    return "InvalidEnum/" + std::to_string(static_cast<uint16_t>(m));
}

static std::string to_string(SignalId id) {
    switch (id) {
    case SignalId::NotImplemented:
        return "NotImplemented";
    case SignalId::FeatureField:
        return "FeatureField";
    case SignalId::SerialNumber:
        return "SerialNumber";
    case SignalId::ProductionInformation1:
        return "ProductionInformation1";
    case SignalId::ProductionInformation2:
        return "ProductionInformation2";
    case SignalId::SwHwVersion:
        return "SwHwVersion";
    case SignalId::HardwareAddr:
        return "HardwareAddr";
    case SignalId::BatchQuery:
        return "BatchQuery";
    case SignalId::VoltageRange:
        return "VoltageRange";
    case SignalId::FlowLimitingRange:
        return "FlowLimitingRange";
    case SignalId::OutputVoltage:
        return "OutputVoltage";
    case SignalId::DefaultVoltage:
        return "DefaultVoltage";
    case SignalId::OutputOVPThreshold:
        return "OutputOVPThreshold";
    case SignalId::OutputCurrentLimit:
        return "OutputCurrentLimit";
    case SignalId::DefaultOutputCurrentLimit:
        return "DefaultOutputCurrentLimit";
    case SignalId::OutputPowerLimit:
        return "OutputPowerLimit";
    case SignalId::DefaultOutputPowerLimit:
        return "DefaultOutputPowerLimit";
    case SignalId::SetOnOffVoltageCurrent:
        return "SetOnOffVoltageCurrent";
    case SignalId::RunningTime:
        return "RunningTime";
    case SignalId::OutputCurrent:
        return "OutputCurrent";
    case SignalId::DefaultOutputCurrent:
        return "DefaultOutputCurrent";
    case SignalId::PFCOnOff:
        return "PFCOnOff";
    case SignalId::FanDutyCycle:
        return "FanDutyCycle";
    case SignalId::RealTimeClock:
        return "RealTimeClock";
    case SignalId::CanCommunicationTimeout:
        return "CanCommunicationTimeout";
    case SignalId::GroupSettingClearingCmd:
        return "GroupSettingClearingCmd";
    case SignalId::TemporaryGroupSettingClearingCmd:
        return "TemporaryGroupSettingClearingCmd";
    case SignalId::EmergencyPowerOffControl:
        return "EmergencyPowerOffControl";
    case SignalId::PowerOnOff:
        return "PowerOnOff";
    case SignalId::ClearOutputOVP:
        return "ClearOutputOVP";
    case SignalId::FullFanSpeed:
        return "FullFanSpeed";
    case SignalId::ModuleSearch:
        return "ModuleSearch";
    case SignalId::DCDCOnOff:
        return "DCDCOnOff";
    case SignalId::AllocateSwAddresses:
        return "AllocateSwAddresses";
    case SignalId::UnassociatedOutputOVP:
        return "UnassociatedOutputOVP";
    case SignalId::ClearShortCircuit:
        return "ClearShortCircuit";
    case SignalId::DisableFlowEqualization:
        return "DisableFlowEqualization";
    case SignalId::OutputSilentMode:
        return "OutputSilentMode";
    case SignalId::ClearRectifierOutput:
        return "ClearRectifierOutput";
    case SignalId::AutomaticOutputModeSwitch:
        return "AutomaticOutputModeSwitch";
    case SignalId::DefaultOutputMode:
        return "DefaultOutputMode";
    case SignalId::CanBaudRate:
        return "CanBaudRate";
    case SignalId::ACInputPower:
        return "ACInputPower";
    case SignalId::ACInputFrequency:
        return "ACInputFrequency";
    case SignalId::ACInputCurrent:
        return "ACInputCurrent";
    case SignalId::DCOutputPower:
        return "DCOutputPower";
    case SignalId::RealTimeEfficiency:
        return "RealTimeEfficiency";
    case SignalId::OutputVoltageQuery:
        return "OutputVoltageQuery";
    case SignalId::ACInputVoltageUV:
        return "ACInputVoltageUV";
    case SignalId::ACInputVoltageVW:
        return "ACInputVoltageVW";
    case SignalId::ACInputVoltageWU:
        return "ACInputVoltageWU";
    case SignalId::PFCTemperature:
        return "PFCTemperature";
    case SignalId::InternalTemperature:
        return "InternalTemperature";
    case SignalId::TemperatureAirIntake:
        return "TemperatureAirIntake";
    case SignalId::AlarmStatus:
        return "AlarmStatus";
    case SignalId::RatedModuleCurrent:
        return "RatedModuleCurrent";
    case SignalId::AlarmInformation:
        return "AlarmInformation";
    case SignalId::OutputCurrent1:
        return "OutputCurrent1";
    case SignalId::OutputCurrent2:
        return "OutputCurrent2";
    case SignalId::OutputVoltageCurrentStatus:
        return "OutputVoltageCurrentStatus";
    }
    return "InvalidEnum/" + std::to_string(static_cast<uint16_t>(id));
}

static std::string to_string(ErrorType e) {
    switch (e) {

    case ErrorType::Success:
        return "Success";
    case ErrorType::InvalidCommand:
        return "InvalidCommand";
    case ErrorType::AddressIdentificationInProgress:
        return "AddressIdentificationInProgress";
    }
    return "InvalidEnum/" + std::to_string(static_cast<uint16_t>(e));
}

// encode the 29 bits ID field
uint32_t encode_can_id(uint8_t module_address, MessageId message_id) {
    constexpr uint8_t PROTOCOL_ID = 0x0D;

    uint32_t id = PROTOCOL_ID << 23; // constant protocol id bits 23..28 (6 bits)

    uint32_t m_addr = module_address & 0x7F; // ensure it is only 7 bits long
    id |= (m_addr << 16);                    // Module address is bits 16..22 (7 bits long)

    id |= (static_cast<std::underlying_type<MessageId>::type>(message_id)
           << 8); // Command/Message id is bits 8..15 (8 bits long)

    id |= (1 << 7); // Source is control unit (bit 7)

    id |= (0x1F << 2); // Group ID: default is all bits 1 (no grouping). Bits 2..6 (5 bits long)

    // Bit 1: Set to 0 to use hardware address
    // Bit 0: Set to 0 there is no further data frame
    return id;
};

std::ostream& operator<<(std::ostream& out, const Packet& self) {
    out << "Huawei " << (self.packet_source_control_unit ? "H->M" : "M->H")
        << " Addr: " << std::to_string(self.module_address) << " MsgId: " << to_string(self.message_id)
        << " ErrorType: " << to_string(self.error_type) << " SignalId: " << to_string(self.signal_id)
        << " Byte2/3: " << std::to_string(self.byte2) << "/" << std::to_string(self.byte3)
        << " Data: " << std::to_string(self.data) << (self.last_packet ? "" : " [+]");
    return out;
}

// Packs the packet as 8 byte binary
Packet::operator std::vector<uint8_t>() {
    std::vector<uint8_t> out;
    // 4 bits ErrorType and 12 bits Signal ID
    to_raw(static_cast<uint8_t>(((static_cast<uint8_t>(error_type) & 0x0F) << 4) |
                                ((static_cast<uint16_t>(signal_id) & 0x0FFF) >> 8)),
           out);
    to_raw(static_cast<uint8_t>(((static_cast<uint16_t>(signal_id) & 0x00FF))), out);

    to_raw(byte2, out); // 1 byte
    to_raw(byte3, out); // 1 byte
    to_raw(data, out);  // 4 bytes of actual data

    return out;
}

// Constructor to initialize from received raw packet
Packet::Packet(const uint32_t can_id, const std::vector<uint8_t>& raw) {
    module_address = module_address_from_can_id(can_id);
    message_id = message_id_from_can_id(can_id);
    group_address = group_address_from_can_id(can_id);
    packet_source_control_unit = packet_source_from_can_id(can_id);
    protocol_id = protocol_id_from_can_id(can_id);
    last_packet = last_packet_from_can_id(can_id);
    signal_id = static_cast<SignalId>(static_cast<uint16_t>(((from_raw<uint8_t>(raw, 0) & 0x0F) << 8)) |
                                      (from_raw<uint8_t>(raw, 1)));
    error_type = static_cast<ErrorType>(from_raw<uint8_t>(raw, 0) >> 4);

    byte2 = from_raw<uint8_t>(raw, 2);
    byte3 = from_raw<uint8_t>(raw, 3);
    data = from_raw<uint32_t>(raw, 4);
}

} // namespace Huawei
