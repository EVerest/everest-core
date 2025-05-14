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

ErrorResObj RpcApiRequestHandler::setChargingAllowed(const std::string& evse_id, bool charging_allowed) {
    ErrorResObj res {};
    return res;
}

ErrorResObj RpcApiRequestHandler::setACCharging(bool charging_allowed, bool max_current, std::optional<int> phase_count) {
    ErrorResObj res {};
    return res;
}

ErrorResObj RpcApiRequestHandler::setACChargingCurrent(const std::string& evse_id, float max_current) {
    ErrorResObj res {};
    return res;
}

ErrorResObj RpcApiRequestHandler::setACChargingPhaseCount(const std::string& evse_id, int phase_count) {
    ErrorResObj res {};
    return res;
}

ErrorResObj RpcApiRequestHandler::setDCCharging(const std::string& evse_id, bool charging_allowed, int max_power) {
    ErrorResObj res {};
    return res;
}

ErrorResObj RpcApiRequestHandler::setDCChargingPower(const std::string& evse_id, int max_power) {
    ErrorResObj res {};
    return res;
}

ErrorResObj RpcApiRequestHandler::enableConnector(int connector_id, bool enable, int priority) {
    ErrorResObj res {};
    return res;
}
