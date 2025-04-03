// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef ENUMS_HPP
#define ENUMS_HPP

// This contains types for all the JSON objects
#include <optional>
#include <string>
#include <vector>

namespace data {

// automatically created

// Enums
enum class ResponseErrorEnum {
    NoError,
    ErrorInvalidParameter,
    ErrorOutOfRange,
    ErrorValuesNotApplied,
    ErrorInvalidEVSEID,
    ErrorNoDataAvailable,
    ErrorUnknownError
};

enum class ChargeProtocolEnum {
    Unknown,
    IEC61851,
    DIN70121,
    ISO15118,
    ISO15118_20
};

enum class EVSEStateEnum {
    Unplugged,
    Disabled,
    Preparing,
    Reserved,
    AuthRequired,
    WaitingForEnergy,
    Charging,
    ChargingPausedEV,
    ChargingPausedEVSE,
    Finished
};

enum class ConnectorTypeEnum {
    ACSinglePhaseCable,
    ACThreePhaseCable,
    ACSinglePhaseSocket,
    ACThreePhaseSocket,
    DCCore,
    DCExtended,
    DCDual2,
    DCDual4,
    Other
};

enum class EVSEErrorEnum {
    NoError,
    power_supply_DC_HardwareFault,
    power_supply_DC_OverTemperature,
    power_supply_DC_UnderTemperature,
    power_supply_DC_UnderVoltageAC,
    power_supply_DC_OverVoltageAC,
    power_supply_DC_UnderVoltageDC,
    power_supply_DC_OverVoltageDC,
    power_supply_DC_OverCurrentAC,
    power_supply_DC_OverCurrentDC,
    power_supply_DC_VendorError,
    power_supply_DC_VendorWarning,
    evse_board_support_MREC2GroundFailure,
    evse_board_support_MREC3HighTemperature,
    evse_board_support_MREC4OverCurrentFailure,
    evse_board_support_MREC5OverVoltage,
    evse_board_support_MREC6UnderVoltage,
    evse_board_support_MREC8EmergencyStop,
    evse_board_support_MREC10InvalidVehicleMode,
    evse_board_support_MREC14PilotFault,
    evse_board_support_MREC15PowerLoss,
    evse_board_support_MREC17EVSEContactorFault,
    evse_board_support_MREC18CableOverTempDerate,
    evse_board_support_MREC19CableOverTempStop,
    evse_board_support_MREC20PartialInsertion,
    evse_board_support_MREC23ProximityFault,
    evse_board_support_MREC24ConnectorVoltageHigh,
    evse_board_support_MREC25BrokenLatch,
    evse_board_support_MREC26CutCable
    // ... Add remaining EVSEErrorEnum values
};

} // namespace data

#endif // ENUMS_HPP
