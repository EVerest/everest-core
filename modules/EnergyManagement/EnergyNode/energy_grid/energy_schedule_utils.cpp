// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "energy_schedule_utils.hpp"

namespace module {
namespace energy_grid {

void process_schedule_with_limits(std::vector<types::energy::ScheduleReqEntry>& schedule, const std::string& source,
                                  double fuse_limit_A, int phase_count, double nominal_voltage_V,
                                  bool enhance_with_current_limits) {

    for (auto& entry : schedule) {
        // Enhance with current limits from power if enabled
        if (enhance_with_current_limits) {
            // Determine phase count to use (schedule entry takes priority over module config)
            int effective_phase_count = phase_count;

            if (entry.limits_to_root.ac_max_phase_count.has_value() &&
                entry.limits_to_root.ac_max_phase_count.value().value > 0) {
                effective_phase_count = entry.limits_to_root.ac_max_phase_count.value().value;
            } else if (entry.limits_to_leaves.ac_max_phase_count.has_value() &&
                       entry.limits_to_leaves.ac_max_phase_count.value().value > 0) {
                effective_phase_count = entry.limits_to_leaves.ac_max_phase_count.value().value;
            }

            // Default to 1 phase if no valid phase count is available
            if (effective_phase_count <= 0) {
                effective_phase_count = 1;
            }

            // Calculate current from power for limits_to_root (only if not already set)
            if (entry.limits_to_root.total_power_W.has_value() &&
                entry.limits_to_root.total_power_W.value().value > 0 && nominal_voltage_V > 0 &&
                !entry.limits_to_root.ac_max_current_A.has_value()) {

                float calculated_current = static_cast<float>(entry.limits_to_root.total_power_W.value().value /
                                                              (nominal_voltage_V * effective_phase_count));
                entry.limits_to_root.ac_max_current_A = {calculated_current, source};
            }

            // Note: limits_to_leaves current limits are not modified to match fuse limit behavior
        }

        // Apply fuse limit to limits_to_root (as a safety constraint)
        if (!entry.limits_to_root.ac_max_current_A.has_value() ||
            entry.limits_to_root.ac_max_current_A->value > fuse_limit_A) {
            entry.limits_to_root.ac_max_current_A = {static_cast<float>(fuse_limit_A), source};
        }

        // Apply phase count limit to limits_to_root (as a safety constraint, same model as fuse limit)
        if (phase_count > 0 && (!entry.limits_to_root.ac_max_phase_count.has_value() ||
                                entry.limits_to_root.ac_max_phase_count->value > phase_count)) {
            entry.limits_to_root.ac_max_phase_count = {phase_count, source};
        }
    }
}

} // namespace energy_grid
} // namespace module
