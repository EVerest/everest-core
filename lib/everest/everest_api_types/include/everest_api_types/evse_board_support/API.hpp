// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <stdint.h>
#include <string>

namespace everest::lib::API::V1_0::types::evse_board_support {

enum class Event {
    A,
    B,
    C,
    D,
    E,
    F,
    PowerOn,
    PowerOff,
    EvseReplugStarted,
    EvseReplugFinished,
    Disconnected,
};

struct BspEvent {
    Event event;
};

enum class ErrorEnum {
    DiodeFault,
    VentilationNotAvailable,
    BrownOut,
    EnergyManagement,
    PermanentFault,
    MREC2GroundFailure,
    MREC3HighTemperature,
    MREC4OverCurrentFailure,
    MREC5OverVoltage,
    MREC6UnderVoltage,
    MREC8EmergencyStop,
    MREC10InvalidVehicleMode,
    MREC14PilotFault,
    MREC15PowerLoss,
    MREC17EVSEContactorFault,
    MREC18CableOverTempDerate,
    MREC19CableOverTempStop,
    MREC20PartialInsertion,
    MREC23ProximityFault,
    MREC24ConnectorVoltageHigh,
    MREC25BrokenLatch,
    MREC26CutCable,
    ConnectorLockCapNotCharged,
    ConnectorLockUnexpectedOpen,
    ConnectorLockUnexpectedClose,
    ConnectorLockFailedLock,
    ConnectorLockFailedUnlock,
    MREC1ConnectorLockFailure,
    Selftest,
    DC,
    AC,
    TiltDetected,
    WaterIngressDetected,
    EnclosureOpen,
    VendorError,
    VendorWarning,
    CommunicationFault
};

struct Error {
    ErrorEnum type;
    std::optional<std::string> sub_type;
    std::optional<std::string> message;
};

enum class Connector_type {
    IEC62196Type2Cable,
    IEC62196Type2Socket,
};

struct HardwareCapabilities {
    float max_current_A_import;
    float min_current_A_import;
    int32_t max_phase_count_import;
    int32_t min_phase_count_import;
    float max_current_A_export;
    float min_current_A_export;
    int32_t max_phase_count_export;
    int32_t min_phase_count_export;
    bool supports_changing_phases_during_charging;
    bool supports_cp_state_E;
    Connector_type connector_type;
    std::optional<float> max_plug_temperature_C;
};

enum class Reason {
    DCCableCheck,
    DCPreCharge,
    FullPowerCharging,
    PowerOff,
};

struct PowerOnOff {
    bool allow_power_on{false};
    Reason reason{Reason::PowerOff};
};

enum class Ampacity {
    None,
    A_13,
    A_20,
    A_32,
    A_63_3ph_70_1ph,
};

struct ProximityPilot {
    Ampacity ampacity;
};

} // namespace everest::lib::API::V1_0::types::evse_board_support
