// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef AUTH_TOKEN_VALIDATOR_API_HPP
#define AUTH_TOKEN_VALIDATOR_API_HPP

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
#include <generated/interfaces/auth_token_validator/Implementation.hpp>
#pragma GCC diagnostic pop

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "everest_api_types/utilities/CommCheckHandler.hpp"
#include <everest_api_types/utilities/Topics.hpp>
namespace ns_ev_api = everest::lib::API;

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    int cfg_communication_check_to_s;
    int cfg_heartbeat_interval_ms;
    int cfg_request_reply_to_s;
};

class auth_token_validator_API : public Everest::ModuleBase {
public:
    auth_token_validator_API() = delete;
    auth_token_validator_API(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                             std::unique_ptr<auth_token_validatorImplBase> p_auth_token_validator, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_auth_token_validator(std::move(p_auth_token_validator)),
        config(config),
        comm_check("generic/CommunicationFault", "Bridge to implementation connection lost",
                   this->p_auth_token_validator){};

    Everest::MqttProvider& mqtt;
    const std::shared_ptr<auth_token_validatorImplBase> p_auth_token_validator;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    const ns_ev_api::Topics& get_topics() const;
    ns_ev_api::CommCheckHandler<auth_token_validatorImplBase> comm_check;

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
    using ParseAndPublishFtor = std::function<bool(const std::string&)>;
    void subscribe_api_var(const std::string& var, const ParseAndPublishFtor& parse_and_publish);

    void generate_api_var_communication_check();
    void generate_api_var_validation_result_update();

    void setup_heartbeat_generator();

    ns_ev_api::Topics topics;

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // AUTH_TOKEN_VALIDATOR_API_HPP
