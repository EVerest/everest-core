// SPDX-License-Identifier: Apache-2.0
// Copyright chargebyte GmbH and Contributors to EVerest

#include <everest/external_energy_limits/external_energy_limits.hpp>
#include <everest/logging.hpp>
#include <utils/date.hpp>

#include "RpcApiRequestHandler.hpp"

using namespace types::json_rpc_api;

static const std::string RPCAPI_MODULE_SOURCE = "RpcApi_module";
static const std::chrono::seconds CURRENT_LIMIT_APPLY_TIMEOUT{5};

types::energy::ExternalLimits get_external_limits(int32_t phases) {
    const auto timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    types::energy::ExternalLimits external_limits;
    types::energy::ScheduleReqEntry target_entry;
    target_entry.timestamp = timestamp;

    types::energy::ScheduleReqEntry zero_entry;
    zero_entry.timestamp = timestamp;
    zero_entry.limits_to_leaves.total_power_W = {0, RPCAPI_MODULE_SOURCE};

    target_entry.limits_to_leaves.ac_max_phase_count = {phases};
    target_entry.limits_to_leaves.ac_min_phase_count = {phases};

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

types::energy::ExternalLimits get_external_limits(float phy_value, bool is_power, int32_t phases) {
    const auto timestamp = Everest::Date::to_rfc3339(date::utc_clock::now());
    types::energy::ExternalLimits external_limits;
    types::energy::ScheduleReqEntry target_entry;
    target_entry.timestamp = timestamp;

    types::energy::ScheduleReqEntry zero_entry;
    zero_entry.timestamp = timestamp;
    zero_entry.limits_to_leaves.total_power_W = {0, RPCAPI_MODULE_SOURCE};

    target_entry.limits_to_leaves.ac_max_phase_count = {phases};
    target_entry.limits_to_leaves.ac_min_phase_count = {phases};

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
    data::DataStoreCharger& dataobj, const std::vector<std::unique_ptr<evse_managerIntf>>& r_evse_managers,
    const std::vector<std::unique_ptr<external_energy_limitsIntf>>& r_evse_energy_sink) :
    data_store(dataobj), evse_managers(r_evse_managers), evse_energy_sink(r_evse_energy_sink) {
}

ErrorResObj RpcApiRequestHandler::set_charging_allowed(const int32_t evse_index, bool charging_allowed) {
    ErrorResObj res{};
    bool success{true};

    // find the EVSE manager for the given index
    const auto it = std::find_if(evse_managers.begin(), evse_managers.end(), [&evse_index](const auto& manager) {
        return (manager->get_mapping().has_value() && (manager->get_mapping().value().evse == evse_index));
    });

    if (it == evse_managers.end()) {
        res.error = ResponseErrorEnum::ErrorInvalidEVSEIndex;
        EVLOG_warning << "No EVSE manager found for index: " << evse_index;
        return res;
    }

    auto& evse_manager = *it;
    auto evse_store = data_store.get_evse_store(evse_index);

    // If the charging allowed state is already set to the desired value, we can return early.
    if (evse_store->evsestatus.get_data()->charging_allowed == charging_allowed) {
        EVLOG_debug << "Charging allowed state for EVSE index: " << evse_index
                    << " is already set to: " << charging_allowed;
        res.error = ResponseErrorEnum::NoError;
        return res;
    }

    // Determine the current state of the EVSE. In case the EVSE is currently charging, we can use
    // the resume-pause methods to control the charging process.
    auto evse_state = evse_store->evsestatus.get_state();
    bool is_charging = (evse_state == types::json_rpc_api::EVSEStateEnum::Charging);
    bool is_charging_paused = (evse_state == types::json_rpc_api::EVSEStateEnum::ChargingPausedEVSE ||
                               evse_state == types::json_rpc_api::EVSEStateEnum::ChargingPausedEV);
    bool is_power_limit = !configured_limits.is_current_set;

    if (charging_allowed) {
        float phy_limit{0.0f};
        // first we need to determine which limits to apply. If the limit (current or power) is already set, we will use
        // that.
        if (configured_limits.evse_limit.has_value()) {
            // If current is set, use the configured current limit
            phy_limit = configured_limits.evse_limit.value();
        } else {
            // If no limits are set, use the default values. TODO: It would be better to get the default values from the
            // EVSE manager.
            phy_limit = 999.9f; // Default maximum current
            is_power_limit = false;
        }

        // If the phases are not set, we assume DC charging. This means there is no need to apply phase limits.
        ErrorResObj result = check_active_phases_and_set_limits(evse_index, phy_limit, is_power_limit);

        if (result.error != ResponseErrorEnum::NoError) {
            EVLOG_warning << "Failed to set external limits for EVSE index: " << evse_index
                          << " with error: " << result.error;
            res.error = result.error;
            return res;
        }
        if (is_charging_paused) {
            if (!evse_manager->call_resume_charging()) {
                success = false;
                EVLOG_warning << "Failed to resume charging for EVSE index: " << evse_index;
            }
        }
    } else {
        if (is_charging) {
            // If charging is not allowed, we need to pause the charging process
            if (!evse_manager->call_pause_charging()) {
                success = false;
                EVLOG_warning << "Failed to pause charging for EVSE index: " << evse_index;
            }
        } else {
            // Additionally, in case the EVSE is not charging, we set the limits to 0 to prevent any charging from
            // starting
            float max_power{0.0f};

            ErrorResObj res_limit = set_external_limit(
                evse_index, max_power,
                std::function<types::energy::ExternalLimits(float)>(
                    [this, is_power_limit](float value) { return get_external_limits(value, is_power_limit); }));

            if (res_limit.error != ResponseErrorEnum::NoError) {
                EVLOG_warning << "Failed to set external limits for EVSE index: " << evse_index
                              << " with error: " << res_limit.error;
                return res;
            }
        }
    }

    // Update the EVSE status in the data store
    // TODO: Add a var to the EVSEManager to track if charging is allowed
    evse_store->evsestatus.set_charging_allowed(charging_allowed);

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

ErrorResObj RpcApiRequestHandler::set_ac_charging(const int32_t evse_index, bool charging_allowed, bool max_current,
                                                  std::optional<int> phase_count) {
    ErrorResObj res{};
    res.error = ResponseErrorEnum::ErrorValuesNotApplied;
    // TODO: Currently not implemented.
    return res;
}

// template method to set the external limits depending on the type of value (current, power, or phase count)
template <typename T>
ErrorResObj RpcApiRequestHandler::set_external_limit(const int32_t evse_index, T value,
                                                     std::function<types::energy::ExternalLimits(T)> make_limits) {
    ErrorResObj res{};
    res.error = ResponseErrorEnum::NoError;

    bool is_sink_configured;
    try {
        is_sink_configured = external_energy_limits::is_evse_sink_configured(evse_energy_sink, evse_index);
    } catch (const std::runtime_error&) {
        is_sink_configured = false;
    }

    if (is_sink_configured) {
        auto& energy_sink = external_energy_limits::get_evse_sink_by_evse_id(evse_energy_sink, evse_index);
        try {
            const auto external_limits = make_limits(value);
            energy_sink.call_set_external_limits(external_limits);
        } catch (const std::invalid_argument& e) {
            EVLOG_warning << "Invalid limit: No conversion of given input could be performed: " << e.what();
            res.error = ResponseErrorEnum::ErrorInvalidParameter;
        }
    } else {
        res.error = ResponseErrorEnum::ErrorInvalidEVSEIndex;
        EVLOG_warning << "No EVSE energy sink configured for evse_index: " << evse_index
                      << ". This module does therefore not allow control of amps or power limits for this EVSE";
    }

    return res;
}

ErrorResObj RpcApiRequestHandler::set_ac_charging_current(const int32_t evse_index, float max_current) {
    configured_limits.is_current_set = true;
    configured_limits.evse_limit = max_current;
    ErrorResObj res{};

    auto evse_store = data_store.get_evse_store(evse_index);
    auto evse_state = evse_store->evsestatus.get_state();

    // Skipping applying limits if the EVSE is in WaitingForEnergy state and charging is not allowed.
    // In this case, the zero limit is already applied to prevent charging. This value should not be overridden.
    if ((evse_state == types::json_rpc_api::EVSEStateEnum::WaitingForEnergy) &&
        (evse_store->evsestatus.get_data()->charging_allowed == false)) {
        res.error = ResponseErrorEnum::NoError;
        return res;
    }

    res = check_active_phases_and_set_limits(evse_index, max_current, false);
    if (res.error != ResponseErrorEnum::NoError) {
        return res;
    }

    // Wait until the limits are applied or timeout occurs
    if (evse_store->evsestatus.wait_until_current_limit_applied(max_current, CURRENT_LIMIT_APPLY_TIMEOUT)) {
        res.error = ResponseErrorEnum::NoError;
    } else {
        res.error = ResponseErrorEnum::ErrorValuesNotApplied;
    }

    return res;
}

ErrorResObj RpcApiRequestHandler::set_ac_charging_phase_count(const int32_t evse_index, int phase_count) {
    ErrorResObj res{};

    if (configured_limits.evse_limit.has_value()) {
        // In case a current or power limit is already set, we need to consider that when applying the phase count
        bool is_power = !configured_limits.is_current_set;
        res = set_external_limit(
            evse_index, configured_limits.evse_limit.value(),
            std::function<types::energy::ExternalLimits(float)>([this, is_power, phase_count](float phy_value) {
                return get_external_limits(phy_value, is_power, phase_count);
            }));
    } else {
        // If no current or power limit is set, we can just apply the phase count
        res = set_external_limit(evse_index, phase_count,
                                 std::function<types::energy::ExternalLimits(int)>(
                                     [this](int value) { return get_external_limits(static_cast<int32_t>(value)); }));
    }
    return res;
}

ErrorResObj RpcApiRequestHandler::set_dc_charging(const int32_t evse_index, bool charging_allowed, float max_power) {
    ErrorResObj res{};
    res.error = ResponseErrorEnum::ErrorValuesNotApplied;
    // TODO: Currently not implemented.
    return res;
}

ErrorResObj RpcApiRequestHandler::set_dc_charging_power(const int32_t evse_index, float max_power) {
    configured_limits.is_current_set = false;
    configured_limits.evse_limit = max_power;
    ErrorResObj res = set_external_limit(evse_index, max_power,
                                         std::function<types::energy::ExternalLimits(float)>(
                                             [this](float value) { return get_external_limits(value, true); }));
    return res;
}

ErrorResObj RpcApiRequestHandler::enable_connector(const int32_t evse_index, int connector_id, bool enable,
                                                   int priority) {
    ErrorResObj res{};

    const auto it = std::find_if(evse_managers.begin(), evse_managers.end(), [&evse_index](const auto& manager) {
        return (manager->get_mapping().has_value() && (manager->get_mapping().value().evse == evse_index));
    });

    if (it == evse_managers.end()) {
        res.error = ResponseErrorEnum::ErrorInvalidEVSEIndex;
        EVLOG_warning << "No EVSE manager found for index: " << evse_index;
        return res;
    }

    auto& evse_manager = *it;

    types::evse_manager::EnableDisableSource cmd_source;
    cmd_source.enable_source = types::evse_manager::Enable_source::LocalAPI;
    cmd_source.enable_state =
        enable ? types::evse_manager::Enable_state::Enable : types::evse_manager::Enable_state::Disable;
    cmd_source.enable_priority = priority;

    const bool result_enabled = evse_manager->call_enable_disable(connector_id, cmd_source);

    if (result_enabled == enable) {
        res.error = ResponseErrorEnum::NoError;
        EVLOG_debug << "Connector " << connector_id << " on EVSE index: " << evse_index << " has been "
                    << (enable ? "enabled" : "disabled") << " with priority: " << priority;
    } else {
        res.error = ResponseErrorEnum::ErrorValuesNotApplied;
        EVLOG_warning << "Failed to enable/disable connector " << connector_id << " on EVSE index: " << evse_index;
    }

    return res;
}

types::json_rpc_api::ErrorResObj RpcApiRequestHandler::check_active_phases_and_set_limits(const int32_t evse_index,
                                                                                          const float phy_value,
                                                                                          const bool is_power) {
    ErrorResObj res{};
    int phases{0};
    auto evse_store = data_store.get_evse_store(evse_index);

    if (evse_store->evsestatus.get_data().has_value()) {
        phases = evse_store->evsestatus.get_data()->ac_charge_status.has_value()
                     ? evse_store->evsestatus.get_data()->ac_charge_status.value().evse_active_phase_count
                     : 0;
    }

    if (phases == 0) {
        res = set_external_limit(evse_index, phy_value,
                                 std::function<types::energy::ExternalLimits(float)>([this, is_power](float phy_value) {
                                     return get_external_limits(phy_value, is_power);
                                 }));
    } else {
        res = set_external_limit(
            evse_index, phy_value,
            std::function<types::energy::ExternalLimits(float)>([this, is_power, phases](float phy_value) {
                return get_external_limits(phy_value, is_power, phases);
            }));
    }

    return res;
}
