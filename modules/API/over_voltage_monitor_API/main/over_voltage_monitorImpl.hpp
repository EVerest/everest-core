// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef MAIN_OVER_VOLTAGE_MONITOR_IMPL_HPP
#define MAIN_OVER_VOLTAGE_MONITOR_IMPL_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 3
//

#include <generated/interfaces/over_voltage_monitor/Implementation.hpp>

#include "../over_voltage_monitor_API.hpp"

// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1
// insert your custom include headers here
// ev@75ac1216-19eb-4182-a85c-820f1fc2c091:v1

namespace module {
namespace main {

struct Conf {};

class over_voltage_monitorImpl : public over_voltage_monitorImplBase {
public:
    over_voltage_monitorImpl() = delete;
    over_voltage_monitorImpl(Everest::ModuleAdapter* ev, const Everest::PtrContainer<over_voltage_monitor_API>& mod,
                             Conf& config) :
        over_voltage_monitorImplBase(ev, "main"), mod(mod), config(config){};

    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1
    // insert your public definitions here
    // ev@8ea32d28-373f-4c90-ae5e-b4fcc74e2a61:v1

protected:
    // command handler functions (virtual)
    virtual void handle_set_limits(double& emergency_over_voltage_limit_V, double& error_over_voltage_limit_V) override;
    virtual void handle_start() override;
    virtual void handle_stop() override;
    virtual void handle_reset_over_voltage_error() override;

    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1
    // insert your protected definitions here
    // ev@d2d1847a-7b88-41dd-ad07-92785f06f5c4:v1

private:
    const Everest::PtrContainer<over_voltage_monitor_API>& mod;
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

} // namespace main
} // namespace module

#endif // MAIN_OVER_VOLTAGE_MONITOR_IMPL_HPP
