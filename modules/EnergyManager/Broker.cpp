// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "Broker.hpp"
#include <everest/logging.hpp>
#include <fmt/core.h>

namespace module {

Broker::Broker(Market& _market, BrokerContext& _context, EnergyManagerConfig _config) :
    local_market(_market),
    context(_context),
    config(_config),
    first_trade(globals.schedule_length, true),
    slot_type(globals.schedule_length, SlotType::Undecided),
    num_phases(globals.schedule_length, {0, "empty"}) {
}

Market& Broker::get_local_market() {
    return local_market;
}

bool Broker::trade(Offer& _offer) {
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
            trading[i].limits_to_root.ac_max_current_A = {0.};
        }
        if (total_power.has_value()) {
            trading[i].limits_to_root.total_power_W = {0.};
        }
    }

    traded = false;

    if (offer->import_offer.size() != offer->export_offer.size()) {
        EVLOG_error << "import and export offer do not have the same size!";
        return false;
    }

    // Run the actual broker implementation, depending on strategy
    tradeImpl();

    // if we want to buy anything:
    if (traded) {
        if (globals.debug) {
            EVLOG_info << fmt::format("\033[1;33m                                {}A {}W \033[1;0m",
                                      (trading[0].limits_to_root.ac_max_current_A.has_value()
                                           ? std::to_string(trading[0].limits_to_root.ac_max_current_A.value().value)
                                           : " [NOT_SET] "),
                                      (trading[0].limits_to_root.total_power_W.has_value()
                                           ? std::to_string(trading[0].limits_to_root.total_power_W.value().value)
                                           : " [NOT_SET] "));
        }
        //   execute the trade on the market
        local_market.trade(trading);

        // clear the source as we are not done yet (or started trading again after a no trade)

        return true;
    } else {
        if (globals.debug) {
            EVLOG_info << fmt::format("\033[1;33m                               NO TRADE \033[1;0m");
        }

        //   execute the zero trade on the market
        local_market.trade(trading);

        // If no trade happens for the first time after successful tradings, set the source. If trading happens again,
        // clear the source again. If this is a second call to no trade, do not update source.
        return false;
    }
}

date::utc_clock::time_point Broker::to_timestamp(const types::energy::ScheduleReqEntry& entry) {
    return Everest::Date::from_rfc3339(entry.timestamp);
}

bool Broker::time_slot_active(const int i, const ScheduleReq& offer) {
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

bool Broker::buy_ampere_import(int index, float ampere, bool allow_less,
                               types::energy::IntegerWithSource number_of_phases) {
    return buy_ampere(offer->import_offer[index], index, ampere, allow_less, true, number_of_phases);
}

bool Broker::buy_ampere_export(int index, float ampere, bool allow_less,
                               types::energy::IntegerWithSource number_of_phases) {
    return buy_ampere(offer->export_offer[index], index, ampere, allow_less, false, number_of_phases);
}

bool Broker::buy_watt_import(int index, float watt, bool allow_less) {
    return buy_watt(offer->import_offer[index], index, watt, allow_less, true);
}

bool Broker::buy_watt_export(int index, float watt, bool allow_less) {
    return buy_watt(offer->export_offer[index], index, watt, allow_less, false);
}

bool Broker::buy_ampere(const types::energy::ScheduleReqEntry& _offer, int index, float ampere, bool allow_less,
                        bool import, types::energy::IntegerWithSource number_of_phases) {
    // make this more readable
    auto& max_current = _offer.limits_to_root.ac_max_current_A;
    auto& total_power = _offer.limits_to_root.total_power_W;

    if (!max_current.has_value()) {
        // no ampere limit set, cannot do anything here.
        EVLOG_error << "[FAIL] called buy_ampere with only watt limit available.";
        return false;
    }

    // enough ampere available?
    if (max_current.value().value >= ampere) {

        // do we have an additional watt limit?
        if (total_power.has_value()) {
            // is the watt limit high enough?
            if (total_power.value().value >= ampere * number_of_phases.value * local_market.nominal_ac_voltage()) {
                // yes, buy both ampere and watt
                // EVLOG_info << "[OK] buy amps and total power is big enough for trade of " << a << "A /"
                //           << a * number_of_phases * local_market.nominal_ac_voltage();
                buy_ampere_unchecked(index, {(import ? +1 : -1) * ampere, max_current.value().source},
                                     number_of_phases);
                // It was not actually limited by the watt limit, set the source from ampere limit
                buy_watt_unchecked(
                    index, {(import ? +1 : -1) * ampere * number_of_phases.value * local_market.nominal_ac_voltage(),
                            max_current.value().source});
                return true;
            }
        } else {
            // no additional watt limit, let's just buy the ampere value
            // EVLOG_info << "[OK] total power is not set, buying amps only " << a;
            buy_ampere_unchecked(index, {(import ? +1 : -1) * ampere, max_current.value().source}, number_of_phases);
            return true;
        }
    }

    // we are still here, so we were not successfull in buying what we wanted.
    // should we try to buy the leftovers?

    if (allow_less && max_current.value().value > 0.) {

        // we have an additional watt limit
        if (total_power.has_value()) {
            if (total_power.value().value > 0) {
                // is the watt limit high enough?
                if (total_power.value().value >=
                    max_current.value().value * number_of_phases.value * local_market.nominal_ac_voltage()) {
                    // yes, buy both ampere and watt
                    // EVLOG_info << "[OK leftovers] total power is big enough for trade of "
                    //           << a * number_of_phases * local_market.nominal_ac_voltage();
                    buy_ampere_unchecked(index,
                                         {(import ? +1 : -1) * max_current.value().value, max_current.value().source},
                                         number_of_phases);
                    // It was not actually limited by the watt limit, set the source from ampere limit
                    buy_watt_unchecked(index, {(import ? +1 : -1) * max_current.value().value * number_of_phases.value *
                                                   local_market.nominal_ac_voltage(),
                                               max_current.value().source});
                    return true;
                } else {
                    // watt limit is lower, try to reduce ampere
                    float reduced_ampere =
                        total_power.value().value / number_of_phases.value / local_market.nominal_ac_voltage();
                    // EVLOG_info << "[OK leftovers] total power is not big enough, buy reduced current " <<
                    // reduced_ampere
                    //            << reduced_ampere * number_of_phases * local_market.nominal_ac_voltage();
                    // Actually limited by watt limit, so use the watt source
                    buy_ampere_unchecked(index, {(import ? +1 : -1) * reduced_ampere, total_power.value().source},
                                         number_of_phases);
                    buy_watt_unchecked(index, {(import ? +1 : -1) * reduced_ampere * number_of_phases.value *
                                                   local_market.nominal_ac_voltage(),
                                               total_power.value().source});
                    return true;
                }
            } else {
                // Don't buy anything if the total power limit is 0
                return false;
            }
        } else {
            buy_ampere_unchecked(index, {(import ? +1 : -1) * max_current.value().value, max_current.value().source},
                                 number_of_phases);
            return true;
        }
    }

    return false;
}

bool Broker::buy_watt(const types::energy::ScheduleReqEntry& _offer, int index, float watt, bool allow_less,
                      bool import) {
    // make this more readable
    auto& total_power = _offer.limits_to_root.total_power_W;

    if (!total_power.has_value()) {
        // no watt limit set, cannot do anything here.
        EVLOG_error << "[FAIL] called buy watt with no watt limit available.";
        return false;
    }

    // enough watt available?
    if (total_power.value().value >= watt) {
        // EVLOG_info << "[OK] enough power available, buying " << a;
        buy_watt_unchecked(index, {(import ? +1 : -1) * watt, total_power.value().source});
        return true;
    }

    // we are still here, so we were not successfull in buying what we wanted.
    // should we try to buy the leftovers?

    if (allow_less && total_power.value().value > 0.) {
        // EVLOG_info << "[OK] buying leftovers " << total_power.value();
        buy_watt_unchecked(index, {(import ? +1 : -1) * total_power.value().value, total_power.value().source});
        return true;
    }

    return false;
}

void Broker::buy_ampere_unchecked(int index, types::energy::NumberWithSource ampere,
                                  types::energy::IntegerWithSource number_of_phases) {
    trading[index].limits_to_root.ac_max_current_A = ampere;
    trading[index].limits_to_root.ac_max_phase_count = number_of_phases;
    traded = true;
    first_trade[index] = false;
}

void Broker::buy_watt_unchecked(int index, types::energy::NumberWithSource watt) {
    trading[index].limits_to_root.total_power_W = watt;
    traded = true;
}

} // namespace module
