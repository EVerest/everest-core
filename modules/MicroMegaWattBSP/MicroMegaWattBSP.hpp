// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MICRO_MEGA_WATT_BSP_HPP
#define MICRO_MEGA_WATT_BSP_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/evse_board_support/Implementation.hpp>
#include <generated/interfaces/power_supply_DC/Implementation.hpp>
#include <generated/interfaces/powermeter/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "umwc_comms/evSerial.h"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string serial_port;
    int baud_rate;
    int reset_gpio;
    int dc_max_voltage;
};

class MicroMegaWattBSP : public Everest::ModuleBase {
public:
    MicroMegaWattBSP() = delete;
    MicroMegaWattBSP(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                     std::unique_ptr<power_supply_DCImplBase> p_dc_supply,
                     std::unique_ptr<powermeterImplBase> p_powermeter,
                     std::unique_ptr<evse_board_supportImplBase> p_board_support, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_dc_supply(std::move(p_dc_supply)),
        p_powermeter(std::move(p_powermeter)),
        p_board_support(std::move(p_board_support)),
        config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<power_supply_DCImplBase> p_dc_supply;
    const std::unique_ptr<powermeterImplBase> p_powermeter;
    const std::unique_ptr<evse_board_supportImplBase> p_board_support;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    evSerial serial;
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

#endif // MICRO_MEGA_WATT_BSP_HPP
