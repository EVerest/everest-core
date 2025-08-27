// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#ifndef REQUEST_HANDLER_INTERFACE_HPP
#define REQUEST_HANDLER_INTERFACE_HPP

#include <optional>

#include <generated/types/json_rpc_api.hpp>

namespace request_interface {

// --- RequestHandlerInterface ---
// This interface is used to handle synchronous requests from the RPC API.
class RequestHandlerInterface {
public:
    virtual ~RequestHandlerInterface() = default;
    virtual types::json_rpc_api::ErrorResObj set_charging_allowed(const int32_t evse_index, bool charging_allowed) = 0;
    virtual types::json_rpc_api::ErrorResObj set_ac_charging(const int32_t evse_index, bool charging_allowed,
                                                             bool max_current, std::optional<int> phase_count) = 0;
    virtual types::json_rpc_api::ErrorResObj set_ac_charging_current(const int32_t evse_index, float max_current) = 0;
    virtual types::json_rpc_api::ErrorResObj set_ac_charging_phase_count(const int32_t evse_index, int phase_count) = 0;
    virtual types::json_rpc_api::ErrorResObj set_dc_charging(const int32_t evse_index, bool charging_allowed,
                                                             float max_power) = 0;
    virtual types::json_rpc_api::ErrorResObj set_dc_charging_power(const int32_t evse_index, float max_power) = 0;
    virtual types::json_rpc_api::ErrorResObj enable_connector(const int32_t evse_index, int connector_id, bool enable,
                                                              int priority) = 0;
};

} // namespace request_interface

#endif // REQUEST_HANDLER_INTERFACE_HPP
