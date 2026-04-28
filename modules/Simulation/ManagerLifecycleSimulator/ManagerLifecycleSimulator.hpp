// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef MANAGERLIFECYCLESIMULATOR_HPP
#define MANAGERLIFECYCLESIMULATOR_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

namespace module {

struct Conf {};

class ManagerLifecycleSimulator : public Everest::ModuleBase {
public:
    ManagerLifecycleSimulator() = delete;
    ManagerLifecycleSimulator(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                              std::unique_ptr<emptyImplBase> p_main, Conf& config) :
        ModuleBase(info), mqtt(mqtt_provider), p_main(std::move(p_main)), config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<emptyImplBase> p_main;
    const Conf& config;

private:
    friend class LdEverest;
    void init();
    void ready();
    void shutdown();
};

} // namespace module

#endif // MANAGERLIFECYCLESIMULATOR_HPP
