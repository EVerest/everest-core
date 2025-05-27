// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#include <everest/logging.hpp>
#include <external_energy_limits.hpp>
#include <utils/date.hpp>

#include "RpcApiRequestHandler.hpp"

using namespace types::json_rpc_api;

static const std::string RPCAPI_MODULE_SOURCE = "RpcApi_module";

types::energy::ExternalLimits get_external_limits(int32_t phases) {
    const auto timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    types::energy::ExternalLimits external_limits;
    types::energy::ScheduleReqEntry target_entry;
    target_entry.timestamp = timestamp;

    types::energy::ScheduleReqEntry zero_entry;
    zero_entry.timestamp = timestamp;
    zero_entry.limits_to_leaves.total_power_W = {0, RPCAPI_MODULE_SOURCE};

    // check if phases are 1 or 3, otherwise throw an exception
    if (phases == 1 || phases == 3) {
        target_entry.limits_to_leaves.ac_max_phase_count = {phases};
        target_entry.limits_to_leaves.ac_min_phase_count = {phases};
    } else {
        std::string error_msg = "Invalid phase count " + std::to_string(phases) + ". Only 1 or 3 phases are supported.";
        throw std::out_of_range(error_msg);
    }

    external_limits.schedule_import = std::vector<types::energy::ScheduleReqEntry>(1, target_entry);
    external_limits.schedule_export = std::vector<types::energy::ScheduleReqEntry>(1, zero_entry);
    
    return external_limits;
}


// This function is used to get the external limits for AC charging based on the current or power value.
types::energy::ExternalLimits get_external_limits(float phy_value, bool is_power = false) {
    const auto timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    types::energy::ExternalLimits external_limits;
    types::energy::ScheduleReqEntry target_entry;
    target_entry.timestamp = timestamp;

    types::energy::ScheduleReqEntry zero_entry;
    zero_entry.timestamp = timestamp;
    zero_entry.limits_to_leaves.total_power_W = {0, RPCAPI_MODULE_SOURCE};

    if (is_power) {
        target_entry.limits_to_leaves.total_power_W = {phy_value, RPCAPI_MODULE_SOURCE};
    } else {
        target_entry.limits_to_leaves.ac_max_current_A = {phy_value, RPCAPI_MODULE_SOURCE};
    }

    if (phy_value > 0) {
        external_limits.schedule_import = std::vector<types::energy::ScheduleReqEntry>(1, target_entry);
    } else {
        external_limits.schedule_export = std::vector<types::energy::ScheduleReqEntry>(1, target_entry);
        external_limits.schedule_import = std::vector<types::energy::ScheduleReqEntry>(1, zero_entry);
    }
    
    return external_limits;
}

RpcApiRequestHandler::RpcApiRequestHandler(
    const std::vector<std::unique_ptr<evse_managerIntf>>& r_evse_managers,
    const std::vector<std::unique_ptr<error_historyIntf>>& r_error_histories,
    const std::vector<std::unique_ptr<external_energy_limitsIntf>>& r_evse_energy_sink)
    : evse_managers(r_evse_managers),
      error_histories(r_error_histories),
      evse_energy_sink(r_evse_energy_sink) {
}

RpcApiRequestHandler::~RpcApiRequestHandler() {
    // Destructor implementation
}

ErrorResObj RpcApiRequestHandler::set_charging_allowed(const int32_t evse_index, bool charging_allowed) {
    ErrorResObj res {};

    const std::string evse_index_str = std::to_string(evse_index);
    const auto it = std::find_if(evse_managers.begin(), evse_managers.end(),
                                 [&evse_index_str](const auto& manager) {
                                     return manager->module_id == evse_index_str;
                                 });

    if (it == evse_managers.end()) {
        res.error = ResponseErrorEnum::ErrorInvalidEVSEIndex;
        EVLOG_warning << "No EVSE manager found for index: " << evse_index;
        return res;
    }

    auto& evse_manager = *it;
    const bool success = charging_allowed
                             ? evse_manager->call_resume_charging()
                             : evse_manager->call_pause_charging();

    if (success) {
        res.error = ResponseErrorEnum::NoError;
        EVLOG_debug << "Charging " << (charging_allowed ? "allowed" : "not allowed")
                    << " for EVSE index: " << evse_index;
    } else {
        res.error = ResponseErrorEnum::ErrorValuesNotApplied;
        EVLOG_warning << "Failed to set charging " << (charging_allowed ? "allowed" : "not allowed")
                      << " for EVSE index: " << evse_index;
    }

    return res;
}

ErrorResObj RpcApiRequestHandler::set_ac_charging(const int32_t evse_index, bool charging_allowed, bool max_current, std::optional<int> phase_count) {
    ErrorResObj res {};
    res.error = ResponseErrorEnum::ErrorValuesNotApplied;
    // TODO: Currently not implemented.
    return res;
}

// template method to set the external limits depending on the type of value (current, power, or phase count)
template <typename T>
ErrorResObj RpcApiRequestHandler::set_external_limit(const int32_t evse_index, T value,
                                                     std::function<types::energy::ExternalLimits(T)> make_limits) {
    ErrorResObj res {};

    if (external_energy_limits::is_evse_sink_configured(evse_energy_sink, evse_index)) {
        auto& energy_sink =
            external_energy_limits::get_evse_sink_by_evse_id(evse_energy_sink, evse_index);
        try {
            const auto external_limits = make_limits(value);
            energy_sink.call_set_external_limits(external_limits);
        } catch (const std::invalid_argument& e) {
            EVLOG_warning << "Invalid limit: No conversion of given input could be performed.";
            res.error = ResponseErrorEnum::ErrorInvalidParameter;
        }
    } else {
        res.error = ResponseErrorEnum::ErrorInvalidEVSEIndex;
        EVLOG_warning << "No evse energy sink configured for evse_index: " << evse_index
                      << ". This module does therefore not allow control of amps or power limits for this EVSE";
    }

    return res;
}

ErrorResObj RpcApiRequestHandler::set_ac_charging_current(const int32_t evse_index, float max_current) {
    return set_external_limit(evse_index, max_current,
                              std::function<types::energy::ExternalLimits(float)>(
                                  [this](float value) { return get_external_limits(value); }));
}

ErrorResObj RpcApiRequestHandler::set_ac_charging_phase_count(const int32_t evse_index, int phase_count) {
    return set_external_limit(evse_index, phase_count,
                              std::function<types::energy::ExternalLimits(int)>(
                                  [this](int value) { return get_external_limits(static_cast<int32_t>(value)); }));
}

ErrorResObj RpcApiRequestHandler::set_dc_charging(const int32_t evse_index, bool charging_allowed, float max_power) {
    ErrorResObj res {};
    res.error = ResponseErrorEnum::ErrorValuesNotApplied;
    // TODO: Currently not implemented.
    return res;
}

ErrorResObj RpcApiRequestHandler::set_dc_charging_power(const int32_t evse_index, float max_power) {
    return set_external_limit(evse_index, max_power, std::function<types::energy::ExternalLimits(float)>(
                                [this](float value) { return get_external_limits(value, true); }));
}

ErrorResObj RpcApiRequestHandler::enable_connector(const int32_t evse_index, int connector_id, bool enable, int priority) {
    ErrorResObj res {};

    const std::string evse_index_str = std::to_string(evse_index);
    const auto it = std::find_if(evse_managers.begin(), evse_managers.end(),
                                 [&evse_index_str](const auto& manager) {
                                     return manager->module_id == evse_index_str;
                                 });

    if (it == evse_managers.end()) {
        res.error = ResponseErrorEnum::ErrorInvalidEVSEIndex;
        EVLOG_warning << "No EVSE manager found for index: " << evse_index;
        return res;
    }

    auto& evse_manager = *it;

    types::evse_manager::EnableDisableSource cmd_source;
    cmd_source.enable_source = types::evse_manager::Enable_source::LocalAPI;
    cmd_source.enable_state = enable ? types::evse_manager::Enable_state::Enable
                                      : types::evse_manager::Enable_state::Disable;
    cmd_source.enable_priority = priority;

    const bool success =  evse_manager->call_enable_disable(connector_id, cmd_source);

    if (success) {
        res.error = ResponseErrorEnum::NoError;
        EVLOG_debug << "Connector " << connector_id << " on EVSE index: " << evse_index
                    << " has been " << (enable ? "enabled" : "disabled") << " with priority: "
                    << priority;
    } else {
        res.error = ResponseErrorEnum::ErrorValuesNotApplied;
        EVLOG_warning << "Failed to enable/disable connector " << connector_id
                      << " on EVSE index: " << evse_index;
    }

    return res;
}
