// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest

#include "Market.hpp"
#include <everest/logging.hpp>
#include <fmt/core.h>

namespace module {

globals_t globals;

void globals_t::init(date::utc_clock::time_point start_time, int _interval_duration, int _schedule_duration,
                     float _slice_ampere, float _slice_watt, bool _debug,
                     const types::energy::EnergyFlowRequest& energy_flow_request) {
    interval_duration = std::chrono::minutes(_interval_duration);
    schedule_length = std::chrono::hours(_schedule_duration) / interval_duration;
    slice_ampere = _slice_ampere;
    slice_watt = _slice_watt;
    debug = _debug;

    create_timestamps(start_time, energy_flow_request);

    zero_schedule_req = create_empty_schedule_req();

    for (auto& a : zero_schedule_req) {
        a.limits_to_root.ac_max_current_A = 0.;
        a.limits_to_root.total_power_W = 0.;
    }

    empty_schedule_req = create_empty_schedule_req();

    zero_schedule_res = create_empty_schedule_res();

    for (auto& a : zero_schedule_res) {
        a.limits_to_root.ac_max_current_A = 0.;
        a.limits_to_root.total_power_W = 0.;
    }

    empty_schedule_res = create_empty_schedule_res();
}

void globals_t::create_timestamps(const date::utc_clock::time_point& start_time,
                                  const types::energy::EnergyFlowRequest& energy_flow_request) {

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
    if (energy_flow_request.schedule_import.has_value()) {
        for (auto t : energy_flow_request.schedule_import.value()) {
            // insert current timestamp
            timestamps.push_back(Everest::Date::from_rfc3339(t.timestamp));
        }
    }
    if (energy_flow_request.schedule_export.has_value()) {
        for (auto t : energy_flow_request.schedule_export.value()) {
            // insert current timestamp
            timestamps.push_back(Everest::Date::from_rfc3339(t.timestamp));
        }
    }

    // recurse to all children
    for (auto& c : energy_flow_request.children)
        add_timestamps(c);
}

ScheduleReq globals_t::create_empty_schedule_req() {
    // initialize schedule with correct size
    types::energy::ScheduleReqEntry e;
    ScheduleReq s(schedule_length, e);

    for (int i = 0; i < schedule_length; i++) {
        s[i].timestamp = Everest::Date::to_rfc3339(timestamps[i]);
    }

    return s;
}

ScheduleRes globals_t::create_empty_schedule_res() {
    // initialize schedule with correct size
    types::energy::ScheduleResEntry e;
    ScheduleRes s(schedule_length, e);

    for (int i = 0; i < schedule_length; i++) {
        s[i].timestamp = Everest::Date::to_rfc3339(timestamps[i]);
    }

    return s;
}

int time_probe::stop() {
    if (running)
        pause();
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
            if (tp_a >= tp_r_1 && tp_a < tp_r_2 || (ir == request.begin() && tp_a < tp_r_1)) {
                r = ir;
                break;
            }
        }

        if (r != request.end()) {
            // apply watt limit from leaf side to root side
            if ((*r).limits_to_leaves.total_power_W.has_value()) {
                a.limits_to_root.total_power_W =
                    (*r).limits_to_leaves.total_power_W.value() / (*r).conversion_efficiency.value_or(1.);
            }
            // do we have a lower watt limit on root side?
            if ((*r).limits_to_root.total_power_W.has_value() && a.limits_to_root.total_power_W.has_value() &&
                a.limits_to_root.total_power_W.value() > (*r).limits_to_root.total_power_W.value()) {
                a.limits_to_root.total_power_W = (*r).limits_to_root.total_power_W.value();
            }
            // apply ampere limit from leaf side to root side
            if ((*r).limits_to_leaves.ac_max_current_A.has_value()) {
                a.limits_to_root.ac_max_current_A =
                    (*r).limits_to_leaves.ac_max_current_A.value() / (*r).conversion_efficiency.value_or(1.);
            }
            // do we have a lower ampere limit on root side?
            if ((*r).limits_to_root.ac_max_current_A.has_value() and
                (a.limits_to_root.ac_max_current_A > (*r).limits_to_root.ac_max_current_A.value() or
                 not(*r).limits_to_leaves.ac_max_current_A.has_value())) {
                a.limits_to_root.ac_max_current_A = (*r).limits_to_root.ac_max_current_A.value();
            }
            // all request limits have been merged on root side in available.
            // copy pricing information data if any
            a.price_per_kwh = (*r).price_per_kwh;
            a.limits_to_root.ac_min_current_A = (*r).limits_to_root.ac_min_current_A;
            a.limits_to_root.ac_min_phase_count = (*r).limits_to_root.ac_min_phase_count;
            a.limits_to_root.ac_max_phase_count = (*r).limits_to_root.ac_max_phase_count;
        }
    }

    return available;
}

ScheduleReq Market::get_available_energy(const ScheduleReq& max_available, bool add_sold) {
    ScheduleReq available = max_available;
    for (int i = 0; i < available.size(); i++) {
        // FIXME: sold_root is the sum of all energy sold, but we need to limit indivdual paths as well
        // add config option for pure star type of cabling here as well.

        float sold_current = (add_sold ? 1 : -1) * sold_root[i].limits_to_root.ac_max_current_A.value_or(0);
        if (sold_current > 0)
            sold_current = 0;

        float sold_watt = (add_sold ? 1 : -1) * sold_root[i].limits_to_root.total_power_W.value_or(0);
        if (sold_watt > 0)
            sold_watt = 0;

        if (available[i].limits_to_root.ac_max_current_A.has_value())
            available[i].limits_to_root.ac_max_current_A.value() += sold_current;

        if (available[i].limits_to_root.total_power_W.has_value())
            available[i].limits_to_root.total_power_W.value() += sold_watt;
    }
    return available;
}

ScheduleReq Market::get_available_energy_import() {
    return get_available_energy(import_max_available, false);
}

ScheduleReq Market::get_available_energy_export() {
    return get_available_energy(export_max_available, true);
}

Market::Market(types::energy::EnergyFlowRequest& _energy_flow_request, const float __nominal_ac_voltage,
               Market* __parent) :
    _nominal_ac_voltage(__nominal_ac_voltage), _parent(__parent), energy_flow_request(_energy_flow_request) {

    // EVLOG_info << "Create market for " << _energy_flow_request.uuid;

    sold_root = globals.empty_schedule_res;

    if (energy_flow_request.schedule_import.has_value()) {
        import_max_available = get_max_available_energy(energy_flow_request.schedule_import.value());
    } else {
        // nothing is available as nothing was requested
        import_max_available = globals.zero_schedule_req;
    }

    if (energy_flow_request.schedule_export.has_value()) {
        export_max_available = get_max_available_energy(energy_flow_request.schedule_export.value());
    } else {
        // nothing is available as nothing was requested
        export_max_available = globals.zero_schedule_req;
    }

    // Recursion: create one Market for each child
    for (auto& flow_child : _energy_flow_request.children) {
        _children.emplace_back(flow_child, _nominal_ac_voltage, this);
    }
}

const std::vector<Market>& Market::children() {
    return _children;
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
    if (a.size() != b.size())
        return;

    for (int i = 0; i < a.size(); i++) {
        if (b[i].limits_to_root.ac_max_current_A.has_value()) {
            a[i].limits_to_root.ac_max_current_A =
                b[i].limits_to_root.ac_max_current_A.value() + a[i].limits_to_root.ac_max_current_A.value_or(0);
        }

        if (b[i].limits_to_root.total_power_W.has_value()) {
            a[i].limits_to_root.total_power_W =
                b[i].limits_to_root.total_power_W.value() + a[i].limits_to_root.total_power_W.value_or(0);
        }

        if (b[i].limits_to_root.ac_max_phase_count.has_value()) {
            if (a[i].limits_to_root.ac_max_phase_count.has_value()) {
                if (b[i].limits_to_root.ac_max_phase_count.value() > a[i].limits_to_root.ac_max_phase_count.value()) {
                    a[i].limits_to_root.ac_max_phase_count.value() = b[i].limits_to_root.ac_max_phase_count.value();
                }
            } else {
                a[i].limits_to_root.ac_max_phase_count.value() = b[i].limits_to_root.ac_max_phase_count.value();
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
