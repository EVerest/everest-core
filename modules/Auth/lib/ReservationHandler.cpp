#include <ReservationHandler.hpp>

#include <algorithm>
#include <iostream>

#include <everest/logging.hpp>
#include <utils/date.hpp>

// TODO mz add reservation to persistent storage??? In libocpp if possible?
// TODO mz state for example authorized but not plugged in etc??

namespace module {

static types::reservation::ReservationResult
connector_state_to_reservation_result(const ConnectorState connector_state);

ReservationHandler::ReservationHandler() {
    // Create this worker thread and io service etc here for the timer.
    this->work = boost::make_shared<boost::asio::io_service::work>(this->io_service);
    this->io_service_thread = std::thread([this]() { this->io_service.run(); });
}

ReservationHandler::~ReservationHandler() {
    work->get_io_context().stop();
    io_service.stop();
    io_service_thread.join();
}

void ReservationHandler::add_connector(const uint32_t evse_id, const uint32_t connector_id,
                                       const types::evse_manager::ConnectorTypeEnum connector_type,
                                       const ConnectorState connector_state) {
    EvseConnectorType evse_connector_type;
    evse_connector_type.connector_type = connector_type;
    evse_connector_type.connector_id = connector_id;
    evse_connector_type.state = connector_state;
    Evse evse;

    std::unique_lock<std::recursive_mutex> lock(reservation_mutex);
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
ReservationHandler::make_reservation(const std::optional<uint32_t> evse_id,
                                     const types::reservation::Reservation& reservation) {
    if (date::utc_clock::now() > Everest::Date::from_rfc3339(reservation.expiry_time)) {
        EVLOG_info << "Rejecting reservation because expire time is in the past.";
        return types::reservation::ReservationResult::Rejected;
    }

    // If a reservation was made with an existing reservation id, the existing reservation must be replaced (H01.FR.01).
    // We cancel the reservation here because of that. That also means that if the reservation can not be made, the old
    // reservation is cancelled anyway.
    std::optional<uint32_t> cancelled_reservation_id =
        this->cancel_reservation(reservation.reservation_id, true, types::reservation::ReservationEndReason::Cancelled);
    if (cancelled_reservation_id.has_value()) {
        EVLOG_info << "Cancelled reservation with id " << cancelled_reservation_id.value()
                   << " because a reservation with the same id was made";
    }

    std::unique_lock<std::recursive_mutex> lock(reservation_mutex);
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
                set_reservation_timer(reservation, evse_id);
                this->evse_reservations[evse_id.value()] = reservation;
                return types::reservation::ReservationResult::Accepted;
            }

            // Make a copy of the evse specific reservations map so we can add this reservation to test if the
            // reservation is possible. Only if it is, we add it to the 'member' map.
            std::map<uint32_t, types::reservation::Reservation> evse_specific_reservations = this->evse_reservations;
            evse_specific_reservations[evse_id.value()] = reservation;

            // Check if the reservations are possible with the added evse specific reservation.
            if (!is_reservation_possible(std::nullopt, this->global_reservations, evse_specific_reservations)) {
                return get_reservation_evse_connector_state(connector_type);
            }

            // Reservation is possible, add to evse specific reservations.
            this->evse_reservations[evse_id.value()] = reservation;
        }
    } else {
        if (reservation.connector_type.has_value() &&
            !does_evse_connector_type_exist(reservation.connector_type.value())) {
            return types::reservation::ReservationResult::Rejected;
        }

        const types::evse_manager::ConnectorTypeEnum connector_type =
            reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown);
        if (!is_reservation_possible(connector_type, this->global_reservations, this->evse_reservations)) {
            // TODO mz find real reason?
            return get_reservation_evse_connector_state(connector_type);
        }

        global_reservations.push_back(reservation);
    }

    set_reservation_timer(reservation, evse_id);
    return types::reservation::ReservationResult::Accepted;

    // TODO mz add debug logging to know which reservations are currently active.
}

void ReservationHandler::set_evse_available(const bool available, const bool faulted, const uint32_t evse_id) {
    std::unique_lock<std::recursive_mutex> lock(reservation_mutex);
    if (evses.count(evse_id) == 0) {
        // TODO mz
        return;
    }

    ConnectorState old_evse_state = this->evses[evse_id].evse_state;

    if (faulted) {
        evses[evse_id].evse_state = ConnectorState::FAULTED;
    } else {
        evses[evse_id].evse_state = (available ? ConnectorState::AVAILABLE : ConnectorState::UNAVAILABLE);
    }

    if ((!available || faulted) && old_evse_state == ConnectorState::AVAILABLE) {
        if (evse_reservations.count(evse_id)) {
            cancel_reservation(evse_reservations[evse_id].reservation_id, true,
                               types::reservation::ReservationEndReason::Cancelled);
            return;
        }

        // One connector is not available anymore but there was no specific reservation for this connector. Let's check
        // if all reservations are still possible and if not: cancel them.
        check_reservations_and_cancel_if_not_possible();
    }
}

void ReservationHandler::set_connector_available(const bool available, const bool faulted, const uint32_t evse_id,
                                                 const uint32_t connector_id) {
    std::unique_lock<std::recursive_mutex> lock(reservation_mutex);

    if (evses.count(evse_id) == 0) {
        // TODO mz
        return;
    }

    auto& connectors = evses[evse_id].connectors;
    auto connector_it =
        std::find_if(connectors.begin(), connectors.end(),
                     [connector_id](EvseConnectorType& connector) { return connector_id == connector.connector_id; });

    if (connector_it == connectors.end()) {
        // Connector with specific connector id not found
        // TODO mz
        return;
    }

    ConnectorState old_connector_state = connector_it->state;
    if (faulted) {
        connector_it->state = ConnectorState::FAULTED;
    } else {
        connector_it->state = (available ? ConnectorState::AVAILABLE : ConnectorState::UNAVAILABLE);
    }

    if ((!available || faulted) && old_connector_state == ConnectorState::AVAILABLE) {
        if (evse_reservations.count(evse_id) != 0 && evse_reservations[evse_id].connector_type.has_value() &&
            (connector_it->connector_type == evse_reservations[evse_id].connector_type.value() ||
             connector_it->connector_type == types::evse_manager::ConnectorTypeEnum::Unknown ||
             evse_reservations[evse_id].connector_type.value() == types::evse_manager::ConnectorTypeEnum::Unknown)) {
            cancel_reservation(evse_reservations[evse_id].reservation_id, true,
                               types::reservation::ReservationEndReason::Cancelled);
            return;
        }

        // Now we have one connector less, let's check if all reservations are still possible now and if not, cancel
        // the one(s) that can not be done anymore.
        check_reservations_and_cancel_if_not_possible();
    }
}

bool ReservationHandler::is_charging_possible(const uint32_t evse_id) {
    std::unique_lock<std::recursive_mutex> lock(reservation_mutex);
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
    return is_reservation_possible(std::nullopt, this->global_reservations, reservations);
}

bool ReservationHandler::is_evse_reserved(const uint32_t evse_id) {
    if (this->evse_reservations.count(evse_id) > 0) {
        return true;
    }

    return false;
}

std::optional<uint32_t> ReservationHandler::cancel_reservation(int reservation_id, bool execute_callback,
                                                               const types::reservation::ReservationEndReason reason) {
    std::unique_lock<std::recursive_mutex> lock(reservation_mutex);

    bool reservation_cancelled = false;

    {
        std::unique_lock<std::recursive_mutex> lk(this->timer_mutex);
        auto reservation_id_timer_it = this->reservation_id_to_reservation_timeout_timer_map.find(reservation_id);
        if (reservation_id_timer_it != this->reservation_id_to_reservation_timeout_timer_map.end()) {
            reservation_id_timer_it->second->stop();
            this->reservation_id_to_reservation_timeout_timer_map.erase(reservation_id_timer_it);
            reservation_cancelled = true;
        }
    }

    if (!reservation_cancelled) {
        return std::nullopt;
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
        }
    }

    if (execute_callback && this->reservation_cancelled_callback != nullptr) {
        this->reservation_cancelled_callback(evse_id, reservation_id, reason);
    }

    return evse_id;
}

void ReservationHandler::register_reservation_cancelled_callback(
    const std::function<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                             const types::reservation::ReservationEndReason reason)>& callback) {
    this->reservation_cancelled_callback = callback;
}

void ReservationHandler::on_reservation_used(const int32_t reservation_id) {
    const std::optional<uint32_t> evse_id =
        this->cancel_reservation(reservation_id, false, types::reservation::ReservationEndReason::UsedToStartCharging);
    if (evse_id.has_value()) {
        EVLOG_info << "Reservation for evse#" << evse_id.value() << " used and cancelled";
    } else {
        EVLOG_info << "Reservation without evse id used and cancelled";
    }
}

std::optional<int32_t> ReservationHandler::matches_reserved_identifier(const std::optional<uint32_t> evse_id,
                                                                       const std::string& id_token,
                                                                       std::optional<std::string> parent_id_token) {
    std::lock_guard<std::recursive_mutex> lock(this->reservation_mutex);

    // Return true if id tokens match or parent id tokens exists and match.
    if (evse_id.has_value()) {
        for (const auto& evse_reservation : this->evse_reservations) {
            if (evse_reservation.second.id_token == id_token ||
                (parent_id_token.has_value() && evse_reservation.second.parent_id_token.has_value() &&
                 parent_id_token.value() == evse_reservation.second.parent_id_token.value())) {
                if (evse_reservation.first != evse_id.value()) {
                    // We could have searched for another reservation with this id, but the problem is that if someone
                    // starts charging at another charging station, we might have a problem with the other reservations.
                    // So here we just return that there is no reservation for this identifier.
                    // TODO mz write test for this
                    return std::nullopt;
                }

                return evse_reservation.second.reservation_id;
            }
        }
    }

    // If evse_id == 0, search globally for reservation with this token.
    for (const auto& reservation : global_reservations) {
        if (reservation.id_token == id_token ||
            (parent_id_token.has_value() && reservation.parent_id_token.has_value() &&
             parent_id_token.value() == reservation.parent_id_token.value())) {
            return reservation.reservation_id;
        }
    }

    return std::nullopt;
}

bool ReservationHandler::has_reservation_parent_id(const std::optional<uint32_t> evse_id) {
    std::lock_guard<std::recursive_mutex> lock(this->reservation_mutex);

    if (evse_id.has_value()) {
        if (this->evse_reservations.count(evse_id.value())) {
            return this->evse_reservations[evse_id.value()].parent_id_token.has_value();
        }

        return false;
    }

    // Check if one of the global reservations has a parent id.
    for (const auto& reservation : this->global_reservations) {
        if (reservation.parent_id_token.has_value()) {
            return true;
        }
    }

    return false;
}

bool ReservationHandler::has_evse_connector_type(const std::vector<EvseConnectorType> evse_connectors,
                                                 const types::evse_manager::ConnectorTypeEnum connector_type) const {
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

bool ReservationHandler::does_evse_connector_type_exist(
    const types::evse_manager::ConnectorTypeEnum connector_type) const {
    for (const auto& [evse_id, evse] : evses) {
        if (has_evse_connector_type(evse.connectors, connector_type)) {
            return true;
        }
    }

    return false;
}

types::reservation::ReservationResult ReservationHandler::get_evse_state(
    const uint32_t evse_id, const std::map<uint32_t, types::reservation::Reservation> evse_specific_reservations) {
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
ReservationHandler::is_connector_available(const uint32_t evse_id,
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
                connector_state = get_new_connector_state(connector_state, connector.state);
            }
        }
    }

    return connector_state_to_reservation_result(connector_state);
}

std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>> ReservationHandler::get_all_possible_orders(
    const std::vector<types::evse_manager::ConnectorTypeEnum>& connectors) const {

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

bool ReservationHandler::can_virtual_car_arrive(
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

bool ReservationHandler::is_reservation_possible(
    const std::optional<types::evse_manager::ConnectorTypeEnum> global_reservation_type,
    const std::vector<types::reservation::Reservation>& reservations_no_evse,
    const std::map<uint32_t, types::reservation::Reservation>& evse_specific_reservations) {

    std::vector<types::evse_manager::ConnectorTypeEnum> types;
    for (const auto& global_reservation : reservations_no_evse) {
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

void ReservationHandler::set_reservation_timer(const types::reservation::Reservation& reservation,
                                               const std::optional<uint32_t> evse_id) {
    std::lock_guard<std::recursive_mutex> lk(this->timer_mutex);
    this->reservation_id_to_reservation_timeout_timer_map[reservation.reservation_id] =
        std::make_unique<Everest::SteadyTimer>(&this->io_service);
    this->reservation_id_to_reservation_timeout_timer_map[reservation.reservation_id]->at(
        [this, reservation, evse_id]() {
            if (evse_id.has_value()) {
                EVLOG_info << "Reservation expired for evse #" << evse_id.value()
                           << " (reservation id: " << reservation.reservation_id << ")";
            } else {
                EVLOG_info << "Reservation expired for reservation id " << reservation.reservation_id;
            }

            this->cancel_reservation(reservation.reservation_id, true,
                                     types::reservation::ReservationEndReason::Expired);
        },
        Everest::Date::from_rfc3339(reservation.expiry_time));
}

std::vector<ReservationHandler::Evse> ReservationHandler::get_all_evses_with_connector_type(
    const types::evse_manager::ConnectorTypeEnum connector_type) const {
    std::vector<Evse> result;
    for (const auto& evse : this->evses) {
        if (this->has_evse_connector_type(evse.second.connectors, connector_type)) {
            result.push_back(evse.second);
        }
    }

    return result;
}

ConnectorState ReservationHandler::get_new_connector_state(ConnectorState current_state,
                                                           const ConnectorState new_state) const {
    if (new_state == ConnectorState::OCCUPIED) {
        return ConnectorState::OCCUPIED;
    }

    if (new_state > current_state) {
        if (new_state > ConnectorState::OCCUPIED) {
            if (new_state == ConnectorState::FAULTED_OCCUPIED) {
                current_state = ConnectorState::OCCUPIED;
            } else if (new_state == ConnectorState::UNAVAILABLE_FAULTED) {
                if (current_state != ConnectorState::OCCUPIED) {
                    current_state = ConnectorState::FAULTED;
                }
            }
        } else {
            current_state = new_state;
        }
    }

    return current_state;
}

types::reservation::ReservationResult ReservationHandler::get_reservation_evse_connector_state(
    const types::evse_manager::ConnectorTypeEnum connector_type) const {
    // TODO mz documentation of this strange function!!
    if (!global_reservations.empty() || !(evse_reservations.empty())) {
        return types::reservation::ReservationResult::Occupied;
    }

    bool found_state = false;

    ConnectorState state = ConnectorState::UNAVAILABLE;

    for (const auto& [evse_id, evse] : evses) {
        if (evse.evse_state != ConnectorState::AVAILABLE) {
            state = get_new_connector_state(state, evse.evse_state);
            found_state = true;
        }
    }

    if (!found_state) {
        const std::vector<Evse> evses_with_connector_type = this->get_all_evses_with_connector_type(connector_type);
        if (evses_with_connector_type.empty()) {
            // This should not happen because then it should have been rejected before already somewhere in the code...
            return types::reservation::ReservationResult::Rejected;
        }

        for (const auto& evse : evses_with_connector_type) {
            for (const auto& connector : evse.connectors) {
                if (connector.connector_type != connector_type ||
                    connector.connector_type == types::evse_manager::ConnectorTypeEnum::Unknown ||
                    connector_type == types::evse_manager::ConnectorTypeEnum::Unknown) {
                    continue;
                }

                if (connector.state != ConnectorState::AVAILABLE) {
                    state = get_new_connector_state(state, connector.state);
                }
            }
        }
    }

    return connector_state_to_reservation_result(state);
}

void ReservationHandler::check_reservations_and_cancel_if_not_possible() {
    std::unique_lock<std::recursive_mutex> lock(reservation_mutex);

    std::vector<int32_t> reservations_to_cancel;
    std::map<uint32_t, types::reservation::Reservation> evse_specific_reservations;
    std::vector<types::reservation::Reservation> reservations_no_evse;

    for (const auto& [evse_id, reservation] : this->evse_reservations) {
        evse_specific_reservations[evse_id] = reservation;
        if (!is_reservation_possible(std::nullopt, reservations_no_evse, evse_specific_reservations)) {
            reservations_to_cancel.push_back(reservation.reservation_id);
            evse_specific_reservations.erase(evse_id);
        }
    }

    for (const auto& reservation : this->global_reservations) {
        if (is_reservation_possible(reservation.connector_type, reservations_no_evse, evse_specific_reservations)) {
            reservations_no_evse.push_back(reservation);
        } else {
            reservations_to_cancel.push_back(reservation.reservation_id);
        }
    }

    for (const int32_t reservation_id : reservations_to_cancel) {
        this->cancel_reservation(reservation_id, true, types::reservation::ReservationEndReason::Cancelled);
    }
}

void ReservationHandler::print_order(const std::vector<types::evse_manager::ConnectorTypeEnum>& order) const {
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

    return types::reservation::ReservationResult::Rejected;
}

} // namespace module
