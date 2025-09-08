// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef OVER_VOLTAGE_MONITOR_HPP
#define OVER_VOLTAGE_MONITOR_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <generated/interfaces/over_voltage_monitor/Implementation.hpp>
#pragma GCC diagnostic pop

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "basecamp/over_voltage_monitor/API.hpp"
#include "companion/utilities/CommCheckHandler.hpp"
#include <companion/paths/Topics.hpp>

//
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {
namespace ns_types_ext = basecamp::API::V1_0::types::over_voltage_monitor;
namespace ns_bc = basecamp::companion;

struct Conf {
    int cfg_communication_check_to_s;
    int cfg_heartbeat_interval_ms;
};

class over_voltage_monitor_API : public Everest::ModuleBase {
public:
    over_voltage_monitor_API() = delete;
    over_voltage_monitor_API(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                             std::unique_ptr<over_voltage_monitorImplBase> p_main, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        config(config),
        comm_check("over_voltage_monitor/CommunicationFault", "Bridge to implementation connection lost",
                   this->p_main){};

    Everest::MqttProvider& mqtt;
    const std::shared_ptr<over_voltage_monitorImplBase> p_main;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    const ns_bc::Topics& get_topics() const;
    ns_bc::CommCheckHandler<over_voltage_monitorImplBase> comm_check;
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
    using ParseAndPublishFtor = std::function<bool(std::string const&)>;
    void subscribe_api_topic(const std::string& var, ParseAndPublishFtor const& parse_and_publish);

    void generate_api_var_communication_check();
    void generate_api_var_raise_error();
    void generate_api_var_clear_error();
    void generate_api_var_voltage_measurement_V();

    std::string make_error_string(ns_types_ext::Error const& error);

    void setup_heartbeat_generator();
    ns_bc::Topics topics;

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // OVER_VOLTAGE_MONITOR_HPP
