// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest

#include "energyImpl.hpp"
#include <chrono>
#include <date/date.h>
#include <date/tz.h>
#include <utils/date.hpp>

namespace module {
namespace energy_grid {

void energyImpl::init() {

    // UUID must be unique also beyond this charging station -> will be handled on framework level and above later
    energy_flow_request.uuid = mod->info.id;
    energy_flow_request.node_type = types::energy::NodeType::Generic;

    const auto local_schedule = get_local_schedule();
    // Initialize with sane defaults
    energy_flow_request.schedule_import.emplace(std::vector<types::energy::ScheduleReqEntry>({local_schedule}));
    energy_flow_request.schedule_export.emplace(std::vector<types::energy::ScheduleReqEntry>({local_schedule}));

    for (auto& entry : mod->r_energy_consumer) {
        entry->subscribe_energy_flow_request([this](types::energy::EnergyFlowRequest e) {
            // Received new energy_flow_request object from a child. Update in the cached object and republish.
            std::scoped_lock lock(energy_mutex);

            bool child_found = false;
            for (auto& child : energy_flow_request.children) {
                if (child.uuid == e.uuid) {
                    child = e;
                    child_found = true;
                }
            }

            if (!child_found) {
                energy_flow_request.children.push_back(e);
            }

            publish_complete_energy_object();
        });
    }

    if (!mod->r_price_information.empty()) {
        mod->r_powermeter[0]->subscribe_powermeter([this](types::powermeter::Powermeter p) {
            EVLOG_debug << "Incoming powermeter readings: " << p;
            std::scoped_lock lock(energy_mutex);
            energy_flow_request.energy_usage_root = p;
            publish_complete_energy_object();
        });
    }

    if (!mod->r_price_information.empty()) {
        mod->r_price_information[0]->subscribe_energy_pricing(
            [this](types::energy_price_information::EnergyPriceSchedule p) {
                EVLOG_debug << "Incoming price schedule: " << p;
                std::scoped_lock lock(energy_mutex);
                energy_pricing = p;
                publish_complete_energy_object();
            });
    }
}

types::energy::ScheduleReqEntry energyImpl::get_local_schedule() {
    // local schedule of this module
    types::energy::ScheduleReqEntry local_schedule;
    auto tp = date::utc_clock::now();

    local_schedule.timestamp =
        Everest::Date::to_rfc3339(date::floor<std::chrono::hours>(tp) + date::get_leap_second_info(tp).elapsed);
    local_schedule.limits_to_root.ac_max_phase_count = mod->config.phase_count;
    local_schedule.limits_to_root.ac_max_current_A = mod->config.fuse_limit_A;
    local_schedule.limits_to_leaves.ac_max_phase_count = mod->config.phase_count;
    local_schedule.limits_to_leaves.ac_max_current_A = mod->config.fuse_limit_A;

    return local_schedule;
}

void energyImpl::set_external_limits(types::energy::ExternalLimits& l) {
    std::scoped_lock lock(energy_mutex);

    if (l.schedule_import.has_value()) {
        energy_flow_request.schedule_import = l.schedule_import;
        if (!energy_flow_request.schedule_import.value().empty()) {
            // add limits from our own fuse settings
            for (auto& e : energy_flow_request.schedule_import.value()) {
                if (!e.limits_to_root.ac_max_current_A.has_value() ||
                    e.limits_to_root.ac_max_current_A.value() > mod->config.fuse_limit_A)
                    e.limits_to_root.ac_max_current_A = mod->config.fuse_limit_A;

                if (!e.limits_to_root.ac_max_phase_count.has_value() ||
                    e.limits_to_root.ac_max_phase_count.value() > mod->config.phase_count)
                    e.limits_to_root.ac_max_phase_count = mod->config.phase_count;
            }
        } else {
            const auto local_schedule = get_local_schedule();
            // Initialize with sane defaults
            energy_flow_request.schedule_import.emplace(std::vector<types::energy::ScheduleReqEntry>({local_schedule}));
        }
    }

    if (l.schedule_export.has_value()) {

        energy_flow_request.schedule_export = l.schedule_export;

        if (!energy_flow_request.schedule_export.value().empty()) {
            // add limits from our own fuse settings
            for (auto& e : energy_flow_request.schedule_export.value()) {
                if (!e.limits_to_root.ac_max_current_A.has_value() ||
                    e.limits_to_root.ac_max_current_A.value() > mod->config.fuse_limit_A)
                    e.limits_to_root.ac_max_current_A = mod->config.fuse_limit_A;

                if (!e.limits_to_root.ac_max_phase_count.has_value() ||
                    e.limits_to_root.ac_max_phase_count.value() > mod->config.phase_count)
                    e.limits_to_root.ac_max_phase_count = mod->config.phase_count;
            }
        } else {
            const auto local_schedule = get_local_schedule();
            // Initialize with sane defaults
            energy_flow_request.schedule_export.emplace(std::vector<types::energy::ScheduleReqEntry>({local_schedule}));
        }
    }
}

void energyImpl::publish_complete_energy_object() {
    // join the different schedules to the complete array (with resampling)
    types::energy::EnergyFlowRequest energy_complete = energy_flow_request;

    if (energy_flow_request.schedule_import.has_value() && energy_pricing.schedule_import.has_value()) {
        merge_price_into_schedule(energy_complete.schedule_import.value(), energy_pricing.schedule_import.value());
    }

    if (energy_flow_request.schedule_export.has_value() && energy_pricing.schedule_export.has_value()) {
        merge_price_into_schedule(energy_complete.schedule_export.value(), energy_pricing.schedule_export.value());
    }

    publish_energy_flow_request(energy_complete);
}

void energyImpl::merge_price_into_schedule(std::vector<types::energy::ScheduleReqEntry>& schedule,
                                           const std::vector<types::energy_price_information::PricePerkWh>& price) {
    auto it_schedule = schedule.begin();
    auto it_price = price.begin();

    std::vector<types::energy::ScheduleReqEntry> joined_schedule;

    // The first element is already valid now even if the timestamp is in the future (per agreement)
    auto next_entry_schedule = *it_schedule;
    auto next_entry_price = *it_price;
    auto currently_valid_entry_schedule = next_entry_schedule;
    auto currently_valid_entry_price = next_entry_price;

    while (it_schedule != schedule.end() && it_price != price.end()) {
        auto tp_schedule = Everest::Date::from_rfc3339(next_entry_schedule.timestamp);
        auto tp_price = Everest::Date::from_rfc3339(next_entry_price.timestamp);

        if ((tp_schedule < tp_price && it_schedule != schedule.end()) || it_price == price.end()) {
            currently_valid_entry_schedule = next_entry_schedule;
            auto joined_entry = currently_valid_entry_schedule;

            joined_entry.price_per_kwh = currently_valid_entry_price;
            joined_schedule.push_back(joined_entry);
            it_schedule++;
            if (it_schedule != schedule.end()) {
                next_entry_schedule = *it_schedule;
            }
            continue;
        }

        if ((tp_price < tp_schedule && it_price != price.end()) || it_schedule == schedule.end()) {
            currently_valid_entry_price = next_entry_price;
            auto joined_entry = currently_valid_entry_schedule;
            joined_entry.price_per_kwh = currently_valid_entry_price;
            joined_entry.timestamp = currently_valid_entry_price.timestamp;
            joined_schedule.push_back(joined_entry);
            it_price++;
            if (it_price != price.end()) {
                next_entry_price = *it_price;
            }
            continue;
        }
    }
}

void energyImpl::ready() {
    // publish own limits at least once
    publish_energy_flow_request(energy_flow_request);
    mod->signalExternalLimit.connect([this](types::energy::ExternalLimits& l) { set_external_limits(l); });
}

void energyImpl::handle_enforce_limits(types::energy::EnforcedLimits& value) {

    // is it for me?
    if (value.uuid == energy_flow_request.uuid) {
        // as a generic node we cannot do much about limits, we just publish it for e.g. OCPP module.
        mod->p_external_limits->publish_enforced_limits(value);
    }
    // if not, route to children
    // FIXME: this sends it to all children, we could do a lookup on which branch it actually is
    else {
        for (auto& entry : mod->r_energy_consumer) {
            entry->call_enforce_limits(value);
        }
    }
};

} // namespace energy_grid
} // namespace module
