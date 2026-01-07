// SPDX-License-Identifier: Apache-2.0
// Copyright Frickly Systems GmbH
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef DOLD_RN5893_HPP
#define DOLD_RN5893_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/isolation_monitor/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/serial_communication_hub/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    int device_id;
    int self_test_timeout_s;
    bool keep_measurement_active;
    bool always_publish_measurements;
    bool timeout_release;
    double timeout_s;
    std::string broken_wire_detect;
    bool storing_insulation_fault;
    std::string switching_mode_indicator_relay;
    std::string power_supply_type;
    int response_value_alarm_kohm;
    int response_value_pre_alarm_kohm;
    std::string coupling_device;
    std::string indicator_relay_k1_function;
    std::string indicator_relay_k2_function;
    bool automatic_self_test;
};

class DoldRN5893 : public Everest::ModuleBase {
public:
    DoldRN5893() = delete;
    DoldRN5893(const ModuleInfo& info, std::unique_ptr<isolation_monitorImplBase> p_main,
               std::unique_ptr<serial_communication_hubIntf> r_serial_comm_hub, Conf& config) :
        ModuleBase(info), p_main(std::move(p_main)), r_serial_comm_hub(std::move(r_serial_comm_hub)), config(config){};

    const std::unique_ptr<isolation_monitorImplBase> p_main;
    const std::unique_ptr<serial_communication_hubIntf> r_serial_comm_hub;
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

#endif // DOLD_RN5893_HPP
