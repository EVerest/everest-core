// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#include "RequestHandlerDummy.hpp"

namespace everest_api_types = everest::lib::API::V1_0::types;
namespace RPCDataTypes = everest_api_types::json_rpc_api;
using namespace RPCDataTypes;

RequestHandlerDummy::RequestHandlerDummy(data::DataStoreCharger& dataobj) : data_store(dataobj) {
}

RPCDataTypes::ErrorResObj RequestHandlerDummy::set_charging_allowed(const int32_t evse_index, bool charging_allowed) {
    RPCDataTypes::ErrorResObj res{RPCDataTypes::ResponseErrorEnum::NoError};
    return res;
}
RPCDataTypes::ErrorResObj RequestHandlerDummy::set_ac_charging(const int32_t evse_index, bool charging_allowed,
                                                               bool max_current, std::optional<int> phase_count) {
    RPCDataTypes::ErrorResObj res{RPCDataTypes::ResponseErrorEnum::NoError};
    return res;
}
RPCDataTypes::ErrorResObj RequestHandlerDummy::set_ac_charging_current(const int32_t evse_index, float max_current) {
    ErrorResObj res{};

    auto evse_store = data_store.get_evse_store(evse_index);
    auto evse_state = evse_store->evsestatus.get_state();

    // Skipping applying limits if the EVSE is in WaitingForEnergy state and charging is not allowed.
    // In this case, the zero limit is already applied to prevent charging. This value should not be overridden.
    if ((evse_state == RPCDataTypes::EVSEStateEnum::WaitingForEnergy) &&
        (evse_store->evsestatus.get_data()->charging_allowed == false)) {
        res.error = ResponseErrorEnum::NoError;
        return res;
    }

    // Wait until the limits are applied or timeout occurs
    if (evse_store->evsestatus.wait_until_current_limit_applied(max_current, std::chrono::milliseconds(100))) {
        res.error = ResponseErrorEnum::NoError;
    } else {
        res.error = ResponseErrorEnum::ErrorValuesNotApplied;
    }

    return res;
}
RPCDataTypes::ErrorResObj RequestHandlerDummy::set_ac_charging_phase_count(const int32_t evse_index, int phase_count) {
    RPCDataTypes::ErrorResObj res{RPCDataTypes::ResponseErrorEnum::NoError};
    return res;
}
RPCDataTypes::ErrorResObj RequestHandlerDummy::set_dc_charging(const int32_t evse_index, bool charging_allowed,
                                                               float max_power) {
    RPCDataTypes::ErrorResObj res{RPCDataTypes::ResponseErrorEnum::NoError};
    return res;
}
RPCDataTypes::ErrorResObj RequestHandlerDummy::set_dc_charging_power(const int32_t evse_index, float max_power) {
    RPCDataTypes::ErrorResObj res{RPCDataTypes::ResponseErrorEnum::NoError};
    return res;
}
RPCDataTypes::ErrorResObj RequestHandlerDummy::enable_connector(const int32_t evse_index, int connector_id, bool enable,
                                                                int priority) {
    RPCDataTypes::ErrorResObj res{RPCDataTypes::ResponseErrorEnum::NoError};
    return res;
}
