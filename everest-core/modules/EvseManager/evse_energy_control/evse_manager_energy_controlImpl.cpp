#include "evse_manager_energy_controlImpl.hpp"

namespace module {
namespace evse_energy_control {

void evse_manager_energy_controlImpl::init() {

}

void evse_manager_energy_controlImpl::ready() {

}

std::string evse_manager_energy_controlImpl::handle_set_max_current(double& max_current){
    if (mod->charger->setMaxCurrent(static_cast<float>(max_current)))
        return "Success";
    else return "Error_OutOfRange";
};

std::string evse_manager_energy_controlImpl::handle_switch_three_phases_while_charging(bool& three_phases){
    // FIXME implement more sophisticated error code return once feature is really implemented
    if (mod->charger->switchThreePhasesWhileCharging(three_phases)) return "Success";
    else return "Error_NotSupported";
};

} // namespace evse_energy_control
} // namespace module
