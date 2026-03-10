// SPDX-License-Identifier: Apache-2.0
// Copyright Pionix GmbH and Contributors to EVerest
#ifndef EV_BOARD_SUPPORT_API_HPP
#define EV_BOARD_SUPPORT_API_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include <generated/interfaces/ev_board_support/Implementation.hpp>
#pragma GCC diagnostic pop

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include <everest_api_types/ev_board_support/API.hpp>
#include <everest_api_types/evse_board_support/API.hpp>
#include <everest_api_types/generic/API.hpp>
#include <everest_api_types/utilities/CommCheckHandler.hpp>
#include <everest_api_types/utilities/Topics.hpp>

namespace ev_API = everest::lib::API;
namespace ev_API_v = everest::lib::API::V1_0;

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    int cfg_communication_check_to_s;
    int cfg_heartbeat_interval_ms;
};

class ev_board_support_API : public Everest::ModuleBase {
public:
    ev_board_support_API() = delete;
    ev_board_support_API(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                         std::unique_ptr<ev_board_supportImplBase> p_main, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        config(config),
        comm_check("generic/CommunicationFault", "Bridge to implementation connection lost", this->p_main){};

    Everest::MqttProvider& mqtt;
    const std::shared_ptr<ev_board_supportImplBase> p_main;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    const ev_API::Topics& get_topics() const;
    ev_API::CommCheckHandler<ev_board_supportImplBase> comm_check;
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
    using HandleErrorFtor = std::function<void()>;
    struct ErrorHandler {
        HandleErrorFtor raiser;
        HandleErrorFtor clearer;
        std::string error_id;
    };
    void subscribe_api_topic(std::string const& var, ParseAndPublishFtor const& parse_and_publish);
    void generate_api_var_bsp_event();
    void generate_api_var_bsp_measurement();
    void generate_api_var_ev_info();
    void generate_api_var_communication_check();

    void generate_api_var_raise_error();
    void generate_api_var_clear_error();

    std::string make_error_string(ev_API_v::types::generic::Error const& error);

    void setup_heartbeat_generator();

    ev_API::Topics topics;
    size_t hb_id{0};

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // EV_BOARD_SUPPORT_API_HPP
