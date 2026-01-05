// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest
#ifndef REQUESTHANDLERDUMMY_HPP
#define REQUESTHANDLERDUMMY_HPP

#include "../data/DataStore.hpp"

#include <../rpc/RequestHandlerInterface.hpp>
#include <everest_api_types/json_rpc_api/codec.hpp>

namespace everest_api_types = everest::lib::API::V1_0::types;
namespace RPCDataTypes = everest_api_types::json_rpc_api;
class RequestHandlerDummy : public request_interface::RequestHandlerInterface {
public:
    RequestHandlerDummy() = delete;
    explicit RequestHandlerDummy(data::DataStoreCharger& data_store);
    ~RequestHandlerDummy() override = default;

    RPCDataTypes::ErrorResObj set_charging_allowed(const int32_t evse_index, bool charging_allowed) override;
    RPCDataTypes::ErrorResObj set_ac_charging(const int32_t evse_index, bool charging_allowed, bool max_current,
                                              std::optional<int> phase_count) override;
    RPCDataTypes::ErrorResObj set_ac_charging_current(const int32_t evse_index, float max_current) override;
    RPCDataTypes::ErrorResObj set_ac_charging_phase_count(const int32_t evse_index, int phase_count) override;
    RPCDataTypes::ErrorResObj set_dc_charging(const int32_t evse_index, bool charging_allowed,
                                              float max_power) override;
    RPCDataTypes::ErrorResObj set_dc_charging_power(const int32_t evse_index, float max_power) override;
    RPCDataTypes::ErrorResObj enable_connector(const int32_t evse_index, int connector_id, bool enable,
                                               int priority) override;

private:
    data::DataStoreCharger& data_store;
};

#endif // REQUESTHANDLERDUMMY_HPP
