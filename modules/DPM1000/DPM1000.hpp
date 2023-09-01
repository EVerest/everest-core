// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef DPM1000_HPP
#define DPM1000_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/power_supply_DC/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string device;
    int device_address;
    double power_limit_W;
    double current_limit_A;
    double voltage_limit_V;
    std::string series_parallel_mode;
    std::string discharge_gpio_chip;
    int discharge_gpio_line;
    bool discharge_gpio_polarity;
    bool debug_print_all_telemetry;
};

class DPM1000 : public Everest::ModuleBase {
public:
    DPM1000() = delete;
    DPM1000(const ModuleInfo& info, std::unique_ptr<power_supply_DCImplBase> p_main, Conf& config) :
        ModuleBase(info), p_main(std::move(p_main)), config(config){};

    const std::unique_ptr<power_supply_DCImplBase> p_main;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1

protected:
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1
    // insert your protected definitions here
    // ev@4714b2ab-a24f-4b95-ab81-36439e1478de:v1

private:
    friend class LdEverest;
    void init();
    void ready();

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
    // insert your private definitions here
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // DPM1000_HPP
