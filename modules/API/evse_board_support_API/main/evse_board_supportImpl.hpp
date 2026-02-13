// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MAIN_EVSE_BOARD_SUPPORT_IMPL_HPP
#define MAIN_EVSE_BOARD_SUPPORT_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/evse_board_support/Implementation.hpp>

#include "../evse_board_support_API.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here

#include <everest_api_types/evse_board_support/API.hpp>
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {};

class evse_board_supportImpl : public evse_board_supportImplBase {
public:
    evse_board_supportImpl() = delete;
    evse_board_supportImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<evse_board_support_API>& mod,
                           Conf& config) :
        evse_board_supportImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_enable(bool& value) override;
    virtual void handle_pwm_on(double& value) override;
    virtual void handle_cp_state_X1() override;
    virtual void handle_cp_state_F() override;
    virtual void handle_cp_state_E() override;
    virtual void handle_allow_power_on(types::evse_board_support::PowerOnOff& value) override;
    virtual void handle_ac_switch_three_phases_while_charging(bool& value) override;
    virtual void handle_evse_replug(int& value) override;
    virtual void handle_ac_set_overcurrent_limit_A(double& value) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<evse_board_support_API>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    template <class T, class ReqT>
    auto generic_request_reply(T const& default_value, ReqT const& request, std::string const& topic);

    int timeout_s{5};
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_EVSE_BOARD_SUPPORT_IMPL_HPP
