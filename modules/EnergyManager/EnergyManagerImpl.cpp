// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include <EnergyManagerImpl.hpp>

#include <chrono>
#include <fstream>

#include "Broker.hpp"
#include "BrokerFastCharging.hpp"
#include "Market.hpp"

using namespace std::literals::chrono_literals;

namespace module {

static BrokerFastCharging::Switch1ph3phMode to_switch_1ph3ph_mode(const std::string& m) {
    if (m == "Both") {
        return BrokerFastCharging::Switch1ph3phMode::Both;
    } else if (m == "Oneway") {
        return BrokerFastCharging::Switch1ph3phMode::Oneway;
    } else {
        return BrokerFastCharging::Switch1ph3phMode::Never;
    }
}

static BrokerFastCharging::StickyNess to_stickyness(const std::string& m) {
    if (m == "DontChange") {
        return BrokerFastCharging::StickyNess::DontChange;
    } else if (m == "SinglePhase") {
        return BrokerFastCharging::StickyNess::SinglePhase;
    } else {
        return BrokerFastCharging::StickyNess::ThreePhase;
    }
}

static BrokerFastCharging::EnergyManagerConfig to_broker_fast_charging_config(const EnergyManagerConfig& config) {
    BrokerFastCharging::EnergyManagerConfig broker_conf;

    broker_conf.max_nr_of_switches_per_session = config.switch_3ph1ph_max_nr_of_switches_per_session;
    broker_conf.power_hysteresis_W = config.switch_3ph1ph_power_hysteresis_W;
    broker_conf.switch_1ph_3ph_mode = to_switch_1ph3ph_mode(config.switch_3ph1ph_while_charging_mode);
    broker_conf.time_hysteresis_s = config.switch_3ph1ph_time_hysteresis_s;
    broker_conf.stickyness = to_stickyness(config.switch_3ph1ph_switch_limit_stickyness);

    return broker_conf;
}

// Check if any node set the priority request flag
bool is_priority_request(const types::energy::EnergyFlowRequest& e) {
    bool prio = e.priority_request.has_value() and e.priority_request.value();

    // If this node has priority, no need to travese the tree any longer
    if (prio) {
        return true;
    }

    // recurse to all children
    for (auto& c : e.children) {
        if (is_priority_request(c)) {
            return true;
        }
    }

    return false;
}

EnergyManagerImpl::EnergyManagerImpl(
    const EnergyManagerConfig& config,
    const std::function<void(const std::vector<types::energy::EnforcedLimits>& limits)>& enforced_limits_callback) :
    config(config), enforced_limits_callback(enforced_limits_callback) {
    this->energy_flow_request.node_type = types::energy::NodeType::Undefined;
}

void EnergyManagerImpl::start() {
    // start thread to update energy optimization
    std::thread([this] {
        while (true) {
            auto optimized_values = this->run_optimizer(energy_flow_request, date::utc_clock::now());
            enforced_limits_callback(optimized_values);
            {
                std::unique_lock<std::mutex> lock(mainloop_sleep_mutex);
                mainloop_sleep_condvar.wait_for(lock, std::chrono::seconds(config.update_interval));
            }
        }
    }).detach();
}

void EnergyManagerImpl::on_energy_flow_request(const types::energy::EnergyFlowRequest& e) {
    // Received new energy object from a child.
    std::scoped_lock lock(energy_mutex);
    energy_flow_request = e;

    if (is_priority_request(e)) {
        // trigger optimization now
        mainloop_sleep_condvar.notify_all();
    }
}

std::vector<types::energy::EnforcedLimits> EnergyManagerImpl::run_optimizer(types::energy::EnergyFlowRequest request,
                                                                            date::utc_clock::time_point start_time,
                                                                            const std::string& test_name) {
    globals.init(start_time, config.schedule_interval_duration, config.schedule_total_duration, config.slice_ampere,
                 config.slice_watt, config.debug, request);

    std::scoped_lock lock(energy_mutex);

    time_probe optimizer_start;
    optimizer_start.start();
    if (globals.debug)
        EVLOG_info << "\033[1;44m---------------- Run energy optimizer ---------------- \033[1;0m";

    time_probe market_tp;

    //  create market for trading energy based on the request tree
    market_tp.start();
    Market market(request, config.nominal_ac_voltage);
    market_tp.pause();

    // create brokers for all evses (they buy/sell energy on behalf of EvseManagers)
    std::vector<std::shared_ptr<Broker>> brokers;

    auto evse_markets = market.get_list_of_evses();

    for (auto m : evse_markets) {
        // Check if we need to clear the context
        // Note that context is created here if it does not exist implicitly by operator[] of the map
        if (m->energy_flow_request.evse_state == types::energy::EvseState::Unplugged or
            m->energy_flow_request.evse_state == types::energy::EvseState::Finished) {
            contexts[m->energy_flow_request.uuid].clear();
            contexts[m->energy_flow_request.uuid].ts_1ph_optimal =
                globals.start_time - std::chrono::seconds(config.switch_3ph1ph_time_hysteresis_s);
        }

        // FIXME: check for actual optimizer_targets and create correct broker for this evse
        // For now always create simple FastCharging broker
        brokers.push_back(std::make_shared<BrokerFastCharging>(*m, contexts[m->energy_flow_request.uuid],
                                                               to_broker_fast_charging_config(config)));
        // EVLOG_info << fmt::format("Created broker for {}", m->energy_flow_request.uuid);
    }

    // for each evse: create a custom offer at their local market place and ask the broker to buy a slice.
    // continue until no one wants to buy/sell anything anymore.

    int max_number_of_trading_rounds = 100;
    time_probe offer_tp;
    time_probe broker_tp;

    while (max_number_of_trading_rounds-- > 0) {
        bool trade_happend_in_this_round = false;
        for (auto broker : brokers) {
            // EVLOG_info << broker->get_local_market().energy_flow_request;
            //     create local offer at evse's marketplace

            offer_tp.start();
            Offer local_offer(broker->get_local_market());
            offer_tp.pause();

            // ask broker to trade
            broker_tp.start();
            if (broker->trade(local_offer))
                trade_happend_in_this_round = true;
            broker_tp.pause();
        }
        if (!trade_happend_in_this_round)
            break;
    }

    if (max_number_of_trading_rounds <= 0) {
        EVLOG_error << "Trading: Maximum number of trading rounds reached.";
    }

    if (globals.debug) {
        EVLOG_info << fmt::format("\033[1;44m---------------- End energy optimizer ({} rounds, offer {}ms market {}ms "
                                  "broker {}ms total {}ms) ---------------- \033[1;0m",
                                  100 - max_number_of_trading_rounds, offer_tp.stop(), market_tp.stop(),
                                  broker_tp.stop(), optimizer_start.stop());
    }

    std::vector<types::energy::EnforcedLimits> optimized_values;
    optimized_values.reserve(brokers.size());

    for (auto& broker : brokers) {
        auto& local_market = broker->get_local_market();
        const auto sold_energy = local_market.get_sold_energy();

        if (sold_energy.size() > 0) {
            using namespace std::chrono;

            types::energy::EnforcedLimits l;
            l.uuid = local_market.energy_flow_request.uuid;
            l.valid_until_ts = duration_cast<seconds>(steady_clock::now().time_since_epoch()).count();
            l.valid_until_ts += config.update_interval * 10;

            l.schedule = sold_energy;

            // select root limit from schedule based on globals.start_time
            l.limits_root_side = sold_energy[0].limits_to_root;

            for (const auto& s : sold_energy) {
                const auto schedule_time = Everest::Date::from_rfc3339(s.timestamp);
                if (globals.start_time < schedule_time) {
                    // all further schedules will be further into the future
                    break;
                } else {
                    // use this schedule as the starting point
                    l.limits_root_side = s.limits_to_root;
                }
            }

            optimized_values.push_back(l);

            if (globals.debug) {
                EVLOG_info << "Sending enforced limits (import) to :" << l.uuid << " " << l.limits_root_side;
            }
        }
    }

    // Print out test case file
    if (not test_name.empty()) {
        json test_case;
        test_case["start_time"] = Everest::Date::to_rfc3339(start_time);
        test_case["request"] = json(request);
        test_case["expected_result"] = json(optimized_values);
        std::ofstream out(test_name.c_str());
        out << test_case;
        out.close();
    }

    return optimized_values;
}

} // namespace module