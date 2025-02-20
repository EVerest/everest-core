// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef OCPP201_ERROR_HANDLING_HPP
#define OCPP201_ERROR_HANDLING_HPP

#include <ocpp/v2/ocpp_types.hpp>
#include <ocpp_conversions.hpp>
#include <utils/error.hpp>

#include <unordered_map>

namespace module {
const std::unordered_map<std::string, std::string> MREC_ERROR_MAP = {
    {"connector_lock/MREC1ConnectorLockFailure", "CX001"},
    {"evse_board_support/MREC2GroundFailure", "CX002"},
    {"evse_board_support/MREC3HighTemperature", "CX003"},
    {"evse_board_support/MREC4OverCurrentFailure", "CX004"},
    {"evse_board_support/MREC5OverVoltage", "CX005"},
    {"evse_board_support/MREC6UnderVoltage", "CX006"},
    {"evse_board_support/MREC8EmergencyStop", "CX008"},
    {"evse_board_support/MREC10InvalidVehicleMode", "CX010"},
    {"evse_board_support/MREC14PilotFault", "CX014"},
    {"evse_board_support/MREC15PowerLoss", "CX015"},
    {"evse_board_support/MREC17EVSEContactorFault", "CX017"},
    {"evse_board_support/MREC18CableOverTempDerate", "CX018"},
    {"evse_board_support/MREC19CableOverTempStop", "CX019"},
    {"evse_board_support/MREC20PartialInsertion", "CX020"},
    {"evse_board_support/MREC23ProximityFault", "CX023"},
    {"evse_board_support/MREC24ConnectorVoltageHigh", "CX024"},
    {"evse_board_support/MREC25BrokenLatch", "CX025"},
    {"evse_board_support/MREC26CutCable", "CX026"},
    {"evse_manager/MREC4OverCurrentFailure", "CX004"},
    {"ac_rcd/MREC2GroundFailure", "CX002"},
};

const auto EVSE_MANAGER_INOPERATIVE_ERROR = "evse_manager/Inoperative";
const auto CHARGING_STATION_COMPONENT_NAME = "ChargingStation";
const auto EVSE_COMPONENT_NAME = "EVSE";
const auto CONNECTOR_COMPONENT_NAME = "Connector";
const auto PROBLEM_VARIABLE_NAME = "Problem";

/// \brief Derives the EventData from the given \p error, \p cleared and \p event_id parameters
ocpp::v2::EventData get_event_data(const Everest::error::Error& error, const bool cleared, const int32_t event_id) {
    ocpp::v2::EventData event_data;
    event_data.eventId = event_id; // This can theoretically conflict with eventIds generated in libocpp (e.g.
                                   // for monitoring events), but the spec does not strictly forbid that
    event_data.timestamp = ocpp::DateTime(error.timestamp);
    event_data.trigger = ocpp::v2::EventTriggerEnum::Alerting;
    event_data.cause = std::nullopt; // TODO: use caused_by when available within error object
    event_data.actualValue = cleared ? "false" : "true";

    if (MREC_ERROR_MAP.count(error.type)) {
        event_data.techCode = MREC_ERROR_MAP.at(error.type);
    } else {
        event_data.techCode = error.type;
    }

    event_data.techInfo = error.description;
    event_data.cleared = cleared;
    event_data.transactionId = std::nullopt;        // TODO: Do we need to set this here?
    event_data.variableMonitoringId = std::nullopt; // We dont need to set this for HardwiredNotification
    event_data.eventNotificationType = ocpp::v2::EventNotificationEnum::HardWiredNotification;

    // TODO: choose proper component
    const auto evse_id = error.origin.mapping.has_value() ? error.origin.mapping.value().evse : 0;
    if (evse_id == 0) {
        // component is ChargingStation
        event_data.component = {CHARGING_STATION_COMPONENT_NAME}; // TODO: use origin of error for mapping to component?
    } else if (not error.origin.mapping.value().connector.has_value()) {
        // component is EVSE
        ocpp::v2::EVSE evse = {evse_id};
        event_data.component = {EVSE_COMPONENT_NAME, std::nullopt, evse};
    } else {
        // component is Connector
        ocpp::v2::EVSE evse = {evse_id, std::nullopt, error.origin.mapping.value().connector.value()};
        event_data.component = {CONNECTOR_COMPONENT_NAME, std::nullopt, evse};
    }
    event_data.variable = {PROBLEM_VARIABLE_NAME}; // TODO: use type of error for mapping to variable?
    return event_data;
}
}; // namespace module

#endif // OCPP201_ERROR_HANDLING_HPP
