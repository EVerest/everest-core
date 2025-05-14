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
    virtual types::json_rpc_api::ErrorResObj setChargingAllowed(const std::string& evse_id, bool charging_allowed) = 0;
    virtual types::json_rpc_api::ErrorResObj setACCharging(bool charging_allowed, bool max_current, std::optional<int> phase_count) = 0;
    virtual types::json_rpc_api::ErrorResObj setACChargingCurrent(const std::string& evse_id, float max_current) = 0;
    virtual types::json_rpc_api::ErrorResObj setACChargingPhaseCount(const std::string& evse_id, int phase_count) = 0;
    virtual types::json_rpc_api::ErrorResObj setDCCharging(const std::string& evse_id, bool charging_allowed, int max_power) = 0;
    virtual types::json_rpc_api::ErrorResObj setDCChargingPower(const std::string& evse_id, int max_power) = 0;
    virtual types::json_rpc_api::ErrorResObj enableConnector(int connector_id, bool enable, int priority) = 0;
};

} // namespace request_interface

#endif // REQUEST_HANDLER_INTERFACE_HPP
