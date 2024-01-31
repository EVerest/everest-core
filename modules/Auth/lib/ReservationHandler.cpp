// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ReservationHandler.hpp>
#include <everest/logging.hpp>
#include <utils/date.hpp>

namespace module {

void ReservationHandler::init_connector(int connector_id) {
    this->connector_to_reservation_timeout_timer_map[connector_id] = std::make_unique<Everest::SteadyTimer>();
}

bool ReservationHandler::matches_reserved_identifier(int connector, const std::string& id_token,
                                                     std::optional<std::string> parent_id_token) {
    // return true if id tokens match or parent id tokens exists and match
    return this->reservations[connector].id_token == id_token ||
           (parent_id_token && this->reservations[connector].parent_id_token &&
            parent_id_token.value() == this->reservations[connector].parent_id_token.value());
}

types::reservation::ReservationResult ReservationHandler::reserve(int connector, const ConnectorState& state,
                                                                  bool is_reservable,
                                                                  const types::reservation::Reservation& reservation) {

    if (connector == 0) {
        EVLOG_info << "Reservation for connector 0 is not supported";
        return types::reservation::ReservationResult::Rejected;
    }

    if (state == ConnectorState::UNAVAILABLE) {
        EVLOG_debug << "Rejecting reservation because connector is unavailable";
        return types::reservation::ReservationResult::Unavailable;
    }

    if (state == ConnectorState::FAULTED) {
        EVLOG_debug << "Rejecting reservation because connector is faulted";
        return types::reservation::ReservationResult::Faulted;
    }

    if (date::utc_clock::now() > Everest::Date::from_rfc3339(reservation.expiry_time)) {
        EVLOG_debug << "Rejecting reservation because expiry_time is in the past";
        return types::reservation::ReservationResult::Rejected;
    }

    if (!is_reservable) {
        EVLOG_debug << "Rejecting reservation because connector is not in state AVAILABLE";
        return types::reservation::ReservationResult::Occupied;
    }

    if (!this->reservations.count(connector)) {
        this->reservations[connector] = reservation;
        std::lock_guard<std::mutex> lk(this->timer_mutex);
        this->connector_to_reservation_timeout_timer_map[connector]->at(
            [this, reservation, connector]() {
                EVLOG_info << "Reservation expired for connector#" << connector;
                this->cancel_reservation(reservation.reservation_id, true);
            },
            Everest::Date::from_rfc3339(reservation.expiry_time));
        return types::reservation::ReservationResult::Accepted;
    } else {
        EVLOG_debug << "Rejecting reservation because connector is already reserved";
        return types::reservation::ReservationResult::Occupied;
    }
}

int ReservationHandler::cancel_reservation(int reservation_id, bool execute_callback) {

    int connector = -1;
    for (const auto& reservation : this->reservations) {
        if (reservation.second.reservation_id == reservation_id) {
            connector = reservation.first;
        }
    }
    if (connector != -1) {
        std::lock_guard<std::mutex> lk(this->timer_mutex);
        this->connector_to_reservation_timeout_timer_map[connector]->stop();
        auto it = this->reservations.find(connector);
        this->reservations.erase(it);
        if (execute_callback) {
            this->reservation_cancelled_callback(connector);
        }
    }
    return connector;
}

void ReservationHandler::on_reservation_used(int connector) {
    if (this->cancel_reservation(this->reservations[connector].reservation_id, false)) {
        EVLOG_info << "Reservation for connector#" << connector << " used and cancelled";
    } else {
        EVLOG_warning
            << "On reservation used called when no reservation for this connector was present. This should not happen";
    }
}

void ReservationHandler::register_reservation_cancelled_callback(
    const std::function<void(const int& connector_id)>& callback) {
    this->reservation_cancelled_callback = callback;
}

} // namespace module
