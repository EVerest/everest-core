#include <ReservationHandler.hpp>

#include <algorithm>

#include <Connector.hpp>

#include <everest/logging.hpp>
#include <everest/staging/helpers/helpers.hpp>
#include <generated/interfaces/kvs/Interface.hpp>
#include <utils/date.hpp>

namespace module {

static types::reservation::ReservationResult
connector_state_to_reservation_result(const ConnectorState connector_state);

ReservationHandler::ReservationHandler(std::map<int, std::unique_ptr<module::EVSEContext>>& evses,
                                       const std::string& id, kvsIntf* store) :
    evses(evses), kvs_store_key_id("reservation_" + id), store(store) {
    // Create this worker thread and io service etc here for the timer.
    this->work = boost::make_shared<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>>(
        boost::asio::make_work_guard(this->io_context));
    this->io_context_thread = std::thread([this]() { this->io_context.run(); });
}

ReservationHandler::~ReservationHandler() {
    work->get_executor().context().stop();
    io_context.stop();
    io_context_thread.join();
}

void ReservationHandler::load_reservations() {
    std::lock_guard<std::recursive_mutex> lk(this->event_mutex);
    if (this->store == nullptr) {
        EVLOG_info << "Can not load reservations because the store is a nullptr.";
        return;
    }

    const auto stored_reservations = store->call_load(this->kvs_store_key_id);
    const Array* reservations_json = std::get_if<Array>(&stored_reservations);
    if (reservations_json == nullptr) {
        EVLOG_warning << "Can not load reservations: reservations is not a json array.";
        return;
    }

    for (const auto& reservation : *reservations_json) {
        types::reservation::Reservation r;
        try {
            r = reservation.at("reservation");
        } catch (const json::exception& e) {
            EVLOG_error << "Could not get reservation from store: " << e.what();
            continue;
        }

        std::optional<uint32_t> evse_id;
        if (reservation.contains("evse_id")) {
            evse_id = reservation.at("evse_id");
        }

        types::reservation::ReservationResult reservation_result = this->make_reservation(evse_id, r);
        if (reservation_result != types::reservation::ReservationResult::Accepted) {
            EVLOG_warning << "Load reservations: Could not make reservation with id " << r.reservation_id
                          << ": reservation cancelled.";
            this->reservation_cancelled_callback(evse_id, r.reservation_id,
                                                 types::reservation::ReservationEndReason::Cancelled, true);
        }
    }
}

types::reservation::ReservationResult
ReservationHandler::make_reservation(const std::optional<uint32_t> evse_id,
                                     const types::reservation::Reservation& reservation) {
    std::lock_guard<std::recursive_mutex> lk(this->event_mutex);
    if (date::utc_clock::now() > Everest::Date::from_rfc3339(reservation.expiry_time)) {
        EVLOG_info << "Rejecting reservation because expire time is in the past.";
        return types::reservation::ReservationResult::Rejected;
    }

    // If a reservation was made with an existing reservation id, the existing reservation must be replaced (H01.FR.01).
    // We cancel the reservation here because of that. That also means that if the reservation can not be made, the old
    // reservation is cancelled anyway.
    std::pair<bool, std::optional<uint32_t>> reservation_cancelled = this->cancel_reservation(
        reservation.reservation_id, false, types::reservation::ReservationEndReason::Cancelled);
    if (reservation_cancelled.first && reservation_cancelled.second.has_value()) {
        EVLOG_debug << "Cancelled reservation with id " << reservation.reservation_id << " for evse id "
                    << reservation_cancelled.second.value() << " because a reservation with the same id was made";
    }

    if (evse_id.has_value()) {
        if (this->evse_reservations.count(evse_id.value()) > 0) {
            // There already is a reservation for this evse.
            EVLOG_debug << "Rejected reservation because there already is a reservation for this evse.";
            return types::reservation::ReservationResult::Occupied;
        }

        if (this->evses.count(evse_id.value()) == 0) {
            // There is no evse with this evse id.
            EVLOG_warning << "Rejected reservation because there is no evse with this evse id: " << evse_id.value();
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
            this->get_evse_connector_state_reservation_result(evse_id.value(), this->evse_reservations);
        const types::reservation::ReservationResult connector_state =
            this->get_connector_availability_reservation_result(evse_id.value(), connector_type);

        if (!has_evse_connector_type(this->evses[evse_id.value()]->connectors, connector_type)) {
            EVLOG_debug << "Rejected reservation because this evse (id: " << evse_id.value()
                        << ") does not have the requested connector type ("
                        << types::evse_manager::connector_type_enum_to_string(connector_type) << ")";
            return types::reservation::ReservationResult::Rejected;
        } else if (evse_state != types::reservation::ReservationResult::Accepted) {
            print_reservations_debug_info(reservation, evse_id, true);
            EVLOG_debug << "Rejecting reservation because connector is not available";
            return evse_state;
        } else if (connector_state != types::reservation::ReservationResult::Accepted) {
            print_reservations_debug_info(reservation, evse_id, true);
            return connector_state;
        } else {
            // Everything fine, continue.
            if (global_reservations.empty()) {
                set_reservation_timer(reservation, evse_id);
                this->evse_reservations[evse_id.value()] = reservation;
                EVLOG_info << "Created reservation for evse id " << evse_id.value() << ", connector type "
                           << types::evse_manager::connector_type_enum_to_string(
                                  reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown));
                return types::reservation::ReservationResult::Accepted;
            }

            // Make a copy of the evse specific reservations map so we can add this reservation to test if the
            // reservation is possible. Only if it is, we add it to the 'member' map.
            std::map<uint32_t, types::reservation::Reservation> evse_specific_reservations = this->evse_reservations;
            evse_specific_reservations[evse_id.value()] = reservation;

            // Check if the reservations are possible with the added evse specific reservation.
            if (!is_reservation_possible(std::nullopt, this->global_reservations, evse_specific_reservations)) {
                print_reservations_debug_info(reservation, evse_id, true);
                return get_reservation_evse_connector_state(connector_type);
            }

            // Reservation is possible, add to evse specific reservations.
            this->evse_reservations[evse_id.value()] = reservation;
            EVLOG_info << "Created reservation for evse id " << evse_id.value() << ", connector type "
                       << types::evse_manager::connector_type_enum_to_string(
                              reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown));
        }
    } else {
        if (reservation.connector_type.has_value() &&
            !does_evse_connector_type_exist(reservation.connector_type.value())) {
            EVLOG_info << "Can not make reservation because the connector type does not exist.";
            return types::reservation::ReservationResult::Rejected;
        }

        const types::evse_manager::ConnectorTypeEnum connector_type =
            reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown);
        if (!is_reservation_possible(connector_type, this->global_reservations, this->evse_reservations)) {
            print_reservations_debug_info(reservation, evse_id, true);
            return get_reservation_evse_connector_state(connector_type);
        }

        EVLOG_info << "Created reservation for connector type "
                   << types::evse_manager::connector_type_enum_to_string(
                          reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown));
        global_reservations.push_back(reservation);
        store_reservations();
    }

    set_reservation_timer(reservation, evse_id);

    return types::reservation::ReservationResult::Accepted;
}

void ReservationHandler::on_connector_state_changed(const ConnectorState connector_state, const uint32_t evse_id,
                                                    const uint32_t connector_id) {
    std::lock_guard<std::recursive_mutex> lk(this->event_mutex);
    if (connector_state == ConnectorState::AVAILABLE) {
        // Nothing to cancel.
        return;
    }

    if (this->evses.count(static_cast<int32_t>(evse_id)) == 0) {
        EVLOG_warning << "on_connector_state_changed: evse " << evse_id << " does not exist. This should not happen.";
        return;
    }

    auto& connectors = this->evses[evse_id]->connectors;
    auto connector_it = std::find_if(connectors.begin(), connectors.end(),
                                     [connector_id](const auto& connector) { return connector_id == connector.id; });

    if (connector_it == connectors.end()) {
        // Connector with specific connector id not found
        EVLOG_warning << "Could not change connector state for connector id " << connector_id << " of evse " << evse_id
                      << ": connector id does not exist. This should not happen.";
        return;
    }

    const bool reservation_exists = evse_reservations.count(evse_id) != 0;

    if (reservation_exists && evse_reservations[evse_id].connector_type.has_value() &&
        (connector_it->type == evse_reservations[evse_id].connector_type.value() ||
         connector_it->type == types::evse_manager::ConnectorTypeEnum::Unknown ||
         evse_reservations[evse_id].connector_type.value() == types::evse_manager::ConnectorTypeEnum::Unknown)) {
        cancel_reservation(evse_reservations[evse_id].reservation_id, true,
                           types::reservation::ReservationEndReason::Cancelled);
        return;
    }

    // Now we might have one connector less, let's check if all reservations are still possible now and if not, cancel
    // the one(s) that can not be done anymore.
    check_reservations_and_cancel_if_not_possible();
}

bool ReservationHandler::is_charging_possible(const uint32_t evse_id) {
    std::lock_guard<std::recursive_mutex> lk(this->event_mutex);
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
    std::lock_guard<std::recursive_mutex> lk(this->event_mutex);
    if (this->evse_reservations.count(evse_id) > 0) {
        return true;
    }

    return false;
}

std::pair<bool, std::optional<uint32_t>>
ReservationHandler::cancel_reservation(const int reservation_id, const bool execute_callback,
                                       const types::reservation::ReservationEndReason reason) {
    std::lock_guard<std::recursive_mutex> lk(this->event_mutex);
    std::pair<bool, std::optional<uint32_t>> result;

    bool reservation_cancelled = false;

    auto reservation_id_timer_it = this->reservation_id_to_reservation_timeout_timer_map.find(reservation_id);
    if (reservation_id_timer_it != this->reservation_id_to_reservation_timeout_timer_map.end()) {
        reservation_id_timer_it->second->stop();
        this->reservation_id_to_reservation_timeout_timer_map.erase(reservation_id_timer_it);
        reservation_cancelled = true;
        result.first = true;
    }

    if (!reservation_cancelled) {
        result.first = false;
        return result;
    }

    EVLOG_info << "Cancel reservation with reservation id " << reservation_id;

    std::optional<uint32_t> evse_id;
    for (const auto& reservation : this->evse_reservations) {
        if (reservation.second.reservation_id == reservation_id) {
            evse_id = reservation.first;
        }
    }

    if (evse_id.has_value()) {
        auto it = this->evse_reservations.find(evse_id.value());
        if (it != this->evse_reservations.end()) {
            this->evse_reservations.erase(it);
        } else {
            EVLOG_warning << "Could not remove reservation with evse id " << evse_id.value()
                          << ": this should not happen";
        }

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

    this->store_reservations();

    if (execute_callback && this->reservation_cancelled_callback != nullptr) {
        this->reservation_cancelled_callback(evse_id, reservation_id, reason, execute_callback);
    }

    result.second = evse_id;
    return result;
}

bool ReservationHandler::cancel_reservation(const uint32_t evse_id, const bool execute_callback) {
    auto it = this->evse_reservations.find(evse_id);
    if (it != this->evse_reservations.end()) {
        int reservation_id = it->second.reservation_id;
        return this
            ->cancel_reservation(reservation_id, execute_callback, types::reservation::ReservationEndReason::Cancelled)
            .first;
    } else {
        EVLOG_warning << "Could not cancel reservation with evse id " << evse_id;
        return false;
    }
}

void ReservationHandler::register_reservation_cancelled_callback(
    const std::function<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id,
                             const types::reservation::ReservationEndReason reason,
                             const bool send_reservation_update)>& callback) {
    this->reservation_cancelled_callback = callback;
}

void ReservationHandler::on_reservation_used(const int32_t reservation_id) {
    std::lock_guard<std::recursive_mutex> lk(this->event_mutex);
    const std::pair<bool, std::optional<uint32_t>> cancelled =
        this->cancel_reservation(reservation_id, false, types::reservation::ReservationEndReason::UsedToStartCharging);
    if (cancelled.first) {
        if (cancelled.second.has_value()) {
            EVLOG_info << "Reservation (" << reservation_id << ") for evse#" << cancelled.second.value()
                       << " used and cancelled";
        } else {
            EVLOG_info << "Reservation (" << reservation_id << ") without evse id used and cancelled";
        }
    } else {
        EVLOG_info << "Could not cancel reservation with reservation id " << reservation_id;
    }
}

std::optional<int32_t> ReservationHandler::matches_reserved_identifier(const std::string& id_token,
                                                                       const std::optional<uint32_t> evse_id,
                                                                       std::optional<std::string> parent_id_token) {
    std::lock_guard<std::recursive_mutex> lk(this->event_mutex);
    EVLOG_debug << "Matches reserved identifier for evse id " << (evse_id.has_value() ? evse_id.value() : 9999)
                << " and id token " << everest::staging::helpers::redact(id_token) << " and parent id token "
                << (parent_id_token.has_value() ? everest::staging::helpers::redact(parent_id_token.value()) : "-");

    // Return true if id tokens match or parent id tokens exists and match.
    if (evse_id.has_value()) {
        if (this->evse_reservations.count(evse_id.value())) {
            const types::reservation::Reservation& reservation = this->evse_reservations[evse_id.value()];
            if (reservation.id_token == id_token ||
                (parent_id_token.has_value() && reservation.parent_id_token.has_value() &&
                 parent_id_token.value() == reservation.parent_id_token.value())) {
                EVLOG_debug << "There is a reservation (" << reservation.reservation_id << ") for evse "
                            << evse_id.value() << " and the token matches";
                return reservation.reservation_id;
            } else {
                EVLOG_debug << "There is a reservation for evse id " << evse_id.value() << ", but token does not match";
                return std::nullopt;
            }
        }
    }

    // If evse_id == 0 or there is no reservation found with the given evse id, search globally for reservation with
    // this token.
    for (const auto& reservation : global_reservations) {
        if (reservation.id_token == id_token ||
            (parent_id_token.has_value() && reservation.parent_id_token.has_value() &&
             parent_id_token.value() == reservation.parent_id_token.value())) {
            EVLOG_debug << "There is a reservation for the token, reservation id: " << reservation.reservation_id;
            return reservation.reservation_id;
        }
    }

    EVLOG_debug << "No reservation found which matches the reserved identifier";
    return std::nullopt;
}

bool ReservationHandler::has_reservation_parent_id(const std::optional<uint32_t> evse_id) {
    std::lock_guard<std::recursive_mutex> lk(this->event_mutex);
    if (evse_id.has_value()) {
        if (this->evses.count(evse_id.value()) == 0) {
            // EVSE id does not exist.
            return false;
        }

        if (this->evse_reservations.count(evse_id.value())) {
            return this->evse_reservations[evse_id.value()].parent_id_token.has_value();
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

ReservationEvseStatus ReservationHandler::check_number_global_reservations_match_number_available_evses() {
    std::unique_lock<std::recursive_mutex> reservation_lock(this->event_mutex);
    std::set<int32_t> available_evses;
    // Get all evse's that are not reserved or used.
    for (const auto& evse : this->evses) {
        if (get_evse_connector_state_reservation_result(static_cast<uint32_t>(evse.first), this->evse_reservations) ==
                types::reservation::ReservationResult::Accepted &&
            get_connector_availability_reservation_result(static_cast<uint32_t>(evse.first),
                                                          types::evse_manager::ConnectorTypeEnum::Unknown) ==
                types::reservation::ReservationResult::Accepted) {
            available_evses.insert(evse.first);
        }
    }
    if (available_evses.size() == this->global_reservations.size()) {
        // There are as many evses available as 'global' reservations, so all evse's are reserved. Set all available
        // evse's to reserved.
        return get_evse_global_reserved_status_and_set_new_status(available_evses, available_evses);
    }

    // There are not as many global reservations as available evse's, but we have to check for specific connector types
    // as well.
    std::set<int32_t> reserved_evses_with_specific_connector_type;
    for (const auto& global_reservation : this->global_reservations) {
        if (!is_reservation_possible(global_reservation.connector_type, this->global_reservations,
                                     this->evse_reservations)) {
            // A new reservation with this type is not possible (so also arrival of an extra car is not), so all evse's
            // with this connector type should be set to reserved.
            for (const auto& evse : this->evses) {
                if (available_evses.find(evse.first) != available_evses.end() &&
                    this->has_evse_connector_type(
                        evse.second->connectors,
                        global_reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown))) {
                    // This evse is available and has a specific connector type. So it should be set to unavailable.
                    reserved_evses_with_specific_connector_type.insert(evse.first);
                }
            }
        }
    }

    return get_evse_global_reserved_status_and_set_new_status(available_evses,
                                                              reserved_evses_with_specific_connector_type);
}

bool ReservationHandler::has_evse_connector_type(const std::vector<Connector>& evse_connectors,
                                                 const types::evse_manager::ConnectorTypeEnum connector_type) const {
    if (connector_type == types::evse_manager::ConnectorTypeEnum::Unknown) {
        return true;
    }

    for (const auto& connector : evse_connectors) {
        if (connector.type == types::evse_manager::ConnectorTypeEnum::Unknown || connector.type == connector_type) {
            return true;
        }
    }

    return false;
}

bool ReservationHandler::does_evse_connector_type_exist(
    const types::evse_manager::ConnectorTypeEnum connector_type) const {
    for (const auto& [evse_id, evse] : evses) {
        if (has_evse_connector_type(evse->connectors, connector_type)) {
            return true;
        }
    }

    return false;
}

types::reservation::ReservationResult ReservationHandler::get_evse_connector_state_reservation_result(
    const uint32_t evse_id, const std::map<uint32_t, types::reservation::Reservation>& evse_specific_reservations) {
    if (evses.count(evse_id) == 0) {
        EVLOG_warning << "Get evse state for evse " << evse_id
                      << " not possible: evse id does not exists. This should not happen.";
        return types::reservation::ReservationResult::Rejected;
    }

    // Check if evse is available.
    if (evses[evse_id]->plugged_in) {
        return connector_state_to_reservation_result(ConnectorState::OCCUPIED);
    }

    // If one connector is occupied, then the other connector can also not be used (one connector of an evse can be
    // used at the same time).
    for (const auto& connector : evses[evse_id]->connectors) {
        if (connector.get_state() == ConnectorState::OCCUPIED ||
            connector.get_state() == ConnectorState::FAULTED_OCCUPIED) {
            return connector_state_to_reservation_result(connector.get_state());
        }
    }

    // If evse is reserved, it is not available.
    if (evse_specific_reservations.count(evse_id) > 0) {
        return types::reservation::ReservationResult::Occupied;
    }

    return types::reservation::ReservationResult::Accepted;
}

types::reservation::ReservationResult ReservationHandler::get_connector_availability_reservation_result(
    const uint32_t evse_id, const types::evse_manager::ConnectorTypeEnum connector_type) {
    if (evses.count(evse_id) == 0) {
        EVLOG_warning << "Request if connector is available for evse id " << evse_id
                      << ", but evse id does not exist. This should not happen.";
        return types::reservation::ReservationResult::Rejected;
    }

    ConnectorState connector_state = ConnectorState::UNAVAILABLE;

    for (const auto& connector : evses[evse_id]->connectors) {
        if ((connector.type == connector_type || connector.type == types::evse_manager::ConnectorTypeEnum::Unknown ||
             connector_type == types::evse_manager::ConnectorTypeEnum::Unknown)) {
            if (connector.get_state() == ConnectorState::AVAILABLE) {
                return types::reservation::ReservationResult::Accepted;
            } else {
                connector_state = get_new_connector_state(connector_state, connector.get_state());
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

    // For next_permutation, the input must be ordered or it will stop halfway. So if it stops halafway,
    // prev_permutation will find the others.
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

        if (get_evse_connector_state_reservation_result(evse_id, evse_specific_reservations) ==
                types::reservation::ReservationResult::Accepted &&
            has_evse_connector_type(evse->connectors, next_car_arrival_order.at(0)) &&
            get_connector_availability_reservation_result(evse_id, next_car_arrival_order.at(0)) ==
                types::reservation::ReservationResult::Accepted) {
            is_possible = true;

            std::vector<uint32_t> next_used_evse_ids = used_evse_ids;
            // Add evse id to list when we call the function recursively.
            next_used_evse_ids.push_back(evse_id);

            // Check if this is the last.
            if (next_car_arrival_order.size() == 1) {
                // If this is the last and a car can arrive, then this combination is possible.
                return true;
            }

            // Call next level recursively.
            // Remove connector type ('car') from list when we call the function recursively.
            const std::vector<types::evse_manager::ConnectorTypeEnum> next_arrival_order(
                next_car_arrival_order.begin() + 1, next_car_arrival_order.end());

            if (!this->can_virtual_car_arrive(next_used_evse_ids, next_arrival_order, evse_specific_reservations)) {
                return false;
            }
        }
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
        if (!this->can_virtual_car_arrive({}, o, evse_specific_reservations)) {
            return false;
        }
    }

    return true;
}

void ReservationHandler::set_reservation_timer(const types::reservation::Reservation& reservation,
                                               const std::optional<uint32_t> evse_id) {
    std::lock_guard<std::recursive_mutex> lk(this->event_mutex);
    this->reservation_id_to_reservation_timeout_timer_map[reservation.reservation_id] =
        std::make_unique<Everest::SteadyTimer>(&this->io_context);

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

std::vector<EVSEContext*> ReservationHandler::get_all_evses_with_connector_type(
    const types::evse_manager::ConnectorTypeEnum connector_type) const {
    std::vector<EVSEContext*> result;
    for (const auto& evse : this->evses) {
        if (this->has_evse_connector_type(evse.second->connectors, connector_type)) {
            result.push_back(evse.second.get());
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
    // If at least one connector is occupied, return occupied.
    if (!global_reservations.empty() || !(evse_reservations.empty())) {
        return types::reservation::ReservationResult::Occupied;
    }

    bool found_state = false;

    ConnectorState state = ConnectorState::UNAVAILABLE;

    for (const auto& [evse_id, evse] : evses) {
        if (evse->plugged_in) {
            // Overwrite state if we found a connector that was not available (if needed).
            state = get_new_connector_state(state, ConnectorState::OCCUPIED);
            found_state = true;
        }
    }

    if (!found_state) {
        const std::vector<EVSEContext*> evses_with_connector_type =
            this->get_all_evses_with_connector_type(connector_type);
        if (evses_with_connector_type.empty()) {
            // This should not happen because then it should have been rejected before already somewhere in the
            // code...
            return types::reservation::ReservationResult::Rejected;
        }

        // Get all evse's with this specific connector type and check the connectors availability states.
        for (const auto& evse : evses_with_connector_type) {
            for (const auto& connector : evse->connectors) {
                if (connector.type != connector_type &&
                    connector.type != types::evse_manager::ConnectorTypeEnum::Unknown &&
                    connector_type != types::evse_manager::ConnectorTypeEnum::Unknown) {
                    continue;
                }

                if (connector.get_state() != ConnectorState::AVAILABLE) {
                    state = get_new_connector_state(state, connector.get_state());
                }
            }
        }
    }

    return connector_state_to_reservation_result(state);
}

void ReservationHandler::check_reservations_and_cancel_if_not_possible() {

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

void ReservationHandler::store_reservations() {
    if (this->store == nullptr) {
        return;
    }

    Array reservations = json::array();
    for (const auto& reservation : this->evse_reservations) {

        json r = json::object({{"evse_id", reservation.first}, {"reservation", reservation.second}});
        reservations.push_back(r);
    }

    for (const auto& reservation : this->global_reservations) {
        json r = json::object({{"reservation", reservation}});
        reservations.push_back(r);
    }

    if (!reservations.empty()) {
        this->store->call_store(this->kvs_store_key_id, reservations);
    }
}

ReservationEvseStatus ReservationHandler::get_evse_global_reserved_status_and_set_new_status(
    const std::set<int32_t>& currently_available_evses, const std::set<int32_t>& reserved_evses) {
    ReservationEvseStatus evse_status_to_send;
    std::set<int32_t> new_reserved_evses;

    for (const auto evse_id : reserved_evses) {
        if (this->last_reserved_status.find(evse_id) != this->last_reserved_status.end()) {
            // Evse was already reserved, don't add it to the new status.
        } else {
            evse_status_to_send.reserved.insert(evse_id);
        }
    }

    for (const auto evse_id : currently_available_evses) {
        const bool is_reserved = reserved_evses.find(evse_id) != reserved_evses.end();
        const bool was_reserved = this->last_reserved_status.find(evse_id) != this->last_reserved_status.end();
        if (not is_reserved) {
            if (was_reserved) {
                evse_status_to_send.available.insert(evse_id);
            }
        }
    }

    new_reserved_evses = reserved_evses;
    this->last_reserved_status = new_reserved_evses;

    return evse_status_to_send;
}

void ReservationHandler::print_reservations_debug_info(const types::reservation::Reservation& reservation,
                                                       const std::optional<uint32_t> evse_id,
                                                       const bool reservation_failed) {
    std::string reservation_information;
    if (reservation_failed) {
        reservation_information = "Reservation not possible";
    } else {
        reservation_information = "New reservation";
    }
    EVLOG_debug << reservation_information
                << ". Evse id: " << (evse_id.has_value() ? std::to_string(evse_id.value()) : "no evse id")
                << ", connector type: "
                << (reservation.connector_type.has_value()
                        ? types::evse_manager::connector_type_enum_to_string(reservation.connector_type.value())
                        : "no connector type given");
    std::string evse_info;
    for (const auto& evse : this->evses) {
        evse_info += "- " + std::to_string(evse.first) + ":\n";
        for (const auto& connector : evse.second->connectors) {
            evse_info += "--- " + std::to_string(connector.id) + " " +
                         types::evse_manager::connector_type_enum_to_string(connector.type) +
                         ", available: " + (connector.get_state() == ConnectorState::AVAILABLE ? "yes" : "no") + "\n";
        }
    }
    std::string reservation_info;
    for (const auto& evse_reservation : this->evse_reservations) {
        reservation_info +=
            "- evse " + std::to_string(evse_reservation.first) + ": " +
            types::evse_manager::connector_type_enum_to_string(
                evse_reservation.second.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown)) +
            "\n";
    }

    for (const auto& reservation : this->global_reservations) {
        reservation_info += "- global : " +
                            types::evse_manager::connector_type_enum_to_string(
                                reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown)) +
                            "\n";
    }

    EVLOG_debug << "Current evse's and states: \n" << evse_info;
    EVLOG_debug << "Current reservations: \n" << reservation_info;
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
