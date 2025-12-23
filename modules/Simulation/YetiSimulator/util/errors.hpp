// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#pragma once

#include <optional>
#include <tuple>

#include <utils/error.hpp>
#include <utils/error/error_factory.hpp>

enum class ErrorTarget {
    BoardSupport,
    ConnectorLock,
    Rcd,
    Powermeter
};

struct ErrorDefinition {
    const char* type;
    const char* sub_type{""};
    const char* message{""};
    Everest::error::Severity severity{Everest::error::Severity::High};
    ErrorTarget error_target;
};

Everest::error::Error create_error(const Everest::error::ErrorFactory& error_factory, const ErrorDefinition& def);

std::tuple<bool, std::optional<ErrorDefinition>> parse_error_type(const std::string& payload);

template <typename T> void forward_error(T& target, const Everest::error::Error& error, bool raise) {
    if (raise) {
        target->raise_error(error);
    } else {
        target->clear_error(error.type);
    }
}

namespace error_definitions {
inline const auto connector_lock_ConnectorLockUnexpectedClose =
    ErrorDefinition{"connector_lock/ConnectorLockUnexpectedClose"};

inline const auto evse_board_support_DiodeFault =
    ErrorDefinition{"evse_board_support/DiodeFault", "", "Simulated fault event"};

inline const auto evse_board_support_BrownOut =
    ErrorDefinition{"evse_board_support/BrownOut", "", "Simulated fault event"};

inline const auto evse_board_support_EnergyManagement =
    ErrorDefinition{"evse_board_support/EnergyManagement", "", "Simulated fault event"};

inline const auto evse_board_support_PermanentFault =
    ErrorDefinition{"evse_board_support/PermanentFault", "", "Simulated fault event"};

inline const auto evse_board_support_MREC2GroundFailure =
    ErrorDefinition{"evse_board_support/MREC2GroundFailure", "", "Simulated fault event"};

inline const auto evse_board_support_MREC3HighTemperature =
    ErrorDefinition{"evse_board_support/MREC3HighTemperature", "", "Simulated fault event"};

inline const auto evse_board_support_MREC4OverCurrentFailure =
    ErrorDefinition{"evse_board_support/MREC4OverCurrentFailure", "", "Simulated fault event"};

inline const auto evse_board_support_MREC5OverVoltage =
    ErrorDefinition{"evse_board_support/MREC5OverVoltage", "", "Simulated fault event"};

inline const auto evse_board_support_MREC6UnderVoltage =
    ErrorDefinition{"evse_board_support/MREC6UnderVoltage", "", "Simulated fault event"};

inline const auto evse_board_support_MREC8EmergencyStop =
    ErrorDefinition{"evse_board_support/MREC8EmergencyStop", "", "Simulated fault event"};

inline const auto evse_board_support_MREC10InvalidVehicleMode =
    ErrorDefinition{"evse_board_support/MREC10InvalidVehicleMode", "", "Simulated fault event"};

inline const auto evse_board_support_MREC14PilotFault =
    ErrorDefinition{"evse_board_support/MREC14PilotFault", "", "Simulated fault event"};

inline const auto evse_board_support_MREC15PowerLoss =
    ErrorDefinition{"evse_board_support/MREC15PowerLoss", "", "Simulated fault event"};

inline const auto evse_board_support_MREC17EVSEContactorFault =
    ErrorDefinition{"evse_board_support/MREC17EVSEContactorFault", "", "Simulated fault event"};

inline const auto evse_board_support_MREC18CableOverTempDerate =
    ErrorDefinition{"evse_board_support/MREC18CableOverTempDerate", "", "Simulated fault event"};

inline const auto evse_board_support_MREC19CableOverTempStop =
    ErrorDefinition{"evse_board_support/MREC19CableOverTempStop", "", "Simulated fault event"};

inline const auto evse_board_support_MREC20PartialInsertion =
    ErrorDefinition{"evse_board_support/MREC20PartialInsertion", "", "Simulated fault event"};

inline const auto evse_board_support_MREC23ProximityFault =
    ErrorDefinition{"evse_board_support/MREC23ProximityFault", "", "Simulated fault event"};

inline const auto evse_board_support_MREC24ConnectorVoltageHigh =
    ErrorDefinition{"evse_board_support/MREC24ConnectorVoltageHigh", "", "Simulated fault event"};

inline const auto evse_board_support_MREC25BrokenLatch =
    ErrorDefinition{"evse_board_support/MREC25BrokenLatch", "", "Simulated fault event"};

inline const auto evse_board_support_MREC26CutCable =
    ErrorDefinition{"evse_board_support/MREC26CutCable", "", "Simulated fault event"};

inline const auto evse_board_support_TiltDetected =
    ErrorDefinition{"evse_board_support/TiltDetected", "", "Simulated fault event"};

inline const auto evse_board_support_WaterIngressDetected =
    ErrorDefinition{"evse_board_support/WaterIngressDetected", "", "Simulated fault event"};

inline const auto evse_board_support_EnclosureOpen =
    ErrorDefinition{"evse_board_support/EnclosureOpen", "", "Simulated fault event"};

inline const auto ac_rcd_VendorError = ErrorDefinition{"ac_rcd/VendorError", "", "Simulated fault event"};

inline const auto ac_rcd_Selftest = ErrorDefinition{"ac_rcd/Selftest", "", "Simulated fault event"};

inline const auto ac_rcd_AC = ErrorDefinition{"ac_rcd/AC", "", "Simulated fault event"};

inline const auto ac_rcd_DC = ErrorDefinition{"ac_rcd/DC", "", "Simulated fault event"};

inline const auto ac_rcd_MREC2GroundFailure = ErrorDefinition{"ac_rcd/MREC2GroundFailure", "", "Simulated fault event"};

inline const auto connector_lock_ConnectorLockCapNotCharged =
    ErrorDefinition{"connector_lock/ConnectorLockCapNotCharged", "", "Simulated fault event"};

inline const auto connector_lock_ConnectorLockUnexpectedOpen =
    ErrorDefinition{"connector_lock/ConnectorLockUnexpectedOpen", "", "Simulated fault event"};

inline const auto connector_lock_ConnectorLockFailedLock =
    ErrorDefinition{"connector_lock/ConnectorLockFailedLock", "", "Simulated fault event"};

inline const auto connector_lock_ConnectorLockFailedUnlock =
    ErrorDefinition{"connector_lock/ConnectorLockFailedUnlock", "", "Simulated fault event"};

inline const auto connector_lock_MREC1ConnectorLockFailure =
    ErrorDefinition{"connector_lock/MREC1ConnectorLockFailure", "", "Simulated fault event"};

inline const auto connector_lock_VendorError =
    ErrorDefinition{"connector_lock/VendorError", "", "Simulated fault event"};

inline const auto powermeter_CommunicationFault =
    ErrorDefinition{"powermeter/CommunicationFault", "", "Simulated fault event", Everest::error::Severity::High,
                    ErrorTarget::Powermeter};

} // namespace error_definitions
