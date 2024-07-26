// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef BROKER_FAST_CHARGING_HPP
#define BROKER_FAST_CHARGING_HPP

#include "Broker.hpp"

namespace module {

// This broker tries to charge as fast as possible.
class BrokerFastCharging : public Broker {
public:
    enum class Switch1ph3phMode {
        Never,
        Oneway,
        Both,
    };
    explicit BrokerFastCharging(Market& market, Switch1ph3phMode mode);
    virtual bool trade(Offer& offer) override;

private:
    void buy_ampere_unchecked(int index, float ampere, int number_of_phases);
    void buy_watt_unchecked(int index, float watt);

    bool buy_ampere_import(int index, float ampere, bool allow_less, int number_of_phases);
    bool buy_ampere_export(int index, float ampere, bool allow_less, int number_of_phases);
    bool buy_ampere(const types::energy::ScheduleReqEntry& _offer, int index, float ampere, bool allow_less,
                    bool import, int number_of_phases);

    bool buy_watt_import(int index, float watt, bool allow_less);
    bool buy_watt_export(int index, float watt, bool allow_less);
    bool buy_watt(const types::energy::ScheduleReqEntry& _offer, int index, float watt, bool allow_less, bool import);

    ScheduleRes trading;
    Offer* offer{nullptr};
    bool traded{false};

    Switch1ph3phMode switch_1ph3ph_mode;
};

} // namespace module

#endif // BROKER_FAST_CHARGING_HPP
