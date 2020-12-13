#ifndef EVSE_MANAGER_HPP
#define EVSE_MANAGER_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 0.0.1
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/evse_manager/Implementation.hpp>
#include <generated/evse_manager_energy_control/Implementation.hpp>

// headers for required interface implementations
#include <generated/board_support_AC/Interface.hpp>
#include <generated/powermeter/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
#include "Charger.hpp"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    bool three_phases;
    bool has_ventilation;
    std::string country_code;
    bool rcd_enabled;
};

class EvseManager : public Everest::ModuleBase {
public:
    EvseManager() = delete;
    EvseManager(
        Everest::MqttProvider& mqtt_provider,
        std::unique_ptr<evse_managerImplBase> p_evse,
        std::unique_ptr<evse_manager_energy_controlImplBase> p_evse_energy_control,
        std::unique_ptr<board_support_ACIntf> r_bsp,
        std::unique_ptr<powermeterIntf> r_powermeter,
        Conf& config
    ) :
        mqtt(mqtt_provider),
        p_evse(std::move(p_evse)),
        p_evse_energy_control(std::move(p_evse_energy_control)),
        r_bsp(std::move(r_bsp)),
        r_powermeter(std::move(r_powermeter)),
        config(config)
    {};

    const Conf& config;
    Everest::MqttProvider& mqtt;
    const std::unique_ptr<evse_managerImplBase> p_evse;
    const std::unique_ptr<evse_manager_energy_controlImplBase> p_evse_energy_control;
    const std::unique_ptr<board_support_ACIntf> r_bsp;
    const std::unique_ptr<powermeterIntf> r_powermeter;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    std::unique_ptr<Charger> charger;
    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1

protected:
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1
    // insert your protected definitions here
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1

private:
    friend class LdEverest;
    void init();
    void ready();

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1

};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // EVSE_MANAGER_HPP
