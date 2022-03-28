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

    r_energy_trunk->subscribe_energy([this](json e) {
        // Received new energy object from a child.

        // Re-Run global optimizer and create limits and schedules for each evse type leaf.
        Array results = run_optimizer(e);

        // rate-limit enforced limit update
        auto now = std::chrono::system_clock::now();
        auto timeSinceLastUpdate = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastLimitUpdate).count();
        if (timeSinceLastUpdate >= 5000){
            EVLOG(debug) << "timeSinceLastUpdate: " << timeSinceLastUpdate;
            lastLimitUpdate = std::chrono::system_clock::now();
            // run enforce_limits commands.
            for (auto it = results.begin(); it != results.end(); ++it) {
                sanitize_object(*it);
                r_energy_trunk->call_enforce_limits((*it)["uuid"], 
                                                    (*it)["limits_import"], 
                                                    (*it)["limits_export"],
                                                    (*it)["schedule_import"], 
                                                    (*it)["schedule_export"]);
                EVLOG(debug) << "it: " << (*it)["uuid"];
            }
        }
    });
}

void EnergyManager::ready() {
    invoke_ready(*p_main);
}

Array EnergyManager::run_optimizer(json energy) {
    // traverse tree, set result limits for each evse node
    Array results;
    auto timepoint = std::chrono::system_clock::now();
    optimize_one_level(energy, results, timepoint);
    return results;
}

// recursive optimization of one level
void EnergyManager::optimize_one_level(json& energy, Array& results,
                                       const std::chrono::system_clock::time_point timepoint_now) {
    // find max_current limit for this level
    // min of (limit_from_parent, local_limit_from_schedule)
    if (energy.contains("schedule_import")) {
        json grid_import_limit = get_sub_element_from_schedule_at_time(energy["schedule_import"], timepoint_now);

        // choose max current
        float max_current_for_next_level_A = get_current_limit_from_energy_object(grid_import_limit, energy);

        int number_children = 0;
        if (energy.contains("children")) {
            // get number of children
            number_children = energy["children"].size();
        }    

        if (number_children > 0) { 
            // EVLOG(error) << "object energy: " << energy; // TODO(LAD): remove me

            // get current price / kWh
            double current_price_per_kwh = INVALID_PRICE_PER_KWH;
            current_price_per_kwh = get_currently_valid_price_per_kwh(energy, timepoint_now);

            // check if any children have price_limits set
            int children_requesting_power = check_for_children_requesting_power(energy, current_price_per_kwh);

            double equal_current_per_child = max_current_for_next_level_A / children_requesting_power;
            double sum_current_for_non_limited_children = 0;

            // EVLOG(error) << "energy[children]: " << energy["children"];
            // for (json& child : energy["children"]) {
            //     if (child.at()) {

            //     }
            // }

            // set limit_from_parent on each child and optimize it
            for (json& child : energy["children"]) {
                if (child.contains("requesting_power") && child.at("requesting_power") == true) {
                    // if child is requesting power now
                    child["limit_from_parent"] = max_current_for_next_level_A / children_requesting_power;
                } else {
                    // child is NOT requesting power, set limit to zero
                    child["limit_from_parent"] = 0; 
                }

                optimize_one_level(child, results, timepoint_now);
            }
        }

        // is this an EVSE? Add to results then.
        if (energy["node_type"] == "Evse") {
            json limits_import;
            limits_import["valid_until"] = to_rfc3339(std::chrono::system_clock::now() + std::chrono::seconds(10) );
            limits_import["limit"]["ac_current_A"]["current_A"] = max_current_for_next_level_A;

            json result;
            result["limits_import"] = limits_import;
            result["limits_export"] = json::object();
            result["uuid"] = energy["uuid"];
            // TODO(LAD): add import schedule for currently projected plan
            result["schedule_import"] = json::array();
            result["schedule_export"] = json::array();

            results.push_back(result);
        }
    }
}

json EnergyManager::get_sub_element_from_schedule_at_time(json s, const std::chrono::system_clock::time_point timepoint) {
    // first entry is valid now per agreement
    json ret = s[0];
    // walk through schedule to find a better fit
    for (auto it = s.begin(); it != s.end(); ++it) {
        if (from_rfc3339((*it).at("timestamp")) > timepoint) break;
        ret = (*it);
    }
    return ret;
}

void EnergyManager::sanitize_object(json& obj_to_sanitize){
    if (obj_to_sanitize.contains("schedule_import")) {
        if (obj_to_sanitize.at("schedule_import").is_null()) {
            obj_to_sanitize.at("schedule_import") = json::array();
        }
    }

    if (obj_to_sanitize.contains("schedule_export")) {
        if (obj_to_sanitize.at("schedule_export").is_null()) {
            obj_to_sanitize.at("schedule_export") = json::array();
        }
    }

    if (obj_to_sanitize.contains("children")) {
        if (obj_to_sanitize.at("children").is_null()) {
            obj_to_sanitize.at("children") = json::array();
        }
    }
}

float EnergyManager::get_current_limit_from_energy_object(const json& limit_object, const json& energy_object) {
    bool limit_object_is_complete_flag = false;
    float max_current_A = 80.0f;

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

    if (energy_object.contains("limit_from_parent")) {
        if (!energy_object.at("limit_from_parent").is_null()) {
            if (energy_object.at("limit_from_parent") < max_current_A)
                max_current_A = energy_object.at("limit_from_parent");
        }
    }

    return max_current_A;
}

double EnergyManager::get_currently_valid_price_per_kwh(json& energy_object, const std::chrono::system_clock::time_point timepoint_now){
    
    double currently_valid_price_per_kwh = INVALID_PRICE_PER_KWH;

    if (energy_object.contains("schedule_import")) {
        if (!energy_object.at("schedule_import").is_null()) {
            // get current timeslot from price import schedule
            auto schedule_at_current_timeslot = get_sub_element_from_schedule_at_time(energy_object.at("schedule_import"), timepoint_now);
            if (schedule_at_current_timeslot.contains("price_per_kwh")) {
                if (schedule_at_current_timeslot.at("price_per_kwh").contains("currency")) {
                    // currency: EUR
                    if (schedule_at_current_timeslot.at("price_per_kwh").at("currency") == "EUR") {
                        if (schedule_at_current_timeslot.at("price_per_kwh").contains("value")) {
                            currently_valid_price_per_kwh = (double)schedule_at_current_timeslot.at("price_per_kwh").at("value");
                        }
                    } else {
                        // error: wrong currency / currency not (yet) implemented
                        EVLOG(error) << "Currency \"" << schedule_at_current_timeslot.at("price_per_kwh").at("currency") << "\" not recognized/implemented!!!";
                    }
                    // add other currencies here
                }
            }
        }
    }

    return currently_valid_price_per_kwh;
}

int EnergyManager::check_for_children_requesting_power(json& energy_object, const double current_price_per_kwh){
    int children_requesting_power = 0;

    for (json& child : energy_object["children"]) {
        // check if this child has price_limits set
        if (child.contains("optimizer_target")) {
            if (child.at("optimizer_target").contains("price_limit")) {
                // check if price limits are valid now
                if (current_price_per_kwh <= child.at("optimizer_target").at("price_limit")) {
                    // if price limits are valid now, increase children_requesting_power
                    children_requesting_power++;
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
        if (child.at("optimizer_target_is_set") == false) {
            child["requesting_power"] = true;
        }
    }

    return children_requesting_power;
}

} // namespace module
