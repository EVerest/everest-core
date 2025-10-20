// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef INFY_POWER_HPP
#define INFY_POWER_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/power_supply_DC/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
#include "can_driver_acdc/InfyCanDevice.hpp"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string can_device;
    std::string module_addresses;
    int group_address;
    int device_connection_timeout_s;
    double conversion_efficiency_export;
    int controller_address;
};

class InfyPower : public Everest::ModuleBase {
public:
    InfyPower() = delete;
    InfyPower(const ModuleInfo& info, std::unique_ptr<power_supply_DCImplBase> p_main, Conf& config) :
        ModuleBase(info), p_main(std::move(p_main)), config(config){};

    const std::unique_ptr<power_supply_DCImplBase> p_main;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    std::unique_ptr<InfyCanDevice> acdc;
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

#endif // INFY_POWER_HPP
