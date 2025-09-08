// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef AUTH_CONSUMER_API_HPP
#define AUTH_CONSUMER_API_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <generated/interfaces/generic_error/Implementation.hpp>
#pragma GCC diagnostic pop

// headers for required interface implementations
#include <generated/interfaces/auth/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "companion/paths/Topics.hpp"
#include "companion/utilities/CommCheckHandler.hpp"

namespace ns_bc = basecamp::companion;

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    int cfg_communication_check_to_s;
    int cfg_heartbeat_interval_ms;
};

class auth_consumer_API : public Everest::ModuleBase {
public:
    auth_consumer_API() = delete;
    auth_consumer_API(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                      std::unique_ptr<generic_errorImplBase> p_main, std::unique_ptr<authIntf> r_auth, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        r_auth(std::move(r_auth)),
        config(config),
        comm_check("generic/CommunicationFault", "Bridge to implementation connection lost", this->p_main){};

    Everest::MqttProvider& mqtt;
    const std::shared_ptr<generic_errorImplBase> p_main;
    const std::unique_ptr<authIntf> r_auth;
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

    void generate_api_cmd_withdraw_authorization();

    void generate_api_var_token_validation_status();
    void generate_api_var_communication_check();

    void setup_heartbeat_generator();

    ns_bc::Topics topics;
    ns_bc::CommCheckHandler<generic_errorImplBase> comm_check;

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // AUTH_CONSUMER_API_HPP
