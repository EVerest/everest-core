#include <ReservationEVSEs.h>
#include <iostream>

namespace module {
ReservationEVSEs::ReservationEVSEs() {
}

void ReservationEVSEs::add_connector(const uint32_t evse_id, const uint32_t connector_id,
                                     const types::evse_manager::ConnectorTypeEnum connector_type,
                                     const ConnectorState connector_state) {
    evses[evse_id].push_back({connector_id, connector_type, connector_state});

    if (max_scenarios.size() == 0) {
        max_scenarios.push_back(Scenario());
    }

    bool added = false;
    for (auto& evse : max_scenarios.at(0).evse) {
        if (evse.evse_id == evse_id) {
            // TODO mz we assume add_connector is called once per connector, document!!
            evse.connector_type.push_back(connector_type);
            added = true;
            break;
        }
    }

    if (!added) {
        EVSE_Connectors c;
        c.evse_id = evse_id;

        c.connector_type.push_back(connector_type);
        max_scenarios.at(0).evse.push_back(c);
    }

    // TODO mz create current scenarios and remove occupied, faulted and unavailable connectors / evses
    current_scenarios = max_scenarios;

    // print_scenarios(max_scenarios);

    // create_scenarios();
}

bool ReservationEVSEs::make_reservation(std::optional<uint32_t> evse_id,
                                        const types::evse_manager::ConnectorTypeEnum connector_type) {
    if (evse_id.has_value()) {
        // TODO mz
    } else {
        // Check if connector type is available.
        for (const auto& scenario : current_scenarios) {
            bool connector_available = false;
            for (const auto& evse : scenario.evse) {
                for (const auto& evse_connector_type : evse.connector_type) {
                    if (evse_connector_type == connector_type) {
                        connector_available = true;
                        break;
                    }
                }

                if (connector_available) {
                    break;
                }
            }

            if (!connector_available) {
                return false;
            }
        }
    }

    make_new_current_scenario(evse_id, connector_type);
    global_reservations.push_back(connector_type);

    return true;
}

void ReservationEVSEs::print_scenarios(const std::vector<Scenario> scenarios) {
    uint32_t scenario_count = 0;
    for (const auto& scenario : scenarios) {
        std::cout << "\n=====\n Scenario: " << scenario_count++ << std::endl;
        for (const auto& evse : scenario.evse) {
            std::cout << "\n  EVSE: " << evse.evse_id << std::endl;
            for (const auto& connector_type : evse.connector_type) {
                std::cout << "    " << types::evse_manager::connector_type_enum_to_string(connector_type) << " ";
            }
        }
    }
    std::cout << "\n---------------------\n";
}

// std::vector<ReservationEVSEs::Scenario> ReservationEVSEs::create_scenarios() {
//     std::vector<ReservationEVSEs::Scenario> scenario;
//     for (const auto& [evse_id, connectors] : evses) {
//         bool add_evse = true;
//         EVSE_Connectors c;
//         c.evse_id = evse_id;
//         for (const auto& connector : connectors) {
//             if (connector.state == ConnectorState::AVAILABLE) {
//                 c.connector_type.push_back(connector.connector_type);
//             } else if (connector.state == ConnectorState::OCCUPIED ||
//                        connector.state == ConnectorState::FAULTED_OCCUPIED) {
//                 // Connector is occupied, so whole EVSE can not charge, do not add scenario.
//                 add_evse = false;
//                 break;
//             }
//         }

//         if (add_evse) {
//         }
//     }

//     return scenario;
// }

void ReservationEVSEs::make_new_current_scenario(std::optional<uint32_t> evse_id,
                                                 const types::evse_manager::ConnectorTypeEnum connector_type) {
    if (evse_id != std::nullopt) {
        // TODO mz
    } else {
        std::vector<Scenario> new_scenarios;
        for (const auto& scenario : current_scenarios) {
            for (const auto& evse : scenario.evse) {
                // if (!has_evse_connector_type(evse, connector_type)) {
                //     continue;
                // }
                Scenario new_scenario;
                for (const auto& other_evse : scenario.evse) {
                    if (other_evse.evse_id == evse.evse_id) {
                        continue;
                    }

                    new_scenario.evse.push_back(other_evse);
                }
                new_scenarios.push_back(new_scenario);
            }
        }

        current_scenarios = new_scenarios;
    }

    std::cout << "\n\n new current scenario: ----------\n";
    print_scenarios(current_scenarios);
}

bool ReservationEVSEs::has_evse_connector_type(const EVSE_Connectors& evse,
                                               const types::evse_manager::ConnectorTypeEnum connector_type) {
    for (const auto& type : evse.connector_type) {
        if (type == connector_type) {
            return true;
        }
    }

    return false;
}
} // namespace module
