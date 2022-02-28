// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest

#include "energyImpl.hpp"
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <chrono>
#include <date/date.h>

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
    initializeEnergyObject();

    mod->r_powermeter->subscribe_powermeter([this](json p) {
        // Received new power meter values, update our energy object.
        energy["energy_usage"] = p;

        // Publish to the energy tree
        publish_energy(energy);
    });
}

void energyImpl::ready() {
    json hw_caps = mod->get_hw_capabilities();
    json schedule_entry;
    schedule_entry["timestamp"] = to_rfc3339(std::chrono::system_clock::now());
    schedule_entry["capabilities"]["limit_type"] = "Hard";
    schedule_entry["capabilities"]["ac_current_A"] = hw_caps;

    energy["schedule_import"] = json::array({schedule_entry});
}

void energyImpl::handle_enforce_limits(std::string& uuid, Object& limits_import, Object& limits_export,
                                       Array& schedule_import, Array& schedule_export) {
    // is it for me?
    if (uuid == energy["uuid"]) {
        // apply enforced limits
  
        // 3 or one phase only when we have the capability to actually switch during charging?
        // if we have capability we'll switch while charging. otherwise in between sessions.
        // FIXME implement phase count limiting here

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
        // ---not implemented---

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
