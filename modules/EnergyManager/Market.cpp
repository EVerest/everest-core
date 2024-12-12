// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "Market.hpp"
#include <everest/logging.hpp>
#include <fmt/core.h>

namespace module {

globals_t globals;

void globals_t::init(date::utc_clock::time_point _start_time, int _interval_duration, int _schedule_duration,
                     float _slice_ampere, float _slice_watt, bool _debug,
                     const types::energy::EnergyFlowRequest& energy_flow_request) {
    start_time = _start_time;
    interval_duration = std::chrono::minutes(_interval_duration);
    schedule_length = std::chrono::hours(_schedule_duration) / interval_duration;
    slice_ampere = _slice_ampere;
    slice_watt = _slice_watt;
    debug = _debug;

    create_timestamps(energy_flow_request);

    create_empty_schedule(zero_schedule_req);

    for (auto& a : zero_schedule_req) {
        a.limits_to_root.ac_max_current_A = {0.};
        a.limits_to_root.total_power_W = {0.};
    }

    create_empty_schedule(empty_schedule_req);

    create_empty_schedule(zero_schedule_res);

    for (auto& a : zero_schedule_res) {
        a.limits_to_root.ac_max_current_A = {0.};
        a.limits_to_root.total_power_W = {0.};
    }

    create_empty_schedule(empty_schedule_res);

    create_empty_schedule(empty_schedule_setpoints);
}

void globals_t::create_timestamps(const types::energy::EnergyFlowRequest& energy_flow_request) {

    timestamps.clear();
    timestamps.reserve(schedule_length);

    auto minutes_overflow = start_time.time_since_epoch() % interval_duration;
    auto start = start_time - minutes_overflow;

    // Add leap seconds
    date::get_leap_second_info(start_time);
    auto timepoint = start + date::get_leap_second_info(start_time).elapsed;

    // Insert all our pre defined time slots
    for (int i = 0; i < schedule_length; i++) {
        timestamps.push_back(timepoint);
        timepoint += interval_duration;
    }

    // Insert timestamps of all requests
    add_timestamps(energy_flow_request);

    // sort
    std::sort(timestamps.begin(), timestamps.end());

    // remove duplicates
    timestamps.erase(unique(timestamps.begin(), timestamps.end()), timestamps.end());

    schedule_length = timestamps.size();
}

void globals_t::add_timestamps(const types::energy::EnergyFlowRequest& energy_flow_request) {
    // add local timestamps
    for (auto t : energy_flow_request.schedule_import) {
        // insert current timestamp
        timestamps.push_back(Everest::Date::from_rfc3339(t.timestamp));
    }

    for (auto t : energy_flow_request.schedule_export) {
        // insert current timestamp
        timestamps.push_back(Everest::Date::from_rfc3339(t.timestamp));
    }

    for (auto t : energy_flow_request.schedule_setpoints) {
        // insert current timestamp
        timestamps.push_back(Everest::Date::from_rfc3339(t.timestamp));
    }

    // recurse to all children
    for (auto& c : energy_flow_request.children)
        add_timestamps(c);
}

template <typename T> void globals_t::create_empty_schedule(T& s) {
    // initialize schedule with correct size
    typename T::value_type e;
    s = T(schedule_length, e);

    for (int i = 0; i < schedule_length; i++) {
        s[i].timestamp = Everest::Date::to_rfc3339(timestamps[i]);
    }
}

int time_probe::stop() {
    if (running) {
        pause();
    }
    return std::chrono::duration_cast<std::chrono::milliseconds>(total_duration).count();
}

void time_probe::start() {
    timepoint_start = std::chrono::high_resolution_clock::now();
    running = true;
}

void time_probe::pause() {
    if (running) {
        total_duration += std::chrono::high_resolution_clock::now() - timepoint_start;
        running = false;
    }
}

// returns the smaller of two optionals. Note that comparison operators on optionals are a little weird if not both
// sides have a value, we explicitly want:
// - If both are not set, it should return an empty optional
// - If either a or b is set but not both, return the one set.
// - If both have a value, return the smaller one.
template <typename T> std::optional<T> min_optional(std::optional<T> a, std::optional<T> b) {

    if (a.has_value() and b.has_value()) {
        if (a.value().value < b.value().value) {
            return a;
        } else {
            return b;
        }
    }

    if (a.has_value()) {
        return a;
    }

    return b;
}

template <typename T> std::optional<T> max_optional(std::optional<T> a, std::optional<T> b) {

    if (a.has_value() and b.has_value()) {
        if (a.value().value > b.value().value) {
            return a;
        } else {
            return b;
        }
    }

    if (a.has_value()) {
        return a;
    }

    return b;
}

ScheduleSetpoints Market::resample(const ScheduleSetpoints& request) {

    ScheduleSetpoints sp = globals.empty_schedule_setpoints;

    // First resample request to the timestamps in available and merge all limits on root sides
    for (auto& s : sp) {

        // find corresponding entry in request
        auto r = request.begin();
        auto tp_a = Everest::Date::from_rfc3339(s.timestamp);
        for (auto ir = request.begin(); ir != request.end(); ir++) {
            auto tp_r_1 = Everest::Date::from_rfc3339((*ir).timestamp);
            if ((ir + 1 == request.end())) {
                r = ir;
                break;
            }
            auto tp_r_2 = Everest::Date::from_rfc3339((*(ir + 1)).timestamp);
            if ((tp_a >= tp_r_1 && tp_a < tp_r_2) || (ir == request.begin() && tp_a < tp_r_1)) {
                r = ir;
                break;
            }
        }

        if (r != request.end()) {
            // copy setpoint if any
            s.setpoint = (*r).setpoint;
        }
    }

    return sp;
}

ScheduleReq Market::get_max_available_energy(const ScheduleReq& request) {

    ScheduleReq available = globals.empty_schedule_req;

    // First resample request to the timestamps in available and merge all limits on root sides
    for (auto& a : available) {

        // find corresponding entry in request
        auto r = request.begin();
        auto tp_a = Everest::Date::from_rfc3339(a.timestamp);
        for (auto ir = request.begin(); ir != request.end(); ir++) {
            auto tp_r_1 = Everest::Date::from_rfc3339((*ir).timestamp);
            if ((ir + 1 == request.end())) {
                r = ir;
                break;
            }
            auto tp_r_2 = Everest::Date::from_rfc3339((*(ir + 1)).timestamp);
            if ((tp_a >= tp_r_1 && tp_a < tp_r_2) || (ir == request.begin() && tp_a < tp_r_1)) {
                r = ir;
                break;
            }
        }

        if (r != request.end()) {

            {
                auto leaves_power_W = (*r).limits_to_leaves.total_power_W;
                if (leaves_power_W.has_value()) {
                    leaves_power_W.value().value =
                        leaves_power_W.value().value / (*r).conversion_efficiency.value_or(1.);
                }

                a.limits_to_root.total_power_W = min_optional(leaves_power_W, (*r).limits_to_root.total_power_W);
            }

            a.limits_to_root.ac_max_current_A =
                min_optional((*r).limits_to_leaves.ac_max_current_A, (*r).limits_to_root.ac_max_current_A);

            a.limits_to_root.ac_min_phase_count =
                max_optional((*r).limits_to_root.ac_min_phase_count, (*r).limits_to_leaves.ac_min_phase_count);

            a.limits_to_root.ac_max_phase_count =
                min_optional((*r).limits_to_root.ac_max_phase_count, (*r).limits_to_leaves.ac_max_phase_count);

            a.limits_to_root.ac_min_current_A =
                max_optional((*r).limits_to_root.ac_min_current_A, (*r).limits_to_leaves.ac_min_current_A);

            // all request limits have been merged on root side in available.
            // copy other information if any
            a.price_per_kwh = (*r).price_per_kwh;
            a.limits_to_root.ac_number_of_active_phases = (*r).limits_to_root.ac_number_of_active_phases;
        }
    }

    return available;
}

ScheduleReq Market::get_available_energy(const ScheduleReq& max_available, bool add_sold) {
    ScheduleReq available = max_available;
    for (ScheduleReq::size_type i = 0; i < available.size(); i++) {
        // FIXME: sold_root is the sum of all energy sold, but we need to limit indivdual paths as well
        // add config option for pure star type of cabling here as well.

        float sold_current = 0;

        if (sold_root[i].limits_to_root.ac_max_current_A.has_value()) {
            sold_current = (add_sold ? 1 : -1) * sold_root[i].limits_to_root.ac_max_current_A.value().value;
        }

        if (sold_current > 0)
            sold_current = 0;

        float sold_watt = 0;

        if (sold_root[i].limits_to_root.total_power_W.has_value()) {
            sold_watt = (add_sold ? 1 : -1) * sold_root[i].limits_to_root.total_power_W.value().value;
        }

        if (sold_watt > 0)
            sold_watt = 0;

        if (available[i].limits_to_root.ac_max_current_A.has_value())
            available[i].limits_to_root.ac_max_current_A.value().value += sold_current;

        if (available[i].limits_to_root.total_power_W.has_value())
            available[i].limits_to_root.total_power_W.value().value += sold_watt;
    }
    return available;
}

ScheduleReq Market::get_available_energy_import() {
    return get_available_energy(import_max_available, false);
}

ScheduleReq Market::get_available_energy_export() {
    return get_available_energy(export_max_available, true);
}

float get_watt_from_freq_table(const std::vector<types::energy::FrequencyWattPoint> table, float freq) {
    // the table has to be sorted by freqency

    if (table.size() == 0) {
        return 0.;
    }

    if (table.size() == 1) {
        return table[0].total_power_W;
    }

    float watt1 = table[0].total_power_W;
    float watt2 = 0.;
    float freq1 = 0.;

    for (const auto e : table) {
        watt2 = e.total_power_W;
        if (e.frequency_Hz > freq) {
            break;
        }
        watt1 = e.total_power_W;
        freq1 = e.frequency_Hz;
    }
    return watt1 + (freq - freq1) * (watt2 - watt1);
}

void apply_limit_if_smaller(std::optional<types::energy::NumberWithSource>& base, float limit,
                            const std::string& source) {
    if (not base.has_value() or (base.has_value() and base.value().value > limit)) {
        base = {limit, source};
    }
}

void apply_setpoints(ScheduleReq& imp, ScheduleReq& exp, const ScheduleSetpoints& setpoints,
                     std::optional<float> freq) {
    if (setpoints.size() != imp.size()) {
        EVLOG_error << fmt::format("apply_setpoints: setpoints({}) and import({}) do not have the same size.",
                                   setpoints.size(), imp.size());
        return;
    }
    if (setpoints.size() != exp.size()) {
        EVLOG_error << fmt::format("apply_setpoints: setpoints({}) and export({}) do not have the same size.",
                                   setpoints.size(), exp.size());
        return;
    }

    for (ScheduleReq::size_type i = 0; i < setpoints.size(); i++) {
        // apply setpoints as limits
        if (setpoints[i].setpoint.has_value()) {
            const auto& sp = setpoints[i].setpoint.value();
            auto& imp_limits = imp[i].limits_to_root;
            auto& exp_limits = exp[i].limits_to_root;

            // Allow only one actual setpoint value to be set, in this priority order
            if (sp.ac_current_A.has_value()) {
                if (sp.ac_current_A.value() >= 0.) {
                    // Charging setpoint
                    apply_limit_if_smaller(imp_limits.ac_max_current_A, sp.ac_current_A.value(), sp.source);
                    exp_limits.ac_max_current_A = {0., sp.source};
                } else {
                    // Discharging setpoint
                    apply_limit_if_smaller(exp_limits.ac_max_current_A, -sp.ac_current_A.value(), sp.source);
                    imp_limits.ac_max_current_A = {0., sp.source};
                }
            } else if (sp.total_power_W.has_value()) {
                if (sp.total_power_W.value() >= 0.) {
                    // Charging setpoint
                    apply_limit_if_smaller(imp_limits.total_power_W, sp.total_power_W.value(), sp.source);
                    exp_limits.total_power_W = {0., sp.source};
                } else {
                    // Discharging setpoint
                    apply_limit_if_smaller(exp_limits.total_power_W, -sp.total_power_W.value(), sp.source);
                    imp_limits.total_power_W = {0., sp.source};
                }

            } else if (sp.frequency_table.has_value() and freq.has_value()) {
                // get actual watt limit from table and current frequency from meter
                float watt_limit = get_watt_from_freq_table(sp.frequency_table.value(), freq.value());
                if (watt_limit >= 0.) {
                    // Charging setpoint
                    apply_limit_if_smaller(imp_limits.total_power_W, watt_limit, sp.source);
                    exp_limits.total_power_W = {0., sp.source};
                } else {
                    // Discharging setpoint
                    apply_limit_if_smaller(exp_limits.total_power_W, -watt_limit, sp.source);
                    imp_limits.total_power_W = {0., sp.source};
                }
            }
        }
    }
}

Market::Market(types::energy::EnergyFlowRequest& _energy_flow_request, const float __nominal_ac_voltage,
               Market* __parent) :
    energy_flow_request(_energy_flow_request), _parent(__parent), _nominal_ac_voltage(__nominal_ac_voltage) {

    // EVLOG_info << "Create market for " << _energy_flow_request.uuid;

    sold_root = globals.empty_schedule_res;

    if (not energy_flow_request.schedule_import.empty()) {
        import_max_available = get_max_available_energy(energy_flow_request.schedule_import);
    } else {
        // nothing is available as nothing was requested
        import_max_available = globals.zero_schedule_req;
    }

    if (not energy_flow_request.schedule_export.empty()) {
        export_max_available = get_max_available_energy(energy_flow_request.schedule_export);
    } else {
        // nothing is available as nothing was requested
        export_max_available = globals.zero_schedule_req;
    }

    if (not energy_flow_request.schedule_setpoints.empty()) {
        setpoints = resample(energy_flow_request.schedule_setpoints);
    } else {
        // create an empty setpoint schedule
        setpoints = globals.empty_schedule_setpoints;
    }

    // Try to find a frequency measurement
    std::optional<float> freq;
    if (energy_flow_request.energy_usage_root.has_value() and
        energy_flow_request.energy_usage_root.value().frequency_Hz.has_value()) {
        freq = energy_flow_request.energy_usage_root.value().frequency_Hz.value().L1;
    } else if (energy_flow_request.energy_usage_leaves.has_value() and
               energy_flow_request.energy_usage_leaves.value().frequency_Hz.has_value()) {
        freq = energy_flow_request.energy_usage_leaves.value().frequency_Hz.value().L1;
    }

    // Apply setpoints as limit to both import and export schedules
    apply_setpoints(import_max_available, export_max_available, setpoints, freq);

    // Recursion: create one Market for each child
    for (auto& flow_child : _energy_flow_request.children) {
        _children.emplace_back(flow_child, _nominal_ac_voltage, this);
    }
}

ScheduleRes Market::get_sold_energy() {
    return sold_root;
}

Market* Market::parent() {
    return _parent;
}

bool Market::is_root() {
    return _parent == nullptr;
}

void Market::get_list_of_evses(std::vector<Market*>& list) {
    if (energy_flow_request.node_type == types::energy::NodeType::Evse) {
        list.push_back(this);
    }

    for (auto& child : _children) {
        child.get_list_of_evses(list);
    }
}

std::vector<Market*> Market::get_list_of_evses() {
    std::vector<Market*> list;
    if (energy_flow_request.node_type == types::energy::NodeType::Evse) {
        list.push_back(this);
    }

    for (auto& child : _children) {
        child.get_list_of_evses(list);
    }
    return list;
}

static void schedule_add(ScheduleRes& a, const ScheduleRes& b) {
    if (a.size() != b.size()) {
        EVLOG_critical << "schedule_add: Schedules are not of the same size: a: " << a.size() << " b: " << b.size();
        return;
    }

    const types::energy::NumberWithSource NUMZERO = {0};

    for (ScheduleRes::size_type i = 0; i < a.size(); i++) {
        if (b[i].limits_to_root.ac_max_current_A.has_value()) {
            std::string source;

            if (b[i].limits_to_root.ac_max_current_A.value().value not_eq 0.) {
                source = b[i].limits_to_root.ac_max_current_A.value().source;
            } else if (a[i].limits_to_root.ac_max_current_A.has_value()) {
                source = a[i].limits_to_root.ac_max_current_A.value().source;
            }

            a[i].limits_to_root.ac_max_current_A = {b[i].limits_to_root.ac_max_current_A.value().value +
                                                        a[i].limits_to_root.ac_max_current_A.value_or(NUMZERO).value,
                                                    source};
        }

        if (b[i].limits_to_root.total_power_W.has_value()) {
            std::string source;

            if (b[i].limits_to_root.total_power_W.value().value not_eq 0.) {
                source = b[i].limits_to_root.total_power_W.value().source;
            } else if (a[i].limits_to_root.total_power_W.has_value()) {
                source = a[i].limits_to_root.total_power_W.value().source;
            }

            a[i].limits_to_root.total_power_W = {b[i].limits_to_root.total_power_W.value().value +
                                                     a[i].limits_to_root.total_power_W.value_or(NUMZERO).value,
                                                 source};
        }

        if (b[i].limits_to_root.ac_max_phase_count.has_value()) {
            if (a[i].limits_to_root.ac_max_phase_count.has_value()) {
                if (b[i].limits_to_root.ac_max_phase_count.value().value >
                    a[i].limits_to_root.ac_max_phase_count.value().value) {
                    a[i].limits_to_root.ac_max_phase_count = b[i].limits_to_root.ac_max_phase_count;
                }
            } else {
                a[i].limits_to_root.ac_max_phase_count = b[i].limits_to_root.ac_max_phase_count;
            }
        }
    }
}

void Market::trade(const ScheduleRes& traded) {
    schedule_add(sold_root, traded);

    // propagate to root
    if (!is_root()) {
        parent()->trade(traded);
    }
}

float Market::nominal_ac_voltage() {
    return _nominal_ac_voltage;
}

} // namespace module
