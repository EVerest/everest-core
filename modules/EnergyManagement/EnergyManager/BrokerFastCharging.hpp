// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef BROKER_FAST_CHARGING_HPP
#define BROKER_FAST_CHARGING_HPP

#include "Broker.hpp"

namespace module {

// This broker tries to charge as fast as possible.
class BrokerFastCharging : public Broker {
public:
    explicit BrokerFastCharging(Market& market, BrokerContext& context, EnergyManagerConfig config) :
        Broker(market, context, config){};
    virtual void tradeImpl() override;

private:
};

} // namespace module

#endif // BROKER_FAST_CHARGING_HPP
