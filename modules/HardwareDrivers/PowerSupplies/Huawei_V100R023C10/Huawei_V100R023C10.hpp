// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef HUAWEI_V100R023C10_HPP
#define HUAWEI_V100R023C10_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/power_supply_DC/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/evse_board_support/Interface.hpp>
#include <generated/interfaces/isolation_monitor/Interface.hpp>
#include <generated/interfaces/over_voltage_monitor/Interface.hpp>
#include <generated/interfaces/powermeter/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <dispenser.hpp>
#include <vector>
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string ethernet_interface;
    std::string psu_ip;
    int psu_port;
    bool tls_enabled;
    std::string psu_ca_cert;
    std::string client_cert;
    std::string client_key;
    int module_placeholder_allocation_timeout_s;
    std::string esn;
    bool HACK_publish_requested_voltage_current;
    bool HACK_use_ovm_while_cable_check;
    bool send_secure_goose;
    bool allow_insecure_goose;
    bool verify_secure_goose;
    std::string upstream_voltage_source;
};

class Huawei_V100R023C10 : public Everest::ModuleBase {
public:
    Huawei_V100R023C10() = delete;
    Huawei_V100R023C10(const ModuleInfo& info, std::unique_ptr<power_supply_DCImplBase> p_connector_1,
                       std::unique_ptr<power_supply_DCImplBase> p_connector_2,
                       std::unique_ptr<power_supply_DCImplBase> p_connector_3,
                       std::unique_ptr<power_supply_DCImplBase> p_connector_4,
                       std::vector<std::unique_ptr<evse_board_supportIntf>> r_board_support,
                       std::vector<std::unique_ptr<isolation_monitorIntf>> r_isolation_monitor,
                       std::vector<std::unique_ptr<powermeterIntf>> r_carside_powermeter,
                       std::vector<std::unique_ptr<over_voltage_monitorIntf>> r_over_voltage_monitor, Conf& config) :
        ModuleBase(info),
        p_connector_1(std::move(p_connector_1)),
        p_connector_2(std::move(p_connector_2)),
        p_connector_3(std::move(p_connector_3)),
        p_connector_4(std::move(p_connector_4)),
        r_board_support(std::move(r_board_support)),
        r_isolation_monitor(std::move(r_isolation_monitor)),
        r_carside_powermeter(std::move(r_carside_powermeter)),
        r_over_voltage_monitor(std::move(r_over_voltage_monitor)),
        config(config){};

    const std::unique_ptr<power_supply_DCImplBase> p_connector_1;
    const std::unique_ptr<power_supply_DCImplBase> p_connector_2;
    const std::unique_ptr<power_supply_DCImplBase> p_connector_3;
    const std::unique_ptr<power_supply_DCImplBase> p_connector_4;
    const std::vector<std::unique_ptr<evse_board_supportIntf>> r_board_support;
    const std::vector<std::unique_ptr<isolation_monitorIntf>> r_isolation_monitor;
    const std::vector<std::unique_ptr<powermeterIntf>> r_carside_powermeter;
    const std::vector<std::unique_ptr<over_voltage_monitorIntf>> r_over_voltage_monitor;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    /**
     * @brief Number of connectors that are really used and initialized
     */
    std::uint16_t number_of_connectors_used;
    std::unique_ptr<Dispenser> dispenser;

    std::atomic<bool> communication_fault_raised;
    std::atomic<bool> psu_not_running_raised;

    std::atomic<bool> initial_hmac_acquired;

    std::vector<power_supply_DCImplBase*> implementations = {p_connector_1.get(), p_connector_2.get(),
                                                             p_connector_3.get(), p_connector_4.get()};

    enum class UpstreamVoltageSource {
        IMD,
        OVM,
    };
    // PSU upstream voltage source
    UpstreamVoltageSource upstream_voltage_source;
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
    ErrorEventSet raised_errors;

    void acquire_initial_hmac_keys_for_all_connectors();
    void update_psu_not_running_error();
    void update_communication_errors();
    void update_vendor_errors();
    void restart_dispenser_if_needed();
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // HUAWEI_V100R023C10_HPP
