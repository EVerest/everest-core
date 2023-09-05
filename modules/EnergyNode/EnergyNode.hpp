// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef ENERGY_NODE_HPP
#define ENERGY_NODE_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/energy/Implementation.hpp>
#include <generated/interfaces/external_energy_limits/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/energy/Interface.hpp>
#include <generated/interfaces/energy_price_information/Interface.hpp>
#include <generated/interfaces/powermeter/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <sigslot/signal.hpp>
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    double fuse_limit_A;
    int phase_count;
};

class EnergyNode : public Everest::ModuleBase {
public:
    EnergyNode() = delete;
    EnergyNode(const ModuleInfo& info, std::unique_ptr<energyImplBase> p_energy_grid,
               std::unique_ptr<external_energy_limitsImplBase> p_external_limits,
               std::vector<std::unique_ptr<energyIntf>> r_energy_consumer,
               std::vector<std::unique_ptr<powermeterIntf>> r_powermeter,
               std::vector<std::unique_ptr<energy_price_informationIntf>> r_price_information, Conf& config) :
        ModuleBase(info),
        p_energy_grid(std::move(p_energy_grid)),
        p_external_limits(std::move(p_external_limits)),
        r_energy_consumer(std::move(r_energy_consumer)),
        r_powermeter(std::move(r_powermeter)),
        r_price_information(std::move(r_price_information)),
        config(config){};

    const std::unique_ptr<energyImplBase> p_energy_grid;
    const std::unique_ptr<external_energy_limitsImplBase> p_external_limits;
    const std::vector<std::unique_ptr<energyIntf>> r_energy_consumer;
    const std::vector<std::unique_ptr<powermeterIntf>> r_powermeter;
    const std::vector<std::unique_ptr<energy_price_informationIntf>> r_price_information;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    sigslot::signal<types::energy::ExternalLimits&> signalExternalLimit;
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

#endif // ENERGY_NODE_HPP
