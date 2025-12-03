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
    std::optional<Measurement> measurement;
    double max_current_offered = 0;
    double max_power_offered = 0;
    std::shared_ptr<Transaction> transaction = nullptr;
    std::map<int, ChargingProfile> stack_level_tx_default_profiles_map;
    std::map<int, ChargingProfile> stack_level_tx_profiles_map;
    std::optional<std::vector<ChargePointStatus>> trigger_metervalue_on_status;
    std::optional<double> trigger_metervalue_on_power_kw;
    std::optional<double> trigger_metervalue_on_energy_kwh;
    std::unique_ptr<Everest::SystemTimer> trigger_metervalue_at_time_timer;
    std::optional<ChargePointStatus> previous_status;
    std::optional<double> last_triggered_metervalue_power_kw;

    explicit Connector(const int id) : id(id) {
    }
    ~Connector() = default;
    Connector& operator=(const Connector&) = delete;
    Connector(const Connector&) = delete;
};

} // namespace v16
} // namespace ocpp

#endif
