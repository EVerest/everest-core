#pragma once

#include <cstdint>
#include <map>
#include <mutex>
#include <optional>
#include <vector>

#include <ConnectorStateMachine.hpp>
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
    // TODO mz add mutexes everywhere.
    std::mutex reservation_mutex;

public:
    ReservationEVSEs();
    void add_connector(const uint32_t evse_id, const uint32_t connector_id,
                       const types::evse_manager::ConnectorTypeEnum connector_type,
                       const ConnectorState connector_state = ConnectorState::AVAILABLE);

    bool make_reservation(const std::optional<uint32_t> evse_id, const types::reservation::Reservation& reservation);
    void set_evse_available(const bool available, const uint32_t evse_id);
    bool is_charging_possible(const uint32_t evse_id);

private: // Functions
    bool has_evse_connector_type(const std::vector<EvseConnectorType> evse_connectors,
                                 const types::evse_manager::ConnectorTypeEnum connector_type);
    bool is_evse_available(const uint32_t evse_id,
                           const std::map<uint32_t, types::reservation::Reservation> evse_specific_reservations);
    bool is_connector_available(const uint32_t evse_id, const types::evse_manager::ConnectorTypeEnum connector_type);
    std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>>
    get_all_possible_orders(const std::vector<types::evse_manager::ConnectorTypeEnum>& connectors);
    bool can_virtual_car_arrive(const std::vector<uint32_t>& used_evse_ids,
                                const std::vector<types::evse_manager::ConnectorTypeEnum>& next_car_arrival_order,
                                const std::map<uint32_t, types::reservation::Reservation>& evse_specific_reservations);
    bool is_reservation_possible(const std::optional<types::evse_manager::ConnectorTypeEnum> global_reservation_type,
                                 const std::map<uint32_t, types::reservation::Reservation>& evse_specific_reservations);
    void print_order(const std::vector<types::evse_manager::ConnectorTypeEnum>& order);
};

} // namespace module
