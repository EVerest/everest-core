// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef ENERGY_GRID_ENERGY_IMPL_HPP
#define ENERGY_GRID_ENERGY_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 1
//

#include <generated/energy/Implementation.hpp>

#include "../EnergyNode.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace energy_grid {

struct Conf {};

class energyImpl : public energyImplBase {
public:
    energyImpl() = delete;
    energyImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<EnergyNode>& mod, Conf& config) :
        energyImplBase(ev, "energy_grid"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_enforce_limits(std::string& uuid, Object& limits_import, Object& limits_export,
                                       Array& schedule_import, Array& schedule_export) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<EnergyNode>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    // contains our own data and children
    json energy;
    // contains only the pricing informations last update
    json energy_price;
    // contains latest power meter update
    json powermeter;
    void publish_complete_energy_object();
    json merge_price_into_schedule(json s, json price);
    void initializeEnergyObject();
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace energy_grid
} // namespace module

#endif // ENERGY_GRID_ENERGY_IMPL_HPP
