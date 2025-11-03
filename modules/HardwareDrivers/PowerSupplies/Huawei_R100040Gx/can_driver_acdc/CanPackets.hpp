// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef CAN_PACKETS_HPP
#define CAN_PACKETS_HPP

#include <cstdint>
#include <linux/can.h>
#include <ostream>
#include <vector>

namespace Huawei {

constexpr uint32_t RTQ_RUNNING_TIME{1 << 0};
constexpr uint32_t RTQ_AC_INPUT_POWER{1 << 1};
constexpr uint32_t RTQ_AC_INPUT_FREQ{1 << 2};
constexpr uint32_t RTQ_AC_INPUT_CURRENT{1 << 3};
constexpr uint32_t RTQ_DC_OUTPUT_POWER{1 << 4};
constexpr uint32_t RTQ_EFFICIENCY{1 << 5};
constexpr uint32_t RTQ_OUTPUT_VOLTAGE{1 << 6};
constexpr uint32_t RTQ_DC_OUTPUT_CURRENT_LIMIT{1 << 7};
// Bit 8 seems to be a bug in the documentation (same as bit 9)
constexpr uint32_t RTQ_AC_INPUT_VOLTAGE_UV{1 << 9};
constexpr uint32_t RTQ_AC_INPUT_VOLTAGE_VW{1 << 10};
constexpr uint32_t RTQ_AC_INPUT_VOLTAGE_WU{1 << 11};
constexpr uint32_t RTQ_INTERNAL_TEMPERATURE{1 << 12};
constexpr uint32_t RTQ_TEMPERATURE_AIR_INTAKE{1 << 13};
constexpr uint32_t RTQ_OUTPUT_CURRENT_1{1 << 14};
constexpr uint32_t RTQ_OUTPUT_CURRENT_2{1 << 15};
constexpr uint32_t RTQ_ALARM_STATUS{1 << 16};
constexpr uint32_t RTQ_DC_OUTPUT_VOLTAGE_SETPOINT{1 << 17};
constexpr uint32_t RTQ_DC_OUTPUT_CURRENT_PERCENTAGE{1 << 18};
constexpr uint32_t RTQ_DC_OUTPUT_CURRENT_SETPOINT{1 << 19};
constexpr uint32_t RTQ_ALARM_INFORMATION{1 << 20};
constexpr uint32_t RTQ_SETTING_QUERY{1 << 21};
constexpr uint32_t RTQ_OUTPUT_VOLTAGE_CURRENT_STATUS{1 << 22};
constexpr uint32_t RTQ_ACTUAL_OUTPUT_POWER_LIMITING{1 << 23};

enum class MessageId : uint8_t {
    Invalid = 0x00,
    ControlCommand = 0x80,
    ConfigurationCommand = 0x81,
    QueryCommand = 0x82,
    ModuleReport = 0x83,
    QueryAllRealtimeData = 0x40,
    QueryInherentModuleInformation = 0x50,
    ReceiveELabelFromMonitoringUnit = 0xD1,
    ReturnELabelToMonitoringUnit = 0xD2,
    OnlineLoadingStartup1 = 0xD3,
    OnlineLoadingStartup2 = 0xE3,
    FrameTransmission1 = 0xD4,
    FrameTransmission2 = 0xE4,
    FrameConfirmation1 = 0xD5,
    FrameConfirmation2 = 0xE5,
    DownloadEnd1 = 0xD6,
    DownloadEnd2 = 0xE6,
    LoadingFailure1 = 0xD7,
    LoadingFailure2 = 0xE7,
    CommonBlackBoxDataQuery = 0xDA,
};

// encode the 29 bits ID field
uint32_t encode_can_id(uint8_t module_address, MessageId message_id);

inline uint8_t module_address_from_can_id(const uint32_t id) {
    return (id >> 16) & 0x7F;
};

inline uint8_t group_address_from_can_id(const uint32_t id) {
    return (id >> 2) & 0x1F;
};

inline bool packet_source_from_can_id(const uint32_t id) {
    return ((id >> 7) & 0x01) == 0x01;
};

inline bool last_packet_from_can_id(const uint32_t id) {
    return (id & 0x01) == 0x00;
};

inline uint8_t protocol_id_from_can_id(const uint32_t id) {
    return ((id >> 23) & 0x3F);
};

inline MessageId message_id_from_can_id(const uint32_t id) {
    const uint8_t m_id = (id >> 8) & 0xFF;
    return static_cast<MessageId>(m_id);
};

enum class ErrorType : uint8_t {
    Success = 0x00,
    InvalidCommand = 0x02,
    AddressIdentificationInProgress = 0x03,
};

enum class SignalId : uint16_t {
    NotImplemented = 0xFFFF,
    BatchQuery = 0x000,
    FeatureField = 0x001,
    SerialNumber = 0x002,
    ProductionInformation1 = 0x003,
    ProductionInformation2 = 0x004,
    SwHwVersion = 0x005,
    HardwareAddr = 0x006,
    VoltageRange = 0x009,
    FlowLimitingRange = 0x00A,
    OutputVoltage = 0x100,
    DefaultVoltage = 0x101,
    OutputOVPThreshold = 0x102,
    OutputCurrentLimit = 0x103,
    DefaultOutputCurrentLimit = 0x104,
    OutputPowerLimit = 0x105,
    DefaultOutputPowerLimit = 0x106,
    SetOnOffVoltageCurrent = 0x108,
    RunningTime = 0x10E,
    OutputCurrent = 0x10F,
    DefaultOutputCurrent = 0x110,
    PFCOnOff = 0x111,
    FanDutyCycle = 0x114,
    RealTimeClock = 0x117,
    CanCommunicationTimeout = 0x118,
    GroupSettingClearingCmd = 0x119,
    TemporaryGroupSettingClearingCmd = 0x11A,
    EmergencyPowerOffControl = 0x131,
    PowerOnOff = 0x132,
    ClearOutputOVP = 0x133,
    FullFanSpeed = 0x134,
    ModuleSearch = 0x135,
    DCDCOnOff = 0x136,
    AllocateSwAddresses = 0x13A,
    UnassociatedOutputOVP = 0x13C,
    ClearShortCircuit = 0x145,
    DisableFlowEqualization = 0x146,
    OutputSilentMode = 0x148,
    ClearRectifierOutput = 0x149,
    AutomaticOutputModeSwitch = 0x14A,
    DefaultOutputMode = 0x14C,
    CanBaudRate = 0x14D,
    ACInputPower = 0x170,
    ACInputFrequency = 0x171,
    ACInputCurrent = 0x172,
    DCOutputPower = 0x173,
    RealTimeEfficiency = 0x174,
    OutputVoltageQuery = 0x175,
    ACInputVoltageUV = 0x179,
    ACInputVoltageVW = 0x17A,
    ACInputVoltageWU = 0x17B,
    PFCTemperature = 0x17E,
    InternalTemperature = 0x17F,
    TemperatureAirIntake = 0x180,
    OutputCurrent1 = 0x181,
    OutputCurrent2 = 0x182,
    AlarmStatus = 0x183,
    RatedModuleCurrent = 0x188,
    AlarmInformation = 0x18F,
    OutputVoltageCurrentStatus = 0x191,
};

// Addresses
const uint8_t ADDR_BROADCAST = 0x00;

struct Packet {
    // Constructor for Packets to be sent to modules
    Packet(MessageId _message_id, SignalId _signal_id, uint8_t _byte2, uint8_t _byte3, uint32_t _data) :
        message_id(_message_id),
        signal_id(_signal_id),
        byte2(_byte2),
        byte3(_byte3),
        data(_data),
        packet_source_control_unit(true){};

    // Constructor for simple requests to module
    Packet(MessageId _message_id, SignalId _signal_id) :
        message_id(_message_id), signal_id(_signal_id), packet_source_control_unit(true){};

    // Constructor to initialize from received raw packet
    Packet(const uint32_t can_id, const std::vector<uint8_t>& raw);
    friend std::ostream& operator<<(std::ostream& out, const Packet& self);
    operator std::vector<uint8_t>();

    void set_module_address(uint8_t address) {
        module_address = address;
    };

    uint32_t get_can_id() {
        return encode_can_id(module_address, message_id);
    };

    // Part of CAN ID field
    uint8_t protocol_id{0x0D};
    uint8_t module_address{0};
    uint8_t group_address{0};
    MessageId message_id{MessageId::Invalid};

    // In CAN payload bytes
    ErrorType error_type{ErrorType::Success};
    uint8_t byte2{0};
    uint8_t byte3{0};
    uint32_t data{0};
    SignalId signal_id{SignalId::NotImplemented};
    bool packet_source_control_unit{false};

    bool last_packet{true};
};

} // namespace Huawei

#endif // CAN_PACKETS_HPP
