// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MAIN_SLAC_IMPL_HPP
#define MAIN_SLAC_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/slac/Implementation.hpp>

#include "../EvseSlac.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {
    std::string device;
    std::string evse_id;
    std::string nid;
    int number_of_sounds;
    bool ac_mode_five_percent;
    int set_key_timeout_ms;
    int sounding_attenuation_adjustment;
    bool publish_mac_on_match_cnf;
    bool publish_mac_on_first_parm_req;
    bool do_chip_reset;
    int chip_reset_delay_ms;
    int chip_reset_timeout_ms;
    bool link_status_detection;
    int link_status_retry_ms;
    int link_status_timeout_ms;
    bool debug_simulate_failed_matching;
};

class slacImpl : public slacImplBase {
public:
    slacImpl() = delete;
    slacImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<EvseSlac>& mod, Conf& config) :
        slacImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_reset(bool& enable) override;
    virtual bool handle_enter_bcd() override;
    virtual bool handle_leave_bcd() override;
    virtual bool handle_dlink_terminate() override;
    virtual bool handle_dlink_error() override;
    virtual bool handle_dlink_pause() override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<EvseSlac>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    void run();
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_SLAC_IMPL_HPP
