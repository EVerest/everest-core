#ifndef EVSE_ENERGY_CONTROL_EVSE_MANAGER_ENERGY_CONTROL_IMPL_HPP
#define EVSE_ENERGY_CONTROL_EVSE_MANAGER_ENERGY_CONTROL_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 0.0.1
//

#include <generated/evse_manager_energy_control/Implementation.hpp>

#include "../EvseManager.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace evse_energy_control {

struct Conf {
};

class evse_manager_energy_controlImpl : public evse_manager_energy_controlImplBase {
public:
    evse_manager_energy_controlImpl() = delete;
    evse_manager_energy_controlImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<EvseManager> &mod, Conf& config) :
        evse_manager_energy_controlImplBase(ev, "evse_energy_control"),
        mod(mod),
        config(config)
    {};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual std::string handle_set_max_current(double& max_current) override;
    virtual std::string handle_switch_three_phases_while_charging(bool& three_phases) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<EvseManager>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace evse_energy_control
} // namespace module

#endif // EVSE_ENERGY_CONTROL_EVSE_MANAGER_ENERGY_CONTROL_IMPL_HPP
