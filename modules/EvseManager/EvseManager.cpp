#include "EvseManager.hpp"

namespace module {

void EvseManager::init() {
    invoke_init(*p_evse);       
}

void EvseManager::ready() {
    charger=std::unique_ptr<Charger>(new Charger(r_bsp));
    r_bsp->subscribe_event([this](std::string event) {
        charger->processEvent(event);
    });

    r_powermeter->subscribe_powermeter([this](json p) {
        charger->setCurrentDrawnByVehicle(p["current_A"]["L1"], p["current_A"]["L2"], p["current_A"]["L3"]);
    });

    charger->setup(config.three_phases, config.has_ventilation, config.country_code, config.rcd_enabled);
    invoke_ready(*p_evse);
    charger->run();
    charger->enable();
}

} // namespace module
