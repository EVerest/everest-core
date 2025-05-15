// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef REQUEST_HANDLER_INTERFACE_HPP
#define REQUEST_HANDLER_INTERFACE_HPP

#include <string>
#include <optional>

#include <generated/types/json_rpc_api.hpp>


namespace request_interface {

// --- Abstraktes Interface ---
class RequestHandlerInterface {
public:
    virtual ~RequestHandlerInterface() = default;
    virtual types::json_rpc_api::ErrorResObj setChargingAllowed(const int32_t, bool charging_allowed) = 0;
    virtual types::json_rpc_api::ErrorResObj setACCharging(const int32_t, bool charging_allowed, bool max_current, std::optional<int> phase_count) = 0;
    virtual types::json_rpc_api::ErrorResObj setACChargingCurrent(const int32_t, float max_current) = 0;
    virtual types::json_rpc_api::ErrorResObj setACChargingPhaseCount(const int32_t, int phase_count) = 0;
    virtual types::json_rpc_api::ErrorResObj setDCCharging(const int32_t, bool charging_allowed, int max_power) = 0;
    virtual types::json_rpc_api::ErrorResObj setDCChargingPower(const int32_t, int max_power) = 0;
    virtual types::json_rpc_api::ErrorResObj enableConnector(const int32_t, int connector_id, bool enable, int priority) = 0;
};

} // namespace request_interface

#endif // REQUEST_HANDLER_INTERFACE_HPP
