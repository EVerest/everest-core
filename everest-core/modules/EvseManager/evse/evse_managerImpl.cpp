#include "evse_managerImpl.hpp"

namespace module {
namespace evse {

bool str_to_bool(std::string data) {
    if (data == "true") {
        return true;
    }
    return false;
}

void evse_managerImpl::init() {

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
    // Legacy external mqtt pubs
    mod->charger->signalMaxCurrent.connect([this](float c){
        mod->mqtt.publish("/external/state/max_current", c);
    });

    mod->charger->signalState.connect([this](Charger::EvseState s){
        mod->mqtt.publish("/external/state/state_string", mod->charger->evseStateToString(s));
        mod->mqtt.publish("/external/state/state", static_cast<int>(s));
    });

    mod->charger->signalError.connect([this](Charger::ErrorState s){
        mod->mqtt.publish("/external/state/error_type", static_cast<int>(s));
        mod->mqtt.publish("/external/state/error_string", mod->charger->errorStateToString(s));
    });
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

} // namespace evse
} // namespace module
