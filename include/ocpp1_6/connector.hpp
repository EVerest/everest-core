// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2022 Pionix GmbH and Contributors to EVerest

#ifndef OCPP1_6_CONNECTOR_HPP
#define OCPP1_6_CONNECTOR_HPP

#include <ocpp1_6/types.hpp>
#include <ocpp1_6/ocpp_types.hpp>
#include <ocpp1_6/transaction.hpp>

namespace ocpp1_6 {

struct Connector {
    int32_t id;
    Powermeter powermeter;
    double max_current_offered = 0;
    std::shared_ptr<Transaction> transaction = nullptr;
    std::map<int, ChargingProfile> stack_level_tx_default_profiles_map;
    std::map<int, ChargingProfile> stack_level_tx_profiles_map;

    explicit Connector(const int id) : id(id) {};
};

} // namespace ocpp1_6

#endif
