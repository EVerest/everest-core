// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest
#include "RequestHandlerDummy.hpp"

types::json_rpc_api::ErrorResObj RequestHandlerDummy::set_charging_allowed(const int32_t evse_index,
                                                                           bool charging_allowed) {
    types::json_rpc_api::ErrorResObj res{types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::set_ac_charging(const int32_t evse_index, bool charging_allowed,
                                                                      bool max_current,
                                                                      std::optional<int> phase_count) {
    types::json_rpc_api::ErrorResObj res{types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::set_ac_charging_current(const int32_t evse_index,
                                                                              float max_current) {
    types::json_rpc_api::ErrorResObj res{types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::set_ac_charging_phase_count(const int32_t evse_index,
                                                                                  int phase_count) {
    types::json_rpc_api::ErrorResObj res{types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::set_dc_charging(const int32_t evse_index, bool charging_allowed,
                                                                      float max_power) {
    types::json_rpc_api::ErrorResObj res{types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::set_dc_charging_power(const int32_t evse_index, float max_power) {
    types::json_rpc_api::ErrorResObj res{types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
types::json_rpc_api::ErrorResObj RequestHandlerDummy::enable_connector(const int32_t evse_index, int connector_id,
                                                                       bool enable, int priority) {
    types::json_rpc_api::ErrorResObj res{types::json_rpc_api::ResponseErrorEnum::NoError};
    return res;
}
