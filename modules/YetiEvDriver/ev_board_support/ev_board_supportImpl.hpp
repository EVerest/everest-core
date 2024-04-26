// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EV_BOARD_SUPPORT_EV_BOARD_SUPPORT_IMPL_HPP
#define EV_BOARD_SUPPORT_EV_BOARD_SUPPORT_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/ev_board_support/Implementation.hpp>

#include "../YetiEvDriver.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace ev_board_support {

struct Conf {};

class ev_board_supportImpl : public ev_board_supportImplBase {
public:
    ev_board_supportImpl() = delete;
    ev_board_supportImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<YetiEvDriver>& mod, Conf& config) :
        ev_board_supportImplBase(ev, "ev_board_support"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_enable(bool& value) override;
    virtual void handle_set_cp_state(types::ev_board_support::EvCpState& cp_state) override;
    virtual void handle_allow_power_on(bool& value) override;
    virtual void handle_diode_fail(bool& value) override;
    virtual void handle_set_ac_max_current(double& current) override;
    virtual void handle_set_three_phases(bool& three_phases) override;
    virtual void handle_set_rcd_error(double& rcd_current_mA) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<YetiEvDriver>& mod;
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

} // namespace ev_board_support
} // namespace module

#endif // EV_BOARD_SUPPORT_EV_BOARD_SUPPORT_IMPL_HPP
