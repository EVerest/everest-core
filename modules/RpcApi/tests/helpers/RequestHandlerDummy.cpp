// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest
#include "RequestHandlerDummy.hpp"

types::json_rpc_api::ErrorResObj RequestHandlerDummy::setChargingAllowed(const std::string& evse_id, bool charging_allowed) {
    types::json_rpc_api::ErrorResObj res {types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::setACCharging(bool charging_allowed, bool max_current, std::optional<int> phase_count) {
    types::json_rpc_api::ErrorResObj res {types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::setACChargingCurrent(const std::string& evse_id, float max_current) {
    types::json_rpc_api::ErrorResObj res {res.error = types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::setACChargingPhaseCount(const std::string& evse_id, int phase_count) {
    types::json_rpc_api::ErrorResObj res {types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::setDCCharging(const std::string& evse_id, bool charging_allowed, int max_power) {
    types::json_rpc_api::ErrorResObj res {types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::setDCChargingPower(const std::string& evse_id, int max_power) {
    types::json_rpc_api::ErrorResObj res {types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::enableConnector(int connector_id, bool enable, int priority) {
    types::json_rpc_api::ErrorResObj res {types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
