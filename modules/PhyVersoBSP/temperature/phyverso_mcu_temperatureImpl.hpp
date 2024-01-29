// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef TEMPERATURE_MAIN_IMPL_HPP
#define TEMPERATURE_MAIN_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/phyverso_mcu_temperature/Implementation.hpp>

#include "../PhyVersoBSP.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace temperature {

struct Conf {};

class phyverso_mcu_temperatureImpl : public phyverso_mcu_temperatureImplBase {
public:
    phyverso_mcu_temperatureImpl() = delete;
    phyverso_mcu_temperatureImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<PhyVersoBSP>& mod,
                                 Conf& config) :
        phyverso_mcu_temperatureImplBase(ev, "phyverso_mcu_temperature"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

private:
    const Everest::PtrContainer<PhyVersoBSP>& mod;
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

} // namespace temperature
} // namespace module

#endif // TEMPERATURE_MAIN_IMPL_HPP
