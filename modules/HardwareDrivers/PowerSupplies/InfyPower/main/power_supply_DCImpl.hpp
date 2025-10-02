// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MAIN_POWER_SUPPLY_DC_IMPL_HPP
#define MAIN_POWER_SUPPLY_DC_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/power_supply_DC/Implementation.hpp>

#include "../InfyPower.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
#include <atomic>
#include <memory>
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {};

class power_supply_DCImpl : public power_supply_DCImplBase {
public:
    power_supply_DCImpl() = delete;
    power_supply_DCImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<InfyPower>& mod, Conf& config) :
        power_supply_DCImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
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
    const Everest::PtrContainer<InfyPower>& mod;
    const Conf& config;

    virtual void init() override;
    virtual void ready() override;

    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
    std::atomic<types::power_supply_DC::Mode> mode{types::power_supply_DC::Mode::Off};
    std::atomic<types::power_supply_DC::ChargingPhase> phase{types::power_supply_DC::ChargingPhase::CableCheck};
    std::atomic<double> exportVoltage{0.};
    std::atomic<double> exportCurrentLimit{0.};
    std::atomic<double> minImportVoltage{0.};
    std::atomic<double> importCurrentLimit{0.};
    types::power_supply_DC::Capabilities caps;

    bool firsttime{true};
    uint8_t last_module_count{0};
    uint8_t throttle_cnt{0};

    // Error handling helpers
    std::string map_infy_error_to_power_supply_dc(InfyCanDevice::Error error);
    std::string create_error_message(uint8_t module_address, InfyCanDevice::Error error, bool active) const;
    // ev@3370e4dd-95f4-47a9-aaec-ea76f34a66c9:v1
};

// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1
// insert other definitions here
// ev@3d7da0ad-02c2-493d-9920-0bbbd56b9876:v1

} // namespace main
} // namespace module

#endif // MAIN_POWER_SUPPLY_DC_IMPL_HPP
