#include <ReservationEVSEs.h>

#include <algorithm>
#include <iostream>

#include <everest/logging.hpp>
#include <utils/date.hpp>

namespace module {

static types::reservation::ReservationResult
connector_state_to_reservation_result(const ConnectorState connector_state);

ReservationEVSEs::ReservationEVSEs() {
}

void ReservationEVSEs::add_connector(const uint32_t evse_id, const uint32_t connector_id,
                                     const types::evse_manager::ConnectorTypeEnum connector_type,
                                     const ConnectorState connector_state) {
    EvseConnectorType evse_connector_type;
    evse_connector_type.connector_type = connector_type;
    evse_connector_type.connector_id = connector_id;
    evse_connector_type.state = connector_state;
    Evse evse;

    std::unique_lock<std::mutex> lock(reservation_mutex);
    if (evses.count(evse_id) > 0) {
        evse = evses[evse_id];
    }
    evse.connectors.push_back(evse_connector_type);
    evse.evse_id = evse_id;

    switch (connector_state) {
    case ConnectorState::AVAILABLE:
    case ConnectorState::UNAVAILABLE:         // Connector is unavailable but evse might be just available?
    case ConnectorState::FAULTED:             // Connector is faulted but evse might be just available?
    case ConnectorState::UNAVAILABLE_FAULTED: // Connector is unavailable and faulted but evse might be just available?
    {
        evse.evse_state = ConnectorState::AVAILABLE;
        break;
    }
    case ConnectorState::OCCUPIED:
    case ConnectorState::FAULTED_OCCUPIED: {
        evse.evse_state = connector_state;
        break;
    }
    }

    evses[evse_id] = evse;
}

types::reservation::ReservationResult
ReservationEVSEs::make_reservation(const std::optional<uint32_t> evse_id,
                                   const types::reservation::Reservation& reservation) {
    if (date::utc_clock::now() > Everest::Date::from_rfc3339(reservation.expiry_time)) {
        EVLOG_info << "Rejecting reservation because expire time is in the past.";
        return types::reservation::ReservationResult::Rejected;
    }

    // If a reservation was made with an existing reservation id, the existing reservation must be replaced (H01.FR.01).
    // We cancel the reservation here because of that. That also means that if the reservation can not be made, the old
    // reservation is cancelled anyway.
    std::optional<uint32_t> cancelled_reservation_id = this->cancel_reservation(reservation.reservation_id, true);
    if (cancelled_reservation_id.has_value()) {
        EVLOG_info << "Cancelled reservation with id " << cancelled_reservation_id.value();
    }

    std::unique_lock<std::mutex> lock(reservation_mutex);
    if (evse_id.has_value()) {
        if (this->evse_reservations.count(evse_id.value()) > 0) {
            // There already is a reservation for this evse.
            return types::reservation::ReservationResult::Occupied;
        }

        if (this->evses.count(evse_id.value()) == 0) {
            // There is no evse with this evse id.
            return types::reservation::ReservationResult::Rejected;
        }

        const types::evse_manager::ConnectorTypeEnum connector_type =
            reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown);

        // We have to return a valid state here.
        // So if one or all connectors are occupied or reserved, return occupied. (H01.FR.11)
        // If one or all are faulted, return faulted. (H01.FR.12)
        // If one or all are unavailable, return unavailable. (H01.FR.13)
        // It is not clear what to return if one is faulted, one occupied and one available so in that case the first
        // in row is returned, which is occupied.
        const types::reservation::ReservationResult evse_state =
            this->get_evse_state(evse_id.value(), this->evse_reservations);
        const types::reservation::ReservationResult connector_state =
            this->is_connector_available(evse_id.value(), connector_type);

        if (!has_evse_connector_type(this->evses[evse_id.value()].connectors, connector_type)) {
            return types::reservation::ReservationResult::Rejected;
        } else if (evse_state != types::reservation::ReservationResult::Accepted) {
            return evse_state;
        } else if (connector_state != types::reservation::ReservationResult::Accepted) {
            return connector_state;
        }

        else {
            // Everything fine, continue.
            if (global_reservations.empty()) {
                this->evse_reservations[evse_id.value()] = reservation;
                return types::reservation::ReservationResult::Accepted;
            }

            // Make a copy of the evse specific reservations map so we can add this reservation to test if the
            // reservation is possible. Only if it is, we add it to the 'member' map.
            std::map<uint32_t, types::reservation::Reservation> evse_specific_reservations = this->evse_reservations;
            evse_specific_reservations[evse_id.value()] = reservation;

            // Check if the reservations are possible with the added evse specific reservation.
            if (!is_reservation_possible(std::nullopt, evse_specific_reservations)) {
                return get_reservation_evse_connector_state();
            }

            // Reservation is possible, add to evse specific reservations.
            this->evse_reservations[evse_id.value()] = reservation;
        }
    } else {
        if (!is_reservation_possible(
                reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown),
                this->evse_reservations)) {
            // TODO mz find real reason?
            return get_reservation_evse_connector_state();
        }

        global_reservations.push_back(reservation);
    }

    set_reservation_timer(reservation, evse_id);
    return types::reservation::ReservationResult::Accepted;
}

void ReservationEVSEs::set_evse_available(const bool available, const uint32_t evse_id) {
    std::unique_lock<std::mutex> lock(reservation_mutex);
    if (evses.count(evse_id) == 0) {
        // TODO mz
        return;
    }

    evses[evse_id].evse_state = (available ? ConnectorState::AVAILABLE : ConnectorState::UNAVAILABLE);

    if (!available) {
        // TODO mz if connector changed from available to unavailable: cancel a reservation???
    }
}

bool ReservationEVSEs::is_charging_possible(const uint32_t evse_id) {
    std::unique_lock<std::mutex> lock(reservation_mutex);
    if (this->evse_reservations.count(evse_id) > 0) {
        return false;
    }

    if (this->evses.count(evse_id) == 0) {
        // Not existing evse id
        return false;
    }

    std::map<uint32_t, types::reservation::Reservation> reservations = this->evse_reservations;
    // We want to test if charging is possible on this evse id with the current reservations. For that, we do like it
    // is a new reservation and check if that reservation is possible. If it is, we can charge on that evse.
    types::reservation::Reservation r;
    // It is a dummy reservation so the details are not important.
    reservations[evse_id] = r;
    return is_reservation_possible(std::nullopt, reservations);
}

std::optional<uint32_t> ReservationEVSEs::cancel_reservation(int reservation_id, bool execute_callback) {
    std::unique_lock<std::mutex> lock(reservation_mutex);

    {
        std::unique_lock<std::recursive_mutex> lk(this->timer_mutex);
        auto reservation_id_timer_it = this->reservation_id_to_reservation_timeout_timer_map.find(reservation_id);
        if (reservation_id_timer_it == this->reservation_id_to_reservation_timeout_timer_map.end()) {
            EVLOG_debug << "Reservation is cancelled, but there was no reservation timer set for this reservation id.";
        } else {
            reservation_id_timer_it->second->stop();
            this->reservation_id_to_reservation_timeout_timer_map.erase(reservation_id_timer_it);
        }
    }

    std::optional<uint32_t> evse_id;
    for (const auto& reservation : this->evse_reservations) {
        if (reservation.second.reservation_id == reservation_id) {
            evse_id = reservation.first;
        }
    }

    if (evse_id.has_value()) {
        auto it = this->evse_reservations.find(evse_id.value());
        this->evse_reservations.erase(it);
    } else {
        // No evse, search in global reservations
        const auto& it = std::find_if(this->global_reservations.begin(), this->global_reservations.end(),
                                      [reservation_id](const types::reservation::Reservation& reservation) {
                                          return reservation.reservation_id == reservation_id;
                                      });

        if (it != this->global_reservations.end()) {
            this->global_reservations.erase(it);
            evse_id = 0;
        }
    }

    if (evse_id.has_value() && execute_callback) {
        this->reservation_cancelled_callback(evse_id.value());
    }

    return evse_id;
}

void ReservationEVSEs::register_reservation_cancelled_callback(
    const std::function<void(const std::optional<uint32_t>)>& callback) {
    this->reservation_cancelled_callback = callback;
}

void ReservationEVSEs::on_reservation_used(const int32_t reservation_id) {
    const std::optional<uint32_t> evse_id = this->cancel_reservation(reservation_id, false);
    if (evse_id.has_value()) {
        EVLOG_info << "Reservation for evse#" << evse_id.value() << " used and cancelled";
    } else {
        EVLOG_info << "Reservation without evse id used and cancelled";
    }
}

bool ReservationEVSEs::has_evse_connector_type(const std::vector<EvseConnectorType> evse_connectors,
                                               const types::evse_manager::ConnectorTypeEnum connector_type) {
    if (connector_type == types::evse_manager::ConnectorTypeEnum::Unknown) {
        return true;
    }

    for (const auto& type : evse_connectors) {
        if (type.connector_type == types::evse_manager::ConnectorTypeEnum::Unknown ||
            type.connector_type == connector_type) {
            return true;
        }
    }

    return false;
}

types::reservation::ReservationResult
ReservationEVSEs::get_evse_state(const uint32_t evse_id,
                                 const std::map<uint32_t, types::reservation::Reservation> evse_specific_reservations) {
    if (evses.count(evse_id) == 0) {
        // TODO mz
        return types::reservation::ReservationResult::Rejected;
    }

    // Check if evse is available.
    if (evses[evse_id].evse_state != ConnectorState::AVAILABLE) {
        return connector_state_to_reservation_result(evses[evse_id].evse_state);
    }

    // If one connector is occupied, then the other connector can also not be used (one connector of an evse can be
    // used at the same time).
    for (const auto& connector : evses[evse_id].connectors) {
        if (connector.state == ConnectorState::OCCUPIED || connector.state == ConnectorState::FAULTED_OCCUPIED) {
            return connector_state_to_reservation_result(connector.state);
        }
    }

    // If evse is reserved, it is not available.
    if (evse_specific_reservations.count(evse_id) > 0) {
        return types::reservation::ReservationResult::Unavailable;
    }

    return types::reservation::ReservationResult::Accepted;
}

types::reservation::ReservationResult
ReservationEVSEs::is_connector_available(const uint32_t evse_id,
                                         const types::evse_manager::ConnectorTypeEnum connector_type) {
    if (evses.count(evse_id) == 0) {
        // TODO mz
        return types::reservation::ReservationResult::Rejected;
    }

    ConnectorState connector_state = ConnectorState::UNAVAILABLE;

    for (const auto& connector : evses[evse_id].connectors) {
        if ((connector.connector_type == connector_type ||
             connector.connector_type == types::evse_manager::ConnectorTypeEnum::Unknown ||
             connector_type == types::evse_manager::ConnectorTypeEnum::Unknown)) {
            if (connector.state == ConnectorState::AVAILABLE) {
                return types::reservation::ReservationResult::Accepted;
            } else {
                if (connector.state > connector_state) {
                    if (connector.state > ConnectorState::OCCUPIED) {
                        if (connector.state == ConnectorState::FAULTED_OCCUPIED) {
                            connector_state = ConnectorState::OCCUPIED;
                        } else if (connector.state == ConnectorState::UNAVAILABLE_FAULTED) {
                            if (connector_state != ConnectorState::OCCUPIED) {
                                connector_state = ConnectorState::FAULTED;
                            }
                        }
                    } else {
                        connector_state = connector.state;
                    }
                }
                connector_state = connector.state;
            }
        }
    }

    return connector_state_to_reservation_result(connector_state);
}

std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>>
ReservationEVSEs::get_all_possible_orders(const std::vector<types::evse_manager::ConnectorTypeEnum>& connectors) {

    std::vector<types::evse_manager::ConnectorTypeEnum> input_next = connectors;
    std::vector<types::evse_manager::ConnectorTypeEnum> input_prev = connectors;
    std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>> output;

    if (connectors.empty()) {
        return output;
    }

    do {
        output.push_back(input_next);
    } while (std::next_permutation(input_next.begin(), input_next.end()));

    while (std::prev_permutation(input_prev.begin(), input_prev.end())) {
        output.push_back(input_prev);
    }

    return output;
}

bool ReservationEVSEs::can_virtual_car_arrive(
    const std::vector<uint32_t>& used_evse_ids,
    const std::vector<types::evse_manager::ConnectorTypeEnum>& next_car_arrival_order,
    const std::map<uint32_t, types::reservation::Reservation>& evse_specific_reservations) {

    bool is_possible = false;

    for (const auto& [evse_id, evse] : evses) {
        // Check if there is a car already at this evse id.
        if (std::find(used_evse_ids.begin(), used_evse_ids.end(), evse_id) != used_evse_ids.end()) {
            continue;
        }

        if (get_evse_state(evse_id, evse_specific_reservations) == types::reservation::ReservationResult::Accepted &&
            has_evse_connector_type(evse.connectors, next_car_arrival_order.at(0)) &&
            is_connector_available(evse_id, next_car_arrival_order.at(0)) ==
                types::reservation::ReservationResult::Accepted) {
            is_possible = true;

            // std::cout << "OK: " << evse_id << "\n";

            std::vector<uint32_t> next_used_evse_ids = used_evse_ids;
            next_used_evse_ids.push_back(evse_id);

            // Check if this is the last.
            if (next_car_arrival_order.size() == 1) {
                std::cout << "OK: evse_id: " << evse_id << "\n";
                for (const uint32_t e : next_used_evse_ids) {
                    std::cout << e << " ";
                }

                std::cout << "\n";

                return true;
            }

            // Call next level recursively.
            const std::vector<types::evse_manager::ConnectorTypeEnum> next_arrival_order(
                next_car_arrival_order.begin() + 1, next_car_arrival_order.end());

            if (!this->can_virtual_car_arrive(next_used_evse_ids, next_arrival_order, evse_specific_reservations)) {

                std::cout << "NOK!!: " << evse_id << "\n";
                for (const uint32_t e : next_used_evse_ids) {
                    std::cout << e << " ";
                }

                std::cout << "\n";

                return false;
            }
        }
    }

    if (!is_possible) {
        std::cout << "Niet mogelijk: \n";
        for (const uint32_t e : used_evse_ids) {
            std::cout << e << " ";
        }

        std::cout << "\n";
    }

    return is_possible;
}

bool ReservationEVSEs::is_reservation_possible(
    const std::optional<types::evse_manager::ConnectorTypeEnum> global_reservation_type,
    const std::map<uint32_t, types::reservation::Reservation>& evse_specific_reservations) {

    std::vector<types::evse_manager::ConnectorTypeEnum> types;
    for (const auto& global_reservation : this->global_reservations) {
        types.push_back(global_reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown));
    }

    if (global_reservation_type.has_value()) {
        types.push_back(global_reservation_type.value());
    }

    // Check if the total amount of reservations is not more than the total amount of evse's.
    if (types.size() + evse_specific_reservations.size() > this->evses.size()) {
        return false;
    }

    const std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>> orders = get_all_possible_orders(types);

    for (const auto& o : orders) {
        print_order(o);
        if (!this->can_virtual_car_arrive({}, o, evse_specific_reservations)) {
            return false;
        }
    }

    return true;
}

void ReservationEVSEs::set_reservation_timer(const types::reservation::Reservation& reservation,
                                             const std::optional<uint32_t> evse_id) {
    std::lock_guard<std::recursive_mutex> lk(this->timer_mutex);
    this->reservation_id_to_reservation_timeout_timer_map[reservation.reservation_id] =
        std::make_unique<Everest::SteadyTimer>();
    this->reservation_id_to_reservation_timeout_timer_map[reservation.reservation_id]->at(
        [this, reservation, evse_id]() {
            if (evse_id.has_value()) {
                EVLOG_info << "Reservation expired for evse #" << evse_id.value()
                           << " (reservation id: " << reservation.reservation_id << ")";
            } else {
                EVLOG_info << "Reservation expired for reservation id " << reservation.reservation_id;
            }

            this->cancel_reservation(reservation.reservation_id, true);
        },
        Everest::Date::from_rfc3339(reservation.expiry_time));
}

types::reservation::ReservationResult ReservationEVSEs::get_reservation_evse_connector_state() {
    // TODO mz documentation of this strange function!!
    if (!global_reservations.empty() || !(evse_reservations.empty())) {
        return types::reservation::ReservationResult::Occupied;
    }

    ConnectorState state = ConnectorState::AVAILABLE;

    for (const auto& [evse_id, evse] : evses) {
        if (evse.evse_state != ConnectorState::AVAILABLE) {
            if (evse.evse_state == ConnectorState::OCCUPIED) {
                return types::reservation::ReservationResult::Occupied;
            }

            if (evse.evse_state > state) {
                if (evse.evse_state > ConnectorState::OCCUPIED) {
                    if (evse.evse_state == ConnectorState::FAULTED_OCCUPIED) {
                        state = ConnectorState::OCCUPIED;
                    } else if (evse.evse_state == ConnectorState::UNAVAILABLE_FAULTED) {
                        state = ConnectorState::FAULTED;
                    }
                } else {
                    state = evse.evse_state;
                }
            }
        }
    }

    return connector_state_to_reservation_result(state);
}

void ReservationEVSEs::print_order(const std::vector<types::evse_manager::ConnectorTypeEnum>& order) {
    std::cout << "\n ----------\n";
    for (const auto& connector_type : order) {
        std::cout << connector_type_enum_to_string(connector_type) << " ";
    }
    std::cout << "\n";
}

static types::reservation::ReservationResult
connector_state_to_reservation_result(const ConnectorState connector_state) {
    switch (connector_state) {
    case ConnectorState::AVAILABLE:
        return types::reservation::ReservationResult::Accepted;
    case ConnectorState::UNAVAILABLE:
        return types::reservation::ReservationResult::Unavailable;
    case ConnectorState::FAULTED:
    case ConnectorState::UNAVAILABLE_FAULTED:
    case ConnectorState::FAULTED_OCCUPIED:
        return types::reservation::ReservationResult::Faulted;
    case ConnectorState::OCCUPIED:
        return types::reservation::ReservationResult::Occupied;
    }
}

} // namespace module
