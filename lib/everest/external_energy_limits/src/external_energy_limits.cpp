// SPDX-License-Identifier: Apache-2.0
// Copyright 2024 Pionix GmbH and Contributors to EVerest

#include <everest/external_energy_limits/external_energy_limits.hpp>

namespace external_energy_limits {

bool is_evse_sink_configured(const std::vector<std::unique_ptr<external_energy_limitsIntf>>& r_evse_energy_sink,
                             const int32_t evse_id) {
    for (const auto& evse_sink : r_evse_energy_sink) {
        if (not evse_sink->get_mapping().has_value()) {
            EVLOG_critical << "Please configure an evse mapping in your configuration file for the connected "
                              "r_evse_energy_sink with module_id: "
                           << evse_sink->module_id;
            throw std::runtime_error("No mapping configured for evse_id: " + std::to_string(evse_id));
        }
        if (evse_sink->get_mapping().value().evse == evse_id) {
            return true;
        }
    }
    return false;
}

external_energy_limitsIntf&
get_evse_sink_by_evse_id(const std::vector<std::unique_ptr<external_energy_limitsIntf>>& r_evse_energy_sink,
                         const int32_t evse_id) {
    for (const auto& evse_sink : r_evse_energy_sink) {
        if (not evse_sink->get_mapping().has_value()) {
            EVLOG_critical << "Please configure an evse mapping in your configuration file for the connected "
                              "r_evse_energy_sink with module_id: "
                           << evse_sink->module_id;
            throw std::runtime_error("No mapping configured for evse_id: " + std::to_string(evse_id));
        }
        if (evse_sink->get_mapping().value().evse == evse_id) {
            return *evse_sink;
        }
    }
    throw std::runtime_error("No mapping configured for evse_id: " + std::to_string(evse_id));
}

} // namespace external_energy_limits
