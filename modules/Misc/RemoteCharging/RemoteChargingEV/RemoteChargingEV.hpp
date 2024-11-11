// SPDX-License-Identifier: Apache-2.0
// Copyright 2020 - 2025 Pionix GmbH and Contributors to EVerest
#ifndef REMOTE_CHARGING_EV_HPP
#define REMOTE_CHARGING_EV_HPP

//
// AUTO GENERATED - MARKED REGIONS WILL BE KEPT
// template version 2
//

#include "ld-ev.hpp"

// headers for provided interface implementations
#include <generated/interfaces/empty/Implementation.hpp>

// headers for required interface implementations
#include <generated/interfaces/ev_board_support/Interface.hpp>
#include <generated/interfaces/ev_board_support_extended/Interface.hpp>

// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1
// insert your custom include headers here
// ev@4bf81b14-a215-475c-a1d3-0a484ae48918:v1

namespace module {

struct Conf {
    std::string tap_device;
    std::string plc_device;
    std::string bridge_device;
    std::string base_topic_sub;
    std::string base_topic_pub;
};

class RemoteChargingEV : public Everest::ModuleBase {
public:
    RemoteChargingEV() = delete;
    RemoteChargingEV(const ModuleInfo& info, Everest::MqttProvider& mqtt_provider,
                     std::unique_ptr<emptyImplBase> p_main, std::unique_ptr<ev_board_supportIntf> r_bsp,
                     std::unique_ptr<ev_board_support_extendedIntf> r_bsp_extended, Conf& config) :
        ModuleBase(info),
        mqtt(mqtt_provider),
        p_main(std::move(p_main)),
        r_bsp(std::move(r_bsp)),
        r_bsp_extended(std::move(r_bsp_extended)),
        config(config){};

    Everest::MqttProvider& mqtt;
    const std::unique_ptr<emptyImplBase> p_main;
    const std::unique_ptr<ev_board_supportIntf> r_bsp;
    const std::unique_ptr<ev_board_support_extendedIntf> r_bsp_extended;
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
    int tap_fd{0};
    std::mutex mutex;
    std::string last_duty_cycle;
    double last_voltage{0.};

    std::string cp_state_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/cp_state";
    };
    std::string cp_pwm_duty_cycle_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/cp_pwm_dc";
    };
    std::string eth_ev_to_evse_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/eth_ev_to_evse";
    };
    std::string eth_evse_to_ev_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/eth_evse_to_ev_topic";
    };
    std::string nmk_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/nmk";
    };
    std::string dc_voltage_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/dc_voltage";
    };
    std::string ping_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/ping";
    };
    std::string pong_topic(bool pub) {
        return (pub ? config.base_topic_pub : config.base_topic_sub) + "/pong";
    };
    // ev@211cfdbe-f69a-4cd6-a4ec-f8aaa3d1b6c8:v1
};

// ev@087e516b-124c-48df-94fb-109508c7cda9:v1
// insert other definitions here
// ev@087e516b-124c-48df-94fb-109508c7cda9:v1

} // namespace module

#endif // REMOTE_CHARGING_EV_HPP
