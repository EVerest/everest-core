// SPDX-License-Identifier: Apache-2.0
// Copyright Frickly Systems GmbH
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <cstdint>
#include <string>

enum class DeviceFault_30001 : std::uint16_t {
    /// 0: No failure
    NoFailure = 0,
    /// 1: Broken wire detection L(+)/L(-)
    BrokenWire_L_PosNeg = 1,
    /// 2: Broken wire detection PE1/PE2
    BrokenWire_PE1_PE2 = 2,
    /// 3: Internal failure detected in test mode (Int. 1)
    InternalFailure_TestMode_Int1 = 3,
    /// 4: Parameter failures (Incorrect potentiometer settings on the device)
    ParameterFailure_PotentiometerSetting = 4,
    /// 9: Communication fault Modbus
    CommunicationFault_Modbus = 9,
    /// 10: Checksum failure EEPROM (Int. 2)
    ChecksumFailure_EEPROM_Int2 = 10,
    /// 11: Internal communication fault (Int. 3)
    InternalCommunicationFault_Int3 = 11,
    /// 12: Internal error 4 (Int. 4)
    InternalError_Int4 = 12
};

std::string_view to_string(DeviceFault_30001 code) noexcept;

enum class DeviceState_30002 : std::uint8_t {
    /// 0: Device initializing
    Initializing = 0,
    /// 1: Device is ready and in measuring mode, no response value is exceeded
    Measuring = 1,
    /// 2: Device in error mode
    ErrorMode = 2,
    /// 3: Device in selftesting
    SelfTesting = 3,
    /// 4: Device in advanced test
    AdvancedTest = 4,
    /// 5: Measuring function stopped
    MeasuringStopped = 5,
    /// 6: Device in measuring mode, response value alarm is exceeded
    Measuring_AlarmExceeded = 6,
    /// 7: Device in measuring mode, response value pre-alarm is exceeded
    Measuring_PreAlarmExceeded = 7
};

std::string_view to_string(DeviceState_30002 state) noexcept;

/**
 * @brief convert a insulation resistance (registers 32001 and 32004) to kOhm
 * @note according to datasheet, the value 0xFFFF means > 2MOhm
 */
uint32_t insulation_resistance_to_ohm(uint16_t insulation_resistance_100ohm) noexcept;
