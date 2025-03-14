// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "errors.hpp"

#include "everest/logging.hpp"

std::tuple<bool, std::optional<ErrorDefinition>> parse_error_type(const std::string& payload) {

    // parsing, getting raise
    const auto e = json::parse(payload);

    const auto raise = (e.value("raise", "false") == "true") ? true : false;

    const auto error_type = static_cast<std::string>(e.at("error_type"));

    if (error_type == "DiodeFault") {
        return {raise, error_definitions::evse_board_support_DiodeFault};
    }
    if (error_type == "BrownOut") {
        return {raise, error_definitions::evse_board_support_BrownOut};
    }
    if (error_type == "EnergyManagement") {
        return {raise, error_definitions::evse_board_support_EnergyManagement};
    }
    if (error_type == "PermanentFault") {
        return {raise, error_definitions::evse_board_support_PermanentFault};
    }
    if (error_type == "MREC2GroundFailure") {
        return {raise, error_definitions::evse_board_support_MREC2GroundFailure};
    }
    if (error_type == "MREC3HighTemperature") {
        return {raise, error_definitions::evse_board_support_MREC3HighTemperature};
    }
    if (error_type == "MREC4OverCurrentFailure") {
        return {raise, error_definitions::evse_board_support_MREC4OverCurrentFailure};
    }
    if (error_type == "MREC5OverVoltage") {
        return {raise, error_definitions::evse_board_support_MREC5OverVoltage};
    }
    if (error_type == "MREC6UnderVoltage") {
        return {raise, error_definitions::evse_board_support_MREC6UnderVoltage};
    }
    if (error_type == "MREC8EmergencyStop") {
        return {raise, error_definitions::evse_board_support_MREC8EmergencyStop};
    }
    if (error_type == "MREC10InvalidVehicleMode") {
        return {raise, error_definitions::evse_board_support_MREC10InvalidVehicleMode};
    }
    if (error_type == "MREC14PilotFault") {
        return {raise, error_definitions::evse_board_support_MREC14PilotFault};
    }
    if (error_type == "MREC15PowerLoss") {
        return {raise, error_definitions::evse_board_support_MREC15PowerLoss};
    }
    if (error_type == "MREC17EVSEContactorFault") {
        return {raise, error_definitions::evse_board_support_MREC17EVSEContactorFault};
    }
    if (error_type == "MREC18CableOverTempDerate") {
        return {raise, error_definitions::evse_board_support_MREC18CableOverTempDerate};
    }
    if (error_type == "MREC19CableOverTempStop") {
        return {raise, error_definitions::evse_board_support_MREC19CableOverTempStop};
    }
    if (error_type == "MREC20PartialInsertion") {
        return {raise, error_definitions::evse_board_support_MREC20PartialInsertion};
    }
    if (error_type == "MREC23ProximityFault") {
        return {raise, error_definitions::evse_board_support_MREC23ProximityFault};
    }
    if (error_type == "MREC24ConnectorVoltageHigh") {
        return {raise, error_definitions::evse_board_support_MREC24ConnectorVoltageHigh};
    }
    if (error_type == "MREC25BrokenLatch") {
        return {raise, error_definitions::evse_board_support_MREC25BrokenLatch};
    }
    if (error_type == "MREC26CutCable") {
        return {raise, error_definitions::evse_board_support_MREC26CutCable};
    }
    if (error_type == "ac_rcd_MREC2GroundFailure") {
        return {raise, error_definitions::ac_rcd_MREC2GroundFailure};
    }
    if (error_type == "ac_rcd_VendorError") {
        return {raise, error_definitions::ac_rcd_VendorError};
    }
    if (error_type == "ac_rcd_Selftest") {
        return {raise, error_definitions::ac_rcd_Selftest};
    }
    if (error_type == "ac_rc_AC") {
        return {raise, error_definitions::ac_rcd_AC};
    }
    if (error_type == "ac_rcd_DC") {
        return {raise, error_definitions::ac_rcd_DC};
    }
    if (error_type == "lock_ConnectorLockCapNotCharged") {
        return {raise, error_definitions::connector_lock_ConnectorLockCapNotCharged};
    }
    if (error_type == "lock_ConnectorLockUnexpectedOpen") {
        return {raise, error_definitions::connector_lock_ConnectorLockUnexpectedOpen};
    }
    if (error_type == "lock_ConnectorLockUnexpectedClose") {
        return {raise, error_definitions::connector_lock_ConnectorLockUnexpectedClose};
    }
    if (error_type == "lock_ConnectorLockFailedLock") {
        return {raise, error_definitions::connector_lock_ConnectorLockFailedLock};
    }
    if (error_type == "lock_ConnectorLockFailedUnlock") {
        return {raise, error_definitions::connector_lock_ConnectorLockFailedUnlock};
    }
    if (error_type == "lock_MREC1ConnectorLockFailure") {
        return {raise, error_definitions::connector_lock_MREC1ConnectorLockFailure};
    }
    if (error_type == "lock_VendorError") {
        return {raise, error_definitions::connector_lock_VendorError};
    }
    EVLOG_error << "Unknown error raised via MQTT";
    return {raise, std::nullopt};
}

Everest::error::Error create_error(const Everest::error::ErrorFactory& error_factory, const ErrorDefinition& def) {
    return error_factory.create_error(def.type, def.sub_type, def.message, def.severity);
}
