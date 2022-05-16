// SPDX-License-Identifier: Apache-2.0
// Copyright 2022 - 2022 Pionix GmbH and Contributors to EVerest

#include "energyImpl.hpp"
#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <date/date.h>
#include <string>

namespace module {
namespace energy_grid {

std::string to_rfc3339(std::chrono::time_point<date::utc_clock> t) {
    return date::format("%FT%TZ", std::chrono::time_point_cast<std::chrono::milliseconds>(t));
}

std::chrono::time_point<date::utc_clock> from_rfc3339(std::string t) {
    std::istringstream infile{t};
    std::chrono::time_point<date::utc_clock> tp;
    infile >> date::parse("%FT%T", tp);
    return tp;
}

void energyImpl::init() {
    _price_limit = 0.0F;
    _price_limit_previous_value = 0.0F;
    _optimizer_mode = EVSE_OPTIMIZER_MODE_MANUAL_LIMITS;
    initializeEnergyObject();

    mod->r_powermeter->subscribe_powermeter([this](json p) {
        // Received new power meter values, update our energy object.
        energy["energy_usage"] = p;

        updateAndPublishEnergyObject();
    });

    mod->mqtt.subscribe("/external/" + mod->info.id + ":" + mod->info.name + "/cmd/set_price_limit", [this](json lim) {
        std::string sLim = lim;
        double new_price_limit = std::stod(sLim);

        EVLOG(debug) << "price limit changed to: " << new_price_limit
                     << " EUR / kWh"; // TODO(LAD): adapt to other currencies

        // update price limits
        if (new_price_limit > 0.0F) {
            // save price limit to not be lost on switching to manual limits
            _price_limit_previous_value =
                new_price_limit; // TODO(LAD): add storage on more permanent medium (config/hdd?)
            _price_limit = new_price_limit;
            _optimizer_mode = EVSE_OPTIMIZER_MODE_PRICE_DRIVEN;
            EVLOG(debug) << "switched to \"price_driven\" optimizer mode";
        } else {
            _price_limit = -1.0F;
            _optimizer_mode = EVSE_OPTIMIZER_MODE_MANUAL_LIMITS;
            EVLOG(debug) << "switched to \"manual_limits\" optimizer mode";
        }

        updateAndPublishEnergyObject();
    });

    mod->mqtt.subscribe("/external/" + mod->info.id + ":" + mod->info.name + "/cmd/switch_optimizer_mode",
                        [this](json mode) {
                            if (mode == EVSE_OPTIMIZER_MODE_MANUAL_LIMITS) {
                                _optimizer_mode = EVSE_OPTIMIZER_MODE_MANUAL_LIMITS;
                                EVLOG(debug) << "switched to \"manual_limits\" optimizer mode";
                            } else if (mode == EVSE_OPTIMIZER_MODE_PRICE_DRIVEN) {
                                _optimizer_mode = EVSE_OPTIMIZER_MODE_PRICE_DRIVEN;
                                EVLOG(debug) << "switched to \"price_driven\" optimizer mode";
                            } else {
                                // error
                                EVLOG(error) << "received unknown optimizer mode: " << mode;
                            }

                            updateAndPublishEnergyObject();
                        });
}

void energyImpl::ready() {
    json hw_caps = mod->get_hw_capabilities();
    json schedule_entry = json::object();
    schedule_entry["timestamp"] = to_rfc3339(date::utc_clock::now());
    schedule_entry["request_parameters"] = json::object();
    schedule_entry["request_parameters"]["limit_type"] = "Hard";
    schedule_entry["request_parameters"]["ac_current_A"] = json::object();
    schedule_entry["request_parameters"]["ac_current_A"] = hw_caps;

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
        if (!limits_import["request_parameters"].is_null() &&
            !limits_import["request_parameters"]["ac_current_A"].is_null() &&
            !limits_import["request_parameters"]["ac_current_A"]["current_A"].is_null() &&
            limits_import["request_parameters"]["ac_current_A"]["current_A"] < limit) {
            limit = limits_import["request_parameters"]["ac_current_A"]["current_A"];
        }

        // update limit at the charger
        if (!limits_import["valid_until"].is_null()) {
            mod->charger->setMaxCurrent(limit, from_rfc3339(limits_import["valid_until"]));
            if (limit > 0)
                mod->charger->resumeChargingPowerAvailable();
        }

        // set phase count limits
        auto phase_count_limit = json::object();
        phase_count_limit["max_phase_count"] = mod->get_hw_capabilities()["max_phase_count"];
        phase_count_limit["min_phase_count"] = mod->get_hw_capabilities()["min_phase_count"];
        phase_count_limit["supports_changing_phases_during_charging"] =
            mod->get_hw_capabilities()["supports_changing_phases_during_charging"];

        if (energy["energy_usage"]["power_W"]["total"] > 0) {
            if (phase_count_limit["supports_changing_phases_during_charging"] != true) {
                EVLOG(debug) << "Cannot apply phase limit: Setting during charging not supported!";
            } else {
                // set phase count
                // ---not implemented---
            }
        } else {
            // set phase count
            // ---not implemented---
        }

        // set export limits
        // ---not implemented---
    }
    // if not, ignore as we do not have children.
}

void energyImpl::initializeEnergyObject() {
    energy["node_type"] = "Evse";

    // UUID must be unique also beyond this charging station -> will be handled on framework level and above later
    energy["uuid"] = mod->info.id;
}

void energyImpl::updateAndPublishEnergyObject() {

    // update optimizer mode
    if (_optimizer_mode == EVSE_OPTIMIZER_MODE_MANUAL_LIMITS) {
        _price_limit = 0.0F;
        if (energy.contains("optimizer_target")) {
            // remove "price_limit" from energy object and switch current limit to manual
            {
                std::lock_guard<std::mutex> lock(this->energy_mutex);

                energy.at("optimizer_target").erase("price_limit");
                energy.erase("optimizer_target");

                EVLOG(debug) << " switched to manual_limits: removing price_limit";
            }
        }
        {
            std::lock_guard<std::mutex> lock(this->energy_mutex);
            if (energy.contains("schedule_import")) {
                energy.at("schedule_import").at(0).at("request_parameters").at("ac_current_A").at("max_current_A") =
                    mod->getLocalMaxCurrentLimit();
            } else {
                return;
            }
        }
    } else if (_optimizer_mode == EVSE_OPTIMIZER_MODE_PRICE_DRIVEN) {
        _price_limit = _price_limit_previous_value;
        {
            // add "price_limit" to energy object and switch current limit to hardware limit
            std::lock_guard<std::mutex> lock(this->energy_mutex);
            energy["optimizer_target"] = json::object();
            energy["optimizer_target"]["price_limit"] = _price_limit;
            if (energy.contains("schedule_import")) {
                energy.at("schedule_import").at(0).at("request_parameters").at("ac_current_A").at("max_current_A") =
                    mod->getLocalMaxCurrentLimit();
            } else {
                return;
            }
        }
    }

    // publish to energy tree
    publish_energy(energy);
}

} // namespace energy_grid
} // namespace module
