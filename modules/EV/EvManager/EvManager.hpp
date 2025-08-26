// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EV_MANAGER_HPP
#define EV_MANAGER_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/car_simulator/Implementation.hpp>
#include <generated/interfaces/ev_manager/Implementation.hpp>

// headers for required interface implementations
#include "generated/interfaces/kvs/Interface.hpp"
#include <generated/interfaces/ISO15118_ev/Interface.hpp>
#include <generated/interfaces/ev_board_support/Interface.hpp>
#include <generated/interfaces/ev_slac/Interface.hpp>
#include <generated/interfaces/kvs/Interface.hpp>
#include <generated/interfaces/powermeter/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    int connector_id;
    bool auto_enable;
    bool auto_exec;
    bool auto_exec_infinite;
    std::string auto_exec_commands;
    double ac_nominal_voltage;
    int dc_max_current_limit;
    int dc_max_power_limit;
    int dc_max_voltage_limit;
    int dc_energy_capacity;
    int dc_target_current;
    int dc_target_voltage;
    bool support_sae_j2847;
    int dc_discharge_max_current_limit;
    int dc_discharge_max_power_limit;
    int dc_discharge_target_current;
    int dc_discharge_v2g_minimal_soc;
    double max_current;
    bool three_phases;
    int departure_time;
    int e_amount;
    int soc;
    bool keep_cross_boot_plugin_state;
};

class EvManager : public Everest::ModuleBase {
public:
    EvManager() = delete;
    EvManager(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
              std::unique_ptr<car_simulatorImplBase> p_main, std::unique_ptr<ev_managerImplBase> p_ev_manager,
              std::unique_ptr<ev_board_supportIntf> r_ev_board_support,
              std::vector<std::unique_ptr<ISO15118_evIntf>> r_ev, std::vector<std::unique_ptr<ev_slacIntf>> r_slac,
              std::vector<std::unique_ptr<powermeterIntf>> r_powermeter, std::vector<std::unique_ptr<kvsIntf>> r_kvs,
              Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        p_ev_manager(std::move(p_ev_manager)),
        r_ev_board_support(std::move(r_ev_board_support)),
        r_ev(std::move(r_ev)),
        r_slac(std::move(r_slac)),
        r_powermeter(std::move(r_powermeter)),
        r_kvs(std::move(r_kvs)),
        config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<car_simulatorImplBase> p_main;
    const std::unique_ptr<ev_managerImplBase> p_ev_manager;
    const std::unique_ptr<ev_board_supportIntf> r_ev_board_support;
    const std::vector<std::unique_ptr<ISO15118_evIntf>> r_ev;
    const std::vector<std::unique_ptr<ev_slacIntf>> r_slac;
    const std::vector<std::unique_ptr<powermeterIntf>> r_powermeter;
    const std::vector<std::unique_ptr<kvsIntf>> r_kvs;
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

#endif // EV_MANAGER_HPP
