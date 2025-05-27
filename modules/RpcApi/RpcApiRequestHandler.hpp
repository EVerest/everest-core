#ifndef RPCAPIREQUESTHANDLER_HPP
#define RPCAPIREQUESTHANDLER_HPP

#include <generated/interfaces/error_history/Interface.hpp>
#include <generated/interfaces/evse_manager/Interface.hpp>
#include <generated/interfaces/external_energy_limits/Interface.hpp>

#include "rpc/RequestHandlerInterface.hpp"

class RpcApiRequestHandler : public request_interface::RequestHandlerInterface {
public:
    // delete default constructor
    RpcApiRequestHandler() = delete;
    RpcApiRequestHandler(const std::vector<std::unique_ptr<evse_managerIntf>>& r_evse_managers,
        const std::vector<std::unique_ptr<error_historyIntf>>& r_error_histories,
        const std::vector<std::unique_ptr<external_energy_limitsIntf>>& r_evse_energy_sink);
    ~RpcApiRequestHandler();

    types::json_rpc_api::ErrorResObj setChargingAllowed(const int32_t evse_index, bool charging_allowed) override;
    types::json_rpc_api::ErrorResObj setACCharging(const int32_t evse_index, bool charging_allowed, bool max_current, std::optional<int> phase_count) override;
    types::json_rpc_api::ErrorResObj setACChargingCurrent(const int32_t evse_index, float max_current) override;
    types::json_rpc_api::ErrorResObj setACChargingPhaseCount(const int32_t evse_index, int phase_count) override;
    types::json_rpc_api::ErrorResObj setDCCharging(const int32_t evse_index, bool charging_allowed, float max_power) override;
    types::json_rpc_api::ErrorResObj setDCChargingPower(const int32_t evse_index, float max_power) override;
    types::json_rpc_api::ErrorResObj enableConnector(const int32_t evse_index, int connector_id, bool enable, int priority) override;
private:
    // Add any private member variables or methods here

    const std::vector<std::unique_ptr<evse_managerIntf>>& evse_managers;
    const std::vector<std::unique_ptr<error_historyIntf>>& error_histories;
    const std::vector<std::unique_ptr<external_energy_limitsIntf>>& evse_energy_sink;
};

#endif // RPCAPIREQUESTHANDLER_HPP