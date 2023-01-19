// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#include <utility>

#include <everest/logging.hpp>
#include <ocpp/v201/connector.hpp>

namespace ocpp {
namespace v201 {

Connector::Connector(const int32_t connector_id,
                     const std::function<void(const ConnectorStatusEnum& status)>& status_notification_callback) :
    connector_id(connector_id),
    state(ConnectorStatusEnum::Available),
    status_notification_callback(status_notification_callback) {
}

ConnectorStatusEnum Connector::get_state() {
    std::lock_guard<std::mutex> lg(this->state_mutex);
    return this->state;
}

void Connector::submit_event(ConnectorEvent event) {

    //FIXME(piet): This state machine implementation is a first draft
    const auto current_state = this->get_state();
    std::lock_guard<std::mutex> lg(this->state_mutex);

    switch (current_state) {
    case ConnectorStatusEnum::Available:
        switch (event) {
        case ConnectorEvent::PlugIn:
            this->state = ConnectorStatusEnum::Occupied;
            break;
        case ConnectorEvent::Reserve:
            this->state = ConnectorStatusEnum::Reserved;
            break;
        case ConnectorEvent::Error:
            this->state = ConnectorStatusEnum::Faulted;
            break;
        case ConnectorEvent::Unavailable:
            this->state = ConnectorStatusEnum::Unavailable;
            break;
        default:
            EVLOG_warning << "Invalid connector event in state Available.";
            return;
        }
        break;
    case ConnectorStatusEnum::Occupied:
        switch (event) {
        case ConnectorEvent::PlugOut:
            this->state = ConnectorStatusEnum::Available;
            break;
        case ConnectorEvent::Error:
            this->state = ConnectorStatusEnum::Faulted;
            break;
        case ConnectorEvent::Unavailable:
            this->state = ConnectorStatusEnum::Unavailable;
            break;
        default:
            EVLOG_warning << "Invalid connector event in state Occupied.";
            return;
        }
        break;
    case ConnectorStatusEnum::Reserved:
        switch (event) {
        case ConnectorEvent::ReservationFinished:
            this->state = ConnectorStatusEnum::Available;
            break;
        case ConnectorEvent::PlugInAndTokenValid:
            this->state = ConnectorStatusEnum::Occupied;
            break;
        case ConnectorEvent::Error:
            this->state = ConnectorStatusEnum::Faulted;
            break;
        case ConnectorEvent::Unavailable:
            this->state = ConnectorStatusEnum::Unavailable;
            break;
        default:
            EVLOG_warning << "Invalid connector event in state Reserved.";
            return;
        }
        break;
    case ConnectorStatusEnum::Unavailable:
        switch (event) {
        case ConnectorEvent::UnavailableToAvailable:
            this->state = ConnectorStatusEnum::Available;
            break;
        case ConnectorEvent::UnavailableToOccupied:
            this->state = ConnectorStatusEnum::Occupied;
            break;
        case ConnectorEvent::UnavailableToReserved:
            this->state = ConnectorStatusEnum::Reserved;
            break;
        case ConnectorEvent::UnavailableFaulted:
            this->state = ConnectorStatusEnum::Faulted;
            break;
        default:
            EVLOG_warning << "Invalid connector event in state Unavailable.";
            return;
        }
        break;
    case ConnectorStatusEnum::Faulted:
        switch (event) {
        case ConnectorEvent::ErrorCleared:
            this->state = ConnectorStatusEnum::Available;
            break;
        case ConnectorEvent::ErrorCleardOnOccupied:
            this->state = ConnectorStatusEnum::Occupied;
            break;
        case ConnectorEvent::ErrorCleardOnReserved:
            this->state = ConnectorStatusEnum::Reserved;
            break;
        case ConnectorEvent::Unavailable:
            this->state = ConnectorStatusEnum::Unavailable;
            break;
        default:
            EVLOG_warning << "Invalid connector event in state Faulted.";
            return;
        }
        break;
    }
    this->status_notification_callback(this->state);
}

} // namespace v201
} // namespace ocpp
