// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest
#include "RequestHandlerDummy.hpp"

types::json_rpc_api::ErrorResObj RequestHandlerDummy::setChargingAllowed(const int32_t evse_index, bool charging_allowed) {
    types::json_rpc_api::ErrorResObj res {types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::setACCharging(const int32_t evse_index, bool charging_allowed, bool max_current, std::optional<int> phase_count) {
    types::json_rpc_api::ErrorResObj res {types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::setACChargingCurrent(const int32_t evse_index, float max_current) {
    types::json_rpc_api::ErrorResObj res {res.error = types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::setACChargingPhaseCount(const int32_t evse_index, int phase_count) {
    types::json_rpc_api::ErrorResObj res {types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::setDCCharging(const int32_t evse_index, bool charging_allowed, float max_power) {
    types::json_rpc_api::ErrorResObj res {types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::setDCChargingPower(const int32_t evse_index, float max_power) {
    types::json_rpc_api::ErrorResObj res {types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::enableConnector(const int32_t evse_index, int connector_id, bool enable, int priority) {
    types::json_rpc_api::ErrorResObj res {types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
