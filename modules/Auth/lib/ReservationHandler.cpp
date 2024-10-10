// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <ReservationHandler.hpp>

#include <ReservationEVSEs.h>
#include <everest/logging.hpp>
#include <utils/date.hpp>

namespace module {

void ReservationHandler::init_connector(const int evse_id, const int connector_id,
                                        const types::evse_manager::ConnectorTypeEnum connector_type) {
    // Evse index here starts with 0.
    // TODO mz do we want that?
    evses[evse_id].push_back({connector_id, connector_type});
    e.add_connector(evse_id, connector_id, connector_type);
}

// TODO mz everywhere where we have a connector id that can be zero, we should replace it for std::optional.
// TODO mz support multiple connectors per evse
// TODO mz in libocpp connectors and evse's start with 1 and in everest core with 0. Make sure the correct translation
//         is made somewhere in the OCPP module. Not sure if they only start with 0 in vectors / lists or everywhere,
//         that needs some testing (for now assume in the interface it starts with 1!)

std::optional<int32_t> ReservationHandler::matches_reserved_identifier(const std::optional<int> evse,
                                                                       const std::string& id_token,
                                                                       std::optional<std::string> parent_id_token) {
    std::lock_guard<std::mutex> lk(this->reservation_mutex);
    // return true if id tokens match or parent id tokens exists and match
    if (evse.has_value() && evse.value() > 0) {
        if (this->reservations[evse.value()].id_token == id_token ||
            (parent_id_token && this->reservations[evse.value()].parent_id_token &&
             parent_id_token.value() == this->reservations[evse.value()].parent_id_token.value())) {
            return this->reservations[evse.value()].reservation_id;
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

bool ReservationHandler::has_reservation_parent_id(int evse) {
    std::lock_guard<std::mutex> lk(this->reservation_mutex);
    if (evse > 0) {
        if (this->reservations.count(evse)) {
            return this->reservations.at(evse).parent_id_token.has_value();
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

types::reservation::ReservationResult ReservationHandler::reserve(std::optional<int> evse, const ConnectorState& state,
                                                                  bool is_reservable,
                                                                  const types::reservation::Reservation& reservation,
                                                                  const std::optional<uint32_t> available_evses) {
    if (date::utc_clock::now() > Everest::Date::from_rfc3339(reservation.expiry_time)) {
        EVLOG_debug << "Rejecting reservation because expiry_time is in the past";
        return types::reservation::ReservationResult::Rejected;
    }

    const auto existing_reservation_it =
        std::find_if(reservations.begin(), reservations.end(), [reservation](const auto& existing_reservation) -> bool {
            if (existing_reservation.second.reservation_id == reservation.reservation_id) {
                return true;
            }

            return false;
        });

    if (evse.has_value() && evse.value() > 0) {
        if (existing_reservation_it != reservations.end()) {
            if (existing_reservation_it->first == evse.value()) {
                // If the connector was not reservable but an existing reservation must be replaced (H01.FR.02), the
                // connector should be reservable now.
                is_reservable = true;
            }
        }
    }

    const auto& global_reservations_it =
        std::find_if(this->global_reservations.begin(), this->global_reservations.end(),
                     [reservation](const types::reservation::Reservation& existing_reservation) {
                         return reservation.reservation_id == existing_reservation.reservation_id;
                     });

    // Cancel the reservation if there already was a reservation with this id. That also means that if the reservation
    // can not be made (because connector is of the wrong type for example), the old reservation is cancelled.
    if (global_reservations_it != this->global_reservations.end() || existing_reservation_it != reservations.end()) {
        this->cancel_reservation(reservation.reservation_id, true);
    }

    if (evse.has_value() && evse.value() > 0) {
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

        if (reservation.connector_type.has_value() &&
            !evse_has_connector_type(evse.value(), reservation.connector_type.value())) {
            EVLOG_debug << "Rejecting reservation because connector type is not equal.";
            return types::reservation::ReservationResult::Rejected;
        }

        // Check if there is already a reservation for this connector.
        if (!this->reservations.count(evse.value())) {
            // No reservation, but we have to check if there are not too much 'global' reservations already.
            if (reservation.connector_type.has_value() &&
                this->is_connector_type_available(reservation.connector_type.value())) {
                this->reservations[evse.value()] = reservation;
                set_reservation_timer(reservation, evse.value());
                return types::reservation::ReservationResult::Accepted;
            } else {
                EVLOG_debug << "Rejecting reservation because there are no free connectors available due to "
                               "reservations for evse id 0.";
                return types::reservation::ReservationResult::Occupied;
            }
        } else {
            EVLOG_debug << "Rejecting reservation because connector is already reserved";
            return types::reservation::ReservationResult::Occupied;
        }
    } else {
        // Any evse / connector.
        if (!available_evses.has_value()) {
            EVLOG_error << "Reserve: connector is 0, but the number of available connectors are not given. This should "
                           "not happen";
            return types::reservation::ReservationResult::Rejected;
        }

        if (available_evses.value() == 0) {
            // There were no connectors available, but since the reservation id is the same, the reservation should be
            // overwritten (H01.FR.02)
            if (global_reservations_it !=
                    this->global_reservations.end() && // Check if the 'old' reservation was global as well.
                (!reservation.connector_type.has_value() ||
                 global_reservations_it->connector_type == reservation.connector_type.value())) {
                // The reservation that was already made can be overwritten.
                this->global_reservations.push_back(reservation);
                this->set_reservation_timer(reservation, std::nullopt);
                return types::reservation::ReservationResult::Accepted;
            } else {
                // No connector available and the reservation is not overwritten (or it should be overwritten but the
                // connector type is not correct).
                return types::reservation::ReservationResult::Occupied;
            }
        }

        types::evse_manager::ConnectorTypeEnum connector_type = types::evse_manager::ConnectorTypeEnum::Unknown;
        if (reservation.connector_type.has_value()) {
            connector_type = reservation.connector_type.value();
        }

        if (is_connector_type_available(connector_type)) {
            this->global_reservations.push_back(reservation);
            this->set_reservation_timer(reservation, evse);
            return types::reservation::ReservationResult::Accepted;
        }

        return types::reservation::ReservationResult::Unavailable;
    }
}

int ReservationHandler::cancel_reservation(int reservation_id, bool execute_callback) {
    std::lock_guard<std::mutex> lk(this->reservation_mutex);
    {
        std::lock_guard<std::mutex> lk(this->timer_mutex);
        auto reservation_id_timer_it = this->reservation_id_to_reservation_timeout_timer_map.find(reservation_id);
        if (reservation_id_timer_it == this->reservation_id_to_reservation_timeout_timer_map.end()) {
            EVLOG_debug << "Reservation is cancelled, but there was no reservation timer set for this reservation id.";
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

// evse1 2 connectors: ccs and chademo      -> not reserved
// evse2 2 connectors: ccs and chademo      -> not reserved
// evse3 2 connectors: ccs and chademo      -> not reserved
// evse4 2 connectors: ccs and chademo      -> CCS reserved
// evse5 1 connector: chademo

// Global reserved: CCS x 2, Chademo x 1

// Try to reserve CCS

bool ReservationHandler::is_connector_type_available(const types::evse_manager::ConnectorTypeEnum connector_type) {
    // TODO mz this does not work if an evse has multiple connector types.
    // (TODO mz also because distracting the two can have a negative number as output???)
    // TODO mz what to do if one of the connectors is faulted???

    uint32_t number_of_reserved_evses = 0;
    if (connector_type == types::evse_manager::ConnectorTypeEnum::Unknown) {
        number_of_reserved_evses = global_reservations.size();
    } else {
        const uint32_t total_number_of_evses = evses.size();
        const uint32_t number_of_evses_with_connector_type = get_no_evses_with_connector_type(connector_type);
        const uint32_t total_number_of_reservations = global_reservations.size();
        int32_t reserved_evses = 0;
        reserved_evses =
            static_cast<int32_t>(std::count_if(global_reservations.begin(), global_reservations.end(),
                                               [connector_type](const types::reservation::Reservation& reservation) {
                                                   return !reservation.connector_type.has_value() ||
                                                          reservation.connector_type.value() ==
                                                              types::evse_manager::ConnectorTypeEnum::Unknown ||
                                                          reservation.connector_type.value() == connector_type;
                                               }));
        if (reserved_evses >= 0) {
            number_of_reserved_evses = static_cast<uint32_t>(reserved_evses);
        } else {
            EVLOG_error << "Number of reserved connectors is negative: this should not be possible";
            return false;
        }
    }

    uint32_t number_of_available_evses = 0;
    for (const auto& [evse_id, connectors] : evses) {
        if (evse_has_connector_type(evse_id, connector_type)) {
            if (reservations.count(evse_id) == 0) {
                number_of_available_evses++;
            }
        }
    }

    // TODO mz euh... is this correct???
    return (number_of_available_evses - number_of_reserved_evses) > 0;
}

void ReservationHandler::set_reservation_timer(const types::reservation::Reservation& reservation,
                                               const std::optional<int32_t> evse) {
    std::lock_guard<std::mutex> lk(this->timer_mutex);
    this->reservation_id_to_reservation_timeout_timer_map[reservation.reservation_id] =
        std::make_unique<Everest::SteadyTimer>();
    this->reservation_id_to_reservation_timeout_timer_map[reservation.reservation_id]->at(
        [this, reservation, evse]() {
            if (evse.has_value()) {
                EVLOG_info << "Reservation expired for evse #" << evse.value()
                           << " (reservation id: " << reservation.reservation_id << ")";
            } else {
                EVLOG_info << "Reservation expired for reservation id " << reservation.reservation_id;
            }

            this->cancel_reservation(reservation.reservation_id, true);
        },
        Everest::Date::from_rfc3339(reservation.expiry_time));
}

bool ReservationHandler::evse_has_connector_type(const int32_t evse_id,
                                                 const types::evse_manager::ConnectorTypeEnum type) {
    if (evses.count(evse_id) == 0) {
        // Evse with this id does not exist in the list.
        return false;
    }

    if (type == types::evse_manager::ConnectorTypeEnum::Unknown) {
        return true;
    }

    for (const EvseConnectorType& connector : evses[evse_id]) {
        if (connector.connector_type == types::evse_manager::ConnectorTypeEnum::Unknown ||
            connector.connector_type == type) {
            return true;
        }
    }

    return false;
}

uint32_t
ReservationHandler::get_no_evses_with_connector_type(const types::evse_manager::ConnectorTypeEnum connector_type) {
    uint32_t evses_with_connector_type = 0;
    for (const auto& evse : evses) {
        if (evse_has_connector_type(evse.first, connector_type)) {
            evses_with_connector_type++;
        }
    }

    return evses_with_connector_type;
}

} // namespace module
