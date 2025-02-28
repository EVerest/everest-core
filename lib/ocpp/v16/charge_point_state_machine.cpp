// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest
#include <ocpp/v16/charge_point_state_machine.hpp>
#include <ocpp/v16/ocpp_enums.hpp>

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
     {{FSMEvent::BecomeAvailable, FSMState::Available},
      {FSMEvent::UsageInitiated, FSMState::Preparing},
      {FSMEvent::ChangeAvailabilityToUnavailable, FSMState::Unavailable},
      {FSMEvent::ReservationEnd, FSMState::Available}}},
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

ErrorInfo::ErrorInfo(const std::string uuid, const ChargePointErrorCode error_code, const bool is_fault) :
    uuid(uuid), timestamp(DateTime()), error_code(error_code), is_fault(is_fault) {
}

ErrorInfo::ErrorInfo(const std::string uuid, const ChargePointErrorCode error_code, const bool is_fault,
                     const std::optional<std::string> info) :
    ErrorInfo(uuid, error_code, is_fault) {
    if (!info.has_value()) {
        return;
    }

    // CiString for info is allowed to have max of 50 characters
    if (info.value().size() > 50) {
        this->info = info.value().substr(0, 50);
    } else {
        this->info = info;
    }
}

ErrorInfo::ErrorInfo(const std::string uuid, const ChargePointErrorCode error_code, const bool is_fault,
                     const std::optional<std::string> info, const std::optional<std::string> vendor_id) :
    ErrorInfo(uuid, error_code, is_fault, info) {
    if (!vendor_id.has_value()) {
        return;
    }

    // CiString for vendor_id is allowed to have max of 50 characters
    if (vendor_id.value().size() > 255) {
        this->vendor_id = vendor_id.value().substr(0, 255);
    } else {
        this->vendor_id = vendor_id;
    }
}

ErrorInfo::ErrorInfo(const std::string uuid, const ChargePointErrorCode error_code, const bool is_fault,
                     const std::optional<std::string> info, const std::optional<std::string> vendor_id,
                     const std::optional<std::string> vendor_error_code) :
    ErrorInfo(uuid, error_code, is_fault, info, vendor_id) {
    if (!vendor_error_code.has_value()) {
        return;
    }
    // CiString for vendor_error_code is allowed to have max of 50 characters
    if (vendor_error_code.value().size() > 50) {
        this->vendor_error_code = vendor_error_code.value().substr(0, 50);
    } else {
        this->vendor_error_code = vendor_error_code;
    }
}

ChargePointFSM::ChargePointFSM(const StatusNotificationCallback& status_notification_callback_,
                               FSMState initial_state) :
    status_notification_callback(status_notification_callback_), state(initial_state) {
}

FSMState ChargePointFSM::get_state() {
    if (this->is_faulted()) {
        return FSMState::Faulted;
    }
    return state;
}

bool ChargePointFSM::is_faulted() {
    // check if there is any active fault
    return std::find_if(this->active_errors.begin(), this->active_errors.end(),
                        [](const std::pair<std::string, ErrorInfo>& entry) { return entry.second.is_fault; }) !=
           this->active_errors.end();
}

std::optional<ErrorInfo> ChargePointFSM::get_latest_error() {
    if (this->active_errors.empty()) {
        return std::nullopt;
    }

    auto latest_error = (*this->active_errors.begin()).second;
    for (const auto& [uuid, error_info] : this->active_errors) {
        if (error_info.timestamp > latest_error.timestamp) {
            latest_error = error_info;
        }
    }
    return latest_error;
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

    const auto error_info = this->get_latest_error().value_or(ErrorInfo("", ChargePointErrorCode::NoError, false));

    // only send a StatusNotification.req with the updated state if not in faulted
    if (!this->is_faulted()) {
        status_notification_callback(state, error_info.error_code, timestamp, info, error_info.vendor_id,
                                     error_info.vendor_error_code);
    }

    return true;
}

bool ChargePointFSM::handle_error(const ErrorInfo& error_info) {
    if (this->active_errors.count(error_info.uuid)) {
        // has already been handled and reported
        return false;
    }

    this->active_errors.insert({error_info.uuid, error_info});

    if (!this->is_faulted()) {
        status_notification_callback(this->state, error_info.error_code, error_info.timestamp, error_info.info,
                                     error_info.vendor_id, error_info.vendor_error_code);
    } else {
        status_notification_callback(FSMState::Faulted, error_info.error_code, error_info.timestamp, error_info.info,
                                     error_info.vendor_id, error_info.vendor_error_code);
    }
    return true;
}

bool ChargePointFSM::handle_error_cleared(const std::string uuid) {
    this->active_errors.erase(uuid);

    // dont report StatusNotification if still "Faulted"
    if (this->is_faulted()) {
        return false;
    }

    // dont report StatusNotification if errors are still active
    if (this->active_errors.size() > 0) {
        return false;
    }

    // no fault present and no error active, so we can send a StatusNotification.req and return to previous state
    status_notification_callback(this->state, ChargePointErrorCode::NoError, DateTime(), std::nullopt, std::nullopt,
                                 std::nullopt);

    return true;
}

bool ChargePointFSM::handle_all_errors_cleared() {
    this->active_errors.clear();
    status_notification_callback(this->state, ChargePointErrorCode::NoError, DateTime(), std::nullopt, std::nullopt,
                                 std::nullopt);
    return true;
}

void ChargePointFSM::trigger_status_notification() {
    // get latest error or report NoError
    const auto error_info = this->get_latest_error().value_or(ErrorInfo("", ChargePointErrorCode::NoError, false));
    if (!this->is_faulted()) {
        status_notification_callback(this->state, error_info.error_code, error_info.timestamp, error_info.info,
                                     error_info.vendor_id, error_info.vendor_error_code);
    } else {
        status_notification_callback(FSMState::Faulted, error_info.error_code, error_info.timestamp, error_info.info,
                                     error_info.vendor_id, error_info.vendor_error_code);
    }
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
    } else if (connector_id > 0 && static_cast<size_t>(connector_id) <= this->state_machines.size()) {
        this->state_machines.at(connector_id - 1).handle_event(event, timestamp, info);
    }
}

void ChargePointStates::submit_error(const int connector_id, const ErrorInfo& error_info) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    if (connector_id == 0) {
        this->state_machine_connector_zero->handle_error(error_info);
    } else if (connector_id > 0 && static_cast<size_t>(connector_id) <= state_machines.size()) {
        state_machines.at(connector_id - 1).handle_error(error_info);
    }
}

void ChargePointStates::submit_error_cleared(const int connector_id, const std::string uuid) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    if (connector_id == 0) {
        this->state_machine_connector_zero->handle_error_cleared(uuid);
    } else if (connector_id > 0 && static_cast<size_t>(connector_id) <= state_machines.size()) {
        state_machines.at(connector_id - 1).handle_error_cleared(uuid);
    }
}

void ChargePointStates::submit_all_errors_cleared(const int32_t connector_id) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    if (connector_id == 0) {
        this->state_machine_connector_zero->handle_all_errors_cleared();
    } else if (connector_id > 0 && static_cast<size_t>(connector_id) <= state_machines.size()) {
        state_machines.at(connector_id - 1).handle_all_errors_cleared();
    }
}

void ChargePointStates::trigger_status_notification(const int connector_id) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    if (connector_id == 0) {
        this->state_machine_connector_zero->trigger_status_notification();
    } else {
        this->state_machines.at(connector_id - 1).trigger_status_notification();
    }
}

void ChargePointStates::trigger_status_notifications() {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    this->state_machine_connector_zero->trigger_status_notification();
    for (size_t connector_id = 0; connector_id < state_machines.size(); ++connector_id) {
        this->state_machines.at(connector_id).trigger_status_notification();
    }
}

ChargePointStatus ChargePointStates::get_state(int connector_id) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    if (connector_id > 0 && static_cast<size_t>(connector_id) <= this->state_machines.size()) {
        return state_machines.at(connector_id - 1).get_state();
    } else if (connector_id == 0) {
        return state_machine_connector_zero->get_state();
    }

    // fall through on invalid id
    return ChargePointStatus::Unavailable;
}

std::optional<ErrorInfo> ChargePointStates::get_latest_error(int connector_id) {
    const std::lock_guard<std::mutex> lck(state_machines_mutex);
    if (connector_id > 0 && static_cast<size_t>(connector_id) <= this->state_machines.size()) {
        return state_machines.at(connector_id - 1).get_latest_error();
    } else {
        return state_machine_connector_zero->get_latest_error();
    }
}

} // namespace v16
} // namespace ocpp
