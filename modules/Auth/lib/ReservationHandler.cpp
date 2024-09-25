// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ReservationHandler.hpp>
#include <everest/logging.hpp>
#include <utils/date.hpp>

namespace module {

// TODO mz make evse 0 available so it does not only have maps with connectors but also reservations without connectors

void ReservationHandler::init_connector(const int connector_id,
                                        const types::evse_manager::ConnectorTypeEnum connector_type) {
    connectors[connector_id] = connector_type;
}

std::optional<int32_t> ReservationHandler::matches_reserved_identifier(int connector, const std::string& id_token,
                                                                       std::optional<std::string> parent_id_token) {
    std::lock_guard<std::mutex> lk(this->reservation_mutex);
    // return true if id tokens match or parent id tokens exists and match
    if (connector > 0) {
        if (this->reservations[connector].id_token == id_token ||
            (parent_id_token && this->reservations[connector].parent_id_token &&
             parent_id_token.value() == this->reservations[connector].parent_id_token.value())) {
            return this->reservations[connector].reservation_id;
        }
    }

    // If connector == 0 or for the given connector no reservation is found, search globally for reservations with this
    // token.
    for (const auto& reservation : this->global_reservations) {
        if (reservation.id_token == id_token ||
            (parent_id_token.has_value() && reservation.parent_id_token.has_value() &&
             parent_id_token.value() == reservation.parent_id_token.value())) {
            return reservation.reservation_id;
        }
    }

    return std::nullopt;
}

bool ReservationHandler::has_reservation_parent_id(int connector) {
    std::lock_guard<std::mutex> lk(this->reservation_mutex);
    if (connector > 0) {
        if (this->reservations.count(connector)) {
            return this->reservations.at(connector).parent_id_token.has_value();
        }
    }

    // Check if one of the global reservations has a parent id.
    for (const auto& reservation : this->global_reservations) {
        if (reservation.parent_id_token.has_value()) {
            return true;
        }
    }

    return false;
}

types::reservation::ReservationResult ReservationHandler::reserve(int connector, const ConnectorState& state,
                                                                  bool is_reservable,
                                                                  const types::reservation::Reservation& reservation,
                                                                  const std::optional<uint32_t> available_connectors) {
    // TODO mz if reservation id matches with another reservation: replace it
    std::lock_guard<std::mutex> lk(this->reservation_mutex);
    if (connector == 0) {
        EVLOG_info << "Reservation for connector 0 is not supported";
        // TODO mz this should be supported for 2.0.1 -> there is also a config item that should be checked
        return types::reservation::ReservationResult::Rejected;
    }

    if (date::utc_clock::now() > Everest::Date::from_rfc3339(reservation.expiry_time)) {
        EVLOG_debug << "Rejecting reservation because expiry_time is in the past";
        return types::reservation::ReservationResult::Rejected;
    }

    const auto existing_reservation_it =
        std::find_if(reservations.begin(), reservations.end(), [reservation](const auto& existing_reservation) -> bool {
            if (existing_reservation.reservation_id == reservation.reservation_id) {
                return true;
            }
        });

    if (connector > 0) {
        if (existing_reservation_it != reservations.end()) {
            if (existing_reservation_it->first == connector) {
                // If the connector was not reservable but an existing reservation must be replaced, the connector
                // should be reservable now.
                is_reservable = true;
            }
        }
    }

    const auto& global_reservations_it =
        std::find_if(this->global_reservations.begin(), this->global_reservations.end(),
                     [reservation](const types::reservation::Reservation& existing_reservation) {
                         return reservation.reservation_id == existing_reservation.reservation_id;
                     });

    if (global_reservations_it != this->global_reservations.end() || existing_reservation_it != reservations.end()) {
        this->cancel_reservation(reservation.reservation_id, true);
    }

    // TODO mz what if a reservation must be replaced but no new reservation can be made (for example because of
    // different connector). Cancel the old reservation or not? Currently it is cancelled

    // TODO mz: If there already was a reservation with this id and for this connector, just replace it.
    // If there was a reservation with this id for another connector, check connector availability

    if (connector > 0) {
        if (state == ConnectorState::UNAVAILABLE) {
            EVLOG_debug << "Rejecting reservation because connector is unavailable";
            return types::reservation::ReservationResult::Unavailable;
        }

        if (state == ConnectorState::FAULTED) {
            EVLOG_debug << "Rejecting reservation because connector is faulted";
            return types::reservation::ReservationResult::Faulted;
        }

        if (!is_reservable) {
            EVLOG_debug << "Rejecting reservation because connector is not in state AVAILABLE";
            return types::reservation::ReservationResult::Occupied;
        }

        if (reservation.connector_type.has_value() && reservation.connector_type.value() != connectors[connector]) {
            EVLOG_debug << "Rejecting reservation because connector type is not equal.";
            return types::reservation::ReservationResult::Rejected;
        }

        if (this->reservations.count(connector) &&
            this->reservations[connector].reservation_id == reservation.reservation_id) {
        }

        if (!this->reservations.count(connector)) {
            this->reservations[connector] = reservation;
            set_reservation_timer(reservation, connector);
            return types::reservation::ReservationResult::Accepted;
        } else {
            EVLOG_debug << "Rejecting reservation because connector is already reserved";
            return types::reservation::ReservationResult::Occupied;
        }
    } else {
        // Connector 0.
        if (!available_connectors.has_value()) {
            EVLOG_error << "Reserve: connector is 0, but the number of available connectors are not given. This should "
                           "not happen";
            return types::reservation::ReservationResult::Rejected;
        }

        // TODO mz check if this specific connector type is available
        // There were no connectors available, but since the reservation id is the same, the reservation should be
        // overwritten
        if (available_connectors.value() == 0) {
            if (global_reservations_it !=
                    this->global_reservations.end() && // Check if the 'old' reservation was global as well.
                (!reservation.connector_type.has_value() ||
                 global_reservations_it->connector_type == reservation.connector_type.value())) {
                // The reservation that was already made can be overwritten.
                // TODO mz what if the connector type has no value or is unknown?
                global_reservations.push_back(reservation);
                set_reservation_timer(reservation, connector);
                return types::reservation::ReservationResult::Accepted;
            } else {
                // No connector available and the reservation is not overwritten (or it should be overwritten but the
                // connector type is not correct).
                return types::reservation::ReservationResult::Occupied;
            }

            // if (global_reservations_it != this->global_reservations.end() &&
            //     (!reservation.connector_type.has_value() ||
            //      is_connector_type_available(reservation.connector_type.value())) &&
            //     std::find_if(connectors.begin(), connectors.end(), [reservation](const Connector& connector_info) {
            //         return connector_info.type == types::evse_manager::ConnectorTypeEnum::Unknown ||
            //                (!reservation.connector_type.has_value() ||
            //                 reservation.connector_type.value() == types::evse_manager::ConnectorTypeEnum::Unknown ||
            //                 connector_info.type == reservation.connector_type);
            //     }) != connectors.end()) {
            //     // TODO mz if a reservation should be overwritten, check if the connector that is now free is of the
            //     // same type and if connector id is 0.
            // }
        }

        types::evse_manager::ConnectorTypeEnum connector_type = types::evse_manager::ConnectorTypeEnum::Unknown;
        if (reservation.connector_type.has_value()) {
            connector_type = reservation.connector_type.value();
        }

        if (is_connector_type_available(connector_type)) {
            return types::reservation::ReservationResult::Accepted;
        }

        // if (global_reservations.size() >= available_connectors.value()) {
        //     return types::reservation::ReservationResult::Occupied;
        // }

        this->global_reservations.push_back(reservation);
    }
}

int ReservationHandler::cancel_reservation(int reservation_id, bool execute_callback) {
    std::lock_guard<std::mutex> lk(this->reservation_mutex);
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

    int connector = -1;
    for (const auto& reservation : this->reservations) {
        if (reservation.second.reservation_id == reservation_id) {
            connector = reservation.first;
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

void ReservationHandler::on_reservation_used(int connector, const int32_t reservation_id) {
    if (this->cancel_reservation(reservation_id, false)) {
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

bool ReservationHandler::is_connector_type_available(const types::evse_manager::ConnectorTypeEnum connector_type) {
    uint32_t number_of_reserved_connectors = 0;
    if (connector_type == types::evse_manager::ConnectorTypeEnum::Unknown) {
        number_of_reserved_connectors = global_reservations.size();
    } else {
        auto reserved_connectors = std::count_if(global_reservations.begin(), global_reservations.end(),
                                                 [connector_type](const types::reservation::Reservation& reservation) {
                                                     return !reservation.connector_type.has_value() ||
                                                            reservation.connector_type.value() ==
                                                                types::evse_manager::ConnectorTypeEnum::Unknown ||
                                                            reservation.connector_type.value() == connector_type;
                                                 });
        if (reserved_connectors >= 0) {
            number_of_reserved_connectors = static_cast<uint32_t>(reserved_connectors);
        } else {
            EVLOG_error << "Number of reserved connectors is negative: this should not be possible";
            return false;
        }
    }

    uint32_t number_of_available_connectors = 0;
    for (const auto& [connector_id, existing_connector_type] : connectors) {
        if (connector_type == existing_connector_type ||
            connector_type == types::evse_manager::ConnectorTypeEnum::Unknown ||
            existing_connector_type == types::evse_manager::ConnectorTypeEnum::Unknown) {
            if (reservations.count(connector_id) == 0) {
                number_of_available_connectors++;
            }
        }
    }

    return (number_of_available_connectors - number_of_reserved_connectors) > 0;
}

void ReservationHandler::set_reservation_timer(const types::reservation::Reservation& reservation,
                                               const int32_t connector) {
    std::lock_guard<std::mutex> lk(this->timer_mutex);
    this->reservation_id_to_reservation_timeout_timer_map[reservation.reservation_id] =
        std::make_unique<Everest::SteadyTimer>();
    this->reservation_id_to_reservation_timeout_timer_map[reservation.reservation_id]->at(
        [this, reservation, connector]() {
            EVLOG_info << "Reservation expired for connector#" << connector;
            this->cancel_reservation(reservation.reservation_id, true);
        },
        Everest::Date::from_rfc3339(reservation.expiry_time));
}

} // namespace module
