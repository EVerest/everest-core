// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef REQUESTHANDLERDUMMY_HPP
#define REQUESTHANDLERDUMMY_HPP

#include <generated/types/json_rpc_api.hpp>
#include <../rpc/RequestHandlerInterface.hpp>

class RequestHandlerDummy : public request_interface::RequestHandlerInterface {
public:
    RequestHandlerDummy() = default;
    ~RequestHandlerDummy() override = default;

    types::json_rpc_api::ErrorResObj setChargingAllowed(const std::string& evse_id, bool charging_allowed) override;
    types::json_rpc_api::ErrorResObj setACCharging(bool charging_allowed, bool max_current, std::optional<int> phase_count) override;
    types::json_rpc_api::ErrorResObj setACChargingCurrent(const std::string& evse_id, float max_current) override;
    types::json_rpc_api::ErrorResObj setACChargingPhaseCount(const std::string& evse_id, int phase_count) override;
    types::json_rpc_api::ErrorResObj setDCCharging(const std::string& evse_id, bool charging_allowed, int max_power) override;
    types::json_rpc_api::ErrorResObj setDCChargingPower(const std::string& evse_id, int max_power) override;
    types::json_rpc_api::ErrorResObj enableConnector(int connector_id, bool enable, int priority) override;
};


#endif // REQUESTHANDLERDUMMY_HPP
