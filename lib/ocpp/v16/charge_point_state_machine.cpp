// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <ocpp/v16/charge_point_state_machine.hpp>
#include <ocpp/v16/enums.hpp>

#include <everest/logging.hpp>
#include <stddef.h>
#include <stdexcept>
#include <utility>

namespace ocpp {
namespace v16 {

static const FSMDefinition FSM_DEF = {
    {FSMState::Available,
     {
         {FSMEvent::UsageInitiated, FSMState::Preparing},
         {FSMEvent::StartCharging, FSMState::Charging},
         {FSMEvent::PauseChargingEV, FSMState::SuspendedEV},
         {FSMEvent::PauseChargingEVSE, FSMState::SuspendedEVSE},
         {FSMEvent::ReserveConnector, FSMState::Reserved},
         {FSMEvent::ChangeAvailabilityToUnavailable, FSMState::Unavailable},
     }},
    {FSMState::Preparing,
     {
         {FSMEvent::BecomeAvailable, FSMState::Available},
         {FSMEvent::StartCharging, FSMState::Charging},
         {FSMEvent::PauseChargingEV, FSMState::SuspendedEV},
         {FSMEvent::PauseChargingEVSE, FSMState::SuspendedEVSE},
         {FSMEvent::TransactionStoppedAndUserActionRequired, FSMState::Finishing},
     }},
    {FSMState::Charging,
     {
         {FSMEvent::BecomeAvailable, FSMState::Available},
         {FSMEvent::PauseChargingEV, FSMState::SuspendedEV},
         {FSMEvent::PauseChargingEVSE, FSMState::SuspendedEVSE},
         {FSMEvent::TransactionStoppedAndUserActionRequired, FSMState::Finishing},
         {FSMEvent::ChangeAvailabilityToUnavailable, FSMState::Unavailable},
     }},
    {FSMState::SuspendedEV,
     {
         {FSMEvent::BecomeAvailable, FSMState::Available},
         {FSMEvent::StartCharging, FSMState::Charging},
         {FSMEvent::PauseChargingEVSE, FSMState::SuspendedEVSE},
         {FSMEvent::TransactionStoppedAndUserActionRequired, FSMState::Finishing},
         {FSMEvent::ChangeAvailabilityToUnavailable, FSMState::Unavailable},
     }},
    {FSMState::SuspendedEVSE,
     {
         {FSMEvent::BecomeAvailable, FSMState::Available},
         {FSMEvent::StartCharging, FSMState::Charging},
         {FSMEvent::PauseChargingEV, FSMState::SuspendedEV},
         {FSMEvent::TransactionStoppedAndUserActionRequired, FSMState::Finishing},
         {FSMEvent::ChangeAvailabilityToUnavailable, FSMState::Unavailable},
     }},
    {FSMState::Finishing,
     {
         {FSMEvent::BecomeAvailable, FSMState::Available},
         {FSMEvent::UsageInitiated, FSMState::Preparing},
         {FSMEvent::ChangeAvailabilityToUnavailable, FSMState::Unavailable},
     }},
    {FSMState::Reserved,
     {
         {FSMEvent::BecomeAvailable, FSMState::Available},
         {FSMEvent::UsageInitiated, FSMState::Preparing},
         {FSMEvent::ChangeAvailabilityToUnavailable, FSMState::Unavailable},
     }},
    {FSMState::Unavailable,
     {
         {FSMEvent::BecomeAvailable, FSMState::Available},
         {FSMEvent::UsageInitiated, FSMState::Preparing},
         {FSMEvent::StartCharging, FSMState::Charging},
         {FSMEvent::PauseChargingEV, FSMState::SuspendedEV},
         {FSMEvent::PauseChargingEVSE, FSMState::SuspendedEVSE},
     }},
    {FSMState::Faulted,
     {
         {FSMEvent::I1_ReturnToAvailable, FSMState::Available},
         {FSMEvent::I2_ReturnToPreparing, FSMState::Preparing},
         {FSMEvent::I3_ReturnToCharging, FSMState::Charging},
         {FSMEvent::I4_ReturnToSuspendedEV, FSMState::SuspendedEV},
         {FSMEvent::I5_ReturnToSuspendedEVSE, FSMState::SuspendedEVSE},
         {FSMEvent::I6_ReturnToFinishing, FSMState::Finishing},
         {FSMEvent::I7_ReturnToReserved, FSMState::Reserved},
         {FSMEvent::I8_ReturnToUnavailable, FSMState::Unavailable},
     }},
};

ChargePointFSM::ChargePointFSM(const StatusNotificationCallback& status_notification_callback_,
                               FSMState initial_state) :
    status_notification_callback(status_notification_callback_), state(initial_state) {
    // FIXME (aw): do we need to call the callback here already?
}

FSMState ChargePointFSM::get_state() const {
    return state;
}

bool ChargePointFSM::handle_event(FSMEvent event) {
    const auto& transitions = FSM_DEF.at(state);
    const auto dest_state_it = transitions.find(event);

    if (dest_state_it == transitions.end()) {
        // no transition defined for this event / should this be logged?
        return false;
    }

    // fall through: transition found
    state = dest_state_it->second;

    status_notification_callback(state, ChargePointErrorCode::NoError);

    return true;
}

bool ChargePointFSM::handle_fault(const ChargePointErrorCode& error_code) {
    state = FSMState::Faulted;
    status_notification_callback(state, error_code);
    return true;
}

ChargePointStates::ChargePointStates(const ConnectorStatusCallback& callback) : connector_status_callback(callback) {
    // TODO special state machine for connector 0
}

void ChargePointStates::reset(std::map<int, v16::AvailabilityType> connector_availability) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    state_machines.clear();

    for (size_t connector_id = 0; connector_id < connector_availability.size(); ++connector_id) {
        const auto availability = connector_availability.at(connector_id);
        const auto initial_state =
            (availability == AvailabilityType::Operative) ? FSMState::Available : FSMState::Unavailable;
        state_machines.emplace_back(
            [this, connector_id](ChargePointStatus status, ChargePointErrorCode error_code) {
                this->connector_status_callback(connector_id, error_code, status);
            },
            initial_state);
    }
}

void ChargePointStates::submit_event(int connector_id, FSMEvent event) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    if (connector_id > 0 && (size_t)connector_id < this->state_machines.size()) {
        this->state_machines.at(connector_id).handle_event(event);
    }
}

void ChargePointStates::submit_error(int connector_id, const ChargePointErrorCode& error_code) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    if (connector_id > 0 && (size_t)connector_id < state_machines.size()) {
        state_machines.at(connector_id).handle_fault(error_code);
    }
}

ChargePointStatus ChargePointStates::get_state(int connector_id) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    if (connector_id > 0 && (size_t)connector_id < this->state_machines.size()) {
        return state_machines.at(connector_id).get_state();
    } else if (connector_id == 0) {
        return ChargePointStatus::Available;
    }

    // fall through on invalid id
    return ChargePointStatus::Unavailable;
}

} // namespace v16
} // namespace ocpp
