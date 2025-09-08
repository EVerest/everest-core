// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef GENERIC_ERROR_RAISER_API_HPP
#define GENERIC_ERROR_RAISER_API_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"
#include "utils/error.hpp"
#include <atomic>
// headers for provided interface implementations
#include <companion/paths/Topics.hpp>
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include <generated/interfaces/generic_error/Implementation.hpp>
#pragma GCC diagnostic pop

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
#include "basecamp/generic/API.hpp"
#include "companion/utilities/CommCheckHandler.hpp"
namespace ns_bc = basecamp::companion;
namespace generic = basecamp::API::V1_0::types::generic;

namespace module {

struct Conf {
    int cfg_communication_check_to_s;
    int cfg_heartbeat_interval_ms;
};

class generic_error_raiser_API : public Everest::ModuleBase {
public:
    generic_error_raiser_API() = delete;
    generic_error_raiser_API(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                             std::unique_ptr<generic_errorImplBase> p_main, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        config(config),
        comm_check("generic/CommunicationFault", "Bridge to implementation connection lost", this->p_main){};

    Everest::MqttProvider& mqtt;
    const std::shared_ptr<generic_errorImplBase> p_main;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    using ParseAndPublishFtor = std::function<bool(std::string const&)>;
    void subscribe_api_topic(const std::string& var, ParseAndPublishFtor const& parse_and_publish);
    std::string make_error_string(generic::Error const& error);

    void generate_api_var_communication_check();
    void generate_api_var_raise_error();
    void generate_api_var_clear_error();

    void setup_heartbeat_generator();

    ns_bc::Topics topics;
    ns_bc::CommCheckHandler<generic_errorImplBase> comm_check;
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

#endif // GENERIC_ERROR_RAISER_API_HPP
