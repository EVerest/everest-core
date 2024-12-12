// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "BrokerFastCharging.hpp"
#include <everest/logging.hpp>
#include <fmt/core.h>

namespace module {

void BrokerFastCharging::tradeImpl() {
    // the offer contains all data we need to decide on a trade
    // we can now buy from/sell to according to the offer at our local market place for this evse
    // our strategy is to charge if we can, and only discharge if charging is not possible. In both
    // cases we buy as much as possible (this is the fast charging implementation).

    // if we have not bought anything, we first need to buy the minimal limits for ac_amp if any.
    for (int i = 0; i < globals.schedule_length; i++) {

        bool time_slot_is_active = time_slot_active(i, offer->import_offer);

        // make this more readable
        auto& max_current_import = offer->import_offer[i].limits_to_root.ac_max_current_A;
        const auto& min_current_import = offer->import_offer[i].limits_to_root.ac_min_current_A;
        auto& total_power_import = offer->import_offer[i].limits_to_root.total_power_W;

        auto& max_current_export = offer->export_offer[i].limits_to_root.ac_max_current_A;
        const auto& min_current_export = offer->export_offer[i].limits_to_root.ac_min_current_A;
        auto& total_power_export = offer->export_offer[i].limits_to_root.total_power_W;

        // If not specified, assume worst case (3ph being active)
        const auto ac_number_of_active_phases_import =
            offer->import_offer[i].limits_to_root.ac_number_of_active_phases.value_or(3);
        const auto ac_number_of_active_phases_export =
            offer->export_offer[i].limits_to_root.ac_number_of_active_phases.value_or(3);

        const types::energy::IntegerWithSource three = {3, "BrokerFastCharging_Fallback"};

        const auto max_phases_import = offer->import_offer[i].limits_to_root.ac_max_phase_count.value_or(three);
        const auto min_phases_import = offer->import_offer[i].limits_to_root.ac_min_phase_count.value_or(three);

        // in each timeslot: do we want to import or export energy?
        if (slot_type[i] == SlotType::Undecided) {
            bool can_import = !((total_power_import.has_value() && total_power_import.value().value == 0.) ||
                                (max_current_import.has_value() && max_current_import.value().value == 0.));

            bool can_export = !((total_power_export.has_value() && total_power_export.value().value == 0.) ||
                                (max_current_export.has_value() && max_current_export.value().value == 0.));

            if (can_import) {
                slot_type[i] = SlotType::Import;
            } else if (can_export) {
                slot_type[i] = SlotType::Export;
            }
        }

        if (num_phases[i].value == 0) {
            num_phases[i] = {ac_number_of_active_phases_import, "BrokerFastCharging_NumPhasesActive"};
        }

        if (slot_type[i] == SlotType::Import) {
            // EVLOG_info << "We can import.";
            if (max_current_import.has_value()) {
                // A current limit is set

                // If an additional watt limit is set check phases, else it is max_phases (typically 3)
                // First decide if we would like to charge 1 phase or 3 phase (if switching is possible at all)
                //   - Check if we are below e.g. 4.2kW (min_current*voltage*3) -> we have to do single phase
                //   - Check if we are above e.g. 4.4kW (min_current*voltage*3 + watt_hysteresis) -> we want to go three
                //   phase
                //   - If we are in between, use what is currently active (hysteresis)

                types::energy::IntegerWithSource number_of_phases = {ac_number_of_active_phases_import,
                                                                     "BrokerFastCharging_Default"};

                float min_power_3ph = 0.;

                if (min_current_import.has_value()) {
                    min_power_3ph =
                        min_current_import.value().value * max_phases_import.value * local_market.nominal_ac_voltage();
                }

                bool number_of_switching_cycles_reached = false;

                if (first_trade[i]) {
                    if (config.switch_1ph_3ph_mode not_eq Switch1ph3phMode::Never and total_power_import.has_value() &&
                        min_power_3ph > 0.) {

                        if (total_power_import.value().value < min_power_3ph) {
                            // We have to do single phase, it is impossible with 3ph
                            number_of_phases = {min_phases_import.value, total_power_import.value().source};
                        } else if (config.switch_1ph_3ph_mode == Switch1ph3phMode::Both and
                                   total_power_import.value().value > min_power_3ph + config.power_hysteresis_W) {
                            number_of_phases = max_phases_import;
                        } else {
                            // Keep number of phases as they are
                            number_of_phases = {ac_number_of_active_phases_import, "BrokerFastCharging_KeepNrOfPhases"};
                        }

                        // Now we made the decision what the optimal number of phases would be (in variable
                        // number_of_phases) We also have a time based hysteresis as well as some limits in maximum
                        // number of switching cycles. This means we maybe cannot use the optimal number of phases just
                        // now. Check those conditions and adjust number_of_phases accordingly.

                        if (config.max_nr_of_switches_per_session > 0 and
                            context.number_1ph3ph_cycles > config.max_nr_of_switches_per_session) {
                            number_of_switching_cycles_reached = true;
                            if (config.stickyness == StickyNess::SinglePhase) {
                                number_of_phases = min_phases_import;
                            } else if (config.stickyness == StickyNess::ThreePhase) {
                                number_of_phases = max_phases_import;
                            } else {
                                number_of_phases = {ac_number_of_active_phases_import,
                                                    "BrokerFastCharging_KeepNrOfPhases"};
                            }
                        }

                        if (number_of_phases.value == min_phases_import.value and time_slot_is_active) {
                            context.ts_1ph_optimal = date::utc_clock::now();
                        }

                        if (config.time_hysteresis_s > 0 and time_slot_is_active) {
                            // Check time based hysteresis:
                            // - store timestamp whenever 1ph is optimal (update continously)
                            // Then now-timestamp is the stable time period for a 3ph condition.
                            // This should only be done in the currently active time slot. Ignore time hysteresis in
                            // other slots in the future or past.
                            // Only allow an actual change to 3ph if the time exceeds the configured hysteresis limit.
                            const auto stable_3ph = std::chrono::duration_cast<std::chrono::seconds>(
                                                        globals.start_time - context.ts_1ph_optimal)
                                                        .count();

                            if (stable_3ph < config.time_hysteresis_s and
                                number_of_phases.value == max_phases_import.value) {
                                number_of_phases = min_phases_import;
                            }
                        }
                    } else {
                        number_of_phases = max_phases_import;
                    }
                }

                // store decision in context
                if (ac_number_of_active_phases_import not_eq context.last_ac_number_of_active_phases_import) {
                    context.number_1ph3ph_cycles++;
                }
                context.last_ac_number_of_active_phases_import = ac_number_of_active_phases_import;

                if (first_trade[i] && min_current_import.has_value() && min_current_import.value().value > 0.) {
                    num_phases[i] = number_of_phases;
                    // EVLOG_info << "I: first trade: try to buy minimal current_A on AC: " <<
                    // min_current_import.value();
                    //    try to buy minimal current_A if we are on AC, but don't buy less.
                    if (not buy_ampere_import(i, min_current_import.value().value, false, number_of_phases) and
                        config.switch_1ph_3ph_mode not_eq Switch1ph3phMode::Never and
                        not number_of_switching_cycles_reached) {
                        // If we cannot buy the minimum amount we need, try again in single phase mode (it may be due to
                        // a watt limit only)
                        number_of_phases = {1, "BrokerFastCharging_Buy3phFailed"};
                        num_phases[i] = number_of_phases;
                        buy_ampere_import(i, min_current_import.value().value, false, number_of_phases);
                    }

                    /*EVLOG_info << "I: " << i << " -- 1ph3ph: " << min_power_3ph << " active_nr_phases "
                               << ac_number_of_active_phases_import << " cycles " << context.number_1ph3ph_cycles
                               << " number_of_phases " << number_of_phases << " time_slot_active "
                               << time_slot_is_active;*/
                } else {
                    // EVLOG_info << "I: Not first trade or nor min current needed.";
                    //  try to buy a slice but allow less to be bought
                    buy_ampere_import(i, globals.slice_ampere, true, num_phases[i]);
                }

            } else if (total_power_import.has_value()) {
                // only a watt limit is available
                // EVLOG_info << "I: Only watt limit is set." << total_power_import.value();
                buy_watt_import(i, globals.slice_watt, true);
            }
        } else if (slot_type[i] == SlotType::Export) {
            // EVLOG_info << "We can export.";
            //  we cannot import, try exporting in this timeslot.
            if (max_current_export.has_value()) {
                // A current limit is set
                if (first_trade[i] && min_current_export.has_value() && min_current_export.value().value > 0.) {
                    // EVLOG_info << "E: first trade: try to buy minimal current_A on AC: " <<
                    // min_current_export.value();
                    //    try to buy minimal current_A if we are on AC, but don't buy less.
                    buy_ampere_export(i, min_current_export.value().value, false, {3, "BrokerFastCharging_FixedValue"});
                } else {
                    // EVLOG_info << "E: Not first trade or nor min current needed.";
                    //  try to buy a slice but allow less to be bought
                    buy_ampere_export(i, globals.slice_ampere, true, {3, "BrokerFastCharging_FixedValue"});
                }
            } else if (total_power_export.has_value()) {
                // only a watt limit is available
                // EVLOG_info << "E: Only watt limit is set." << total_power_export.value();
                buy_watt_export(i, globals.slice_watt, true);
            }
        } else {
            // EVLOG_info << "We can neither import nor export.";
        }
    }
}

} // namespace module
