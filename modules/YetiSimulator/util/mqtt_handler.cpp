// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "mqtt_handler.hpp"
#include "everest/logging.hpp"
#include "nlohmann/json.hpp"

namespace module {

MqttHandler::MqttHandler(evse_board_supportImplBase* p_board_support, ac_rcdImplBase* p_rcd,
                         connector_lockImplBase* p_connector_lock) :
    errorHandler{p_board_support, p_rcd, p_connector_lock} {
}

void MqttHandler::handle_mqtt_payload(const std::string& payload) {
    const auto e = nlohmann::json::parse(payload);

    const auto raise = e["raise"] == "true";

    const auto error_type = std::string{e["error_type"]};

    if (error_type == "DiodeFault") {
        errorHandler.error_DiodeFault(raise);
    } else if (error_type == "BrownOut") {
        errorHandler.error_BrownOut(raise);
    } else if (error_type == "EnergyManagement") {
        errorHandler.error_EnergyManagement(raise);
    } else if (error_type == "PermanentFault") {
        errorHandler.error_PermanentFault(raise);
    } else if (error_type == "MREC2GroundFailure") {
        errorHandler.error_MREC2GroundFailure(raise);
    } else if (error_type == "MREC3HighTemperature") {
        errorHandler.error_MREC3HighTemperature(raise);
    } else if (error_type == "MREC4OverCurrentFailure") {
        errorHandler.error_MREC4OverCurrentFailure(raise);
    } else if (error_type == "MREC5OverVoltage") {
        errorHandler.error_MREC5OverVoltage(raise);
    } else if (error_type == "MREC6UnderVoltage") {
        errorHandler.error_MREC6UnderVoltage(raise);
    } else if (error_type == "MREC8EmergencyStop") {
        errorHandler.error_MREC8EmergencyStop(raise);
    } else if (error_type == "MREC10InvalidVehicleMode") {
        errorHandler.error_MREC10InvalidVehicleMode(raise);
    } else if (error_type == "MREC14PilotFault") {
        errorHandler.error_MREC14PilotFault(raise);
    } else if (error_type == "MREC15PowerLoss") {
        errorHandler.error_MREC15PowerLoss(raise);
    } else if (error_type == "MREC17EVSEContactorFault") {
        errorHandler.error_MREC17EVSEContactorFault(raise);
    } else if (error_type == "MREC18CableOverTempDerate") {
        errorHandler.error_MREC18CableOverTempDerate(raise);
    } else if (error_type == "MREC19CableOverTempStop") {
        errorHandler.error_MREC19CableOverTempStop(raise);
    } else if (error_type == "MREC20PartialInsertion") {
        errorHandler.error_MREC20PartialInsertion(raise);
    } else if (error_type == "MREC23ProximityFault") {
        errorHandler.error_MREC23ProximityFault(raise);
    } else if (error_type == "MREC24ConnectorVoltageHigh") {
        errorHandler.error_MREC24ConnectorVoltageHigh(raise);
    } else if (error_type == "MREC25BrokenLatch") {
        errorHandler.error_MREC25BrokenLatch(raise);
    } else if (error_type == "MREC26CutCable") {
        errorHandler.error_MREC26CutCable(raise);
    } else if (error_type == "ac_rcd_MREC2GroundFailure") {
        errorHandler.error_ac_rcd_MREC2GroundFailure(raise);
    } else if (error_type == "ac_rcd_VendorError") {
        errorHandler.error_ac_rcd_VendorError(raise);
    } else if (error_type == "ac_rcd_Selftest") {
        errorHandler.error_ac_rcd_Selftest(raise);
    } else if (error_type == "ac_rcd_AC") {
        errorHandler.error_ac_rcd_AC(raise);
    } else if (error_type == "ac_rcd_DC") {
        errorHandler.error_ac_rcd_DC(raise);
    } else if (error_type == "lock_ConnectorLockCapNotCharged") {
        errorHandler.error_lock_ConnectorLockCapNotCharged(raise);
    } else if (error_type == "lock_ConnectorLockUnexpectedOpen") {
        errorHandler.error_lock_ConnectorLockUnexpectedOpen(raise);
    } else if (error_type == "lock_ConnectorLockUnexpectedClose") {
        errorHandler.error_lock_ConnectorLockUnexpectedClose(raise);
    } else if (error_type == "lock_ConnectorLockFailedLock") {
        errorHandler.error_lock_ConnectorLockFailedLock(raise);
    } else if (error_type == "lock_ConnectorLockFailedUnlock") {
        errorHandler.error_lock_ConnectorLockFailedUnlock(raise);
    } else if (error_type == "lock_MREC1ConnectorLockFailure") {
        errorHandler.error_lock_MREC1ConnectorLockFailure(raise);
    } else if (error_type == "lock_VendorError") {
        errorHandler.error_lock_VendorError(raise);
    } else {
        EVLOG_error << "Unknown error raised via MQTT";
    }
}

} // namespace module