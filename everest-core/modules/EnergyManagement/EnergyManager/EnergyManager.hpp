// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef ENERGY_MANAGER_HPP
#define ENERGY_MANAGER_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/energy_manager/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/energy/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "EnergyManagerImpl.hpp"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    double nominal_ac_voltage;
    int update_interval;
    int schedule_interval_duration;
    int schedule_total_duration;
    double slice_ampere;
    double slice_watt;
    bool debug;
    std::string switch_3ph1ph_while_charging_mode;
    int switch_3ph1ph_max_nr_of_switches_per_session;
    std::string switch_3ph1ph_switch_limit_stickyness;
    int switch_3ph1ph_power_hysteresis_W;
    int switch_3ph1ph_time_hysteresis_s;
};

class EnergyManager : public Everest::ModuleBase {
public:
    EnergyManager() = delete;
    EnergyManager(const ModuleInfo& info, std::unique_ptr<energy_managerImplBase> p_main,
                  std::unique_ptr<energyIntf> r_energy_trunk, Conf& config) :
        ModuleBase(info), p_main(std::move(p_main)), r_energy_trunk(std::move(r_energy_trunk)), config(config){};

    const std::unique_ptr<energy_managerImplBase> p_main;
    const std::unique_ptr<energyIntf> r_energy_trunk;
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
    std::unique_ptr<EnergyManagerImpl> impl;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // ENERGY_MANAGER_HPP
