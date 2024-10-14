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
    evses[evse_id].push_back(evse_connector_type);
}

bool ReservationEVSEs::make_reservation(const std::optional<uint32_t> evse_id,
                                        const types::reservation::Reservation& reservation) {
    if (evse_id.has_value()) {
        // TODO mz
        evse_reservations[evse_id.value()] = reservation;
    } else {
        std::vector<types::evse_manager::ConnectorTypeEnum> types;
        for (const auto& global_reservation : this->global_reservations) {
            types.push_back(
                global_reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown));
        }

        types.push_back(reservation.connector_type.value_or(types::evse_manager::ConnectorTypeEnum::Unknown));

        std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>> orders = get_all_possible_orders(types);

        for (const auto& o : orders) {
            print_order(o);
            if (!can_virtual_car_arrive({}, o)) {
                return false;
            }
        }

        global_reservations.push_back(reservation);
    }

    return true;
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
    const std::vector<types::evse_manager::ConnectorTypeEnum>& next_car_arrival_order) {

    bool is_possible = false;

    for (const auto& [evse_id, evse_connector_types] : evses) {
        // Check if there is a car already at this evse id.
        if (std::find(used_evse_ids.begin(), used_evse_ids.end(), evse_id) != used_evse_ids.end()) {
            continue;
        }

        if (has_evse_connector_type(evse_connector_types, next_car_arrival_order.at(0))) {
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

            if (!can_virtual_car_arrive(next_used_evse_ids, next_arrival_order)) {

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

void ReservationEVSEs::print_order(const std::vector<types::evse_manager::ConnectorTypeEnum>& order) {
    std::cout << "\n ----------\n";
    for (const auto& connector_type : order) {
        std::cout << connector_type_enum_to_string(connector_type) << " ";
    }
    std::cout << "\n";
}

#if 0

void ReservationEVSEs::add_connector(const uint32_t evse_id, const uint32_t connector_id,
                                     const types::evse_manager::ConnectorTypeEnum connector_type,
                                     const ConnectorState connector_state) {
    EvseConnectorType evse_connector_type;
    evse_connector_type.connector_type = connector_type;
    evse_connector_type.connector_id = connector_id;
    evse_connector_type.state = connector_state;
    evses[evse_id].push_back(evse_connector_type);
    // max_scenarios.clear();
    // max_scenarios = create_scenarios();
    // create({}, nullptr);

    evse_combinations = make_evse_combinations();
    // print_scenarios(max_scenarios);

    // if (max_scenarios.size() == 0) {
    //     max_scenarios.push_back(Scenario());
    // }

    // bool added = false;
    // for (auto& evse : max_scenarios.at(0).evse) {
    //     if (evse.evse_id == evse_id) {
    //         // TODO mz we assume add_connector is called once per connector, document!!
    //         evse.connector_type.push_back(connector_type);
    //         added = true;
    //         break;
    //     }
    // }

    // if (!added) {
    //     EVSE_Connectors c;
    //     c.evse_id = evse_id;

    //     c.connector_type.push_back(connector_type);
    //     max_scenarios.at(0).evse.push_back(c);
    // }

    // // TODO mz create current scenarios and remove occupied, faulted and unavailable connectors / evses
    // current_scenarios = max_scenarios;

    // print_scenarios(max_scenarios);

    // create_scenarios();
}

bool ReservationEVSEs::make_reservation(std::optional<uint32_t> evse_id,
                                        const types::evse_manager::ConnectorTypeEnum connector_type) {
    if (evse_id.has_value()) {
        // TODO mz
    } else {
        std::vector<types::evse_manager::ConnectorTypeEnum> types = global_reservations;
        types.push_back(connector_type);
        std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>> orders = get_all_possible_orders(types);

        std::vector<std::vector<uint32_t>> evse_combi = this->evse_combinations;

        for (const std::vector<types::evse_manager::ConnectorTypeEnum>& car_arrive_order : orders) {
            if (car_arrive_order.size() > evses.size()) {
                // More possible cars than evses, this is not possible.
                return false;
            }

            for (uint32_t i = 0; i < car_arrive_order.size(); ++i) {

                if (i + 1 < car_arrive_order.size()) {
                    // When the last possible car / connector combination is not yet reached, just filter the possible
                    // evse order combinations.
                    evse_combi.erase(
                        std::remove_if(evse_combi.begin(), evse_combi.end(),
                                       [this, i, &car_arrive_order](std::vector<uint32_t> evse_order) {
                                           // TODO mz check if size of evse_order is correct
                                           const uint32_t evse_id = evse_order.at(i);
                                           // Remove all evse orders where a scenario is simply not possible because
                                           // the evse does not have the given connector type.
                                           // TODO mz extend with unavailable evse, faulted, etc
                                           if (has_evse_connector_type(evses[evse_id], car_arrive_order.at(i))) {
                                               return false;
                                           }

                                           return true;
                                       }));

                    if (evse_combi.empty()) {
                        return false;
                    }
                } else {
                    // For the last car, there should be an evse available as well. In this case, all combinations of
                    // evse order / car arrive order (connector type) should be possible.
                    for (const std::vector<uint32_t>& evse_order : evse_combi) {
                        const uint32_t evse_id = evse_order.at(i);
                        if (!has_evse_connector_type(evses[evse_id], car_arrive_order.at(i))) {
                            return false;
                        }
                    }
                }
            }
        }

        // for (const auto& possible_order : orders) {
        //     if (!is_scenario_available(possible_order)) {
        //         return false;
        //     }
        // }
    }

    // make_new_current_scenario(evse_id, connector_type);
    global_reservations.push_back(connector_type);

    return true;
}

void ReservationEVSEs::print_scenarios(const std::vector<Scenario> scenarios) {
    uint32_t scenario_count = 0;
    for (const auto& scenario : scenarios) {
        std::cout << "\n=====\n Scenario: " << scenario_count++ << std::endl;
        for (const auto& evse : scenario.evse_connector) {
            std::cout << "\n  EVSE: " << evse.evse_id << ", Connector: " << evse.connector_type << std::endl;
        }
    }
    std::cout << "\n---------------------\n";
}

std::vector<ReservationEVSEs::Scenario> ReservationEVSEs::create_scenarios() {

    std::vector<ReservationEVSEs::Scenario> scenario;
    using namespace types::evse_manager;
    // 1A 2A 3A
    scenario =
        add_scenario(scenario, 0, ConnectorTypeEnum::cCCS2, 1, ConnectorTypeEnum::cCCS2, 2, ConnectorTypeEnum::cCCS2);
    // 1A 2A 3B
    scenario =
        add_scenario(scenario, 0, ConnectorTypeEnum::cCCS2, 1, ConnectorTypeEnum::cCCS2, 2, ConnectorTypeEnum::cType2);
    // 1A 2B 3A
    scenario =
        add_scenario(scenario, 0, ConnectorTypeEnum::cCCS2, 1, ConnectorTypeEnum::cType2, 2, ConnectorTypeEnum::cCCS2);
    // 1A 2B 3B
    scenario =
        add_scenario(scenario, 0, ConnectorTypeEnum::cCCS2, 1, ConnectorTypeEnum::cType2, 2, ConnectorTypeEnum::cType2);

    // 1A 3A 2A
    scenario =
        add_scenario(scenario, 0, ConnectorTypeEnum::cCCS2, 2, ConnectorTypeEnum::cCCS2, 1, ConnectorTypeEnum::cCCS2);
    // 1A 3B 2A
    scenario =
        add_scenario(scenario, 0, ConnectorTypeEnum::cCCS2, 2, ConnectorTypeEnum::cType2, 1, ConnectorTypeEnum::cCCS2);
    // 1A 3A 2B
    scenario =
        add_scenario(scenario, 0, ConnectorTypeEnum::cCCS2, 2, ConnectorTypeEnum::cCCS2, 1, ConnectorTypeEnum::cType2);
    // 1A 3B 2B
    scenario =
        add_scenario(scenario, 0, ConnectorTypeEnum::cCCS2, 2, ConnectorTypeEnum::cType2, 1, ConnectorTypeEnum::cType2);

    // 2A 1A 3A
    scenario =
        add_scenario(scenario, 1, ConnectorTypeEnum::cCCS2, 0, ConnectorTypeEnum::cCCS2, 2, ConnectorTypeEnum::cCCS2);
    // 2A 1A 3B
    scenario =
        add_scenario(scenario, 1, ConnectorTypeEnum::cCCS2, 0, ConnectorTypeEnum::cCCS2, 2, ConnectorTypeEnum::cType2);
    // 2A 3A 1A
    scenario =
        add_scenario(scenario, 1, ConnectorTypeEnum::cCCS2, 2, ConnectorTypeEnum::cCCS2, 0, ConnectorTypeEnum::cCCS2);
    // 2A 3B 1A
    scenario =
        add_scenario(scenario, 1, ConnectorTypeEnum::cCCS2, 2, ConnectorTypeEnum::cType2, 0, ConnectorTypeEnum::cCCS2);

    // 2B 1A 3A
    scenario =
        add_scenario(scenario, 1, ConnectorTypeEnum::cType2, 0, ConnectorTypeEnum::cCCS2, 2, ConnectorTypeEnum::cCCS2);
    // 2B 1A 3B
    scenario =
        add_scenario(scenario, 1, ConnectorTypeEnum::cType2, 0, ConnectorTypeEnum::cCCS2, 2, ConnectorTypeEnum::cType2);
    // 2B 3A 1A
    scenario =
        add_scenario(scenario, 1, ConnectorTypeEnum::cType2, 2, ConnectorTypeEnum::cCCS2, 0, ConnectorTypeEnum::cCCS2);
    // 2B 3B 1A
    scenario =
        add_scenario(scenario, 1, ConnectorTypeEnum::cType2, 2, ConnectorTypeEnum::cType2, 0, ConnectorTypeEnum::cCCS2);

    // 3A 1A 2A
    scenario =
        add_scenario(scenario, 2, ConnectorTypeEnum::cCCS2, 0, ConnectorTypeEnum::cCCS2, 1, ConnectorTypeEnum::cCCS2);
    // 3A 1A 2B
    scenario =
        add_scenario(scenario, 2, ConnectorTypeEnum::cCCS2, 0, ConnectorTypeEnum::cCCS2, 1, ConnectorTypeEnum::cType2);
    // 3A 2A 1A
    scenario =
        add_scenario(scenario, 2, ConnectorTypeEnum::cCCS2, 1, ConnectorTypeEnum::cCCS2, 0, ConnectorTypeEnum::cCCS2);
    // 3A 2B 1A
    scenario =
        add_scenario(scenario, 2, ConnectorTypeEnum::cCCS2, 1, ConnectorTypeEnum::cType2, 0, ConnectorTypeEnum::cCCS2);

    // 3B 1A 2A
    scenario =
        add_scenario(scenario, 2, ConnectorTypeEnum::cType2, 0, ConnectorTypeEnum::cCCS2, 1, ConnectorTypeEnum::cCCS2);
    // 3B 1A 2B
    scenario =
        add_scenario(scenario, 2, ConnectorTypeEnum::cType2, 0, ConnectorTypeEnum::cCCS2, 1, ConnectorTypeEnum::cType2);
    // 3B 2A 1A
    scenario =
        add_scenario(scenario, 2, ConnectorTypeEnum::cType2, 1, ConnectorTypeEnum::cCCS2, 0, ConnectorTypeEnum::cCCS2);
    // 3B 2B 1A
    scenario =
        add_scenario(scenario, 2, ConnectorTypeEnum::cType2, 1, ConnectorTypeEnum::cType2, 0, ConnectorTypeEnum::cCCS2);

    // for (const auto& [evse_id, connectors] : evses) {
    //     for (const auto connector : connectors) {
    //         Scenario scenario;
    //         EVSE_Connector c;
    //         c.connector_type = connector;
    //         c.evse_id = evse_id;
    //         scenario.evse_connector.push_back(c);
    //         for (const auto& [evse_id_inner, connectors_inner] : evses) {
    //             if (evse_id == evse_id_inner) {
    //                 continue;
    //             }
    //             for (const auto connector_inner : connectors_inner) {
    //                 EVSE_Connector c;
    //                 c.evse_id = evse_id_inner;
    //                 c.connector_type = connector_inner;
    //             }
    //         }

    //         max_scenarios.push_back(scenario);
    //     }
    // }

    return scenario;
}

void ReservationEVSEs::make_new_current_scenario(std::optional<uint32_t> evse_id,
                                                 const types::evse_manager::ConnectorTypeEnum connector_type) {
    // if (evse_id != std::nullopt) {
    //     // TODO mz
    // } else {
    //     std::vector<Scenario> new_scenarios;
    //     for (const auto& scenario : current_scenarios) {
    //         for (const auto& evse : scenario.evse) {
    //             // if (!has_evse_connector_type(evse, connector_type)) {
    //             //     continue;
    //             // }
    //             Scenario new_scenario;
    //             for (const auto& other_evse : scenario.evse) {
    //                 if (other_evse.evse_id == evse.evse_id) {
    //                     continue;
    //                 }

    //                 new_scenario.evse.push_back(other_evse);
    //             }
    //             new_scenarios.push_back(new_scenario);
    //         }
    //     }

    //     current_scenarios = new_scenarios;
    // }

    // std::cout << "\n\n new current scenario: ----------\n";
    // print_scenarios(current_scenarios);
}

bool ReservationEVSEs::has_evse_connector_type(std::vector<EvseConnectorType> evse_connectors,
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

void ReservationEVSEs::create(std::vector<uint32_t> evse_ids, Scenario* scenario) {
    for (const auto& [evse_id, connector_types] : evses) {
        if (!evse_ids.empty() && std::find(evse_ids.begin(), evse_ids.end(), evse_id) != evse_ids.end()) {
            continue;
        }
        std::vector<uint32_t> ids = evse_ids;
        // ids.push_back(evse_id);

        for (uint32_t i = 0; i < connector_types.size(); ++i) {
            // for (const auto& connector_type : connector_types) {
            bool add_scenario = false;
            Scenario s;
            if (scenario == nullptr) {
                scenario = &s;
                add_scenario = true;
            } /*else {
                s = *scenario;
            }*/

            EVSE_Connector c;
            c.evse_id = evse_id;
            c.connector_type = connector_types[i].connector_type;
            // std::vector<uint32_t> ids = evse_ids;
            ids.push_back(evse_id);
            scenario->evse_connector.push_back(c);
            create(ids, scenario);

            if (add_scenario) {
                max_scenarios.push_back(*scenario);
            }
        }
    }
}

std::vector<ReservationEVSEs::Scenario>
ReservationEVSEs::add_scenario(std::vector<Scenario> scenarios, const uint32_t evse_id1,
                               const types::evse_manager::ConnectorTypeEnum connector_type1, const uint32_t evse_id2,
                               const types::evse_manager::ConnectorTypeEnum connector_type2, const uint32_t evse_id3,
                               const types::evse_manager::ConnectorTypeEnum connector_type3) {
    Scenario s;
    EVSE_Connector c;
    c.connector_type = connector_type1;
    c.evse_id = evse_id1;

    EVSE_Connector c2;
    c2.connector_type = connector_type2;
    c2.evse_id = evse_id2;

    EVSE_Connector c3;
    c3.evse_id = evse_id3;
    c3.connector_type = connector_type3;

    s.evse_connector.push_back(c);
    s.evse_connector.push_back(c2);
    s.evse_connector.push_back(c3);

    scenarios.push_back(s);
    return scenarios;
}

bool ReservationEVSEs::is_scenario_available(std::vector<types::evse_manager::ConnectorTypeEnum> connectors) {
    bool result = false;

    // for (uint32_t i = 0; i < connectors.size(); ++i) {
    //     // TODO mz does evses.at(i) look for evse with id i?? I want to have the i'rd/st evse in row
    //     if (has_evse_connector_type(evses.at(i), connectors.at(i))) {
    //         if (i + 1 == connectors.size()) {
    //             return true;
    //         } else {
    //             continue;
    //         }
    //     }
    // }

    for (const auto& scenario : max_scenarios) {
        if (scenario.evse_connector.size() < connectors.size()) {
            // TODO mz Return false? Because all scenarios should have the same size.
            continue;
        }

        uint32_t connectors_fit = 1;
        if (scenario.evse_connector.at(0).connector_type != connectors.at(0)) {
            continue;
        }

        bool scenario_possible = false;

        for (uint32_t i = 1; i < connectors.size(); ++i) {
            if (scenario.evse_connector.at(i).connector_type != connectors.at(i)) {
                // TODO mz is evse id needed here? Because it has another connector available as well and we need to
                // know if both options are not ok or if there is still one option (it is an or).
                // TODO mz why does this make sense? I do not understand the '-1' (but it works???)
                if (i + 1 == connectors.size() - 1 /* && !has_evse_connector_type(evses.at(i), connectors.at(i))*/) {
                    return false;
                }
            }
            connectors_fit++;
        }

        if (connectors_fit == connectors.size()) {
            scenario_possible = true;
            result = true;
            return true;
        }
    }

    return result;
}

std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>>
ReservationEVSEs::get_all_possible_orders(std::vector<types::evse_manager::ConnectorTypeEnum> connectors) {
    std::vector<types::evse_manager::ConnectorTypeEnum> c = connectors;
    std::vector<std::vector<types::evse_manager::ConnectorTypeEnum>> result;
    do {
        result.push_back(c);
    } while (std::next_permutation(c.begin(), c.end()));

    // if (connectors.size() > 1) {
    //     result.push_back(c);
    // }

    return result;
}

std::vector<std::vector<uint32_t>> ReservationEVSEs::make_evse_combinations() {
    std::vector<std::vector<uint32_t>> combinations;
    std::vector<uint32_t> evses;
    for (const auto& evse : this->evses) {
        evses.push_back(evse.first);
    }

    do {
        combinations.push_back(evses);
    } while (std::next_permutation(evses.begin(), evses.end()));

    return combinations;
}

#endif

} // namespace module
