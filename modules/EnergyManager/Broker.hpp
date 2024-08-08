// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef BROKER_HPP
#define BROKER_HPP

#include "Market.hpp"
#include "Offer.hpp"

namespace module {

enum class SlotType {
    Import,
    Export,
    Undecided
};

// All context data that is stored in between optimization runs
struct BrokerContext {
    BrokerContext() {
        clear();
    };

    void clear() {
        number_1ph3ph_cycles = 0;
        last_ac_number_of_active_phases_import = 0;
        ts_1ph_optimal = date::utc_clock::now();
    };

    int number_1ph3ph_cycles;
    int last_ac_number_of_active_phases_import;
    std::chrono::time_point<date::utc_clock> ts_1ph_optimal;
};

// base class for different Brokers
class Broker {
public:
    Broker(Market& market, BrokerContext& context);
    virtual ~Broker(){};
    virtual bool trade(Offer& offer) = 0;
    Market& get_local_market();

protected:
    // reference to local market at the broker's node
    Market& local_market;
    std::vector<bool> first_trade;
    std::vector<SlotType> slot_type;
    std::vector<int> num_phases;

    BrokerContext& context;
};

} // namespace module

#endif // BROKER_HPP
