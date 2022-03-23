// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest

#include "energyImpl.hpp"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <date/date.h>
#include <string>

namespace module {
namespace energy_grid {

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

void energyImpl::init() {
    _price_limit = 0;
    _price_limit_previous_value = 0;
    initializeEnergyObject();

    mod->r_powermeter->subscribe_powermeter([this](json p) {
        // Received new power meter values, update our energy object.
        energy["energy_usage"] = p;

        // Publish to the energy tree
        publish_energy(energy);
    });

    mod->mqtt.subscribe("/external/" + mod->info.id + ":" + mod->info.name + "/cmd/set_price_limit", [this](json lim) { 
        std::string sLim = lim;
        _price_limit = std::stod(sLim);

        EVLOG(debug) << "price limit changed to: " << _price_limit << " EUR / kWh"; // TODO(LAD): adapt to other currencies

        if (_price_limit > 0.0f) {  // only add price_limit if optimizer mode is set to "price_driven"
            // save price limit to not be lost on switching to manual limits
            _price_limit_previous_value = _price_limit;   // TODO(LAD): add storage on more permanent medium (config/hdd?)

            {
                std::lock_guard<std::mutex> lock(this->energy_mutex);
                energy["optimizer_target"] = json::object();
                energy["optimizer_target"]["price_limit"] = _price_limit;
            }

            // Publish to the energy tree
            publish_energy(energy);
        } else {
            if (energy.contains("optimizer_target")) {
                {
                    std::lock_guard<std::mutex> lock(this->energy_mutex);
                    energy.at("optimizer_target").erase("price_limit");
                }
            }
        }
    });

    mod->mqtt.subscribe("/external/" + mod->info.id + ":" + mod->info.name + "/cmd/switch_optimizer_mode", [this](json mode) { 
        std::string sMode = mode;
        EVLOG(debug) << "optimizer mode set to: " << sMode;

        if (sMode == "manual_limits") {
            _price_limit = 0.0f;
            EVLOG(error) << "Manual limits optimizer mode set";
            if (energy.contains("optimizer_target")) {
                {
                    std::lock_guard<std::mutex> lock(this->energy_mutex);
                    energy.at("optimizer_target").erase("price_limit");
                }
            }
        } else if (sMode == "price_driven") {
            _price_limit = _price_limit_previous_value;
            EVLOG(error) << "Price-driven optimizer mode set";
            {
                std::lock_guard<std::mutex> lock(this->energy_mutex);
                energy["optimizer_target"] = json::object();
                energy["optimizer_target"]["price_limit"] = _price_limit;
            }
        }
    });
}

void energyImpl::ready() {
    json hw_caps = mod->get_hw_capabilities();
    json schedule_entry = json::object();
    schedule_entry["timestamp"] = to_rfc3339(std::chrono::system_clock::now());
    schedule_entry["capabilities"] = json::object();
    schedule_entry["capabilities"]["limit_type"] = "Hard";
    schedule_entry["capabilities"]["ac_current_A"] = json::object();
    schedule_entry["capabilities"]["ac_current_A"] = hw_caps;

    {
        std::lock_guard<std::mutex> lock(this->energy_mutex);
        energy["schedule_import"] = json::array({});
        energy["schedule_import"].push_back(schedule_entry);
    }
}

void energyImpl::handle_enforce_limits(std::string& uuid, Object& limits_import, Object& limits_export,
                                       Array& schedule_import, Array& schedule_export) {
    // is it for me?
    if (uuid == energy["uuid"]) {
        // apply enforced limits
  
        // 3 or one phase only when we have the capability to actually switch during charging?
        // if we have capability we'll switch while charging. otherwise in between sessions.
        // LAD: FIXME implement phase count limiting here

        // set import limits
        // load HW/module config limit
        float limit = mod->get_hw_capabilities()["max_current_A"];

        // apply local limit
        if (mod->getLocalMaxCurrentLimit() < limit) {
            limit = mod->getLocalMaxCurrentLimit();
        }            

        // apply enforced AC current limits
        if (!limits_import["limit"].is_null() && 
            !limits_import["limit"]["ac_current_A"].is_null() &&
            !limits_import["limit"]["ac_current_A"]["current_A"].is_null() &&
            limits_import["limit"]["ac_current_A"]["current_A"] < limit) {
            limit = limits_import["limit"]["ac_current_A"]["current_A"];
        }

        // update limit at the charger
        if (!limits_import["valid_until"].is_null()) {
            mod->charger->setMaxCurrent(limit, from_rfc3339(limits_import["valid_until"]));
        }

        // set phase count limits
        auto phase_count_limit = json::object();
        phase_count_limit["max_phase_count"] = mod->get_hw_capabilities()["max_phase_count"];
        phase_count_limit["min_phase_count"] = mod->get_hw_capabilities()["min_phase_count"];
        phase_count_limit["supports_changing_phases_during_charging"] = mod->get_hw_capabilities()["supports_changing_phases_during_charging"];
        // EVLOG(error) << "phase count limits object: " << phase_count_limit;
        // EVLOG(error) << "####################: " << energy;
        if (energy["energy_usage"]["power_W"]["total"] > 0) {
            if (phase_count_limit["supports_changing_phases_during_charging"] != true) {
                EVLOG(debug) << "Cannot apply phase limit: Setting during charging not supported!";
            }
            else {
                // set phase count
                // ---not implemented---
            }
        }
        else {
            // set phase count
            // ---not implemented---
        }

        // set export limits
        // ---not implemented---
    }
    // if not, ignore as we do not have children.
}

void energyImpl::initializeEnergyObject(){
    energy["node_type"] = "Evse";
    
    // UUID must be unique also beyond this charging station
    energy["uuid"] = mod->info.id + "_" + boost::uuids::to_string(boost::uuids::random_generator()());
}

} // namespace energy_grid
} // namespace module
