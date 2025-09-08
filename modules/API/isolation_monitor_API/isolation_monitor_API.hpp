// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef ISOLATION_MONITOR_API_HPP
#define ISOLATION_MONITOR_API_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/isolation_monitor/Implementation.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <companion/paths/Topics.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic pop
#include "basecamp/isolation_monitor/API.hpp"
#include "companion/utilities/CommCheckHandler.hpp"

namespace ns_bc = basecamp::companion;
namespace ns_types_ext = basecamp::API::V1_0::types::isolation_monitor;
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string cfg_target_module_id;
    int cfg_communication_check_to_s;
    int cfg_heartbeat_interval_ms;
};

class isolation_monitor_API : public Everest::ModuleBase {
public:
    isolation_monitor_API() = delete;
    isolation_monitor_API(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                          std::unique_ptr<isolation_monitorImplBase> p_if_isolation_monitor, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_if_isolation_monitor(std::move(p_if_isolation_monitor)),
        config(config),
        comm_check("isolation_monitor/CommunicationFault", "Bridge to implementation connection lost",
                   this->p_if_isolation_monitor){};

    Everest::MqttProvider& mqtt;
    const std::shared_ptr<isolation_monitorImplBase> p_if_isolation_monitor;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    const ns_bc::Topics& get_topics() const;
    ns_bc::CommCheckHandler<isolation_monitorImplBase> comm_check;

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
    void subscribe_api_var(const std::string& var, const ParseAndPublishFtor& parse_and_publish);
    void generate_api_var_isolation_measurement();
    void generate_api_var_self_test_result();
    void generate_api_var_raise_error();
    void generate_api_var_clear_error();
    void generate_api_var_communication_check();

    std::string make_error_string(ns_types_ext::Error const& error);

    void setup_heartbeat_generator();

    ns_bc::Topics topics;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // ISOLATION_MONITOR_API_HPP
