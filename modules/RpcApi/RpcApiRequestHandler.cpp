// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include "RpcApiRequestHandler.hpp"

#include <everest/logging.hpp>

using namespace types::json_rpc_api;

RpcApiRequestHandler::RpcApiRequestHandler(
    const std::vector<std::unique_ptr<evse_managerIntf>>& r_evse_managers,
    const std::vector<std::unique_ptr<error_historyIntf>>& r_error_histories,
    const std::vector<std::unique_ptr<external_energy_limitsIntf>>& r_evse_energy_sink)
    : evse_managers(r_evse_managers),
      error_histories(r_error_histories),
      evse_energy_sink(r_evse_energy_sink) {
}

RpcApiRequestHandler::~RpcApiRequestHandler() {
    // Destructor implementation
}

ErrorResObj RpcApiRequestHandler::setChargingAllowed(const int32_t evse_index, bool charging_allowed) {
    ErrorResObj res {};
    bool rv = evse_managers[0]->call_pause_charging();
    if (rv) {
        res.error = ResponseErrorEnum::NoError;
    } else {
        res.error = ResponseErrorEnum::ErrorValuesNotApplied;
    }
    return res;
}

ErrorResObj RpcApiRequestHandler::setACCharging(const int32_t evse_index, bool charging_allowed, bool max_current, std::optional<int> phase_count) {
    ErrorResObj res {};
    
    return res;
}

ErrorResObj RpcApiRequestHandler::setACChargingCurrent(const int32_t evse_index, float max_current) {
    ErrorResObj res {};
    return res;
}

ErrorResObj RpcApiRequestHandler::setACChargingPhaseCount(const int32_t evse_index, int phase_count) {
    ErrorResObj res {};
    return res;
}

ErrorResObj RpcApiRequestHandler::setDCCharging(const int32_t evse_index, bool charging_allowed, int max_power) {
    ErrorResObj res {};
    return res;
}

ErrorResObj RpcApiRequestHandler::setDCChargingPower(const int32_t evse_index, int max_power) {
    ErrorResObj res {};
    return res;
}

ErrorResObj RpcApiRequestHandler::enableConnector(const int32_t evse_index, int connector_id, bool enable, int priority) {
    ErrorResObj res {};
    return res;
}
