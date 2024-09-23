// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "BrokerFastCharging.hpp"
#include <everest/logging.hpp>
#include <fmt/core.h>

namespace module {

BrokerFastCharging::BrokerFastCharging(Market& _market, BrokerContext& _context, Config _config) :
    Broker(_market, _context), config(_config) {
}

static auto to_timestamp(const types::energy::ScheduleReqEntry& entry) {
    return Everest::Date::from_rfc3339(entry.timestamp);
}

static bool time_slot_active(const int i, const ScheduleReq& offer) {
    const auto& now = globals.start_time;
    const auto t_i = to_timestamp(offer[i]);

    int active_slot = 0;
    // Get active slot:
    if (now < to_timestamp(offer[0])) {
        // First element already in the future
        active_slot = 0;
    } else if (now > to_timestamp(offer[offer.size() - 1])) {
        // Last element in the past
        active_slot = offer.size() - 1;
    } else {
        // Somewhere in between
        for (int n = 0; n < offer.size() - 1; n++) {
            if (now > to_timestamp(offer[n]) and now < to_timestamp(offer[n + 1])) {
                active_slot = n;
                break;
            }
        }
    }

    return active_slot == i;
}

bool BrokerFastCharging::trade(Offer& _offer) {
    // the offer contains all data we need to decide on a trade
    // we can now buy from/sell to according to the offer at our local market place for this evse
    // our strategy is to charge if we can, and only discharge if charging is not possible.
    offer = &_offer;
    if (globals.debug)
        EVLOG_info << local_market.energy_flow_request.uuid << " Broker: " << *offer;

    // create a new schedules that contains everything we want to buy
    trading = globals.empty_schedule_res;

    // buy/sell nothing in the beginning

    for (int i = 0; i < globals.schedule_length; i++) {
        // make this more readable
        auto& max_current = offer->import_offer[i].limits_to_root.ac_max_current_A;
        auto& total_power = offer->import_offer[i].limits_to_root.total_power_W;
        if (max_current.has_value()) {
            trading[i].limits_to_root.ac_max_current_A = 0.;
        }
        if (total_power.has_value()) {
            trading[i].limits_to_root.total_power_W = 0.;
        }
    }

    traded = false;

    if (offer->import_offer.size() != offer->export_offer.size()) {
        EVLOG_error << "import and export offer do not have the same size!";
        return false;
    }

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

        const auto max_phases_import = offer->import_offer[i].limits_to_root.ac_max_phase_count.value_or(3);
        const auto min_phases_import = offer->import_offer[i].limits_to_root.ac_min_phase_count.value_or(3);

        // in each timeslot: do we want to import or export energy?
        if (slot_type[i] == SlotType::Undecided) {
            bool can_import = !((total_power_import.has_value() && total_power_import.value() == 0.) ||
                                (max_current_import.has_value() && max_current_import.value() == 0.));

            bool can_export = !((total_power_export.has_value() && total_power_export.value() == 0.) ||
                                (max_current_export.has_value() && max_current_export.value() == 0.));

            if (can_import) {
                slot_type[i] = SlotType::Import;
            } else if (can_export) {
                slot_type[i] = SlotType::Export;
            }
        }

        if (num_phases[i] == 0) {
            num_phases[i] = ac_number_of_active_phases_import;
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

                int number_of_phases = ac_number_of_active_phases_import;

                const auto min_power_3ph =
                    min_current_import.value_or(0) * max_phases_import * local_market.nominal_ac_voltage();

                bool number_of_switching_cycles_reached = false;

                if (first_trade[i]) {
                    if (config.switch_1ph_3ph_mode not_eq Switch1ph3phMode::Never and total_power_import.has_value() &&
                        min_power_3ph > 0.) {

                        if (total_power_import.value() < min_power_3ph) {
                            // We have to do single phase, it is impossible with 3ph
                            number_of_phases = min_phases_import;
                        } else if (config.switch_1ph_3ph_mode == Switch1ph3phMode::Both and
                                   total_power_import.value() > min_power_3ph + config.power_hysteresis_W) {
                            number_of_phases = max_phases_import;
                        } else {
                            // Keep number of phases as they are
                            number_of_phases = ac_number_of_active_phases_import;
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
                                number_of_phases = ac_number_of_active_phases_import;
                            }
                        }

                        if (number_of_phases == min_phases_import) {
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

                            if (stable_3ph < config.time_hysteresis_s and number_of_phases == max_phases_import) {
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

                if (first_trade[i] && min_current_import.has_value() && min_current_import.value() > 0.) {
                    num_phases[i] = number_of_phases;
                    // EVLOG_info << "I: first trade: try to buy minimal current_A on AC: " <<
                    // min_current_import.value();
                    //    try to buy minimal current_A if we are on AC, but don't buy less.
                    if (not buy_ampere_import(i, min_current_import.value(), false, number_of_phases) and
                        config.switch_1ph_3ph_mode not_eq Switch1ph3phMode::Never and
                        not number_of_switching_cycles_reached) {
                        // If we cannot buy the minimum amount we need, try again in single phase mode (it may be due to
                        // a watt limit only)
                        number_of_phases = 1;
                        num_phases[i] = number_of_phases;
                        buy_ampere_import(i, min_current_import.value(), false, number_of_phases);
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
                if (first_trade[i] && min_current_export.has_value() && min_current_export.value() > 0.) {
                    // EVLOG_info << "E: first trade: try to buy minimal current_A on AC: " <<
                    // min_current_export.value();
                    //    try to buy minimal current_A if we are on AC, but don't buy less.
                    buy_ampere_export(i, min_current_export.value(), false, 3);
                } else {
                    // EVLOG_info << "E: Not first trade or nor min current needed.";
                    //  try to buy a slice but allow less to be bought
                    buy_ampere_export(i, globals.slice_ampere, true, 3);
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

    // if we want to buy anything:
    if (traded) {
        if (globals.debug) {
            EVLOG_info << fmt::format("\033[1;33m                                {}A {}W \033[1;0m",
                                      (trading[0].limits_to_root.ac_max_current_A.has_value()
                                           ? std::to_string(trading[0].limits_to_root.ac_max_current_A.value())
                                           : " [NOT_SET] "),
                                      (trading[0].limits_to_root.total_power_W.has_value()
                                           ? std::to_string(trading[0].limits_to_root.total_power_W.value())
                                           : " [NOT_SET] "));
        }
        //   execute the trade on the market
        local_market.trade(trading);
        return true;
    } else {
        if (globals.debug)
            EVLOG_info << fmt::format("\033[1;33m                               NO TRADE \033[1;0m");

        //   execute the zero trade on the market
        local_market.trade(trading);
        return false;
    }
}

bool BrokerFastCharging::buy_ampere_import(int index, float ampere, bool allow_less, int number_of_phases) {
    return buy_ampere(offer->import_offer[index], index, ampere, allow_less, true, number_of_phases);
}

bool BrokerFastCharging::buy_ampere_export(int index, float ampere, bool allow_less, int number_of_phases) {
    return buy_ampere(offer->export_offer[index], index, ampere, allow_less, false, number_of_phases);
}

bool BrokerFastCharging::buy_watt_import(int index, float watt, bool allow_less) {
    return buy_watt(offer->import_offer[index], index, watt, allow_less, true);
}

bool BrokerFastCharging::buy_watt_export(int index, float watt, bool allow_less) {
    return buy_watt(offer->export_offer[index], index, watt, allow_less, false);
}

bool BrokerFastCharging::buy_ampere(const types::energy::ScheduleReqEntry& _offer, int index, float ampere,
                                    bool allow_less, bool import, int number_of_phases) {
    // make this more readable
    auto& max_current = _offer.limits_to_root.ac_max_current_A;
    auto& total_power = _offer.limits_to_root.total_power_W;

    if (!max_current.has_value()) {
        // no ampere limit set, cannot do anything here.
        EVLOG_error << "[FAIL] called buy_ampere with only watt limit available.";
        return false;
    }

    // enough ampere available?
    if (max_current.value() >= ampere) {

        // do we have an additional watt limit?
        if (total_power.has_value()) {
            // is the watt limit high enough?
            if (total_power.value() >= ampere * number_of_phases * local_market.nominal_ac_voltage()) {
                // yes, buy both ampere and watt
                // EVLOG_info << "[OK] buy amps and total power is big enough for trade of " << a << "A /"
                //           << a * number_of_phases * local_market.nominal_ac_voltage();
                buy_ampere_unchecked(index, (import ? +1 : -1) * ampere, number_of_phases);
                buy_watt_unchecked(index,
                                   (import ? +1 : -1) * ampere * number_of_phases * local_market.nominal_ac_voltage());
                return true;
            }
        } else {
            // no additional watt limit, let's just buy the ampere value
            // EVLOG_info << "[OK] total power is not set, buying amps only " << a;
            buy_ampere_unchecked(index, (import ? +1 : -1) * ampere, number_of_phases);
            return true;
        }
    }

    // we are still here, so we were not successfull in buying what we wanted.
    // should we try to buy the leftovers?

    if (allow_less && max_current.value() > 0.) {

        // we have an additional watt limit
        if (total_power.has_value()) {
            if (total_power.value() > 0) {
                // is the watt limit high enough?
                if (total_power.value() >= max_current.value() * number_of_phases * local_market.nominal_ac_voltage()) {
                    // yes, buy both ampere and watt
                    // EVLOG_info << "[OK leftovers] total power is big enough for trade of "
                    //           << a * number_of_phases * local_market.nominal_ac_voltage();
                    buy_ampere_unchecked(index, (import ? +1 : -1) * max_current.value(), number_of_phases);
                    buy_watt_unchecked(index, (import ? +1 : -1) * max_current.value() * number_of_phases *
                                                  local_market.nominal_ac_voltage());
                    return true;
                } else {
                    // watt limit is lower, try to reduce ampere
                    float reduced_ampere = total_power.value() / number_of_phases / local_market.nominal_ac_voltage();
                    // EVLOG_info << "[OK leftovers] total power is not big enough, buy reduced current " <<
                    // reduced_ampere
                    //            << reduced_ampere * number_of_phases * local_market.nominal_ac_voltage();
                    buy_ampere_unchecked(index, (import ? +1 : -1) * reduced_ampere, number_of_phases);
                    buy_watt_unchecked(index, (import ? +1 : -1) * reduced_ampere * number_of_phases *
                                                  local_market.nominal_ac_voltage());
                    return true;
                }
            } else {
                // Don't buy anything if the total power limit is 0
                return false;
            }
        } else {
            buy_ampere_unchecked(index, (import ? +1 : -1) * max_current.value(), number_of_phases);
            return true;
        }
    }

    return false;
}

bool BrokerFastCharging::buy_watt(const types::energy::ScheduleReqEntry& _offer, int index, float watt, bool allow_less,
                                  bool import) {
    // make this more readable
    auto& total_power = _offer.limits_to_root.total_power_W;

    if (!total_power.has_value()) {
        // no watt limit set, cannot do anything here.
        EVLOG_error << "[FAIL] called buy watt with no watt limit available.";
        return false;
    }

    // enough watt available?
    if (total_power.value() >= watt) {
        // EVLOG_info << "[OK] enough power available, buying " << a;
        buy_ampere_unchecked(index, (import ? +1 : -1) * watt, _offer.limits_to_root.ac_max_phase_count.value_or(1));
        return true;
    }

    // we are still here, so we were not successfull in buying what we wanted.
    // should we try to buy the leftovers?

    if (allow_less && total_power.value() > 0.) {
        // EVLOG_info << "[OK] buying leftovers " << total_power.value();
        buy_ampere_unchecked(index, (import ? +1 : -1) * total_power.value(),
                             _offer.limits_to_root.ac_max_phase_count.value_or(1));
        return true;
    }

    return false;
}

void BrokerFastCharging::buy_ampere_unchecked(int index, float ampere, int number_of_phases) {
    trading[index].limits_to_root.ac_max_current_A = ampere;
    trading[index].limits_to_root.ac_max_phase_count = number_of_phases;
    traded = true;
    first_trade[index] = false;
}

void BrokerFastCharging::buy_watt_unchecked(int index, float watt) {
    trading[index].limits_to_root.total_power_W = watt;
    traded = true;
}

} // namespace module
