// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "EnergyManager.hpp"
#include <chrono>
#include <date/date.h>

namespace module {

#define INVALID_PRICE_PER_KWH   (-1.1f)

std::string to_rfc3339(std::chrono::time_point<std::chrono::system_clock> t) {
    return date::format("%FT%TZ", std::chrono::time_point_cast<std::chrono::milliseconds>(t));
}

std::chrono::time_point<std::chrono::system_clock> from_rfc3339(std::string t) {
    std::istringstream infile{t};
    std::chrono::time_point<std::chrono::system_clock> tp;
    infile >> date::parse("%FT%T", tp);

    // std::cout <<"timepoint"<<" "<<t<<" "<< tp.time_since_epoch().count()<<std::endl;
    return tp;
}

void EnergyManager::init() {
    invoke_init(*p_main);
    lastLimitUpdate = std::chrono::system_clock::now();

    for (auto& entry : r_energy_trunk) {
        entry->subscribe_energy([this, &entry](json e) {

            // Received new energy object from a child.

            // Re-Run global optimizer and create limits and schedules for each evse type leaf.
            json optimized_values = run_optimizer(e);

            // rate-limit the enforced limit update
            auto now = std::chrono::system_clock::now();
            auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLimitUpdate).count();
            if (timeSinceLastUpdate >= 5000){
                
                lastLimitUpdate = std::chrono::system_clock::now();
                
                // run enforce_limits commands.
                for (auto it = optimized_values.begin(); it != optimized_values.end(); ++it) {
                    sanitize_object(*it);
                    entry->call_enforce_limits( (*it)["uuid"], 
                                                (*it)["limits_import"], 
                                                (*it)["limits_export"],
                                                (*it)["schedule_import"], 
                                                (*it)["schedule_export"]);
                }
            }
        });
    }
}

void EnergyManager::ready() {
    invoke_ready(*p_main);
}

Array EnergyManager::run_optimizer(json energy) {
    // traverse tree, set result limits for each evse node
    json optimized_values = json::array();
    auto timepoint = std::chrono::system_clock::now();
    optimize_one_level(energy, optimized_values, timepoint);
    return optimized_values;
}

// recursive optimization of one level
void EnergyManager::optimize_one_level(json& energy, json& optimized_values,
                                       const std::chrono::system_clock::time_point timepoint_now) {
    // find max_current limit for this level
    // min of (limit_from_parent, local_limit_from_schedule)
    if (energy.contains("schedule_import")) {
        json grid_import_limit = get_sub_element_from_schedule_at_time(energy.at("schedule_import"), timepoint_now);

        // choose max current
        float max_current_for_next_level_A = get_current_limit_from_energy_object(grid_import_limit, energy);

        int number_children = 0;
        if (energy.contains("children")) {
            // get number of children
            number_children = energy.at("children").size();
        }    

        if (number_children > 0) { 
            // EVLOG(error) << "object energy: " << energy; // TODO(LAD): remove me

            // get current price / kWh
            double current_price_per_kwh = INVALID_PRICE_PER_KWH;
            current_price_per_kwh = get_currently_valid_price_per_kwh(energy, timepoint_now);

            // check if any children have price_limits set
            check_for_children_requesting_power(energy, current_price_per_kwh);

            scale_and_distribute_power(energy);

            // optimize each child 
            for (json& child : energy.at("children")) {
                optimize_one_level(child, optimized_values, timepoint_now);
            }
        }

        // is this an EVSE? Add to optimized_values then.
        if (energy.at("node_type") == "Evse") {
            json limits_import;
            limits_import["valid_until"] = to_rfc3339( std::chrono::system_clock::now() + std::chrono::seconds(10) );
            limits_import["limit"] = json::object();
            limits_import["limit"]["ac_current_A"] = json::object();
            limits_import["limit"]["ac_current_A"]["current_A"] = max_current_for_next_level_A;

            json result;
            result["limits_import"] = limits_import;
            result["limits_export"] = json::object();
            result["uuid"] = energy.at("uuid");
            // TODO(LAD): add import schedule for currently projected plan
            result["schedule_import"] = json::array();
            result["schedule_export"] = json::array();

            optimized_values.push_back(result);
        }
    }
}

json EnergyManager::get_sub_element_from_schedule_at_time(json s, const std::chrono::system_clock::time_point timepoint) {
    // first entry is valid now per agreement
    json ret = s[0];
    // walk through schedule to find a better fit
    for (auto it = s.begin(); it != s.end(); ++it) {
        if (from_rfc3339((*it)["timestamp"]) > timepoint) break;
        ret = (*it);
    }
    return ret;
}

void EnergyManager::sanitize_object(json& obj_to_sanitize){
    if (obj_to_sanitize.contains("schedule_import")) {
        if (obj_to_sanitize.at("schedule_import").is_null()) {
            obj_to_sanitize["schedule_import"] = json::array();
        }
    }

    if (obj_to_sanitize.contains("schedule_export")) {
        if (obj_to_sanitize.at("schedule_export").is_null()) {
            obj_to_sanitize["schedule_export"] = json::array();
        }
    }

    if (obj_to_sanitize.contains("children")) {
        if (obj_to_sanitize.at("children").is_null()) {
            obj_to_sanitize["children"] = json::array();
        }
    }
}

float EnergyManager::get_current_limit_from_energy_object(const json& limit_object, const json& energy_object) {
    bool limit_object_is_complete_flag = false;
    float max_current_A = 80.0f;

    if (limit_object.contains("request_parameters")) {
        if (limit_object.at("request_parameters").contains("ac_current_A")) {
            if (limit_object.at("request_parameters").at("ac_current_A").contains("max_current_A")) {
                max_current_A = limit_object["request_parameters"]["ac_current_A"]["max_current_A"];
                limit_object_is_complete_flag = true;
            }
        }
    }
    if (limit_object_is_complete_flag == false) {
        EVLOG(error) << "limit object incomplete: " << limit_object; 
    }

    if (energy_object.contains("limit_from_parent")) {
        if (!energy_object.at("limit_from_parent").is_null()) {
            if (energy_object["limit_from_parent"] < max_current_A)
                max_current_A = energy_object["limit_from_parent"];
        }
    }

    return max_current_A;
}

double EnergyManager::get_currently_valid_price_per_kwh(json& energy_object, const std::chrono::system_clock::time_point timepoint_now){
    
    double currently_valid_price_per_kwh = INVALID_PRICE_PER_KWH;

    if (energy_object.contains("schedule_import")) {
        if (!energy_object.at("schedule_import").is_null()) {
            // get current timeslot from price import schedule
            auto schedule_at_current_timeslot = get_sub_element_from_schedule_at_time(energy_object["schedule_import"], timepoint_now);
            if (schedule_at_current_timeslot.contains("price_per_kwh")) {
                if (schedule_at_current_timeslot.at("price_per_kwh").contains("currency")) {
                    // currency: EUR
                    if (schedule_at_current_timeslot["price_per_kwh"]["currency"] == "EUR") {
                        if (schedule_at_current_timeslot.at("price_per_kwh").contains("value")) {
                            currently_valid_price_per_kwh = (double)schedule_at_current_timeslot["price_per_kwh"]["value"];
                        }
                    } else {
                        // error: wrong currency / currency not (yet) implemented
                        EVLOG(error) << "Currency \"" << schedule_at_current_timeslot["price_per_kwh"]["currency"] << "\" not recognized/implemented!!!";
                    }
                    // add other currencies here
                }
            }
        }
    }

    return currently_valid_price_per_kwh;
}

void EnergyManager::check_for_children_requesting_power(json& energy_object, const double current_price_per_kwh){

    for (json& child : energy_object["children"]) {
        // check if this child has price_limits set
        if (child.contains("optimizer_target")) {
            if (child["optimizer_target"].contains("price_limit")) {
                // check if price limits are valid now
                if (current_price_per_kwh <= child["optimizer_target"]["price_limit"]) {
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
    double sum_current_requests = 0.0;
    bool recalculate = false;
    double child_max_current_A = 0.0;
    double current_scaling_factor = 1.0;

    do {
        recalculate = false;
        sum_current_requests = 0.0;

        // add all children's current requests
        for (json& child : energy_object["children"]) {
            if (child.contains("requesting_power") && child.at("requesting_power") == true) {
                if (child.contains("schedule_import")) {
                    child.at("schedule_import").at(0).at("request_parameters").at("ac_current_A").at("max_current_A").get_to(child_max_current_A);
                    sum_current_requests += child_max_current_A;
                } else {
                    // "schedule_import" not yet set, no sense in continuing
                    continue;
                } 
            }
        }

        // divide maximum current available to this level by sum of current requests
        if (energy_object.contains("schedule_import")) {
            double current_limit_at_this_level =
                energy_object.at("schedule_import").at(0).at("request_parameters").at("ac_current_A").at("max_current_A");
            current_scaling_factor = current_limit_at_this_level / sum_current_requests;
        } else {
            // something is wrong, abort
            return;
        }

        // apply scaling factor to all requesting children
        for (json& child : energy_object["children"]) {
            if (child.contains("requesting_power") && child.at("requesting_power") == true) {
                // TODO(LAD): insert check for availability of "schedule_import"
                child.at("schedule_import").at(0).at("request_parameters").at("ac_current_A").at("max_current_A").get_to(child_max_current_A);
                child["scaled_current"] = current_scaling_factor * child_max_current_A;
            }
        }

        // check if "min_current_A" is breached for any children
        for (json& child : energy_object["children"]) {
            if (child.contains("requesting_power") && child.at("requesting_power") == true) {
                // TODO(LAD): insert check for availability of "schedule_import"
                if (child["scaled_current"] <
                    child.at("schedule_import").at(0).at("request_parameters").at("ac_current_A").at("max_current_A")) {
                    // if "min_current_A" breached, drop first child from power request group and recalculate
                    child["requesting_power"] = false;
                    recalculate = true;
                    break;
                }
            }
        }

        if (recalculate == false) {
            not_done = false;

            for (json& child : energy_object["children"]) {

                if (child.contains("requesting_power") && child.at("requesting_power") == true) {
                    // if child is requesting power now
                    if(child.contains("scaled_current")) {
                        // assign either scaled_current (if necessary)...
                        child["limit_from_parent"] = child.at("scaled_current");
                    } else {
                        // ...or assign its requested current (if possible)
                        // TODO(LAD): insert check for availability of "schedule_import"
                        child["limit_from_parent"] = child.at("schedule_import").at(0).at("request_parameters").at("ac_current_A").at("max_current_A");
                    }
                } else {
                    // child is NOT requesting power, set limit to zero
                    child["limit_from_parent"] = 0; 
                }
            }
        }

    } while (not_done);
}

} // namespace module
