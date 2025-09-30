// SPDX-License-Identifier: Apache-2.0
// Copyright Frickly Systems GmbH
// Copyright Pionix GmbH and Contributors to EVerest

#include "registers.hpp"

std::string_view to_string(DeviceFault_30001 code) noexcept {
    switch (code) {
    case DeviceFault_30001::NoFailure:
        return "No failure";
    case DeviceFault_30001::BrokenWire_L_PosNeg:
        return "Broken wire detection L(+)/L(-)";
    case DeviceFault_30001::BrokenWire_PE1_PE2:
        return "Broken wire detection PE1/PE2";
    case DeviceFault_30001::InternalFailure_TestMode_Int1:
        return "Internal failure detected in test mode (Int. 1)";
    case DeviceFault_30001::ParameterFailure_PotentiometerSetting:
        return "Parameter failures (Incorrect setting of potentiometers on the device)";
    case DeviceFault_30001::CommunicationFault_Modbus:
        return "Communication fault Modbus";
    case DeviceFault_30001::ChecksumFailure_EEPROM_Int2:
        return "Checksum failure EEPROM (Int. 2)";
    case DeviceFault_30001::InternalCommunicationFault_Int3:
        return "Internal communication fault (Int. 3)";
    case DeviceFault_30001::InternalError_Int4:
        return "Internal error 4 (Int. 4)";
    }
    return "Unknown code";
}

std::string_view to_string(DeviceState_30002 state) noexcept {
    switch (state) {
    case DeviceState_30002::Initializing:
        return "Initializing";
    case DeviceState_30002::Measuring:
        return "Ready and measuring";
    case DeviceState_30002::ErrorMode:
        return "Error mode";
    case DeviceState_30002::SelfTesting:
        return "Selftesting";
    case DeviceState_30002::AdvancedTest:
        return "Selftest in advanced test mode";
    case DeviceState_30002::MeasuringStopped:
        return "Measuring stopped";
    case DeviceState_30002::Measuring_AlarmExceeded:
        return "Measuring, alarm is exceeded";
    case DeviceState_30002::Measuring_PreAlarmExceeded:
        return "Measuring, pre-alarm is exceeded";
    }
    return "Unknown state";
}

uint32_t insulation_resistance_to_ohm(uint16_t insulation_resistance_100ohm) noexcept {
    if (insulation_resistance_100ohm == 0xFFFF) {
        return 2000001; // > 2MOhm
    }
    return static_cast<std::uint32_t>(insulation_resistance_100ohm) * 100;
}
