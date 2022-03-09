// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest
#include "EnergyManager.hpp"
#include <chrono>
#include <date/date.h>

namespace module {

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

    r_energy_trunk->subscribe_energy([this](json e) {
        // Received new energy object from a child.

        // Re-Run global optimizer and create limits and schedules for each evse type leaf.
        Array results = run_optimizer(e);

        // run enforce_limits commands.
        for (auto it = results.begin(); it != results.end(); ++it) {
            sanitize_object(*it);
            r_energy_trunk->call_enforce_limits((*it)["uuid"], 
                                                (*it)["limits_import"], 
                                                (*it)["limits_export"],
                                                (*it)["schedule_import"], 
                                                (*it)["schedule_export"]);
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
                                       const std::chrono::system_clock::time_point timepoint) {
    // find max_current limit for this level
    // min of (limit_from_parent, local_limit_from_schedule)
    if (energy.contains("schedule_import")) {
        json grid_import_limit = get_limit_from_schedule(energy["schedule_import"], timepoint);

        bool grid_import_limit_is_complete_flag = false;
        float max_current = 80.;
        if (grid_import_limit.contains("capabilities")){
            if (grid_import_limit["capabilities"].contains("ac_current_A")){
                if (grid_import_limit["capabilities"]["ac_current_A"].contains("max_current_A")){
                    max_current = grid_import_limit["capabilities"]["ac_current_A"]["max_current_A"];
                    grid_import_limit_is_complete_flag = true;
                }
            }
        }
        if (grid_import_limit_is_complete_flag == false) {
            EVLOG(error) << "grid_import_limit object incomplete: " << grid_import_limit; 
        }

        if (!energy["limit_from_parent"].is_null()) {
            if (energy["limit_from_parent"] < max_current)
                max_current = energy["limit_from_parent"];
        }

        int n = 0;
        if (energy.contains("children")) {
            // get number of children
            n = energy["children"].size();
        }    

        if (n > 0) {
            // EVLOG(error) << "energy[children]: " << energy["children"]; 
            // set limit_from_parent on each child and optimize it
            for (json it : energy["children"]) {
                it["limit_from_parent"] = max_current / n;
                optimize_one_level(it, results, timepoint);
            }
        }

        // is this an EVSE? Add to results then.
        if (energy["node_type"] == "Evse") {
            json limits_import;
            limits_import["valid_until"] = to_rfc3339(std::chrono::system_clock::now() + std::chrono::seconds(10) );
            limits_import["limit"]["ac_current_A"]["current_A"] = max_current;

            json result;
            result["limits_import"] = limits_import;
            result["limits_export"] = json::object();
            result["uuid"] = energy["uuid"];
            result["schedule_import"] = json::array();
            result["schedule_export"] = json::array();

            results.push_back(result);
        }
    }
}

json EnergyManager::get_limit_from_schedule(json s, const std::chrono::system_clock::time_point timepoint) {
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
        if (obj_to_sanitize["schedule_import"].is_null()) {
            obj_to_sanitize["schedule_import"] = json::array();
        }
    }

    if (obj_to_sanitize.contains("schedule_export")) {
        if (obj_to_sanitize["schedule_export"].is_null()) {
            obj_to_sanitize["schedule_export"] = json::array();
        }
    }

    if (obj_to_sanitize.contains("children")) {
        if (obj_to_sanitize["children"].is_null()) {
            obj_to_sanitize["children"] = json::array();
        }
    }
}

} // namespace module
