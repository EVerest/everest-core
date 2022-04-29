// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "EnergyManager.hpp"
#include <chrono>
#include <date/date.h>

namespace module {

#define INVALID_PRICE_PER_KWH (-1.1f)

std::string to_rfc3339(std::chrono::time_point<std::chrono::system_clock> t) {
    return date::format("%FT%TZ", std::chrono::time_point_cast<std::chrono::milliseconds>(t));
}

std::chrono::time_point<std::chrono::system_clock> from_rfc3339(std::string t) {
    std::istringstream infile{t};
    std::chrono::time_point<std::chrono::system_clock> tp;
    infile >> date::parse("%FT%T", tp);

    return tp;
}

void EnergyManager::init() {
    invoke_init(*p_main);
    
    r_energy_trunk->subscribe_energy([this](json e) {
        // Received new energy object from a child.
        {
            std::lock_guard<std::mutex> lock(this->global_energy_object_mutex);
            global_energy_object = e;
        }
    });
}

void EnergyManager::ready() {
    invoke_ready(*p_main);

    interval_start([this](){this->run_enforce_limits();}, ENERGY_MANAGER_OPTIMIZER_INTERVAL_MS);
}

void EnergyManager::interval_start(const std::function<void(void)>& func, unsigned int interval_ms) {
    std::thread([func, interval_ms]() { 
        while (true) { 
            auto next_interval_time = std::chrono::steady_clock::now() + std::chrono::milliseconds(interval_ms);
            func();
            std::this_thread::sleep_until(next_interval_time);
        }
    }).detach();
}

void EnergyManager::run_enforce_limits() {
    try {

        json optimized_values = json::object();
        {
            std::lock_guard<std::mutex> lock(this->global_energy_object_mutex);
            optimized_values = run_optimizer(this->global_energy_object);
        }

        for (auto it = optimized_values.begin(); it != optimized_values.end(); ++it) {
            sanitize_object(*it);
            try {
                this->r_energy_trunk->call_enforce_limits( (*it).at("uuid"), 
                                        (*it).at("limits_import"), 
                                        (*it).at("limits_export"),
                                        (*it).at("schedule_import"),
                                        (*it).at("schedule_export"));
            } catch (const std::exception& e) {
                EVLOG(error) << "Cannot enforce limits: Exception occurred: optimized object faulty: " << e.what();
            }
        }

    } catch (const std::exception& e) {
        EVLOG(error) << "Cannot enforce limits: Exception occurred: r_energy_trunk connection faulty " << e.what();
    }
}

Array EnergyManager::run_optimizer(json energy) {
    // traverse tree, set result limits for each evse node
    json optimized_values = json::array();
    auto timepoint = std::chrono::system_clock::now();
    json price_schedule = json::array();

    try {
        if (energy.contains("schedule_import") && !energy["schedule_import"].is_null() ) {  // need "[]" to prevent nlohmann error 304: cannot use at() with null
            price_schedule = energy.at("schedule_import");
        }
    } catch (const std::exception& e) {
        EVLOG(error) << "run_optimizer: Exception occurred: energy object does not have an import schedule: " << e.what();
    }
    
    try {
        optimize_one_level(energy, optimized_values, timepoint, price_schedule);
    } catch (const std::exception& e) {
        EVLOG(error) << "run_optimizer: Exception occurred: calling optimizer for next level failed: " << e.what();
    }

    return optimized_values;
}

// recursive optimization of one level
void EnergyManager::optimize_one_level(json& energy, json& optimized_values,
                                       const std::chrono::system_clock::time_point timepoint_now,
                                       const json price_schedule) {

    // find max_current limit for this level
    // min of (limit_from_parent, local_limit_from_schedule)
    if (energy.contains("schedule_import") && !energy["schedule_import"].is_null() ) {  // need "[]" to prevent nlohmann error 304: cannot use at() with null
        json grid_import_limit = {};
        try {
            grid_import_limit = get_sub_element_from_schedule_at_time(energy.at("schedule_import"), timepoint_now);
        } catch (const std::exception& e) {
            EVLOG(error) << "optimize_one_level: Exception occurred: failed to get schedule item for timepoint_now: " << e.what();
            return;
        }

        double max_current_for_next_level_A = 0.0F;
        try {
            // choose max current
            max_current_for_next_level_A = get_current_limit_from_energy_object(grid_import_limit, energy);
        } catch (const std::exception& e) {
            EVLOG(error) << "optimize_one_level: Exception occurred: failed to get current limit for next level: " << e.what();
            return;
        }

        int number_children = 0;
        if (energy.contains("children")) {
            // get number of children
            number_children = energy.at("children").size();
        }

        if (number_children > 0) {
            // get current price / kWh
            double current_price_per_kwh = INVALID_PRICE_PER_KWH;
            current_price_per_kwh = get_currently_valid_price_per_kwh(energy, timepoint_now);

            // check if any children have price_limits set
            check_for_children_requesting_power(energy, current_price_per_kwh);

            scale_and_distribute_power(energy);

            // optimize each child
            for (json& child : energy.at("children")) {
                optimize_one_level(child, optimized_values, timepoint_now, price_schedule);
            }
        }

        // is this an EVSE? Add to optimized_values then.
        try {
            if (energy.at("node_type") == "Evse") {
                json limits_import;
                limits_import["valid_until"] = to_rfc3339(std::chrono::system_clock::now() + std::chrono::seconds(10));
                limits_import["request_parameters"] = json::object();
                limits_import["request_parameters"]["ac_current_A"] = json::object();
                limits_import["request_parameters"]["ac_current_A"]["current_A"] = max_current_for_next_level_A;

                json result;
                result["limits_import"] = limits_import;
                result["limits_export"] = json::object();
                result["uuid"] = energy.at("uuid");

                // add import schedule for currently projected plan
                if (!price_schedule.is_null()) {
                    result["schedule_import"] = price_schedule;
                } else {
                    result["schedule_import"] = json::array();
                }
                result["schedule_export"] = json::array();

                optimized_values.push_back(result);
            }
        } catch (const std::exception& e) {
            EVLOG(error) << "optimize_one_level: Exception occurred: energy object faulty: " << e.what();
        }
    }
}

json EnergyManager::get_sub_element_from_schedule_at_time(json s,
                                                          const std::chrono::system_clock::time_point timepoint) {
    // first entry is valid now per agreement
    json ret = s[0];
    // walk through schedule to find a better fit
    for (auto it = s.begin(); it != s.end(); ++it) {
        try {
            if ((*it).contains("timestamp") && !(*it)["timestamp"].is_null()) {  // need "[]" to prevent nlohmann error 304: cannot use at()}
                if (from_rfc3339((*it).at("timestamp")) > timepoint) {
                    break;
                }
                ret = (*it);
            }
        } catch (const std::exception& e) {
            EVLOG(error) << "Exception occurred: (no timestamp available)" << e.what();
            continue;
        }
    }
    return ret;
}

void EnergyManager::sanitize_object(json& obj_to_sanitize) {
    if (obj_to_sanitize.contains("limits_import") && obj_to_sanitize["limits_import"].is_null()) { // need "[]" to prevent nlohmann error 304: cannot use at() with null
        obj_to_sanitize["limits_import"] = json::array();
    }

    if (obj_to_sanitize.contains("limits_export") && obj_to_sanitize["limits_export"].is_null()) {  // need "[]" to prevent nlohmann error 304: cannot use at() with null
        obj_to_sanitize["limits_export"] = json::array();
    }

    if (obj_to_sanitize.contains("schedule_import") && obj_to_sanitize["schedule_import"].is_null()) {  // need "[]" to prevent nlohmann error 304: cannot use at() with null
        obj_to_sanitize["schedule_import"] = json::array();
    }

    if (obj_to_sanitize.contains("schedule_export") && obj_to_sanitize["schedule_export"].is_null()) {  // need "[]" to prevent nlohmann error 304: cannot use at() with null
        obj_to_sanitize["schedule_export"] = json::array();
    }

    if (obj_to_sanitize.contains("children") && obj_to_sanitize["children"].is_null()) {  // need "[]" to prevent nlohmann error 304: cannot use at() with null
        obj_to_sanitize["children"] = json::array();
    }
}

double EnergyManager::get_current_limit_from_energy_object(const json& limit_object, const json& energy_object) {
    bool limit_object_is_complete_flag = false;
    double max_current_A = ENERGY_MANAGER_ABSOLUTE_MAX_CURRENT;

    if (limit_object.contains("request_parameters")) {
        if (limit_object.at("request_parameters").contains("ac_current_A")) {
            if (limit_object.at("request_parameters").at("ac_current_A").contains("max_current_A")) {
                max_current_A = limit_object.at("request_parameters").at("ac_current_A").at("max_current_A");
                limit_object_is_complete_flag = true;
            }
        }
    }
    if (limit_object_is_complete_flag == false) {
        EVLOG(error) << "limit object incomplete: " << limit_object;
    }

    if (energy_object.contains("limit_from_parent") && !energy_object["limit_from_parent"].is_null()) {  // need "[]" to prevent nlohmann error 304: cannot use at() with null
        if (energy_object.at("limit_from_parent") < max_current_A) {
            max_current_A = energy_object.at("limit_from_parent");
        }
    }

    return max_current_A;
}

double EnergyManager::get_currently_valid_price_per_kwh(json& energy_object,
                                                        const std::chrono::system_clock::time_point timepoint_now) {

    double currently_valid_price_per_kwh = INVALID_PRICE_PER_KWH;

    if (energy_object.contains("schedule_import") && !energy_object["schedule_import"].is_null()) {  // need "[]" to prevent nlohmann error 304: cannot use at() with null
        // get current timeslot from price import schedule
        json schedule_at_current_timeslot =
            get_sub_element_from_schedule_at_time(energy_object.at("schedule_import"), timepoint_now);
        if (schedule_at_current_timeslot.contains("price_per_kwh")) {
            if (schedule_at_current_timeslot.at("price_per_kwh").contains("value")) {
                currently_valid_price_per_kwh = (double)schedule_at_current_timeslot.at("price_per_kwh").at("value");
            }
        }
    }

    return currently_valid_price_per_kwh;
}

void EnergyManager::check_for_children_requesting_power(json& energy_object, const double current_price_per_kwh) {

    for (json& child : energy_object["children"]) {
        // check if this child has price_limits set
        if (child.contains("optimizer_target") && !child["optimizer_target"].is_null()) {  // need "[]" to prevent nlohmann error 304: cannot use at() with null
            if (child.at("optimizer_target").contains("price_limit")) {
                // check if price limits are valid now
                if (current_price_per_kwh <= child.at("optimizer_target").at("price_limit")) {
                    // if price limits are valid now, set request flag
                    child["requesting_power"] = true;
                }
                child["optimizer_target_is_set"] = true;
            } else {
                child["optimizer_target_is_set"] = false;
            }
        } else {
            child["optimizer_target_is_set"] = false;
        }

        // if child has no optimizer target set, assume that child is requesting power continuously (manual limit)
        if (child["optimizer_target_is_set"] == false) {
            child["requesting_power"] = true;
        }
    }
}

void EnergyManager::scale_and_distribute_power(json& energy_object) {
    bool not_done = true;
    double sum_max_current_requests = 0.0F;
    double sum_min_current_requests = 0.0F;
    double child_max_current_A = 0.0F;
    double child_min_current_A = 0.0F;
    double current_scaling_factor = 1.0F;

    // prime all children's max-/min- current requests
    for (json& child : energy_object["children"]) {
        if (child.contains("requesting_power") && child.at("requesting_power") == true) {
            if (child.contains("schedule_import") && !child["schedule_import"].is_null()) {  // need "[]" to prevent nlohmann error 304: cannot use at() with null
                child.at("schedule_import")
                    .at(0)
                    .at("request_parameters")
                    .at("ac_current_A")
                    .at("max_current_A")
                    .get_to(child_max_current_A);
                child.at("schedule_import")
                    .at(0)
                    .at("request_parameters")
                    .at("ac_current_A")
                    .at("min_current_A")
                    .get_to(child_min_current_A);
                sum_max_current_requests += child_max_current_A;
                sum_min_current_requests += child_min_current_A;
                child["scaled_current"] = current_scaling_factor * child_max_current_A;
            } else {
                // "schedule_import" not yet set, no sense in continuing (fault case: remove this child from request
                // group and continue)
                child.at("requesting_power") = false;
                continue;
            }
        } else {
            child["requesting_power"] = false;
        }
    }

    double current_limit_at_this_level = 0.0F;

    try {
        // store current level's current limit
        current_limit_at_this_level =
            energy_object.at("schedule_import").at(0).at("request_parameters").at("ac_current_A").at("max_current_A");
    } catch (const std::exception& e) {
        EVLOG(error) << "energy object has no current limit for this level! " << e.what();
        return;
    }

    try {
        do {
            bool recalculate = false;
            sum_max_current_requests = 0.0;
            sum_min_current_requests = 0.0;

            // add all children's max-/min- current requests
            for (json& child : energy_object["children"]) {
                if (child.at("requesting_power") == true) {
                    child.at("scaled_current").get_to(child_max_current_A);
                    child.at("schedule_import")
                        .at(0)
                        .at("request_parameters")
                        .at("ac_current_A")
                        .at("min_current_A")
                        .get_to(child_min_current_A);
                    // add to overall max-/min- current tally
                    if (child_max_current_A >= child_min_current_A) {
                        sum_max_current_requests += child_max_current_A;
                        sum_min_current_requests += child_min_current_A;
                    } else {
                        // ... and weed out children with too low current requests
                        child.at("requesting_power") = false;
                    }
                }
            }

            // divide maximum current available to this level by sum of current requests
            if (energy_object.contains("schedule_import") && !energy_object["schedule_import"].is_null()) {  // need "[]" to prevent nlohmann error 304: cannot use at() with null

                current_scaling_factor = current_limit_at_this_level / sum_max_current_requests;

                // limit scaling factor to 1
                if (current_scaling_factor > 1.0F) {
                    current_scaling_factor = 1.0F;
                }
            } else {
                // something is wrong, abort (error case: no power available to this level)
                return;
            }

            // if the sum of this level's children's minimum current requests is already larger than its current limit, we
            // need to drop requests from children; if not, start scaling
            if (sum_min_current_requests > current_limit_at_this_level) {
                // drop first child which breaks "min_current_A" requirement and recalculate
                for (json& child : energy_object["children"]) {
                    if (child.at("requesting_power") == true) {
                        child.at("scaled_current").get_to(child_max_current_A);
                        child["scaled_current"] = current_scaling_factor * child_max_current_A;

                        // check if "min_current_A" is breached
                        if (child.at("scaled_current") < child.at("schedule_import")
                                                            .at(0)
                                                            .at("request_parameters")
                                                            .at("ac_current_A")
                                                            .at("min_current_A")) {
                            // if "min_current_A" breached, drop first child and recalculate
                            child["requesting_power"] = false;
                            recalculate = true;
                            break;
                        }
                    }
                }
            } else if (sum_max_current_requests <= current_limit_at_this_level) {
                // this is the end goal for this level
                recalculate = false;
            } else {
                // sum_min_current_requests <= current_limit_at_this_level
                // apply scaling factor to all requesting children
                for (json& child : energy_object["children"]) {
                    if (child.at("requesting_power") == true) {
                        child.at("scaled_current").get_to(child_max_current_A);
                        if (child.at("scaled_current") > child.at("schedule_import")
                                                            .at(0)
                                                            .at("request_parameters")
                                                            .at("ac_current_A")
                                                            .at("min_current_A")) {
                            // only apply scaling if child is not already at minimum current, then check afterward if
                            // scaling brought it below threshold
                            child["scaled_current"] = current_scaling_factor * child_max_current_A;
                        }
                        // check if child has already been scaled to minimum current
                        else if (child.at("scaled_current") == child.at("schedule_import")
                                                                .at(0)
                                                                .at("request_parameters")
                                                                .at("ac_current_A")
                                                                .at("min_current_A")) {
                            // we have already checked that it is possible to distribute all requests if minimum currents
                            // are assigned, thus continue with the next child
                            recalculate = true;
                            continue;
                        }

                        // check if "min_current_A" is breached
                        if (child["scaled_current"] < child.at("schedule_import")
                                                        .at(0)
                                                        .at("request_parameters")
                                                        .at("ac_current_A")
                                                        .at("min_current_A")) {
                            // if "min_current_A" breached, set minimum current as request and recalculate
                            child["scaled_current"] = child.at("schedule_import")
                                                        .at(0)
                                                        .at("request_parameters")
                                                        .at("ac_current_A")
                                                        .at("min_current_A");
                            recalculate = true;
                            break;
                        }
                    }
                }
            }

            if (recalculate == false) {
                not_done = false;
                for (json& child : energy_object["children"]) {

                    if (child.at("requesting_power") == true) {
                        // if child is requesting power still, assign scaled_current
                        child["limit_from_parent"] = child.at("scaled_current");
                    } else {
                        // child is NOT requesting power, set limit to zero
                        child["limit_from_parent"] = 0;
                    }
                }
            }

        } while (not_done);
    } catch (const std::exception& e) {
        EVLOG(error) << "scale_and_distribute_power: Exception occurred: failed to calculate current distribution: " << e.what();
        return;
    }
}

} // namespace module
