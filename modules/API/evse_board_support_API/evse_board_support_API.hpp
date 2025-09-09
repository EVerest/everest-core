// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef EVSE_BOARD_SUPPORT_API_HPP
#define EVSE_BOARD_SUPPORT_API_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"
#include "utils/error.hpp"

// headers for provided interface implementations
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-function"
#include <generated/interfaces/ac_rcd/Implementation.hpp>
#include <generated/interfaces/connector_lock/Implementation.hpp>
#include <generated/interfaces/evse_board_support/Implementation.hpp>
#pragma GCC diagnostic pop

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// #include "API.hpp"
#include <everest_api_types/evse_board_support/API.hpp>
#include "everest_api_types/utilities/Topics.hpp"
#include "everest_api_types/utilities/CommCheckHandler.hpp"

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

namespace ns_ev_api = everest::lib::API;
namespace ns_types_ext = ns_ev_api::V1_0::types::evse_board_support;

struct Conf {
    int cfg_communication_check_to_s;
    int cfg_heartbeat_interval_ms;
    int cfg_request_reply_to_s;
};

class evse_board_support_API : public Everest::ModuleBase {
public:
    evse_board_support_API() = delete;
    evse_board_support_API(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                           std::unique_ptr<evse_board_supportImplBase> p_board_support,
                           std::unique_ptr<ac_rcdImplBase> p_rcd,
                           std::unique_ptr<connector_lockImplBase> p_connector_lock, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_board_support(std::move(p_board_support)),
        p_rcd(std::move(p_rcd)),
        p_connector_lock(std::move(p_connector_lock)),
        config(config),
        comm_check("evse_board_support/CommunicationFault", "Bridge to implementation connection lost",
                   this->p_board_support){};

    Everest::MqttProvider& mqtt;
    const std::shared_ptr<evse_board_supportImplBase> p_board_support;
    const std::unique_ptr<ac_rcdImplBase> p_rcd;
    const std::unique_ptr<connector_lockImplBase> p_connector_lock;
    const Conf& config;

    // ev@1fce4c5e-0ab8-41bb-90f7-14277703d2ac:v1
    // insert your public definitions here
    const ns_ev_api::Topics& get_topics() const;
    ns_ev_api::CommCheckHandler<evse_board_supportImplBase> comm_check;

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
    using HandleErrorFtor = std::function<void()>;
    struct ErrorHandler {
        HandleErrorFtor raiser;
        HandleErrorFtor clearer;
        std::string error_id;
    };

    void subscribe_api_var(const std::string& var, const ParseAndPublishFtor& parse_and_publish);
    void generate_api_var_event();
    void generate_api_var_phase_count();
    void generate_api_var_capabilities();
    void generate_api_var_ac_pp_ampacity();
    void generate_api_var_request_stop_transaction();
    void generate_api_var_rcd_current();
    void generate_api_var_communication_check();

    void generate_api_var_raise_error();
    void generate_api_var_clear_error();

    ErrorHandler make_error_handler(ns_types_ext::Error const& error);

    void setup_heartbeat_generator();

    ns_ev_api::Topics topics;

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // EVSE_BOARD_SUPPORT_API_HPP
