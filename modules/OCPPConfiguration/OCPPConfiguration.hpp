// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef OCPPCONFIGURATION_HPP
#define OCPPCONFIGURATION_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/example_user/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/ocpp/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "main/event_handler.hpp"
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string monitor_variables;
    std::string user_config_path;
    std::string mapping_file_path;
};

class OCPPConfiguration : public Everest::ModuleBase {
public:
    OCPPConfiguration() = delete;
    OCPPConfiguration(const ModuleInfo& info, std::unique_ptr<example_userImplBase> p_example_module,
                      std::unique_ptr<ocppIntf> r_ocpp_module, Conf& config) :
        ModuleBase(info),
        p_example_module(std::move(p_example_module)),
        r_ocpp_module(std::move(r_ocpp_module)),
        config(config){};

    const std::unique_ptr<example_userImplBase> p_example_module;
    const std::unique_ptr<ocppIntf> r_ocpp_module;
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
    std::vector<types::ocpp::ComponentVariable> parseConfigMonitorVariables();

    std::unique_ptr<EventHandler> event_handler;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // OCPPCONFIGURATION_HPP
