// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EVSE_SLAC_IMPL_HPP
#define EVSE_SLAC_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/slac/Implementation.hpp>

#include "../SlacSimulator.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include "../util/state.hpp"
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace evse {

struct Conf {};

class slacImpl : public slacImplBase {
public:
    slacImpl() = delete;
    slacImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<SlacSimulator>& mod, Conf& config) :
        slacImplBase(ev, "evse"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    util::State get_state() const;
    void set_state_matched();
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
    const Everest::PtrContainer<SlacSimulator>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // insert your private definitions here
    void set_state_to_unmatched();
    void set_state_to_matching();

    util::State state;
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace evse
} // namespace module

#endif // EVSE_SLAC_IMPL_HPP
