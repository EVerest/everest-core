#pragma once

#include "ConnectorStateMachine.hpp"
#include <cstdint>
#include <map>
#include <vector>

#include <generated/types/evse_manager.hpp>

namespace module {

/*
 *   [ 0, [ A ]]
 *   [ 1, [ A, B ]]
 *   [ 2, [ A, B ]]
 */

class ReservationEVSEs {
private: // Members
    struct EvseConnectorType {
        uint32_t connector_id;
        types::evse_manager::ConnectorTypeEnum connector_type;
        ConnectorState state;
    };

    struct EVSE_Connectors {
        uint32_t evse_id;
        std::vector<types::evse_manager::ConnectorTypeEnum> connector_type;
    };

    struct Scenario {
        std::vector<EVSE_Connectors> evse;
    };

    std::vector<types::evse_manager::ConnectorTypeEnum> global_reservations;

    std::map<int, std::vector<EvseConnectorType>> evses;

    // Als alle evse's beschikbaar zijn
    std::vector<Scenario> max_scenarios;
    // Als er reserveringen worden gemaakt, faulted, unavailable of laden
    std::vector<Scenario> current_scenarios;

public:
    ReservationEVSEs();
    void add_connector(const uint32_t evse_id, const uint32_t connector_id,
                        const types::evse_manager::ConnectorTypeEnum connector_type,
                        const ConnectorState connector_state = ConnectorState::AVAILABLE);

    bool make_reservation(std::optional<uint32_t> evse_id, const types::evse_manager::ConnectorTypeEnum connector_type);



private: // Functions
    void print_scenarios(const std::vector<Scenario> scenarios);
    // std::vector<Scenario> create_scenarios();
    void make_new_current_scenario(std::optional<uint32_t> evse_id,
                                   const types::evse_manager::ConnectorTypeEnum connector_type);
    bool has_evse_connector_type(const EVSE_Connectors& evse, const types::evse_manager::ConnectorTypeEnum connector_type);
};

} // namespace module
