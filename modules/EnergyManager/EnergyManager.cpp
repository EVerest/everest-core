// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "EnergyManager.hpp"
#include "Broker.hpp"
#include "BrokerFastCharging.hpp"
#include "Market.hpp"
#include <fmt/core.h>

using namespace std::literals::chrono_literals;

namespace module {

void EnergyManager::init() {
    r_energy_trunk->subscribe_energy_flow_request([this](types::energy::EnergyFlowRequest e) {
        // Received new energy object from a child.
        std::scoped_lock lock(energy_mutex);
        energy_flow_request = e;

        if (is_priority_request(e)) {
            // trigger optimization now
            mainloop_sleep_condvar.notify_all();
        }
    });

    invoke_init(*p_main);
}

void EnergyManager::ready() {
    invoke_ready(*p_main);

    // start thread to update energy optimization
    std::thread([this] {
        while (true) {
            globals.init(date::utc_clock::now(), config.schedule_interval_duration, config.schedule_total_duration,
                         config.slice_ampere, config.slice_watt, config.debug, energy_flow_request);
            auto optimized_values = run_optimizer(energy_flow_request);
            enforce_limits(optimized_values);
            {
                std::unique_lock<std::mutex> lock(mainloop_sleep_mutex);
                mainloop_sleep_condvar.wait_for(lock, std::chrono::seconds(config.update_interval));
            }
        }
    }).detach();
}

// Check if any node set the priority request flag
bool EnergyManager::is_priority_request(const types::energy::EnergyFlowRequest& e) {
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

void EnergyManager::enforce_limits(const std::vector<types::energy::EnforcedLimits>& limits) {
    for (const auto& it : limits) {
        if (globals.debug)
            EVLOG_info << fmt::format("\033[1;92m{} Enforce limits {}A {}W \033[1;0m", it.uuid,
                                      it.limits_root_side.value().ac_max_current_A.value_or(-9999),
                                      it.limits_root_side.value().total_power_W.value_or(-9999));
        r_energy_trunk->call_enforce_limits(it);
    }
}

std::vector<types::energy::EnforcedLimits> EnergyManager::run_optimizer(types::energy::EnergyFlowRequest request) {

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
        // FIXME: check for actual optimizer_targets and create correct broker for this evse
        // For now always create simple FastCharging broker
        brokers.push_back(std::make_shared<BrokerFastCharging>(*m));
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

    for (auto broker : brokers) {
        auto local_market = broker->get_local_market();

        // EVLOG_info << "Sending enfored limits (import) to :" << local_market.energy_flow_request.uuid << " "
        //            << local_market.get_sold_energy_import()[0].limits_to_root;
        types::energy::EnforcedLimits l;
        l.uuid = local_market.energy_flow_request.uuid;
        l.valid_until =
            Everest::Date::to_rfc3339(date::utc_clock::now() + std::chrono::seconds(config.update_interval * 10));

        l.limits_root_side = local_market.get_sold_energy()[0].limits_to_root;

        l.schedule = local_market.get_sold_energy();

        optimized_values.push_back(l);
    }

    return optimized_values;
}
} // namespace module
