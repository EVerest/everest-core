// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest

#ifndef EVSE_MANAGER_API_HPP
#define EVSE_MANAGER_API_HPP

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
#pragma GCC diagnostic ignored "-Wunused-parameter"
#include <generated/interfaces/generic_error/Implementation.hpp>
#pragma GCC diagnostic pop

// headers for required interface implementations
#include <generated/interfaces/evse_board_support/Interface.hpp>
#include <generated/interfaces/evse_manager/Interface.hpp>
#include <generated/interfaces/isolation_monitor/Interface.hpp>
#include <generated/interfaces/power_supply_DC/Interface.hpp>
#include <generated/interfaces/slac/Interface.hpp>
#include <generated/interfaces/uk_random_delay/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
#include "everest_api_types/utilities/CommCheckHandler.hpp"
#include "everest_api_types/utilities/Topics.hpp"

namespace ev_API = everest::lib::API;

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    int cfg_communication_check_to_s;
    int cfg_heartbeat_interval_ms;
    int cfg_request_reply_to_s;
};

class evse_manager_consumer_API : public Everest::ModuleBase {
public:
    evse_manager_consumer_API() = delete;

    evse_manager_consumer_API(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                              std::unique_ptr<generic_errorImplBase> p_main,
                              std::unique_ptr<evse_managerIntf> r_evse_manager,
                              std::vector<std::unique_ptr<evse_board_supportIntf>> r_evse_bsp,
                              std::vector<std::unique_ptr<slacIntf>> r_slac,
                              std::vector<std::unique_ptr<isolation_monitorIntf>> r_imd,
                              std::vector<std::unique_ptr<power_supply_DCIntf>> r_ps_dc,
                              std::vector<std::unique_ptr<uk_random_delayIntf>> r_random_delay, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        r_evse_manager(std::move(r_evse_manager)),
        r_evse_bsp(std::move(r_evse_bsp)),
        r_slac(std::move(r_slac)),
        r_imd(std::move(r_imd)),
        r_ps_dc(std::move(r_ps_dc)),
        r_random_delay(std::move(r_random_delay)),

        config(config),
        comm_check("generic/CommunicationFault", "Bridge to implementation connection lost", this->p_main){};

    Everest::MqttProvider& mqtt;
    const std::shared_ptr<generic_errorImplBase> p_main;
    const std::unique_ptr<evse_managerIntf> r_evse_manager;
    const std::vector<std::unique_ptr<evse_board_supportIntf>> r_evse_bsp;
    const std::vector<std::unique_ptr<slacIntf>> r_slac;
    const std::vector<std::unique_ptr<isolation_monitorIntf>> r_imd;
    const std::vector<std::unique_ptr<power_supply_DCIntf>> r_ps_dc;
    const std::vector<std::unique_ptr<uk_random_delayIntf>> r_random_delay;

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

    using ParseAndPublishFtor = std::function<bool(std::string const&)>;
    void subscribe_api_topic(const std::string& var, const ParseAndPublishFtor& parse_and_publish);

    auto forward_api_var(std::string const& var);

    void generate_api_cmd_get_evse();
    void generate_api_cmd_enable_disable();
    void generate_api_cmd_authorize_response();
    void generate_api_cmd_withdraw_authorization();
    void generate_api_cmd_reserve();
    void generate_api_cmd_cancel_reservation();
    void generate_api_cmd_pause_charging();
    void generate_api_cmd_resume_charging();
    void generate_api_cmd_stop_transaction();
    void generate_api_cmd_force_unlock();
    void generate_api_cmd_external_ready_to_start_charging();
    void generate_api_cmd_random_delay_enable();
    void generate_api_cmd_random_delay_disable();
    void generate_api_cmd_random_delay_cancel();
    void generate_api_cmd_random_delay_set_duration_s();

    void generate_api_var_session_event();
    void generate_api_var_limits();
    void generate_api_var_ev_info();
    void generate_api_var_car_manufacturer();
    void generate_api_var_powermeter();
    void generate_api_var_evse_id();
    void generate_api_var_hw_capabilities();
    void generate_api_var_waiting_for_external_ready();
    void generate_api_var_ready();
    void generate_api_var_selected_protocol();
    void generate_api_var_powermeter_public_key_ocmf();

    void generate_api_var_ac_nr_of_phases_available();
    void generate_api_var_ac_pp_ampacity();
    void generate_api_var_dlink_ready();
    void generate_api_var_isolation_measurement();
    void generate_api_var_dc_voltage_current();
    void generate_api_var_dc_mode();
    void generate_api_var_dc_capabilities();
    void generate_api_var_random_delay_countdown();

    void generate_api_var_communication_check();

    void setup_heartbeat_generator();

    ev_API::Topics topics;
    ev_API::CommCheckHandler<generic_errorImplBase> comm_check;

    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // EVSE_MANAGER_API_HPP
