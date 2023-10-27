// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef TEST_POWERMETER_HPP
#define TEST_POWERMETER_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/powermeter/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "../PowermeterGSH01/gsh01_app_layer.hpp"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {};

class TestPowermeter : public Everest::ModuleBase {
public:
    TestPowermeter() = delete;
    TestPowermeter(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                   std::unique_ptr<emptyImplBase> p_if_impl_id_empty, std::unique_ptr<powermeterIntf> r_powermeter,
                   Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_if_impl_id_empty(std::move(p_if_impl_id_empty)),
        r_powermeter(std::move(r_powermeter)),
        config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<emptyImplBase> p_if_impl_id_empty;
    const std::unique_ptr<powermeterIntf> r_powermeter;
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
    float voltage;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // TEST_POWERMETER_HPP
