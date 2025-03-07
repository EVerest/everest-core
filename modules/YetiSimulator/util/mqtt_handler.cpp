// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "mqtt_handler.hpp"
#include "everest/logging.hpp"
#include "nlohmann/json.hpp"

#include <rcd/ac_rcdImpl.hpp>
#include <string>

namespace module {

MqttHandler::MqttHandler(evse_board_supportImplBase* p_board_support, ac_rcdImplBase* p_rcd,
                         connector_lockImplBase* p_connector_lock) :
    errorHandler{p_board_support, p_rcd, p_connector_lock}, error_factory{p_board_support->error_factory.get()} {
}

static std::tuple<bool, ErrorDefinition> parse_error_type(const std::string& payload) {
    // parsing, getting raise
    const auto e = json::parse(payload);

    const auto raise = e.value("raise", false);

    const std::string& error_type = e.at("error_type");

    if (error_type == "lock_ConnectorLockUnexpectedClose") {
        return {raise, error_definitions::connector_lock_ConnectorLockUnexpectedClose};
    }
    if (error_type == "evse_board_support/DiodeFault") {
        return {raise, error_definitions::evse_board_support_DiodeFault};
    }
    if (error_type == "evse_board_support/BrownOut") {
        return {raise, error_definitions::evse_board_support_BrownOut};
    }
    if (error_type == "evse_board_support/EnergyManagement") {
        return {raise, error_definitions::evse_board_support_EnergyManagement};
    }
    if (error_type == "evse_board_support/PermanentFault") {
        return {raise, error_definitions::evse_board_support_PermanentFault};
    }
    if (error_type == "evse_board_support/MREC2GroundFailure") {
        return {raise, error_definitions::evse_board_support_MREC2GroundFailure};
    }
    if (error_type == "evse_board_support/MREC3HighTemperature") {
        return {raise, error_definitions::evse_board_support_MREC3HighTemperature};
    }
    if (error_type == "evse_board_support/MREC4OverCurrentFailure") {
        return {raise, error_definitions::evse_board_support_MREC4OverCurrentFailure};
    }
    if (error_type == "evse_board_support/MREC5OverVoltage") {
        return {raise, error_definitions::evse_board_support_MREC5OverVoltage};
    }
    if (error_type == "evse_board_support/MREC6UnderVoltage") {
        return {raise, error_definitions::evse_board_support_MREC6UnderVoltage};
    }
    if (error_type == "evse_board_support/MREC8EmergencyStop") {
        return {raise, error_definitions::evse_board_support_MREC8EmergencyStop};
    }
    if (error_type == "evse_board_support/MREC10InvalidVehicleMode") {
        return {raise, error_definitions::evse_board_support_MREC10InvalidVehicleMode};
    }
    if (error_type == "evse_board_support/MREC14PilotFault") {
        return {raise, error_definitions::evse_board_support_MREC14PilotFault};
    }
    if (error_type == "evse_board_support/MREC15PowerLoss") {
        return {raise, error_definitions::evse_board_support_MREC15PowerLoss};
    }
    if (error_type == "evse_board_support/MREC17EVSEContactorFault") {
        return {raise, error_definitions::evse_board_support_MREC17EVSEContactorFault};
    }
    if (error_type == "evse_board_support/MREC18CableOverTempDerate") {
        return {raise, error_definitions::evse_board_support_MREC18CableOverTempDerate};
    }
    if (error_type == "evse_board_support/MREC19CableOverTempStop") {
        return {raise, error_definitions::evse_board_support_MREC19CableOverTempStop};
    }
    if (error_type == "evse_board_support/MREC20PartialInsertion") {
        return {raise, error_definitions::evse_board_support_MREC20PartialInsertion};
    }
    if (error_type == "evse_board_support/MREC23ProximityFault") {
        return {raise, error_definitions::evse_board_support_MREC23ProximityFault};
    }
    if (error_type == "evse_board_support/MREC24ConnectorVoltageHigh") {
        return {raise, error_definitions::evse_board_support_MREC24ConnectorVoltageHigh};
    }
    if (error_type == "evse_board_support/MREC25BrokenLatch") {
        return {raise, error_definitions::evse_board_support_MREC25BrokenLatch};
    }
    if (error_type == "evse_board_support/MREC26CutCable") {
        return {raise, error_definitions::evse_board_support_MREC26CutCable};
    }
    if (error_type == "ac_rcd/VendorError") {
        return {raise, error_definitions::ac_rcd_VendorError};
    }
    if (error_type == "ac_rcd/Selftest") {
        return {raise, error_definitions::ac_rcd_Selftest};
    }
    if (error_type == "ac_rcd/DC") {
        return {raise, error_definitions::ac_rcd_DC};
    }
    if (error_type == "connector_lock/ConnectorLockCapNotCharged") {
        return {raise, error_definitions::connector_lock_ConnectorLockCapNotCharged};
    }
    if (error_type == "connector_lock/ConnectorLockUnexpectedOpen") {
        return {raise, error_definitions::connector_lock_ConnectorLockUnexpectedOpen};
    }
    if (error_type == "connector_lock/ConnectorLockFailedLock") {
        return {raise, error_definitions::connector_lock_ConnectorLockFailedLock};
    }
    if (error_type == "connector_lock/ConnectorLockFailedUnlock") {
        return {raise, error_definitions::connector_lock_ConnectorLockFailedUnlock};
    }
    if (error_type == "connector_lock/MREC1ConnectorLockFailure") {
        return {raise, error_definitions::connector_lock_MREC1ConnectorLockFailure};
    }
    if (error_type == "connector_lock/VendorError") {
        return {raise, error_definitions::connector_lock_VendorError};
    }
    EVLOG_error << "Unknown error raised via MQTT";
    throw std::runtime_error("Invalid error type");
}

void MqttHandler::handle_mqtt_payload(const std::string& payload) const {
    const auto [raise, error_definition] = parse_error_type(payload);
    const auto error = create_error(*error_factory, error_definition);

    // TODO
    // if (error_definition.error_destination == ErrorDefinition::ErrorDestination::board_support) {
    //     forward_error<board_support::evse_board_supportImpl>(handle, error_definition, raise);
    // } else if (error_definition.error_destination == ErrorDefinition::ErrorDestination::connector_lock) {
    //     forward_error<connector_lock::connector_lockImpl>(handle, error_definition, raise);
    // } else if (error_definition.error_destination == ErrorDefinition::ErrorDestination::rcd) {
    //     forward_error<rcd::ac_rcdImpl>(handle, error_definition, raise);
    // } else {
    //     EVLOG_error << "Unknown error destination";
    // }
}

} // namespace module