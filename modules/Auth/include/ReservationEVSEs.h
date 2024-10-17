#pragma once

#include <cstdint>
#include <map>
#include <mutex>
#include <optional>
#include <vector>

#include <ConnectorStateMachine.hpp>
#include <everest/timer.hpp>
#include <generated/types/evse_manager.hpp>
#include <generated/types/reservation.hpp>

namespace module {

class ReservationEVSEs {
private: // Members
    struct EvseConnectorType {
        uint32_t connector_id;
        types::evse_manager::ConnectorTypeEnum connector_type;
        ConnectorState state;
    };

    struct Evse {
        uint32_t evse_id;
        ConnectorState evse_state;
        std::vector<EvseConnectorType> connectors;
    };

    std::map<uint32_t, Evse> evses;
    std::map<uint32_t, types::reservation::Reservation> evse_reservations;
    std::vector<types::reservation::Reservation> global_reservations;
    mutable std::recursive_mutex reservation_mutex;
    mutable std::recursive_mutex timer_mutex;
    std::map<int, std::unique_ptr<Everest::SteadyTimer>> reservation_id_to_reservation_timeout_timer_map;
    std::function<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id, const types::reservation::ReservationEndReason reason)> reservation_cancelled_callback;

public:
    ReservationEVSEs();
    void add_connector(const uint32_t evse_id, const uint32_t connector_id,
                       const types::evse_manager::ConnectorTypeEnum connector_type,
                       const ConnectorState connector_state = ConnectorState::AVAILABLE);

    types::reservation::ReservationResult make_reservation(const std::optional<uint32_t> evse_id,
                                                           const types::reservation::Reservation& reservation);
    void set_evse_available(const bool available, const bool faulted, const uint32_t evse_id);
    void set_connector_available(const bool available, const bool faulted, const uint32_t evse_id,
                                 const uint32_t connector_id);
    bool is_charging_possible(const uint32_t evse_id);
    std::optional<uint32_t> cancel_reservation(int reservation_id, bool execute_callback, const types::reservation::ReservationEndReason reason);
    void
    register_reservation_cancelled_callback(const std::function<void(const std::optional<uint32_t>& evse_id, const int32_t reservation_id, const types::reservation::ReservationEndReason reason)>& callback);
    void on_reservation_used(const int32_t reservation_id);

    /**
     * @brief Function checks if the given \p id_token or \p parent_id_token matches the reserved token of the given \p
     * evse_id
     *
     * @param evse_id              Evse id
     * @param id_token          Id token
     * @param parent_id_token   Parent id token
     * @return The reservation id when there is a matching identifier, otherwise std::nullopt.
     */
    std::optional<int32_t> matches_reserved_identifier(const std::optional<uint32_t> evse_id, const std::string& id_token,
                                                       std::optional<std::string> parent_id_token);

    /**
     * @brief Functions check if reservation at the given \p evse_id contains a parent_id
     * @param evse_id  Evse id
     * @return true if reservation for \p evse_id exists and reservation contains a parent_id
     */
    bool has_reservation_parent_id(const std::optional<uint32_t> evse_id);

private: // Functions
    bool has_evse_connector_type(const std::vector<EvseConnectorType> evse_connectors,
                                 const types::evse_manager::ConnectorTypeEnum connector_type) const;
    bool does_evse_connector_type_exist(const types::evse_manager::ConnectorTypeEnum connector_type) const;
    types::reservation::ReservationResult
    get_evse_state(const uint32_t evse_id,
                   const std::map<uint32_t, types::reservation::Reservation> evse_specific_reservations);
    types::reservation::ReservationResult
    is_connector_available(const uint32_t evse_id, const types::evse_manager::ConnectorTypeEnum connector_type);
    std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>>
    get_all_possible_orders(const std::vector<types::evse_manager::ConnectorTypeEnum>& connectors) const;
    bool can_virtual_car_arrive(const std::vector<uint32_t>& used_evse_ids,
                                const std::vector<types::evse_manager::ConnectorTypeEnum>& next_car_arrival_order,
                                const std::map<uint32_t, types::reservation::Reservation>& evse_specific_reservations);
    bool is_reservation_possible(const std::optional<types::evse_manager::ConnectorTypeEnum> global_reservation_type,
                                 const std::vector<types::reservation::Reservation>& reservations_no_evse,
                                 const std::map<uint32_t, types::reservation::Reservation>& evse_specific_reservations);
    void set_reservation_timer(const types::reservation::Reservation& reservation,
                               const std::optional<uint32_t> evse_id);
    ConnectorState get_new_connector_state(ConnectorState currrent_state, const ConnectorState new_state) const;
    types::reservation::ReservationResult get_reservation_evse_connector_state() const;
    void check_reservations_and_cancel_if_not_possible();

    void print_order(const std::vector<types::evse_manager::ConnectorTypeEnum>& order) const;
};

} // namespace module
