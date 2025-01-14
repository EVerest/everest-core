// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef SLAC_SIMULATOR_HPP
#define SLAC_SIMULATOR_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/ev_slac/Implementation.hpp>
#include <generated/interfaces/slac/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "util/state.hpp"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {};

class SlacSimulator : public Everest::ModuleBase {
public:
    SlacSimulator() = delete;
    SlacSimulator(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider, std::unique_ptr<slacImplBase> p_evse,
                  std::unique_ptr<ev_slacImplBase> p_ev, Conf& config) :
        ModuleBase(info), mqtt(mqtt_provider), p_evse(std::move(p_evse)), p_ev(std::move(p_ev)), config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<slacImplBase> p_evse;
    const std::unique_ptr<ev_slacImplBase> p_ev;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    std::size_t cntmatching{0};
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
    void run();

    static constexpr size_t loop_interval_ms{250};
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // SLAC_SIMULATOR_HPP
