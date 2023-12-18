// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef PERF_CLIENT_HPP
#define PERF_CLIENT_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/performance_testing/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "VarContainer.hpp"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    int number_of_commands_to_call;
    bool test_commands;
    bool test_vars;
    bool test_external_mqtt;
};

class PerfClient : public Everest::ModuleBase {
public:
    PerfClient() = delete;
    PerfClient(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider, std::unique_ptr<emptyImplBase> p_main,
               std::unique_ptr<performance_testingIntf> r_server, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        r_server(std::move(r_server)),
        config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<emptyImplBase> p_main;
    const std::unique_ptr<performance_testingIntf> r_server;
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
    void test_commands();
    void test_vars();
    void test_external_mqtt();
    std::vector<std::chrono::milliseconds> send_simple_command();

    VarContainer<types::performance_testing::PerformanceTracing> tracing_object;
    VarContainer<std::string> external_mqtt_object;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // PERF_CLIENT_HPP
