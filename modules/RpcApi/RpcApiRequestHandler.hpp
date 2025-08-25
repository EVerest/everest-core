#ifndef RPCAPIREQUESTHANDLER_HPP
#define RPCAPIREQUESTHANDLER_HPP

#include <generated/interfaces/error_history/Interface.hpp>
#include <generated/interfaces/evse_manager/Interface.hpp>
#include <generated/interfaces/external_energy_limits/Interface.hpp>

#include "data/DataStore.hpp"
#include "rpc/RequestHandlerInterface.hpp"

class RpcApiRequestHandler : public request_interface::RequestHandlerInterface {
public:
    // delete default constructor
    RpcApiRequestHandler() = delete;
    RpcApiRequestHandler(data::DataStoreCharger& data_store,
                         const std::vector<std::unique_ptr<evse_managerIntf>>& r_evse_managers,
                         const std::vector<std::unique_ptr<external_energy_limitsIntf>>& r_evse_energy_sink);
    ~RpcApiRequestHandler();

    types::json_rpc_api::ErrorResObj set_charging_allowed(const int32_t evse_index, bool charging_allowed) override;
    types::json_rpc_api::ErrorResObj set_ac_charging(const int32_t evse_index, bool charging_allowed, bool max_current,
                                                     std::optional<int> phase_count) override;
    types::json_rpc_api::ErrorResObj set_ac_charging_current(const int32_t evse_index, float max_current) override;
    types::json_rpc_api::ErrorResObj set_ac_charging_phase_count(const int32_t evse_index, int phase_count) override;
    types::json_rpc_api::ErrorResObj set_dc_charging(const int32_t evse_index, bool charging_allowed,
                                                     float max_power) override;
    types::json_rpc_api::ErrorResObj set_dc_charging_power(const int32_t evse_index, float max_power) override;
    types::json_rpc_api::ErrorResObj enable_connector(const int32_t evse_index, int connector_id, bool enable,
                                                      int priority) override;

private:
    // Add any private member variables or methods here
    data::DataStoreCharger& data_store;
    types::json_rpc_api::ErrorResObj check_active_phases_and_set_limits(const int32_t evse_index, const float phy_value,
                                                                        const bool is_power);
    template <typename T>
    types::json_rpc_api::ErrorResObj set_external_limit(int32_t evse_index, T value,
                                                        std::function<types::energy::ExternalLimits(T)> make_limits);

    const std::vector<std::unique_ptr<evse_managerIntf>>& evse_managers;
    const std::vector<std::unique_ptr<external_energy_limitsIntf>>& evse_energy_sink;

    struct {
        std::optional<float> evse_limit; ///< Maximum current or power limit for the EVSE
        bool is_current_set = false;     ///< Flag to indicate if current or power limit is set
    } configured_limits;
};

#endif // RPCAPIREQUESTHANDLER_HPP