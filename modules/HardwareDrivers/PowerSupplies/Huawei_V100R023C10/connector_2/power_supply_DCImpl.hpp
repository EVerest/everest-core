// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef CONNECTOR_2_POWER_SUPPLY_DC_IMPL_HPP
#define CONNECTOR_2_POWER_SUPPLY_DC_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/power_supply_DC/Implementation.hpp>

#include "../Huawei_V100R023C10.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include "../connector_base/base.hpp"
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace connector_2 {

struct Conf {
    int global_connector_number;
    double max_export_current_A;
    double max_export_power_W;
};

class power_supply_DCImpl : public power_supply_DCImplBase {
public:
    power_supply_DCImpl() = delete;
    power_supply_DCImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<Huawei_V100R023C10>& mod,
                        Conf& config) :
        power_supply_DCImplBase(ev, "connector_2"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    ConnectorBase base = ConnectorBase(1, this);
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_setMode(types::power_supply_DC::Mode& mode,
                                types::power_supply_DC::ChargingPhase& phase) override;
    virtual void handle_setExportVoltageCurrent(double& voltage, double& current) override;
    virtual void handle_setImportVoltageCurrent(double& voltage, double& current) override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<Huawei_V100R023C10>& mod;
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

} // namespace connector_2
} // namespace module

#endif // CONNECTOR_2_POWER_SUPPLY_DC_IMPL_HPP
