// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ReservationHandler.hpp>
#include <everest/logging.hpp>
#include <utils/date.hpp>

namespace module {

// TODO mz make evse 0 available so it does not only have maps with connectors but also reservations without connectors

// void ReservationHandler::init_connector(int connector_id) {
//     this->reservation_id_to_reservation_timeout_timer_map[reservation_id] = std::make_unique<Everest::SteadyTimer>();
// }

bool ReservationHandler::matches_reserved_identifier(int connector, const std::string& id_token,
                                                     std::optional<std::string> parent_id_token) {
    std::lock_guard<std::mutex> lk(this->reservation_mutex);
    // return true if id tokens match or parent id tokens exists and match
    if (connector > 0) {
        return this->reservations[connector].id_token == id_token ||
               (parent_id_token && this->reservations[connector].parent_id_token &&
                parent_id_token.value() == this->reservations[connector].parent_id_token.value());
    } else {
        for (const auto& reservation : this->global_reservations) {
            if (reservation.id_token == id_token ||
                (parent_id_token.has_value() && reservation.parent_id_token.has_value() &&
                 parent_id_token.value() == reservation.parent_id_token.value())) {
                return true;
            }
        }
    }

    return false;

    // TODO mz what if connector is not 0?
}

bool ReservationHandler::has_reservation_parent_id(int connector) {
    std::lock_guard<std::mutex> lk(this->reservation_mutex);
    // TODO mz also check for global reservations?
    if (!this->reservations.count(connector)) {
        return false;
    }
    return this->reservations.at(connector).parent_id_token.has_value();
}

types::reservation::ReservationResult ReservationHandler::reserve(int connector,
                                                                  const types::evse_manager::ConnectorTypeEnum type,
                                                                  const ConnectorState& state, bool is_reservable,
                                                                  const types::reservation::Reservation& reservation) {
    std::lock_guard<std::mutex> lk(this->reservation_mutex);
    if (connector == 0) {
        EVLOG_info << "Reservation for connector 0 is not supported";
        // TODO mz this should be supported for 2.0.1
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

    if (reservation.connector_type.has_value() && reservation.connector_type.value() != type) {
        EVLOG_debug << "Rejecting reservation because connector type is not equal.";
        return types::reservation::ReservationResult::Rejected;
    }

    if (!this->reservations.count(connector)) {
        this->reservations[connector] = reservation;
        std::lock_guard<std::mutex> lk(this->timer_mutex);
        this->reservation_id_to_reservation_timeout_timer_map[reservation.reservation_id] =
            std::make_unique<Everest::SteadyTimer>();
        this->reservation_id_to_reservation_timeout_timer_map[reservation.reservation_id]->at(
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
    std::lock_guard<std::mutex> lk(this->reservation_mutex);
    int connector = -1;
    for (const auto& reservation : this->reservations) {
        if (reservation.second.reservation_id == reservation_id) {
            connector = reservation.first;
        }
    }

    {
        std::lock_guard<std::mutex> lk(this->timer_mutex);
        auto reservation_id_timer_it = this->reservation_id_to_reservation_timeout_timer_map.find(reservation_id);
        if (reservation_id_timer_it == this->reservation_id_to_reservation_timeout_timer_map.end()) {
            // TODO mz log error etc
        } else {
            reservation_id_timer_it->second->stop();
            this->reservation_id_to_reservation_timeout_timer_map.erase(reservation_id_timer_it);
        }
    }

    if (connector != -1) {

        auto it = this->reservations.find(connector);
        this->reservations.erase(it);

    } else {
        // No connector, search in global reservations
        const auto& it = std::find_if(this->global_reservations.begin(), this->global_reservations.end(),
                                      [reservation_id](const types::reservation::Reservation& reservation) {
                                          return reservation.reservation_id == reservation_id;
                                      });

        if (it != this->global_reservations.end()) {
            this->global_reservations.erase(it);
            connector = 0;
        }
    }

    if (connector != -1 && execute_callback) {
        this->reservation_cancelled_callback(connector);
    }

    return connector;
}

void ReservationHandler::on_reservation_used(int connector) {
    // TODO mz cancel reservation for evse 0
    // TODO mz we need more information on the reservation id for connector 0.
    if (connector == 0) {
    }
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
