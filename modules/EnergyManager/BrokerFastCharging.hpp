// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef BROKER_FAST_CHARGING_HPP
#define BROKER_FAST_CHARGING_HPP

#include "Broker.hpp"

namespace module {

// This broker tries to charge as fast as possible.
class BrokerFastCharging : public Broker {
public:
    explicit BrokerFastCharging(Market& market);
    virtual bool trade(Offer& offer) override;

private:
    void buy_ampere_unchecked(int index, float ampere);
    void buy_watt_unchecked(int index, float watt);

    bool buy_ampere_import(int index, float ampere, bool allow_less);
    bool buy_ampere_export(int index, float ampere, bool allow_less);
    bool buy_ampere(const types::energy::ScheduleReqEntry& _offer, int index, float ampere, bool allow_less, bool import);

    bool buy_watt_import(int index, float watt, bool allow_less);
    bool buy_watt_export(int index, float watt, bool allow_less);
    bool buy_watt(const types::energy::ScheduleReqEntry& _offer, int index, float watt, bool allow_less, bool import);

    ScheduleRes trading;
    Offer* offer{nullptr};
    bool traded{false};
};

} // namespace module

#endif // BROKER_FAST_CHARGING_HPP
