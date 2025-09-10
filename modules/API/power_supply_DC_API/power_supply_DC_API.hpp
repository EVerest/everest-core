// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef POWER_SUPPLY_DC_API_HPP
#define POWER_SUPPLY_DC_API_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include <generated/interfaces/power_supply_DC/Implementation.hpp>
#pragma GCC diagnostic pop

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "utils/error.hpp"
#include <atomic>

#include <everest_api_types/utilities/Topics.hpp>
#include <everest_api_types/power_supply_DC/API.hpp>
#include <everest_api_types/utilities/CommCheckHandler.hpp>

namespace ev_API = everest::lib::API;
namespace API_types = ev_API::V1_0::types;
namespace API_types_ext = API_types::power_supply_DC;

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    int cfg_communication_check_to_s;
    int cfg_heartbeat_interval_ms;
};

class power_supply_DC_API : public Everest::ModuleBase {
public:
    power_supply_DC_API() = delete;
    power_supply_DC_API(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                        std::unique_ptr<power_supply_DCImplBase> p_main, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        config(config),
        comm_check("power_supply_DC/CommunicationFault", "Bridge to implementation connection lost",
                   this->p_main){};

    Everest::MqttProvider& mqtt;
    const std::shared_ptr<power_supply_DCImplBase> p_main;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    const ev_API::Topics& get_topics() const;
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
    using ParseAndPublishFtor = std::function<void(const std::string&)>;
    void subscribe_api_var(const std::string& var, const ParseAndPublishFtor& parse_and_publish);
    void generate_api_var_mode();
    void generate_api_var_voltage_current();
    void generate_api_var_capabilities();
    void generate_api_var_raise_error();
    void generate_api_var_clear_error();
    void generate_api_var_communication_check();

    std::string make_error_string(API_types_ext::Error const& error);

    void setup_heartbeat_generator();

    ev_API::Topics topics;
    ev_API::CommCheckHandler<power_supply_DCImplBase> comm_check;
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // POWER_SUPPLY_DC_API_HPP
