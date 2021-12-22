// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2021 Pionix GmbH and Contributors to EVerest
#include "evse_managerImpl.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>


namespace module {
namespace evse {

bool str_to_bool(std::string data) {
    if (data == "true") {
        return true;
    }
    return false;
}

void evse_managerImpl::init() {
    limits["nr_of_phases_available"] = 1;
    limits["max_current"] = 0.;

    mod->mqtt.subscribe("/external/cmd/set_max_current", [&charger = mod->charger](std::string data) { 
        charger->setMaxCurrent(std::stof(data)); 
    });

    mod->mqtt.subscribe("/external/cmd/set_auth", [&charger = mod->charger](std::string data) {
        charger->Authorize(true, data.c_str());
    });

    mod->mqtt.subscribe("/external/cmd/enable", [&charger = mod->charger](std::string data) {
     charger->enable();
    });

    mod->mqtt.subscribe("/external/cmd/disable", [&charger = mod->charger](std::string data) {
        charger->disable();
    });

    mod->mqtt.subscribe("/external/cmd/switch_three_phases_while_charging", [&charger = mod->charger](std::string data) {
        charger->switchThreePhasesWhileCharging(str_to_bool(data));
    });

    mod->mqtt.subscribe("/external/cmd/pause_charging", [&charger = mod->charger](std::string data) {
        charger->pauseCharging();
    });

    mod->mqtt.subscribe("/external/cmd/resume_charging", [&charger = mod->charger](std::string data) {
        charger->resumeCharging();
    });

    mod->mqtt.subscribe("/external/cmd/restart", [&charger = mod->charger](std::string data) {
        charger->restart();
    });
}

void evse_managerImpl::ready() {

    mod->signalNrOfPhasesAvailable.connect([this](const int n) {
        if (n>=1 && n<=3) {
            limits["nr_of_phases_available"] = n;
            publish_limits(limits);
        }
    });

    mod->charger->signalEvent.connect([this](const Charger::EvseEvent& e) {
        json se;

        se["event"] = mod->charger->evseEventToString(e);

        if (e == Charger::EvseEvent::SessionStarted) {
            se["session_started"]["timestamp"] = std::chrono::seconds(std::time(NULL)).count();
            json p = mod->get_latest_powermeter_data();
            if (p.contains("energy_Wh_import") && p["energy_Wh_import"].contains("total"))
                se["session_started"]["energy_Wh_import"] = p["energy_Wh_import"]["total"];

            if (p.contains("energy_Wh_export") && p["energy_Wh_export"].contains("total")) 
                se["session_started"]["energy_Wh_export"] = p["energy_Wh_export"]["total"]; 

            session_uuid = generate_session_uuid();         
        }

        se["uuid"] = session_uuid;

        if (e == Charger::EvseEvent::SessionFinished) {
            se["session_finished"]["timestamp"] = std::chrono::seconds(std::time(NULL)).count();
            json p = mod->get_latest_powermeter_data();
            if (p.contains("energy_Wh_import") && p["energy_Wh_import"].contains("total"))
                se["session_finished"]["energy_Wh_import"] = p["energy_Wh_import"]["total"];

            if (p.contains("energy_Wh_export") && p["energy_Wh_export"].contains("total")) 
                se["session_finished"]["energy_Wh_export"] = p["energy_Wh_export"]["total"]; 

            session_uuid = "";              
        }

        if (e == Charger::EvseEvent::Error) {
            se["error"] = mod->charger->errorStateToString(mod->charger->getErrorState());
        }

    
        publish_session_events(se);
    });

    // Note: Deprecated. Only kept for Node red compatibility, will be removed in the future 
    // Legacy external mqtt pubs
    mod->charger->signalMaxCurrent.connect([this](float c){
        mod->mqtt.publish("/external/state/max_current", c);

        limits["max_current"] = c;
        publish_limits(limits);
    });

    mod->charger->signalState.connect([this](Charger::EvseState s){
        mod->mqtt.publish("/external/state/state_string", mod->charger->evseStateToString(s));
        mod->mqtt.publish("/external/state/state", static_cast<int>(s));
    });

    mod->charger->signalError.connect([this](Charger::ErrorState s){
        mod->mqtt.publish("/external/state/error_type", static_cast<int>(s));
        mod->mqtt.publish("/external/state/error_string", mod->charger->errorStateToString(s));
    });
    // /Deprecated
}

bool evse_managerImpl::handle_enable(){
    return mod->charger->enable();
};

bool evse_managerImpl::handle_disable(){
    return mod->charger->disable();
};

bool evse_managerImpl::handle_pause_charging(){
    return mod->charger->pauseCharging();
};

bool evse_managerImpl::handle_resume_charging(){
    return mod->charger->resumeCharging();
};

bool evse_managerImpl::handle_cancel_charging(){
    return mod->charger->cancelCharging();
};

bool evse_managerImpl::handle_accept_new_session(){
    return mod->charger->restart();
};

bool evse_managerImpl::handle_reserve_now(std::string& auth_token, double& timeout){
    // your code for cmd reserve_now goes here
    return false;
};

bool evse_managerImpl::handle_cancel_reservation(){
    // your code for cmd cancel_reservation goes here
    return false;
};

std::string evse_managerImpl::generate_session_uuid(){
    return boost::uuids::to_string(boost::uuids::random_generator()());
}

} // namespace evse
} // namespace module
