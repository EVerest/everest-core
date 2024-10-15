#include <ReservationEVSEs.h>

#include <algorithm>
#include <iostream>

namespace module {
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

bool ReservationEVSEs::make_reservation(const std::optional<uint32_t> evse_id,
                                        const types::reservation::Reservation& reservation) {
    if (evse_id.has_value()) {
        if (evse_reservations.count(evse_id.value()) > 0) {
            return false;
        }

        const types::evse_manager::ConnectorTypeEnum connector_type =
            reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown);

        if (has_evse_connector_type(this->evses[evse_id.value()].connectors, connector_type) &&
            is_evse_available(evse_id.value(), this->evse_reservations) &&
            is_connector_available(evse_id.value(), connector_type)) {
            if (global_reservations.empty()) {
                return true;
            }

            // Make a copy of the evse specific reservations map so we can add this reservation to test if the
            // reservation is possible. Only if it is, we add it to the 'member' map.
            std::map<uint32_t, types::reservation::Reservation> evse_specific_reservations = this->evse_reservations;
            evse_specific_reservations[evse_id.value()] = reservation;

            // Check if the reservations are possible with the added evse specific reservation.
            if (!is_reservation_possible(std::nullopt, evse_specific_reservations)) {
                return false;
            }

            // std::vector<types::evse_manager::ConnectorTypeEnum> types;
            // for (const auto& global_reservation : this->global_reservations) {
            //     types.push_back(
            //         global_reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown));
            // }

            // std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>> orders = get_all_possible_orders(types);

            // // Check if reservation is possible.
            // for (const auto& o : orders) {
            //     print_order(o);
            //     if (!this->can_virtual_car_arrive({}, o, evse_specific_reservations)) {
            //         return false;
            //     }
            // }

            // Reservation is possible, add to evse specific reservations.
            this->evse_reservations[evse_id.value()] = reservation;
        } else {
            return false;
        }
    } else {
        if (!is_reservation_possible(
                reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown),
                this->evse_reservations)) {
            return false;
        }
        // std::vector<types::evse_manager::ConnectorTypeEnum> types;
        // for (const auto& global_reservation : this->global_reservations) {
        //     types.push_back(
        //         global_reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown));
        // }

        // types.push_back(reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown));

        // std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>> orders = get_all_possible_orders(types);

        // for (const auto& o : orders) {
        //     print_order(o);
        //     if (!this->can_virtual_car_arrive({}, o, evse_reservations)) {
        //         return false;
        //     }
        // }

        global_reservations.push_back(reservation);
    }

    return true;
}

void ReservationEVSEs::set_evse_available(const bool available, const uint32_t evse_id) {
    if (evses.count(evse_id) == 0) {
        // TODO mz
        return;
    }

    evses[evse_id].evse_state = (available ? ConnectorState::AVAILABLE : ConnectorState::UNAVAILABLE);

    if (!available) {
        // TODO mz if connector changed from available to unavailable: cancel a reservation???
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

bool ReservationEVSEs::is_evse_available(
    const uint32_t evse_id, const std::map<uint32_t, types::reservation::Reservation> evse_specific_reservations) {
    if (evses.count(evse_id) == 0) {
        // TODO mz
        return false;
    }

    // Check if evse is available.
    if (evses[evse_id].evse_state != ConnectorState::AVAILABLE) {
        return false;
    }

    // If one connector is occupied, then the other connector can also not be used (one connector of an evse can be
    // used at the same time).
    for (const auto& connector : evses[evse_id].connectors) {
        if (connector.state == ConnectorState::OCCUPIED || connector.state == ConnectorState::FAULTED_OCCUPIED) {
            return false;
        }
    }

    // If evse is reserved, it is not available.
    if (evse_specific_reservations.count(evse_id) > 0) {
        return false;
    }

    return true;
}

bool ReservationEVSEs::is_connector_available(const uint32_t evse_id,
                                              const types::evse_manager::ConnectorTypeEnum connector_type) {
    if (evses.count(evse_id) == 0) {
        // TODO mz
        return false;
    }

    for (const auto& connector : evses[evse_id].connectors) {
        if ((connector.connector_type == connector_type ||
             connector.connector_type == types::evse_manager::ConnectorTypeEnum::Unknown ||
             connector_type == types::evse_manager::ConnectorTypeEnum::Unknown) &&
            connector.state == ConnectorState::AVAILABLE) {
            return true;
        }
    }

    return false;
}

std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>>
ReservationEVSEs::get_all_possible_orders(const std::vector<types::evse_manager::ConnectorTypeEnum>& connectors) {

    std::vector<types::evse_manager::ConnectorTypeEnum> input_next = connectors;
    std::vector<types::evse_manager::ConnectorTypeEnum> input_prev = connectors;
    std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>> output;

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

        if (is_evse_available(evse_id, evse_specific_reservations) &&
            has_evse_connector_type(evse.connectors, next_car_arrival_order.at(0)) &&
            is_connector_available(evse_id, next_car_arrival_order.at(0))) {
            // TODO mz check if evse is available and if connector is available (not faulted etc)
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

void ReservationEVSEs::print_order(const std::vector<types::evse_manager::ConnectorTypeEnum>& order) {
    std::cout << "\n ----------\n";
    for (const auto& connector_type : order) {
        std::cout << connector_type_enum_to_string(connector_type) << " ";
    }
    std::cout << "\n";
}

} // namespace module
