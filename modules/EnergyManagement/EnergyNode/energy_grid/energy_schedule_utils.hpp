// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#ifndef ENERGY_SCHEDULE_UTILS_HPP
#define ENERGY_SCHEDULE_UTILS_HPP

#include <generated/types/energy.hpp>
#include <string>
#include <vector>

namespace module {
namespace energy_grid {

/**
 * @brief Processes energy schedule entries with limits and fuse constraints
 *
 * This function applies fuse limits to schedule entries and optionally enhances
 * them with current limits calculated from power values.
 *
 * @param schedule The schedule entries to process
 * @param source The source identifier for any modifications made
 * @param fuse_limit_A The fuse limit in amperes to apply as a safety constraint
 * @param phase_count The default phase count to use for calculations
 * @param nominal_voltage_V The nominal voltage to use for power-to-current calculations
 * @param enhance_with_current_limits If true, calculates current limits from power values
 */
void process_schedule_with_limits(std::vector<types::energy::ScheduleReqEntry>& schedule, const std::string& source,
                                  double fuse_limit_A, int phase_count, double nominal_voltage_V,
                                  bool enhance_with_current_limits);

} // namespace energy_grid
} // namespace module

#endif // ENERGY_SCHEDULE_UTILS_HPP
