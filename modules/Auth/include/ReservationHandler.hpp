// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef RESERVATION_HANDLER_HPP
#define RESERVATION_HANDLER_HPP

#include <vector>

#include <Connector.hpp>
#include <everest/timer.hpp>
#include <generated/types/reservation.hpp>
#include <utils/types.hpp>

namespace module {

class ReservationHandler {

private:
    std::map<int, types::reservation::Reservation> reservations;

    std::mutex timer_mutex;
    std::map<int, std::unique_ptr<Everest::SteadyTimer>> connector_to_reservation_timeout_timer_map;

    std::function<void(const int& connector_id)> reservation_cancelled_callback;

public:
    /**
     * @brief Initializes a connector with the given \p connector_id . This creates an entry in the map of timers of the
     * handler.
     *
     * @param connector_id
     */
    void init_connector(int connector_id);

    /**
     * @brief Function checks if the given \p id_token or \p parent_id_token matches the reserved token of the given \p
     * connector
     *
     * @param connector
     * @param id_token
     * @param parent_id_token
     * @return true
     * @return false
     */
    bool matches_reserved_identifier(int connector, const std::string& id_token,
                                     std::optional<std::string> parent_id_token);

    /**
     * @brief Function tries to reserve the given \p connector using the given \p reservation
     *
     * @param connector
     * @param state Current state of the connector
     * @param is_reservable
     * @param reservation
     * @return types::reservation::ReservationResult
     */
    types::reservation::ReservationResult reserve(int connector, const ConnectorState& state, bool is_reservable,
                                                  const types::reservation::Reservation& reservation);

    /**
     * @brief Function tries to cancel reservation with the given \p reservation_id .
     *
     * @param reservation_id
     * @param execute_callback if true, cancel_reservation_callback will be executed
     * @return int -1 if reservation could not been cancelled, else the id of the connnector
     */
    int cancel_reservation(int reservation_id, bool execute_callback);

    /**
     * @brief Handler that is called when a reservation was started / used.
     *
     * @param connector
     */
    void on_reservation_used(int connector);

    /**
     * @brief Registers the given \p callback that is called when a reservation should be cancelled.
     *
     * @param callback
     */
    void register_reservation_cancelled_callback(const std::function<void(const int& connector_id)>& callback);
};

} // namespace module

#endif // RESERVATION_HANDLER_HPP
