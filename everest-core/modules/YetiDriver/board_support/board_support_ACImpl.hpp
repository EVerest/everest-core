#ifndef BOARD_SUPPORT_BOARD_SUPPORT_AC_IMPL_HPP
#define BOARD_SUPPORT_BOARD_SUPPORT_AC_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 0.0.1
//

#include <generated/board_support_AC/Implementation.hpp>

#include "../YetiDriver.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace board_support {

struct Conf {
};

class board_support_ACImpl : public board_support_ACImplBase {
public:
    board_support_ACImpl() = delete;
    board_support_ACImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<YetiDriver> &mod, Conf& config) :
        board_support_ACImplBase(ev, "board_support"),
        mod(mod),
        config(config)
    {};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_setup(bool& three_phases, bool& has_ventilation, std::string& country_code, bool& rcd_enabled) override;
    virtual void handle_enable(bool& value) override;
    virtual void handle_pwm_on(double& value) override;
    virtual void handle_pwm_off() override;
    virtual void handle_pwm_F() override;
    virtual void handle_allow_power_on(bool& value) override;
    virtual void handle_force_unlock() override;
    virtual void handle_switch_three_phases_while_charging(bool& value) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<YetiDriver>& mod;
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

} // namespace board_support
} // namespace module

#endif // BOARD_SUPPORT_BOARD_SUPPORT_AC_IMPL_HPP
