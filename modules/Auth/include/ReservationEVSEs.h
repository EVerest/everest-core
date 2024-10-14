#pragma once

#include <cstdint>
#include <map>
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
    std::map<uint32_t, std::vector<EvseConnectorType>> evses;
    std::map<uint32_t, types::reservation::Reservation> evse_reservations;
    std::vector<types::reservation::Reservation> global_reservations;

public:
    ReservationEVSEs();
    void add_connector(const uint32_t evse_id, const uint32_t connector_id,
                       const types::evse_manager::ConnectorTypeEnum connector_type,
                       const ConnectorState connector_state = ConnectorState::AVAILABLE);

    bool make_reservation(const std::optional<uint32_t> evse_id, const types::reservation::Reservation& reservation);

private: // Functions
    bool has_evse_connector_type(const std::vector<EvseConnectorType> evse_connectors,
                                 const types::evse_manager::ConnectorTypeEnum connector_type);
    std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>>
    get_all_possible_orders(const std::vector<types::evse_manager::ConnectorTypeEnum>& connectors);
    bool can_virtual_car_arrive(const std::vector<uint32_t>& used_evse_ids,
                                const std::vector<types::evse_manager::ConnectorTypeEnum>& next_car_arrival_order);
    void print_order(const std::vector<types::evse_manager::ConnectorTypeEnum>& order);

#if 0

        struct EvseConnectorType {
        uint32_t connector_id;
        types::evse_manager::ConnectorTypeEnum connector_type;
        ConnectorState state;
    };

    struct EVSE_Connector {
        uint32_t evse_id;
        types::evse_manager::ConnectorTypeEnum connector_type;
    };

    struct Scenario {
        std::vector<EVSE_Connector> evse_connector;
    };


    std::vector<types::evse_manager::ConnectorTypeEnum> global_reservations;

    std::map<uint32_t, std::vector<EvseConnectorType>> evses;

    // Als alle evse's beschikbaar zijn
    std::vector<Scenario> max_scenarios;
    // Als er reserveringen worden gemaakt, faulted, unavailable of laden
    std::vector<Scenario> current_scenarios;

    std::vector<std::vector<uint32_t>> evse_combinations;

public:
    ReservationEVSEs();
    void add_connector(const uint32_t evse_id, const uint32_t connector_id,
                        const types::evse_manager::ConnectorTypeEnum connector_type,
                        const ConnectorState connector_state = ConnectorState::AVAILABLE);

    bool make_reservation(std::optional<uint32_t> evse_id, const types::evse_manager::ConnectorTypeEnum connector_type);



private: // Functions
    void print_scenarios(const std::vector<Scenario> scenarios);
    std::vector<Scenario> create_scenarios();
    void make_new_current_scenario(std::optional<uint32_t> evse_id,
                                   const types::evse_manager::ConnectorTypeEnum connector_type);
    bool has_evse_connector_type(const std::vector<EvseConnectorType> evse_connectors,
                                 const types::evse_manager::ConnectorTypeEnum connector_type);
    void create(std::vector<uint32_t> evse_id, Scenario* scenario);
    std::vector<Scenario> add_scenario(std::vector<Scenario> scenarios, const uint32_t evse_id1, const types::evse_manager::ConnectorTypeEnum connector_type1,
                                       const uint32_t evse_id2, const types::evse_manager::ConnectorTypeEnum connector_type2,
                                       const uint32_t evse_id3, const types::evse_manager::ConnectorTypeEnum connector_type3);
    bool is_scenario_available(std::vector<types::evse_manager::ConnectorTypeEnum> connectors);
    std::vector<std::vector<types::evse_manager::ConnectorTypeEnum> > get_all_possible_orders(std::vector<types::evse_manager::ConnectorTypeEnum> connectors);
    std::vector<std::vector<uint32_t>> make_evse_combinations();
#endif
};

} // namespace module
