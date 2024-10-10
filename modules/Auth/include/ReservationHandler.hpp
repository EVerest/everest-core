// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef RESERVATION_HANDLER_HPP
#define RESERVATION_HANDLER_HPP

#include <vector>

#include <Connector.hpp>
#include <ReservationEVSEs.h>
#include <everest/timer.hpp>
#include <generated/types/reservation.hpp>
#include <utils/types.hpp>

namespace module {

class ReservationHandler {

private: // Members
    struct EvseConnectorType {
        int32_t connector_id;
        types::evse_manager::ConnectorTypeEnum connector_type;
    };

    std::map<int, types::reservation::Reservation> reservations;
    std::vector<types::reservation::Reservation> global_reservations;
    // std::map<int, types::evse_manager::ConnectorTypeEnum> connectors;
    std::map<int, std::vector<EvseConnectorType>> evses;

    std::mutex timer_mutex;
    std::mutex reservation_mutex;
    // std::map<int, std::unique_ptr<Everest::SteadyTimer>> connector_to_reservation_timeout_timer_map;
    std::map<int, std::unique_ptr<Everest::SteadyTimer>> reservation_id_to_reservation_timeout_timer_map;

    std::function<void(const int& connector_id)> reservation_cancelled_callback;
    ReservationEVSEs e;

public:
    /**
     * @brief Initializes a connector with the given \p connector_id . This creates an entry in the map of timers of
     the
     * handler.
     *
     * @param evse_id           The evse id this connector belongs to.
     * @param connector_id      The connector id.
     * @param connector_type    The connector type
     */
    void init_connector(const int evse_id, const int connector_id,
                        const types::evse_manager::ConnectorTypeEnum connector_type);

    /**
     * @brief Function checks if the given \p id_token or \p parent_id_token matches the reserved token of the given \p
     * evse
     *
     * @param evse              Evse id
     * @param id_token          Id token
     * @param parent_id_token   Parent id token
     * @return The reservation id when there is a matching identifier, otherwise std::nullopt.
     */
    std::optional<int32_t> matches_reserved_identifier(const std::optional<int> evse, const std::string& id_token,
                                                       std::optional<std::string> parent_id_token);

    /**
     * @brief Functions check if reservation at the given \p evse contains a parent_id
     * @param evse  Evse id
     * @return true if reservation for \p evse exists and reservation contains a parent_id
     */
    bool has_reservation_parent_id(int evse);

    /**
     * @brief Function tries to reserve the given \p evse using the given \p reservation
     *
     * @param evse                  The evse id of the reservation. If not given: reserve any evse
     * @param state Current state of the evse
     * @param is_reservable
     * @param reservation
     * @param available_evses  The number of evses available. Only needed when evse id is not given.
     * @return types::reservation::ReservationResult
     */
    types::reservation::ReservationResult reserve(std::optional<int> evse, const ConnectorState& state,
                                                  bool is_reservable,
                                                  const types::reservation::Reservation& reservation,
                                                  const std::optional<uint32_t> available_evses);

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
     * @param connector         Connector on which the car is charging
     * @param reservation_id    The reservation id
     */
    void on_reservation_used(int connector, const int32_t reservation_id);

    /**
     * @brief Registers the given \p callback that is called when a reservation should be cancelled.
     *
     * @param callback
     */
    void register_reservation_cancelled_callback(const std::function<void(const int& connector_id)>& callback);

    // TODO mz documentation
    bool is_connector_type_available(const types::evse_manager::ConnectorTypeEnum connector_type);

private: // Functions
    void set_reservation_timer(const types::reservation::Reservation& reservation, const std::optional<int32_t> evse);
    bool evse_has_connector_type(const int32_t evse_id, const types::evse_manager::ConnectorTypeEnum type);
    uint32_t get_no_evses_with_connector_type(const types::evse_manager::ConnectorTypeEnum connector_type);
};

} // namespace module

#endif // RESERVATION_HANDLER_HPP
