// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef D20EVSE_HPP
#define D20EVSE_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/ISO15118_charger/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string device;
    std::string certificate_path;
    std::string logging_path;
    std::string tls_negotiation_strategy;
    bool enable_ssl_logging;
};

class D20Evse : public Everest::ModuleBase {
public:
    D20Evse() = delete;
    D20Evse(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
            std::unique_ptr<ISO15118_chargerImplBase> p_charger, Conf& config) :
        ModuleBase(info), mqtt(mqtt_provider), p_charger(std::move(p_charger)), config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<ISO15118_chargerImplBase> p_charger;
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

#endif // D20EVSE_HPP
