// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef EXTERNAL_ENERGY_LIMITS_CONSUMER_API_HPP
#define EXTERNAL_ENERGY_LIMITS_CONSUMER_API_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/generic_error/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/external_energy_limits/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <everest_api_types/utilities/CommCheckHandler.hpp>
#include <everest_api_types/utilities/Topics.hpp>

namespace ev_API = everest::lib::API;

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    int cfg_communication_check_to_s;
    int cfg_heartbeat_interval_ms;
};

class external_energy_limits_consumer_API : public Everest::ModuleBase {
public:
    external_energy_limits_consumer_API() = delete;
    external_energy_limits_consumer_API(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                                        std::unique_ptr<generic_errorImplBase> p_main,
                                        std::unique_ptr<external_energy_limitsIntf> r_energy_node, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        r_energy_node(std::move(r_energy_node)),
        config(config),
        comm_check("generic/CommunicationFault", "Bridge to implementation connection lost", this->p_main) {};

    Everest::MqttProvider& mqtt;
    const std::shared_ptr<generic_errorImplBase> p_main;
    const std::unique_ptr<external_energy_limitsIntf> r_energy_node;
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
    auto forward_api_var(std::string const& var);
    using ParseAndPublishFtor = std::function<bool(std::string const&)>;
    void subscribe_api_topic(const std::string& var, const ParseAndPublishFtor& parse_and_publish);

    void generate_api_var_enforced_limits();
    void generate_api_var_communication_check();
    void generate_api_cmd_set_external_limits();

    void setup_heartbeat_generator();

    ev_API::Topics topics;
    ev_API::CommCheckHandler<generic_errorImplBase> comm_check;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // EXTERNAL_ENERGY_LIMITS_CONSUMER_API_HPP
