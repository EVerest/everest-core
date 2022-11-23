// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef AC_METER_SUNSPEC_AC_METER_IMPL_HPP
#define AC_METER_SUNSPEC_AC_METER_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/sunspec_ac_meter/Implementation.hpp>

#include "../PowermeterBSM.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace ac_meter {

struct Conf {};

class sunspec_ac_meterImpl : public sunspec_ac_meterImplBase {
public:
    sunspec_ac_meterImpl() = delete;
    sunspec_ac_meterImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<PowermeterBSM>& mod, Conf& config) :
        sunspec_ac_meterImplBase(ev, "ac_meter"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual types::sunspec_ac_meter::Result handle_get_sunspec_ac_meter_value(std::string& auth_token) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<PowermeterBSM>& mod;
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

} // namespace ac_meter
} // namespace module

#endif // AC_METER_SUNSPEC_AC_METER_IMPL_HPP
