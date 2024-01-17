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

// special fsm definition for connector 0 wih reduced states
static const FSMDefinition FSM_DEF_CONNECTOR_ZERO = {
    {FSMState::Available,
     {
         {FSMEvent::ChangeAvailabilityToUnavailable, FSMState::Unavailable},
     }},
    {FSMState::Unavailable,
     {
         {FSMEvent::BecomeAvailable, FSMState::Available},
     }},
    {FSMState::Faulted,
     {
         {FSMEvent::I1_ReturnToAvailable, FSMState::Available},
         {FSMEvent::I8_ReturnToUnavailable, FSMState::Unavailable},
     }},
};

ChargePointFSM::ChargePointFSM(const StatusNotificationCallback& status_notification_callback_,
                               FSMState initial_state) :
    status_notification_callback(status_notification_callback_),
    state(initial_state),
    error_code(ChargePointErrorCode::NoError),
    faulted(false) {
    // FIXME (aw): do we need to call the callback here already?
}

FSMState ChargePointFSM::get_state() const {
    return state;
}

bool ChargePointFSM::handle_event(FSMEvent event, const ocpp::DateTime timestamp,
                                  const std::optional<CiString<50>>& info) {
    const auto& transitions = FSM_DEF.at(state);
    const auto dest_state_it = transitions.find(event);

    if (dest_state_it == transitions.end()) {
        // no transition defined for this event / should this be logged?
        return false;
    }

    // fall through: transition found
    state = dest_state_it->second;

    if (!faulted) {
        status_notification_callback(state, this->error_code, timestamp, info, std::nullopt, std::nullopt);
    }

    return true;
}

bool ChargePointFSM::handle_fault(const ChargePointErrorCode& error_code, const ocpp::DateTime& timestamp,
                                  const std::optional<CiString<50>>& info,
                                  const std::optional<CiString<255>>& vendor_id,
                                  const std::optional<CiString<50>>& vendor_error_code) {
    if (error_code == this->error_code) {
        // has already been handled and reported
        return false;
    }

    this->error_code = error_code;
    if (this->error_code == ChargePointErrorCode::NoError) {
        faulted = false;
        status_notification_callback(state, this->error_code, timestamp, info, vendor_id, vendor_error_code);
    } else {
        faulted = true;
        status_notification_callback(FSMState::Faulted, error_code, timestamp, info, vendor_id, vendor_error_code);
    }

    return true;
}

bool ChargePointFSM::handle_error(const ChargePointErrorCode& error_code, const ocpp::DateTime& timestamp,
                                  const std::optional<CiString<50>>& info,
                                  const std::optional<CiString<255>>& vendor_id,
                                  const std::optional<CiString<50>>& vendor_error_code) {
    if (error_code == this->error_code) {
        // has already been handled and reported
        return false;
    }

    this->error_code = error_code;
    if (!faulted) {
        status_notification_callback(this->state, error_code, timestamp, info, vendor_id, vendor_error_code);
    } else {
        status_notification_callback(FSMState::Faulted, error_code, timestamp, info, vendor_id, vendor_error_code);
    }
    return true;
}

ChargePointStates::ChargePointStates(const ConnectorStatusCallback& callback) : connector_status_callback(callback) {
}

void ChargePointStates::reset(std::map<int, ChargePointStatus> connector_status_map) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    state_machines.clear();

    for (size_t connector_id = 0; connector_id < connector_status_map.size(); ++connector_id) {
        const auto initial_state = connector_status_map.at(connector_id);

        if (connector_id == 0 and initial_state != ChargePointStatus::Available and
            initial_state != ChargePointStatus::Unavailable and initial_state != ChargePointStatus::Faulted) {
            throw std::runtime_error("Invalid initial status for connector 0: " +
                                     conversions::charge_point_status_to_string(initial_state));
        } else if (connector_id == 0) {
            state_machine_connector_zero = std::make_unique<ChargePointFSM>(
                [this](const ChargePointStatus status, const ChargePointErrorCode error_code,
                       const ocpp::DateTime& timestamp, const std::optional<CiString<50>>& info,
                       const std::optional<CiString<255>>& vendor_id,
                       const std::optional<CiString<50>>& vendor_error_code) {
                    this->connector_status_callback(0, error_code, status, timestamp, info, vendor_id,
                                                    vendor_error_code);
                },
                initial_state);
        } else {
            state_machines.emplace_back(
                [this, connector_id](ChargePointStatus status, ChargePointErrorCode error_code,
                                     ocpp::DateTime timestamp, std::optional<CiString<50>> info,
                                     std::optional<CiString<255>> vendor_id,
                                     std::optional<CiString<50>> vendor_error_code) {
                    this->connector_status_callback(connector_id, error_code, status, timestamp, info, vendor_id,
                                                    vendor_error_code);
                },
                initial_state);
        }
    }
}

void ChargePointStates::submit_event(const int connector_id, FSMEvent event, const ocpp::DateTime& timestamp,
                                     const std::optional<CiString<50>>& info) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);

    if (connector_id == 0) {
        this->state_machine_connector_zero->handle_event(event, timestamp, info);
    } else if (connector_id > 0 && (size_t)connector_id <= this->state_machines.size()) {
        this->state_machines.at(connector_id - 1).handle_event(event, timestamp, info);
    }
}

void ChargePointStates::submit_fault(const int connector_id, const ChargePointErrorCode& error_code,
                                     const ocpp::DateTime& timestamp, const std::optional<CiString<50>>& info,
                                     const std::optional<CiString<255>>& vendor_id,
                                     const std::optional<CiString<50>>& vendor_error_code) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    if (connector_id == 0) {
        this->state_machine_connector_zero->handle_fault(error_code, timestamp, info, vendor_id, vendor_error_code);
    } else if (connector_id > 0 && (size_t)connector_id <= state_machines.size()) {
        state_machines.at(connector_id - 1).handle_fault(error_code, timestamp, info, vendor_id, vendor_error_code);
    }
}

void ChargePointStates::submit_error(const int connector_id, const ChargePointErrorCode& error_code,
                                     const ocpp::DateTime& timestamp, const std::optional<CiString<50>>& info,
                                     const std::optional<CiString<255>>& vendor_id,
                                     const std::optional<CiString<50>>& vendor_error_code) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    if (connector_id == 0) {
        this->state_machine_connector_zero->handle_error(error_code, timestamp, info, vendor_id, vendor_error_code);
    } else if (connector_id > 0 && (size_t)connector_id <= state_machines.size()) {
        state_machines.at(connector_id - 1).handle_error(error_code, timestamp, info, vendor_id, vendor_error_code);
    }
}

ChargePointStatus ChargePointStates::get_state(int connector_id) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    if (connector_id > 0 && (size_t)connector_id <= this->state_machines.size()) {
        return state_machines.at(connector_id - 1).get_state();
    } else if (connector_id == 0) {
        return state_machine_connector_zero->get_state();
    }

    // fall through on invalid id
    return ChargePointStatus::Unavailable;
}

} // namespace v16
} // namespace ocpp
