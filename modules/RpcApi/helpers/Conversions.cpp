// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#include "Conversions.hpp"

namespace types {
namespace json_rpc_api {

EVSEStateEnum evse_manager_session_event_to_evse_state(types::evse_manager::SessionEvent state) {
    using Event = types::evse_manager::SessionEventEnum;
    using State = types::json_rpc_api::EVSEStateEnum;

    switch (state.event) {
    case Event::Enabled:
        return State::Unplugged;
    case Event::Disabled:
        return State::Disabled;
    case Event::AuthRequired:
        return State::AuthRequired;
    case Event::PrepareCharging:
    case Event::SessionStarted:
    case Event::TransactionStarted:
        return State::Preparing;
    case Event::ChargingResumed:
    case Event::ChargingStarted:
        return State::Charging;
    case Event::ChargingPausedEV:
        return State::ChargingPausedEV;
    case Event::ChargingPausedEVSE:
        return State::ChargingPausedEVSE;
    case Event::WaitingForEnergy:
        return State::WaitingForEnergy;
    case Event::ChargingFinished:
        return State::Finished;
    case Event::StoppingCharging:
        return State::FinishedEV;
    case Event::TransactionFinished: {
        if (state.transaction_finished.has_value() &&
            state.transaction_finished->reason == types::evse_manager::StopTransactionReason::Local) {
            return State::FinishedEVSE;
        } else {
            return State::Finished;
        }
        break;
    }
    case Event::PluginTimeout:
        return State::AuthTimeout;
    case Event::ReservationStart:
        return State::Reserved;
    case Event::ReservationEnd:
    case Event::SessionFinished:
        return State::Unplugged;
    case Event::ReplugStarted:
    case Event::ReplugFinished:
    default:
        return State::Unknown;
    }
}
ChargeProtocolEnum evse_manager_protocol_to_charge_protocol(std::string protocol) {
    if (protocol == "IEC61851-1") {
        return ChargeProtocolEnum::IEC61851;
    } else if (protocol == "DIN70121") {
        return ChargeProtocolEnum::DIN70121;
    } else if (protocol.compare(0, 11, "ISO15118-20") == 0) {
        return ChargeProtocolEnum::ISO15118_20;
    }
    // This check must be after the ISO15118-20 check
    else if (protocol.compare(0, 10, "ISO15118-2") == 0) {
        return ChargeProtocolEnum::ISO15118;
    } else {
        return ChargeProtocolEnum::Unknown;
    }
}

types::json_rpc_api::ErrorObj everest_error_to_rpc_error(const Everest::error::Error& error_object) {
    std::vector<types::json_rpc_api::ErrorObj> rpc_errors;

    types::json_rpc_api::ErrorObj rpc_error;
    rpc_error.type = error_object.type;
    rpc_error.description = error_object.description;
    rpc_error.message = error_object.message;

    switch (error_object.severity) {
    case Everest::error::Severity::High:
        rpc_error.severity = types::json_rpc_api::Severity::High;
        break;
    case Everest::error::Severity::Medium:
        rpc_error.severity = types::json_rpc_api::Severity::Medium;
        break;
    case Everest::error::Severity::Low:
        rpc_error.severity = types::json_rpc_api::Severity::Low;
        break;
    default:
        throw std::out_of_range("Provided severity " + std::to_string(static_cast<int>(error_object.severity)) +
                                " could not be converted to enum of type SeverityEnum");
    }
    rpc_error.origin.module_id = error_object.origin.module_id;
    rpc_error.origin.implementation_id = error_object.origin.implementation_id;

    if (error_object.origin.mapping.has_value()) {
        rpc_error.origin.evse_index = error_object.origin.mapping.value().evse;

        if (error_object.origin.mapping.value().connector.has_value()) {
            rpc_error.origin.connector_index = error_object.origin.mapping.value().connector.value();
        }
    }

    rpc_error.origin.evse_index = 0;
    rpc_error.origin.connector_index = 0;

    rpc_error.timestamp = Everest::Date::to_rfc3339(error_object.timestamp);
    rpc_error.uuid = error_object.uuid.to_string();

    return rpc_error;
}

} // namespace json_rpc_api
} // namespace types
