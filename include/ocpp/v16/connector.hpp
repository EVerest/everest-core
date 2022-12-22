// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2023 Pionix GmbH and Contributors to EVerest

#ifndef OCPP_V16_CONNECTOR_HPP
#define OCPP_V16_CONNECTOR_HPP

#include <ocpp/v16/ocpp_types.hpp>
#include <ocpp/v16/transaction.hpp>
#include <ocpp/v16/types.hpp>

namespace ocpp {
namespace v16 {

struct Connector {
    int32_t id;
    Powermeter powermeter;
    double max_current_offered = 0;
    std::shared_ptr<Transaction> transaction = nullptr;
    std::map<int, ChargingProfile> stack_level_tx_default_profiles_map;
    std::map<int, ChargingProfile> stack_level_tx_profiles_map;

    explicit Connector(const int id) : id(id){};
};

} // namespace v16
} // namespace ocpp

#endif
