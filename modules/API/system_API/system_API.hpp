// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef SYSTEM_API_HPP
#define SYSTEM_API_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include <generated/interfaces/system/Implementation.hpp>
#pragma GCC diagnostic pop

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
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

class system_API : public Everest::ModuleBase {
public:
    system_API() = delete;
    system_API(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
               std::unique_ptr<systemImplBase> p_if_system, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_if_system(std::move(p_if_system)),
        config(config),
        comm_check("system/CommunicationFault", "Bridge to implementation connection lost", this->p_if_system){};

    Everest::MqttProvider& mqtt;
    const std::shared_ptr<systemImplBase> p_if_system;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    const ns_ev_api::Topics& get_topics() const;

    ns_ev_api::CommCheckHandler<systemImplBase> comm_check;

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
    using ParseAndPublishFtor = std::function<void(std::string const&)>;
    void subscribe_api_var(const std::string& var, const ParseAndPublishFtor& parse_and_publish);

    void generate_api_var_firmware_update_status();
    void generate_api_var_log_status();

    void generate_api_var_communication_check();

    void setup_heartbeat_generator();

    ns_ev_api::Topics topics;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // SYSTEM_API_HPP
